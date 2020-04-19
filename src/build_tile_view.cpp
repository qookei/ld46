#include "build_tile_view.h"

build_tile_view::build_tile_view(shader_prog *prog)
:_mesh{prog}, _tex{} {
	memset(_pixels.data(), 0, width * height * 4);
	_mesh.gen_buffers();
	_mesh.gen_vao();

	vertex v[6];

	int x = 72, y = 140;
	v[0].pos = glm::vec3{x, y, 0};
	v[0].uv = {0, 0};
	v[0].color = {1, 1, 1, 1};
	v[1].pos = glm::vec3{x + width, y + height, 0};
	v[1].uv = {1, 1};
	v[1].color = {1, 1, 1, 1};
	v[2].pos = glm::vec3{x, y + height, 0};
	v[2].uv = {0, 1};
	v[2].color = {1, 1, 1, 1};

	v[3].pos = glm::vec3{x, y, 0};
	v[3].uv = {0, 0};
	v[3].color = {1, 1, 1, 1};
	v[4].pos = glm::vec3{x + width, y, 0};
	v[4].uv = {1, 0};
	v[4].color = {1, 1, 1, 1};
	v[5].pos = glm::vec3{x + width, y + height, 0};
	v[5].uv = {1, 1};
	v[5].color = {1, 1, 1, 1};

	_mesh.vbo()->store_regenerate(v, 6 * sizeof(vertex), GL_STATIC_DRAW);
	_tex.load(_pixels.data(), width, height);
	_mesh.mark_valid();
}

void build_tile_view::render() {
	_mesh.render(_tex);
}

