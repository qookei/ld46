#include "imgui_drawer.h"
#include <SDL.h>

imgui_drawer::imgui_drawer(shader_prog *prog, window *wnd)
:_mesh{prog} {
	_mesh.add_ebo();
	_mesh.gen_buffers();
	//_mesh.gen_vao();

	_mesh.vao()->bind();
	_mesh.vbo()->bind();
	_mesh.ebo()->bind();

	_mesh.vao()->set_attrib_pointer(prog, vertex_attr("vert_pos", 2, GL_FLOAT, GL_FALSE, ImDrawVert, pos));
	_mesh.vao()->set_attrib_pointer(prog, vertex_attr("tex_pos", 2, GL_FLOAT, GL_FALSE, ImDrawVert, uv));
	_mesh.vao()->set_attrib_pointer(prog, vertex_attr("color", 4, GL_UNSIGNED_BYTE, GL_TRUE, ImDrawVert, col));

	_mesh.vao()->unbind();
	_mesh.vbo()->unbind();
	_mesh.ebo()->unbind();


	_mesh.mark_valid();

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

	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
}

void imgui_drawer::process_event(SDL_Event *ev) {
	auto &io = ImGui::GetIO();

	switch (ev->type) {
		case SDL_MOUSEWHEEL:
			io.MouseWheelH += ev->wheel.x;
			io.MouseWheel += ev->wheel.y;
			break;
	}
}

void imgui_drawer::update() {
	auto &io = ImGui::GetIO();

	int x, y;
	uint32_t buttons = SDL_GetMouseState(&x, &y);

	io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
	io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
	io.MouseDown[2] = buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE);

	io.MousePos = ImVec2{static_cast<float>(x), static_cast<float>(y)};
}

static glm::vec4 rgba_to_vec(uint32_t rgba) {
	float r, g, b, a;
	r = static_cast<float>((rgba >> 0) & 255) / 255.f;
	g = static_cast<float>((rgba >> 8) & 255) / 255.f;
	b = static_cast<float>((rgba >> 16) & 255) / 255.f;
	a = static_cast<float>((rgba >> 24) & 255) / 255.f;

	return {r, g, b, a};
}

void imgui_drawer::render() {
	auto &io = ImGui::GetIO();

	static uint64_t frequency = SDL_GetPerformanceFrequency();
	static uint64_t last_time = 0;

	Uint64 current_time = SDL_GetPerformanceCounter();
	io.DeltaTime = last_time > 0 ? (float)((double)(current_time - last_time) / frequency) : (1.0f / 60.0f);
	last_time = current_time;

	auto draw_data = ImGui::GetDrawData();

	for (int i = 0; i < draw_data->CmdListsCount; i++) {
		auto cmd_list = draw_data->CmdLists[i];

		_mesh.vbo()->store_regenerate(
			cmd_list->VtxBuffer.Data,
			cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
			GL_STREAM_DRAW);

		_mesh.ebo()->store_regenerate(
			cmd_list->IdxBuffer.Data,
			cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
			GL_STREAM_DRAW);

		for (int j = 0; j < cmd_list->CmdBuffer.Size; j++) {
			auto pcmd = &cmd_list->CmdBuffer[j];

			if (pcmd->UserCallback) {
				if (pcmd->UserCallback != ImDrawCallback_ResetRenderState)
					pcmd->UserCallback(cmd_list, pcmd);
				else
					fprintf(stderr, "imgui_drawer::render: user callback wants us to reset the state?\n");
				continue;
			}

			assert(reinterpret_cast<uintptr_t>(pcmd->TextureId) == _tex.get_id());

			static_assert(sizeof(ImDrawIdx) == 2);

			_tex.bind();
			_mesh.vao()->bind();
			glDrawElementsBaseVertex(
				GL_TRIANGLES,
				static_cast<GLsizei>(pcmd->ElemCount),
				GL_UNSIGNED_SHORT
				reinterpret_cast<void *>(
					pcmd->IdxOffset *
					sizeof(ImDrawIdx)),
				static_cast<GLint>(pcmd->VtxOffset));
			_mesh.vao()->unbind();
		}
	}

	_mesh.render(_tex);
}

