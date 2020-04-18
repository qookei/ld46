#include <utils/mapped_file.h>
#include <console.h>
#include <utils/errors.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#endif

mapped_file::mapped_file(const std::string &path, off_t offset)
: _data{nullptr}, _size{0} {
#if defined(_WIN32) || defined(_WIN64)
	_file = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (_file == INVALID_HANDLE_VALUE) {
		auto e = error_from_winapi();
		console::err(
			"mapped_file::mapped_file: CreateFileA failed: %s",
			e.c_str());
		return;
	}

	DWORD lo_size, hi_size;
	lo_size = GetFileSize(_file, &hi_size);

	_size = static_cast<size_t>(lo_size) | (static_cast<size_t>(hi_size) << 32);

	_file_mapping = CreateFileMappingA(_file, nullptr, PAGE_READWRITE, 0, 0, nullptr);

	if (_file_mapping == nullptr) {
		auto e = error_from_winapi();
		console::err(
			"mapped_file::mapped_file: CreateFileMappingA failed: %s",
			e.c_str());
		return;
	}

	_data = MapViewOfFile(_file_mapping, FILE_MAP_WRITE | FILE_MAP_READ | FILE_MAP_COPY, 0, static_cast<DWORD>(offset), _size);

	if (!_data) {
		auto e = error_from_winapi();
		console::err(
			"mapped_file::mapped_file: MapViewOfFile failed: %s",
			e.c_str());
		return;
	}
#else
	int _fd = open(path.c_str(), O_RDONLY);
	if (_fd < 0) {
		auto e = error_from_errno();
		console::err(
			"mapped_file::mapped_file: open failed: %s",
			e.c_str());
		return;
	}

	struct stat st;
	if (fstat(_fd, &st) < 0) {
		auto e = error_from_errno();
		console::err(
			"mapped_file::mapped_file: fstat failed: %s",
			e.c_str());
		return;
	}

	_size = st.st_size;

	_data = mmap(NULL, _size, PROT_READ | PROT_WRITE, MAP_PRIVATE, _fd, offset);
	if (_data == MAP_FAILED) {
		_data = nullptr;
		auto e = error_from_errno();
		console::err(
			"mapped_file::mapped_file: mmap failed: %s",
			e.c_str());
		return;
	}

	close(_fd);
#endif
}

mapped_file::~mapped_file() {
#if defined(_WIN32) || defined(_WIN64)
	UnmapViewOfFile(_data);
	CloseHandle(_file_mapping);
	CloseHandle(_file);
#else
	munmap(_data, _size);
#endif
}
