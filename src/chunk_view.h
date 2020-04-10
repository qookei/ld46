#pragma once

#include <mesh.h>
#include <glm/glm.hpp>

struct chunk_view {
	static constexpr int width = 32;
	static constexpr int height = 32;
	static constexpr int vertices = width * height * 6 * 2;

	chunk_view(shader_prog *prog, int *tile_store);

	void update_mesh();

	void render(texture &tex);

private:
	struct tile_info {
		int bg_idx;
		int fg_idx;
		glm::vec4 bg_color;
		glm::vec4 fg_color;
	};

	static inline tile_info fetch_tile_info_for(int id);

	size_t generate_mesh(vertex *);

	mesh _mesh;
	int *_tile_store;
};
