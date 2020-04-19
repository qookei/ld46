#pragma once

#include <mesh.h>
#include <glm/glm.hpp>

struct build_tile_view {
	static constexpr int width = 1120;
	static constexpr int height = 376;
	static constexpr int pixels = width * height;

	build_tile_view(shader_prog *prog);

	void upload_texture() {
		_tex.reload(_pixels.data());
	}

	void render();

	bool is_valid_spot(int x, int y) {
		return (x >= 0 &&
			x < width &&
			y >= 0 &&
			y < height) &&
			((x < 480 || x >= 656) ? true : y < 136);
	}

	void set(int x, int y, bool v) {
		uint8_t col = (((1103515245u + 12345u) * static_cast<unsigned int>(x + width * y) / 32u) % 16u) + 48u;

		if (v)
			_pixels[x + y * width] = (0xFF << 24)
						| (col << 16)
						| (col << 8)
						| (col);
		else
			_pixels[x + y * width] = 0x00000000;
	}

	bool get(int x, int y) {
		return _pixels[x + y * width] & 0xFF000000;
	}

private:
	mesh _mesh;
	std::array<uint32_t, pixels> _pixels;

	texture _tex;
};
