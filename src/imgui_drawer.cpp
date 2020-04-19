#include "imgui_drawer.h"
#include <SDL.h>

static const char *imgui_drawer_get_clipboard(void *) {
	static char *clipboard_ptr = nullptr;

	if (clipboard_ptr)
		SDL_free(clipboard_ptr);

	clipboard_ptr = SDL_GetClipboardText();
	return clipboard_ptr;
}

static void imgui_drawer_set_clipboard(void *, const char *text) {
	SDL_SetClipboardText(text);
}

imgui_drawer::imgui_drawer(shader_prog *prog, window *wnd)
:_mesh{prog}, _wnd{wnd}, _prog{prog}, _tex{} {
	_mesh.add_ebo();
	_mesh.gen_buffers();

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
	io.IniFilename = nullptr;

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

	io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
	io.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
	io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
	io.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
	io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

	io.SetClipboardTextFn = imgui_drawer_set_clipboard;
	io.GetClipboardTextFn = imgui_drawer_get_clipboard;
	io.ClipboardUserData = nullptr;
}

bool imgui_drawer::process_event(SDL_Event *ev) {
	auto &io = ImGui::GetIO();

	switch (ev->type) {
		case SDL_MOUSEWHEEL:
			io.MouseWheelH += ev->wheel.x;
			io.MouseWheel += ev->wheel.y;
			return io.WantCaptureMouse;
		case SDL_MOUSEBUTTONDOWN:
			return io.WantCaptureMouse;
		case SDL_TEXTINPUT:
			io.AddInputCharactersUTF8(ev->text.text);
			return io.WantCaptureKeyboard;

		case SDL_KEYDOWN:
		case SDL_KEYUP: {
			int key = ev->key.keysym.scancode;
			IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
			io.KeysDown[key] = (ev->type == SDL_KEYDOWN);
			io.KeyShift = (SDL_GetModState() & KMOD_SHIFT);
			io.KeyCtrl = (SDL_GetModState() & KMOD_CTRL);
			io.KeyAlt = (SDL_GetModState() & KMOD_ALT);
#ifdef _WIN32
			io.KeySuper = false;
#else
			io.KeySuper = (SDL_GetModState() & KMOD_GUI);
#endif
			return io.WantCaptureKeyboard;
		}
	}

	return false;
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

void imgui_drawer::render() {
	auto &io = ImGui::GetIO();

	static uint64_t frequency = SDL_GetPerformanceFrequency();
	static uint64_t last_time = 0;

	Uint64 current_time = SDL_GetPerformanceCounter();
	io.DeltaTime = last_time > 0 ? (float)((double)(current_time - last_time) / frequency) : (1.0f / 60.0f);
	last_time = current_time;

	auto draw_data = ImGui::GetDrawData();

	glm::mat4 ortho = glm::ortho(
		0.f, draw_data->DisplaySize.x,
		draw_data->DisplaySize.y, 0.f
	);
	glm::mat4 model = glm::mat4{1};

	_prog->use();
	_prog->set_uniform("ortho", ortho);
	_prog->set_uniform("model", model);
	glEnable(GL_SCISSOR_TEST);

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
				continue;
			}

			assert(reinterpret_cast<uintptr_t>(pcmd->TextureId) == _tex.get_id());

			static_assert(sizeof(ImDrawIdx) == 2);

			glm::ivec4 cr;
			cr.x = pcmd->ClipRect.x;
			cr.y = pcmd->ClipRect.y;
			cr.z = pcmd->ClipRect.z;
			cr.w = pcmd->ClipRect.w;

			int w = _wnd->get_width(), h = _wnd->get_height();

			if (cr.x > w || cr.y > h || cr.z < 0.f || cr.w < 0.f)
				continue;

			glScissor(cr.x, h - cr.w, cr.z - cr.x, cr.w - cr.y);
			_tex.bind();
			_mesh.vao()->bind();
			glDrawElementsBaseVertex(
				GL_TRIANGLES,
				static_cast<GLsizei>(pcmd->ElemCount),
				GL_UNSIGNED_SHORT,
				reinterpret_cast<void *>(
					pcmd->IdxOffset *
					sizeof(ImDrawIdx)),
				static_cast<GLint>(pcmd->VtxOffset));
			_mesh.vao()->unbind();
		}
	}

	glDisable(GL_SCISSOR_TEST);
}

