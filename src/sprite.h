#pragma once

#include <mesh.h>
#include <texture.h>
#include <glm/vec3.hpp>
#include <console.h>

struct sprite {
	friend void swap(sprite &a, sprite &b) {
		using std::swap;
		swap(a._m, b._m);
		swap(a._t, b._t);
		swap(a._prog, b._prog);
		swap(a._pos, b._pos);
	}

	sprite(const std::string &path, shader_prog *prog, int sw, int sh)
	:_m{prog}, _t{}, _prog{prog}, _pos{0, 0, 0} {
		_t.load(path);

		std::vector<vertex> _vertices;
		_vertices.resize(6);

		int w = _t.width(), h = _t.height();
		if (sw == -1) sw = w;
		if (sh == -1) sh = h;

		_vertices[0].pos = {0, 0, 0};
		_vertices[0].uv = {0, 0};
		_vertices[0].color = {1, 1, 1, 1};
		_vertices[1].pos = {sw, sh, 0};
		_vertices[1].uv = {1, 1};
		_vertices[1].color = {1, 1, 1, 1};
		_vertices[2].pos = {0, sh, 0};
		_vertices[2].uv = {0, 1};
		_vertices[2].color = {1, 1, 1, 1};
		_vertices[3].pos = {0, 0, 0};
		_vertices[3].uv = {0, 0};
		_vertices[3].color = {1, 1, 1, 1};
		_vertices[4].pos = {sw, 0, 0};
		_vertices[4].uv = {1, 0};
		_vertices[4].color = {1, 1, 1, 1};
		_vertices[5].pos = {sw, sh, 0};
		_vertices[5].uv = {1, 1};
		_vertices[5].color = {1, 1, 1, 1};

		_m.gen_buffers();
		_m.gen_vao();
		_m.vbo()->store_regenerate(
			_vertices.data(),
			_vertices.size() * sizeof(vertex),
			GL_STATIC_DRAW);
		_m.mark_valid();
	}

	sprite(const std::string &path, shader_prog *prog, int sw, int sh, int tw, int th, int tu, int tv)
	:_m{prog}, _t{}, _prog{prog}, _pos{0, 0, 0} {
		_t.load(path);

		std::vector<vertex> _vertices;
		_vertices.resize(6);

		int w = _t.width(), h = _t.height();
		if (sw == -1) sw = w;
		if (sh == -1) sh = h;

		float u1 = static_cast<float>(tu) / static_cast<float>(sw);
		float v1 = static_cast<float>(tv) / static_cast<float>(sh);
		float u2 = u1 + static_cast<float>(tw) / static_cast<float>(sw);
		float v2 = v1 + static_cast<float>(th) / static_cast<float>(sh);

		_vertices[0].pos = {0, 0, 0};
		_vertices[0].uv = {u1, v1};
		_vertices[0].color = {1, 1, 1, 1};
		_vertices[1].pos = {tw, th, 0};
		_vertices[1].uv = {u2, v2};
		_vertices[1].color = {1, 1, 1, 1};
		_vertices[2].pos = {0, th, 0};
		_vertices[2].uv = {u1, v2};
		_vertices[2].color = {1, 1, 1, 1};
		_vertices[3].pos = {0, 0, 0};
		_vertices[3].uv = {u1, v1};
		_vertices[3].color = {1, 1, 1, 1};
		_vertices[4].pos = {tw, 0, 0};
		_vertices[4].uv = {u2, v1};
		_vertices[4].color = {1, 1, 1, 1};
		_vertices[5].pos = {tw, th, 0};
		_vertices[5].uv = {u2, v2};
		_vertices[5].color = {1, 1, 1, 1};

		_m.gen_buffers();
		_m.gen_vao();
		_m.vbo()->store_regenerate(
			_vertices.data(),
			_vertices.size() * sizeof(vertex),
			GL_STATIC_DRAW);
		_m.mark_valid();
	}

	sprite(const sprite &) = delete;
	sprite &operator=(const sprite &) = delete;

	sprite(sprite &&other) {
		swap(*this, other);
	}

	sprite &operator=(sprite &&other) {
		swap(*this, other);
		return *this;
	}

	void render(glm::vec3 world_transform, float angle = 0, bool do_rotate = false) {
		_prog->use();
		glm::mat4 model = glm::translate(_pos + world_transform);
		if (do_rotate) {
			model = glm::rotate(model,
					angle, glm::vec3{0, 0, 1});
			model = glm::translate(model,
					world_transform
					- glm::vec3{
						_t.width() / 2,
						_t.height() / 2, 0});
		}

		_prog->set_uniform("model", model);
		_m.render(_t);
	}

	glm::vec3 &position() {
		return _pos;
	}

private:
	mesh _m;
	texture _t;
	shader_prog *_prog;

	glm::vec3 _pos;
};
