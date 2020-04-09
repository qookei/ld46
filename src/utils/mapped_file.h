#pragma once

#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

struct mapped_file {
	mapped_file(const std::string &path, off_t offset = 0);

	mapped_file(const mapped_file &file) = delete;
	mapped_file &operator=(const mapped_file &file) = delete;

	~mapped_file();

	void *data() {
		return _data;
	}

	size_t size() {
		return _size;
	}

	operator bool() {
		return _data;
	}

private:
	void *_data;
	size_t _size;

#if defined(_WIN32) || defined(_WIN64)
	HANDLE _file;
	HANDLE _file_mapping;
#endif
};
