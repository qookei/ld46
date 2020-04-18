#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <utils/mapped_file.h>
#include <iostream>
#include <console.h>

void texture::load(const std::string &path) {
	glGenTextures(1, &_id);
	bind();

	mapped_file f{path};
	if (!f) {
		glDeleteTextures(1, &_id);
		console::warn("failed to open file '%s'", path.c_str());
		load("res/missing.png");
		return;
	}

	int w, h, n;
	uint8_t *pixel_data = stbi_load_from_memory(reinterpret_cast<uint8_t *>(f.data()), f.size(), &w, &h, &n, 0);

	if (pixel_data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data);
		_width = w;
		_height = h;
	} else {
		console::warn("failed to parse file");
		load("res/missing.png");
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unbind();

	stbi_image_free(pixel_data);
}

void texture::load(const void *data, int width, int height) {
	glGenTextures(1, &_id);
	bind();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	_width = width;
	_height = height;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unbind();
}

void texture::reload(const void *data) {
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	unbind();
}
