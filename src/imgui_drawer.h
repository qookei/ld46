#pragma once

#include <mesh.h>
#include <glm/glm.hpp>
#include <window.h>
#include <imgui.h>

struct imgui_drawer {
	imgui_drawer(shader_prog *prog, window *wnd);

	void update_mesh();

	void render();
private:
	size_t generate_mesh(ImDrawData *, vertex *);

	texture _tex;
	mesh _mesh;
	int *_tile_store;
};
