#pragma once

#include <errno.h>
#include <stdlib.h>

#include <string>

std::string error_from_errno();

#if defined(_WIN32) || defined(_WIN64)
std::string error_from_winapi();
#endif
