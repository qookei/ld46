#pragma once

#include <epoxy/gl.h>
#include <cassert>
#include <memory>
#include <numeric>

#include <shader_prog.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct vertex {
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec4 color;
};

struct vertex_object {
	vertex_object()
	:_vbo{0}, _vao{0}, _max_vertices{0}, _used_vertices{0},
	_buffer_ptr{nullptr}, _prog{nullptr}, _usage{0} {}

	~vertex_object() {
		if (_buffer_ptr)
			unmap();

		glDeleteBuffers(1, &_vbo);
		glDeleteVertexArrays(1, &_vao);
	}

	void attach_program(shader_prog *prog) {
		_prog = prog;
	}

	void generate(size_t n_vertices, GLenum usage = GL_DYNAMIC_DRAW) {
		_usage = usage;

		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);

		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);

		glBufferData(GL_ARRAY_BUFFER, n_vertices * sizeof(vertex), nullptr, _usage);
		_max_vertices = n_vertices;

		GLuint xyz_attr = _prog->attribute_location("vert_pos");
		GLuint uv_attr = _prog->attribute_location("tex_pos");
		GLuint rgba_attr = _prog->attribute_location("color");

		glVertexAttribPointer(xyz_attr, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void *>(offsetof(vertex, pos)));
		glEnableVertexAttribArray(xyz_attr);

		glVertexAttribPointer(uv_attr, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void *>(offsetof(vertex, uv)));
		glEnableVertexAttribArray(uv_attr);

		glVertexAttribPointer(rgba_attr, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void *>(offsetof(vertex, color)));
		glEnableVertexAttribArray(rgba_attr);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void render(int instances = 1) {
		glBindVertexArray(_vao);
		_prog->use();

		glDrawArraysInstanced(GL_TRIANGLES, 0, _used_vertices, instances);

		glBindVertexArray(0);
	}

	vertex *map() {
		_buffer_ptr = static_cast<vertex *>(
			glMapNamedBufferRange(
				_vbo,
				0,
				_max_vertices * sizeof(vertex),
				GL_MAP_READ_BIT | GL_MAP_WRITE_BIT
			)
		);

		assert(_buffer_ptr);
		return _buffer_ptr;
	}

	void tx_subdata(vertex *data, size_t n_vertices) {
		assert(n_vertices <= _max_vertices);

		glNamedBufferSubData(_vbo, 0, n_vertices * sizeof(vertex), data);

		_used_vertices = n_vertices;
	}

	void unmap() {
		glUnmapNamedBuffer(_vbo);

		_buffer_ptr = nullptr;
	}

	size_t max_vertices() const {
		return _max_vertices;
	}

	void set_used(size_t n_vertices) {
		_used_vertices = n_vertices;
	}

	size_t used_vertices() const {
		return _used_vertices;
	}

	vertex *mapped_buffer() {
		return _buffer_ptr;
	}

	// note: this thrashes existing buffer data
	void resize(size_t size) {
		_max_vertices = size;
		if (_used_vertices > size)
			_used_vertices = size;

		glNamedBufferData(_vbo, size * sizeof(vertex), nullptr, _usage);
	}

private:
	GLuint _vbo;
	GLuint _vao;

	size_t _max_vertices;
	size_t _used_vertices;
	vertex *_buffer_ptr;

	shader_prog *_prog;

	GLenum _usage;
};
