#pragma once

#include <epoxy/gl.h>
#include <string>

struct texture {
	static texture load_from_file(const std::string &path) {
		texture t{};
		t.load(path);
		return t;
	}

	friend void swap (texture &a, texture &b) {
		using std::swap;
		swap(a._id, b._id);
		swap(a._width, b._width);
		swap(a._height, b._height);
	}

	texture()
	:_id{}, _width{}, _height{} {}

	~texture() {
		glDeleteTextures(1, &_id);
	}

	texture(const texture &other) = delete;
	texture(texture &&other) {
		swap(*this, other);
	}

	texture &operator=(const texture &other) = delete;
	texture &operator=(texture &&other) {
		swap(*this, other);
		return *this;
	}

	void load(const std::string &path);
	void load(const void *data, int width, int height);

	void reload(const void *data);

	void bind() {
		glBindTexture(GL_TEXTURE_2D, _id);
	}

	void unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint get_id() const {
		return _id;
	}

	int width() const {
		return _width;
	}

	int height() const {
		return _height;
	}

private:
	GLuint _id;

	int _width, _height;
};
