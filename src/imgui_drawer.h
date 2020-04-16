#pragma once

#include <mesh.h>
#include <glm/glm.hpp>
#include <window.h>
#include <imgui.h>
#include <SDL.h>

struct imgui_drawer {
	imgui_drawer(shader_prog *prog, window *wnd);

	bool process_event(SDL_Event *ev);
	void update();
	void render();
private:
	mesh _mesh;
	window *_wnd;
	shader_prog *_prog;
	texture _tex;
};
