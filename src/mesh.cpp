#include "mesh.h"
#include <chrono>

mesh::mesh(shader_prog *prog)
:_prog{prog}, _vao{}, _vbo{}, _ebo{}, _future{}, _valid{false} {
	_vao = std::make_unique<vao>();
	_vbo = std::make_unique<buffer<GL_ARRAY_BUFFER, vertex *>>();
}

void mesh::add_ebo() {
	_ebo = std::make_unique<buffer<GL_ELEMENT_ARRAY_BUFFER, GLint *>>();
}

void mesh::gen_buffers() {
	_vao->generate();
	_vbo->generate();
	if (_ebo) _ebo->generate();
}

void mesh::gen_vao() {
	_vao->bind();
	_vbo->bind();
	if (_ebo) _ebo->bind();

	_vao->set_attrib_pointer(_prog, vertex_attr("vert_pos", 3, GL_FLOAT, GL_FALSE, vertex, pos));
	_vao->set_attrib_pointer(_prog, vertex_attr("tex_pos", 2, GL_FLOAT, GL_FALSE, vertex, uv));
	_vao->set_attrib_pointer(_prog, vertex_attr("color", 4, GL_FLOAT, GL_FALSE, vertex, color));

	_vao->unbind();
	_vbo->unbind();
	if (_ebo) _ebo->unbind();
}

void mesh::render(texture &tex) {
	if (_future.valid() && !_valid) {
		auto status = _future.wait_for(std::chrono::seconds(0));

		if (status == std::future_status::timeout) {
			fprintf(stderr, "mesh::render: mesh is not generated yet\n");
			return;
		} else {
			printf("mesh::render: mesh becomes ready\n");
			assert(status == std::future_status::ready);
			assert(_vbo->mapped_buffer());
			assert(!_ebo || _ebo->mapped_buffer());

			_future.get();
			_vbo->unmap();
			if (_ebo)
				_ebo->unmap();
			_valid = true;
		}
	} else if (!_valid) {
		fprintf(stderr, "mesh::render: mesh is not valid at this point\n");
		return;
	}

	assert(!_vbo->mapped_buffer());
	assert(!_ebo || !_ebo->mapped_buffer());

	tex.bind();
	_vao->bind();
	glDrawArrays(GL_TRIANGLES, 0, _vbo->size() / sizeof(vertex));
}

