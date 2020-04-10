#pragma once

#include <vertex_object.h>
#include <shader_prog.h>
#include <texture.h>
#include <glm/glm.hpp>
#include <future>
#include <functional>
#include <atomic>

struct mesh {
	mesh(shader_prog *prog, size_t n_vertices);

	template <typename Func, typename ...Args>
	void update_sync(Func &&functor, Args &&...args) {
		_valid = false;
		vertex *verts = _obj.map();
		size_t used = std::invoke(functor, std::forward<Args>(args)..., verts);
		_obj.set_used(used);

		std::promise<void> _promise;
		_future = _promise.get_future();
		_promise.set_value();
	}

	template <typename Func, typename ...Args>
	void update_async(Func &&functor, Args &&...args) {
		_valid = false;
		_future = std::async(
			std::launch::async,
			perform_async_update,
			this,
			_obj.map(),
			std::forward<Func>(functor),
			std::forward<Args>(args)...
		);
	}

	void generate_mesh();

	void resize(size_t n_vertices) {
		_obj.resize(n_vertices);
		_n_vertices = n_vertices;
	}

	void render(texture &tex);

private:
	shader_prog *_prog;
	vertex_object _obj;

	size_t _n_vertices;

	std::future<void> _future;
	std::atomic_bool _valid;

	struct {
		template <typename Func, typename ...Args>
		void operator()(mesh *m, vertex *verts, Func &&functor, Args &&...args) {
			size_t used = std::invoke(functor, std::forward<Args>(args)..., verts);
			m->_obj.set_used(used);
		}
	} perform_async_update;
};
