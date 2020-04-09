#include "chunk_view.h"

chunk_view::chunk_view(shader_prog *prog, int *tile_store)
:_mesh{prog, vertices}, _tile_store{tile_store} {
}

inline chunk_view::tile_info chunk_view::fetch_tile_info_for(int id) {
	static constexpr tile_info infos[] = {
		{0, 0, {1, 1, 1, 1}}, // air
		{2, 1, {1, 1, 1, 1}, {0.063f, 0.645f, 0.011f, 1}}, // grass
		{2, 0, {1, 1, 1, 1}}, // dirt
		{3, 0, {1, 1, 1, 1}}, // stone
		{4, 0, {1, 1, 1, 1}}, // cobble
		{3, 5, {1, 1, 1, 1}, {1, 1, 1, 1}}, // iron ore
		{3, 6, {1, 1, 1, 1}, {1, 1, 1, 1}}, // coal ore
		{7, 0, {1, 1, 1, 1}}, // wood log
		{8, 0, {0.063f, 0.645f, 0.011f, 1}}, // leaves
		{9, 0, {1, 1, 1, 1}}, // planks
		{10, 0, {1, 1, 1, 1}}, // water
		{11, 0, {1, 1, 1, 1}}, // ice
		{12, 0, {1, 1, 1, 1}, {1, 1, 1, 1}, true, {1, 1, 1, 1}}, // torch
		{13, 0, {1, 1, 1, 1}, {1, 1, 1, 1}, true, {1, 1, 1, 1}}, // lava
		{14, 0, {1, 1, 1, 1}}, // sand
		{15, 0, {1, 1, 1, 1}}, // cactus
	};

	if (id < 0 || id >= static_cast<int>(sizeof(infos) / sizeof(*infos)))
		return {16, 0, {1, 1, 1, 1}, {1, 1, 1, 1}, false, {1, 1, 1, 1}}; // error

	return infos[id];
}

size_t chunk_view::generate_mesh(vertex *verts) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			vertex *back_verts = verts + ((x + y * width) * 6);
			vertex *front_verts = back_verts + (width * width * 6);

			tile_info info = fetch_tile_info_for(_tile_store[x + y * width]);

			float vx = x * 16, vy = y * 16, w = 16, h = 16;
			float back_u1 = static_cast<float>(info.bg_idx % 16) / 16.f;
			float back_v1 = static_cast<float>(info.bg_idx / 16) / 16.f;
			float back_u2 = back_u1 + 1.f / 16.f;
			float back_v2 = back_v1 + 1.f / 16.f;

			float front_u1 = static_cast<float>(info.fg_idx % 16) / 16.f;
			float front_v1 = static_cast<float>(info.fg_idx / 16) / 16.f;
			float front_u2 = front_u1 + 1.f / 16.f;
			float front_v2 = front_v1 + 1.f / 16.f;

			back_verts[0].pos = glm::vec3{vx, vy, 0};
			back_verts[0].uv = {back_u1, back_v1};
			back_verts[0].color = info.bg_color;
			back_verts[1].pos = glm::vec3{vx + w, vy + h, 0};
			back_verts[1].uv = {back_u2, back_v2};
			back_verts[1].color = info.bg_color;
			back_verts[2].pos = glm::vec3{vx, vy + h, 0};
			back_verts[2].uv = {back_u1, back_v2};
			back_verts[2].color = info.bg_color;

			back_verts[3].pos = glm::vec3{vx, vy, 0};
			back_verts[3].uv = {back_u1, back_v1};
			back_verts[3].color = info.bg_color;
			back_verts[4].pos = glm::vec3{vx + w, vy, 0};
			back_verts[4].uv = {back_u2, back_v1};
			back_verts[4].color = info.bg_color;
			back_verts[5].pos = glm::vec3{vx + w, vy + h, 0};
			back_verts[5].uv = {back_u2, back_v2};
			back_verts[5].color = info.bg_color;

			front_verts[0].pos = glm::vec3{vx, vy, 0};
			front_verts[0].uv = {front_u1, front_v1};
			front_verts[0].color = info.fg_color;
			front_verts[1].pos = glm::vec3{vx + w, vy + h, 0};
			front_verts[1].uv = {front_u2, front_v2};
			front_verts[1].color = info.fg_color;
			front_verts[2].pos = glm::vec3{vx, vy + h, 0};
			front_verts[2].uv = {front_u1, front_v2};
			front_verts[2].color = info.fg_color;

			front_verts[3].pos = glm::vec3{vx, vy, 0};
			front_verts[3].uv = {front_u1, front_v1};
			front_verts[3].color = info.fg_color;
			front_verts[4].pos = glm::vec3{vx + w, vy, 0};
			front_verts[4].uv = {front_u2, front_v1};
			front_verts[4].color = info.fg_color;
			front_verts[5].pos = glm::vec3{vx + w, vy + h, 0};
			front_verts[5].uv = {front_u2, front_v2};
			front_verts[5].color = info.fg_color;
		}
	}
	return vertices;
}

void chunk_view::update_mesh() {
	_mesh.update_async(&chunk_view::generate_mesh, this);
}

void chunk_view::render(texture &tex) {
	_mesh.render(tex);
}

