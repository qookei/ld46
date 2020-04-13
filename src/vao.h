#pragma once

#include <buffer.h>
#include <shader_prog.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define vertex_attr(name, elements, type, normalize, object, member) \
	gl_vao::attr_desc{name, elements, type, normalize, sizeof(object), offsetof(object, member)}

struct gl_vao {
	gl_vao() :_id{0} {}

	~gl_vao() {
		if (_id)
			glDeleteVertexArrays(1, &_id);
	}

	void generate() {
		glGenVertexArrays(1, &_id);
	}

	struct attr_desc {
		const std::string &name;
		size_t n_elements;
		GLenum type;
		bool normalize;
		size_t whole_field_size;
		size_t offset;
	};

	void set_attrib_pointer(shader_prog *prog, const attr_desc &desc) {
		GLuint attr_loc = prog->attribute_location(desc.name);

		glVertexAttribPointer(
			attr_loc,
			desc.n_elements,
			desc.type,
			desc.normalize,
			desc.whole_field_size,
			reinterpret_cast<void *>(desc.offset)
		);

		glEnableVertexAttribArray(attr_loc);
	}

	void bind() {
		assert(_id);
		glBindVertexArray(_id);
	}

	void unbind() {
		glBindVertexArray(0);
	}

	GLuint id() const {
		return _id;
	}

private:
	GLuint _id;
};
