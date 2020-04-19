#pragma once

#include <vector>
#include <memory>

#include <epoxy/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct shader_prog {
	shader_prog();

	shader_prog(const shader_prog &) = delete;
	shader_prog &operator=(const shader_prog &) = delete;

	~shader_prog();

	shader_prog &with_vertex_shader(const std::string &path);
	shader_prog &with_fragment_shader(const std::string &path);

	shader_prog &bind_fragment_data_location(const char *name);
	shader_prog &link();
	shader_prog &use();

	GLuint get_id() const;

	GLint attribute_location(const std::string &name);

	void set_uniform(const std::string &name, glm::mat4 val) {
		auto loc = glGetUniformLocation(_id, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
	}

	void set_uniform(const std::string &name, glm::vec4 val) {
		auto loc = glGetUniformLocation(_id, name.c_str());
		glUniform4fv(loc, 1, glm::value_ptr(val));
	}
private:

	GLuint with_shader(const std::string &path, GLenum type);

	GLuint _id;
	std::vector<GLuint> _shaders;
};
