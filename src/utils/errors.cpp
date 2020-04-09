#include <utils/errors.h>
#include <cstring>

std::string error_from_errno() {
	return std::string{strerror(errno)};
}

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <window.h>

std::string error_from_winapi() {
	DWORD err = GetLastError();
	if (!err)
		return "No error";

	char *buf = nullptr;
	DWORD len = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_FROM_SYSTEM
			| FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPTSTR>(&buf), 0, nullptr);

	if (!len)
		window::report_fatal("error_from_winapi: FormatMessageA failed");

	std::string ret{buf, len};

	LocalFree(buf);

	return ret;
}
#endif
