#pragma once

#include <mesh.h>
#include <texture.h>
#include <unordered_map>

#include <glm/vec3.hpp>

struct level {
	level(const std::string &path, shader_prog *prog);

	struct level_object {
		std::string tex_name;
		glm::vec2 pos;
		glm::vec2 size;
		glm::vec2 tex_pos;
		glm::vec2 tex_size;
		int times;
	};

	struct level_texture {
		texture tex;
		glm::vec2 size;
	};

	void render(glm::vec3 world_transform);

	level_object &object(const std::string &name) {
		return _objs[name];
	}

private:
	shader_prog *_prog;

	std::unordered_map<std::string, level_object> _objs;
	std::unordered_map<std::string, level_texture> _tex;

	std::unordered_map<std::string, mesh> _meshes;

	void generate_mesh();
};
