#pragma once

#include <mesh.h>
#include <glm/glm.hpp>
#include <window.h>
#include <imgui.h>
#include <SDL.h>

struct imgui_drawer {
	imgui_drawer(shader_prog *prog, window *wnd);

	void process_event(SDL_Event *ev);
	void update();
	void render();
private:
	texture _tex;
	mesh _mesh;
	int *_tile_store;
};
