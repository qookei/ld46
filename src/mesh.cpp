#include "mesh.h"
#include <chrono>

mesh::mesh(shader_prog *prog, size_t n_vertices)
:_prog{prog}, _obj{}, _n_vertices{n_vertices}, _future{}, _valid{false} {
	_obj.attach_program(prog);
	_obj.generate(sizeof(vertex) * n_vertices);
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
			assert(_obj.mapped_buffer());

			_future.get();
			_obj.unmap();
			_valid = true;
		}
	} else if (!_valid) {
		fprintf(stderr, "mesh::render: mesh is not valid at this point\n");
		return;
	}

	assert(!_obj.mapped_buffer());
	tex.bind();
	_obj.render();
}

