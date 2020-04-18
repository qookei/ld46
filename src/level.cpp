#include "level.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <console.h>
#include <glm/gtx/transform.hpp>

using json = nlohmann::json;

static glm::vec2 vec2_from_json(json j) {
	return {j[0].get<int>(), j[1].get<int>()};
}

level::level(const std::string &path, shader_prog *prog)
:_prog{prog}, _objs{}, _tex{} {

	std::ifstream i{path};
	if (!i.good()) {
		console::err("failed to open level '%s'", path.c_str());
		return;
	}

	json level_desc;
	i >> level_desc;

	for (auto [name, info] : level_desc["textures"].items()) {
		auto size = vec2_from_json(info["size"]);
		console::dbg("got texture '%s': path '%s' size %fx%f",
			name.c_str(), info["path"].get<std::string>().c_str(), size.x, size.y);
		_tex[name].size = size;
		_tex[name].tex = std::move(
			texture::load_from_file(
				info["path"].get<std::string>()));
	}

	for (auto [name, info] : level_desc["objects"].items()) {
		auto pos = vec2_from_json(info["pos"]);
		auto tex_pos = vec2_from_json(info["tex_pos"]);
		auto size = vec2_from_json(info["size"]);
		auto tex_size = vec2_from_json(info["tex_size"]);
		auto tex = info["tex"].get<std::string>();
		auto times = info.count("times") ? info["times"].get<int>() : 1;
		console::dbg("got object '%s': texture '%s' pos %fx%f tex %fx%f size %fx%f",
			name.c_str(), tex.c_str(), pos.x, pos.y, tex_pos.x, tex_pos.y, size.x, size.y);
		_objs.emplace(name, level_object{tex, pos, size, tex_pos, tex_size, times});
	}

	generate_mesh();
}

void level::render(glm::vec3 world_transform) {
	_prog->use();
	glm::mat4 model = glm::translate(world_transform);

	_prog->set_uniform("model", model);

	for (auto &[_tname, _m] : _meshes) {
		_m.render(_tex[_tname].tex);
	}
}

void level::generate_mesh() {
	std::unordered_map<std::string, std::vector<vertex>> _verts;
	for (auto &[_, obj] : _objs) {
		auto &tex = obj.tex_name;
		auto &tobj = _tex[tex];

		auto &vert_list = _verts[tex];

		auto obj_pos = obj.pos;

		for (int i = 0; i < obj.times; i++) {
			float u1 = obj.tex_pos.x / tobj.size.x;
			float v1 = obj.tex_pos.y / tobj.size.y;
			float u2 = u1 + obj.tex_size.x / tobj.size.x;
			float v2 = v1 + obj.tex_size.y / tobj.size.y;

			vertex _vertices[6];
			_vertices[0].pos = {obj_pos, 0};
			_vertices[0].uv = {u1, v1};
			_vertices[0].color = {1, 1, 1, 1};
			_vertices[1].pos = {obj_pos + obj.size, 0};
			_vertices[1].uv = {u2, v2};
			_vertices[1].color = {1, 1, 1, 1};
			_vertices[2].pos = {obj_pos.x, obj_pos.y + obj.size.y, 0};
			_vertices[2].uv = {u1, v2};
			_vertices[2].color = {1, 1, 1, 1};
			_vertices[3].pos = {obj_pos, 0};
			_vertices[3].uv = {u1, v1};
			_vertices[3].color = {1, 1, 1, 1};
			_vertices[4].pos = {obj_pos.x + obj.size.x, obj_pos.y, 0};
			_vertices[4].uv = {u2, v1};
			_vertices[4].color = {1, 1, 1, 1};
			_vertices[5].pos = {obj_pos + obj.size, 0};
			_vertices[5].uv = {u2, v2};
			_vertices[5].color = {1, 1, 1, 1};

			obj_pos.x += obj.size.x;

			for (int i = 0; i < 6; i++)
				vert_list.push_back(_vertices[i]);
		}
	}

	for (auto &[name, verts] : _verts) {
		auto it = _meshes.emplace(name, _prog).first;

		auto &_m = it->second;
		_m.gen_buffers();
		_m.gen_vao();
		_m.vbo()->store_regenerate(
			verts.data(),
			verts.size() * sizeof(vertex),
			GL_STATIC_DRAW);
		_m.mark_valid();
	}
}
