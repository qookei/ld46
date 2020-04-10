#pragma once

#include <epoxy/gl.h>
#include <string>

struct texture {
	texture()
	:_id{} {}

	~texture() {
		glDeleteTextures(1, &_id);
	}

	void load(const std::string &path);
	void load(const void *data, int width, int height);

	void bind() {
		glBindTexture(GL_TEXTURE_2D, _id);
	}

	void unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint get_id() const {
		return _id;
	}

private:
	GLuint _id;
};
