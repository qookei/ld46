#include "imgui_drawer.h"
#include <SDL.h>

imgui_drawer::imgui_drawer(shader_prog *prog, window *wnd)
:_mesh{prog, 0} {
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	auto &io = ImGui::GetIO();

	io.DisplaySize = ImVec2{
		static_cast<float>(wnd->get_width()),
		static_cast<float>(wnd->get_height())
	};

	io.DisplayFramebufferScale = ImVec2{1.0f, 1.0f};

	uint8_t *pixel_data;
	int t_width, t_height;
	io.Fonts->GetTexDataAsRGBA32(&pixel_data, &t_width, &t_height);

	_tex.load(pixel_data, t_width, t_height);

	io.Fonts->TexID = reinterpret_cast<ImTextureID>(_tex.get_id());
}

static size_t calc_n_verts(ImDrawData *draw_data) {
	size_t total = 0;

	for (int i = 0; i < draw_data->CmdListsCount; i++) {
		auto cmd_list = draw_data->CmdLists[i];

		for (int j = 0; j < cmd_list->CmdBuffer.Size; j++) {
			auto pcmd = &cmd_list->CmdBuffer[j];

			if (!pcmd->UserCallback) {
				total += pcmd->ElemCount;
			}
		}
	}

	return total;
}

static glm::vec4 rgba_to_vec(uint32_t rgba) {
	float r, g, b, a;
	r = static_cast<float>(rgba >> 24) / 255.f;
	g = static_cast<float>((rgba >> 16) & 255) / 255.f;
	b = static_cast<float>((rgba >> 8) & 255) / 255.f;
	a = static_cast<float>(rgba & 255) / 255.f;

	return {r, g, b, a};
}

size_t imgui_drawer::generate_mesh(ImDrawData *draw_data, vertex *verts) {
	size_t total = 0;

	for (int i = 0; i < draw_data->CmdListsCount; i++) {
		auto cmd_list = draw_data->CmdLists[i];

		for (int j = 0; j < cmd_list->CmdBuffer.Size; j++) {
			auto pcmd = &cmd_list->CmdBuffer[j];

			if (pcmd->UserCallback)
				continue;

			assert(reinterpret_cast<uintptr_t>(pcmd->TextureId) == _tex.get_id());
			for (size_t n = 0; n < pcmd->ElemCount; n++) {
				vertex &v = verts[n];
				ImDrawVert &iv = cmd_list->VtxBuffer.Data[cmd_list->IdxBuffer.Data[pcmd->IdxOffset + n]];
				v.pos = {iv.pos.x, iv.pos.y, 0.f};
				v.uv = {iv.uv.x, iv.uv.y};
				v.color = rgba_to_vec(iv.col);
			}

			verts += pcmd->ElemCount;
			total += pcmd->ElemCount;
		}
	}

	return total;
}

void imgui_drawer::update_mesh() {
	size_t n_verts = calc_n_verts(ImGui::GetDrawData());

	_mesh.resize(n_verts);
	_mesh.update_sync(&imgui_drawer::generate_mesh, this, ImGui::GetDrawData());
}

void imgui_drawer::render() {
	auto &io = ImGui::GetIO();

	static uint64_t frequency = SDL_GetPerformanceFrequency();
	static uint64_t last_time = 0;

	Uint64 current_time = SDL_GetPerformanceCounter();
	io.DeltaTime = last_time > 0 ? (float)((double)(current_time - last_time) / frequency) : (1.0f / 60.0f);
	last_time = current_time;

	_mesh.render(_tex);
}

