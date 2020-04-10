#pragma once

#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <iostream>

struct window {
	window();

	window(const window &) = delete;
	window &operator=(const window &) = delete;

	~window();

	void swap();

	// TODO
	int get_width() const {
		return 1280;
	}

	// TODO
	int get_height() const {
		return 720;
	}

	[[noreturn]] static void report_fatal(const std::string &format, ...);
private:
	SDL_Window *_window;
	SDL_GLContext _context;
};
