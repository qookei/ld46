#include "window.h"
#include <cstdarg>

window::window() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		report_fatal("window::window: SDL_Init failed: %s", SDL_GetError());
		return;
	}

	_window = SDL_CreateWindow("UFO Shield", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);

	if (!_window) {
		report_fatal("window::window: SDL_CreateWindow failed: %s", SDL_GetError());
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	_context = SDL_GL_CreateContext(_window);
	if (!_context) {
		report_fatal("window::window: SDL_GL_CreateContext failed: %s", SDL_GetError());
		return;
	}

	SDL_GL_SetSwapInterval(1);
}

window::~window() {
	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

void window::swap() {
	SDL_GL_SwapWindow(_window);
}

[[noreturn]] void window::report_fatal(const char *format, ...) {
	va_list va;
	va_start(va, format);

	char buf[2048];
	memset(buf, 0, 2048);

	vsnprintf(buf, 2047, format, va);

	fprintf(stderr, "%s\n", buf);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Engine Error", buf, nullptr);

	va_end(va);
	abort();
}
