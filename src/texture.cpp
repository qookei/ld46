#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <utils/mapped_file.h>
#include <iostream>

void texture::load(const std::string &path) {
	glGenTextures(1, &_id);
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	mapped_file f{path};
	if (!f) {
		fprintf(stderr, "failed to open file %s\n", path.c_str());
		return;
	}

	int w, h, n;
	uint8_t *pixel_data = stbi_load_from_memory(reinterpret_cast<uint8_t *>(f.data()), f.size(), &w, &h, &n, 0);
	if (pixel_data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data);
	} else {
		fprintf(stderr, "failed to parse file\n");
	}
	stbi_image_free(pixel_data);

}
