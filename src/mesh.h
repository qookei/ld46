#pragma once

#include <vao.h>
#include <buffer.h>
#include <shader_prog.h>
#include <texture.h>
#include <glm/glm.hpp>
#include <future>
#include <functional>
#include <atomic>

struct vertex {
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec4 color;
};

struct mesh {
	mesh(shader_prog *prog);

	void add_ebo();
	void gen_buffers();
	void gen_vao();

	template <typename Func, typename ...Args>
	void update_sync(Func &&functor, Args &&...args) {
		_valid = false;
		auto vbo_buf = _vbo ? _vbo->map() : nullptr;
		auto ebo_buf = _ebo ? _ebo->map() : nullptr;
		std::invoke(
			functor,
			std::forward<Args>(args)...,
			vbo_buf,
			ebo_buf
		);

		std::promise<void> _promise;
		_future = _promise.get_future();
		_promise.set_value();
	}

	template <typename Func, typename ...Args>
	void update_async(Func &&functor, Args &&...args) {
		_valid = false;
		auto vbo_buf = _vbo ? _vbo->map() : nullptr;
		auto ebo_buf = _ebo ? _ebo->map() : nullptr;
		_future = std::async(
			std::launch::async,
			functor,
			std::forward<Args>(args)...,
			vbo_buf,
			ebo_buf
		);
	}

	bool is_ready();

	gl_vao *vao() {
		return _vao.get();
	}

	buffer<GL_ARRAY_BUFFER, vertex *> *vbo() {
		return _vbo.get();
	}

	buffer<GL_ELEMENT_ARRAY_BUFFER, GLuint *> *ebo() {
		return _ebo.get();
	}

	void render(texture &tex);

	void mark_valid() {
		_valid = true;
	}
private:
	shader_prog *_prog;
	std::unique_ptr<gl_vao> _vao;
	std::unique_ptr<buffer<GL_ARRAY_BUFFER, vertex *>> _vbo;
	std::unique_ptr<buffer<GL_ELEMENT_ARRAY_BUFFER, GLuint *>> _ebo;

	std::future<void> _future;
	std::atomic_bool _valid;
};
