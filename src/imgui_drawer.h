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
	void generate_mesh(ImDrawData *, vertex *, int *);

	texture _tex;
	mesh _mesh;
	int *_tile_store;
};
