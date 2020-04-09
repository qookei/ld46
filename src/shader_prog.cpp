#include "shader_prog.h"
#include <iostream>
#include <utils/mapped_file.h>

shader_prog::shader_prog() {
	_id = glCreateProgram();
}

shader_prog::~shader_prog() {
	if (_id)
		glDeleteProgram(_id);
}

shader_prog &shader_prog::bind_fragment_data_location(const char *name) {
	glBindFragDataLocation(_id, 0, name);

	return *this;
}

shader_prog &shader_prog::with_vertex_shader(const std::string &path) {
	if (GLuint s_id = with_shader(path, GL_VERTEX_SHADER); s_id) {
		glAttachShader(_id, s_id);
		_shaders.push_back(s_id);
	}

	return *this;
}

shader_prog &shader_prog::with_fragment_shader(const std::string &path) {
	if (GLuint s_id = with_shader(path, GL_FRAGMENT_SHADER); s_id) {
		glAttachShader(_id, s_id);
		_shaders.push_back(s_id);
	}

	return *this;
}

GLuint shader_prog::with_shader(const std::string &path, GLenum type) {
	GLuint shader_id = 0;

	mapped_file file{path};

	if (!file) {
		fprintf(stderr, "failed to load shader \"%s\"\n", path.c_str());
		return 0;
	}

	const char *c_str = static_cast<const char *>(file.data());
	int size = file.size();

	shader_id = glCreateShader(type);
	glShaderSource(shader_id, 1, &c_str, &size);
	glCompileShader(shader_id);

	return shader_id;
}

shader_prog &shader_prog::link() {
	glLinkProgram(_id);

	for (auto s : _shaders)
		glDeleteShader(s);

	return *this;
}

shader_prog &shader_prog::use() {
	glUseProgram(_id);
	return *this;
}

GLuint shader_prog::get_id() const {
	return _id;
}

GLint shader_prog::attribute_location(const std::string &name) {
	return glGetAttribLocation(_id, name.c_str());
}
