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
		bool enabled;
	};

	struct level_texture {
		texture tex;
		glm::vec2 size;
	};

	void render(glm::vec3 world_transform);

	void swap_object_with_replacement(const std::string &name) {
		using std::swap;
		swap(_replac[name], _objs[name]);
		generate_mesh();
	}

	void remove_object(const std::string &name) {
		_objs.erase(name);
		generate_mesh();
	}

	level_object &object(const std::string &name) {
		return _objs[name];
	}

	void generate_mesh();

private:
	shader_prog *_prog;

	std::unordered_map<std::string, level_object> _objs;
	std::unordered_map<std::string, level_object> _replac;
	std::unordered_map<std::string, level_texture> _tex;

	std::unordered_map<std::string, mesh> _meshes;
};
