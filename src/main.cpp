#include <iostream>
#include <window.h>
#include <shader_prog.h>
#include <vertex_object.h>
#include <texture.h>
#include <chunk_view.h>
#include <imgui_drawer.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>

#include <stdlib.h>
#include <time.h>

#include <type_traits>

void gl_debug_callback(GLenum source, GLenum type, GLuint, GLenum,
		GLsizei length, const char *message, const void *) {

	const char *src = "?";
	switch (source) {
		case GL_DEBUG_SOURCE_API: src = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: src = "window system"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: src = "shader compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: src = "third party"; break;
		case GL_DEBUG_SOURCE_APPLICATION: src = "app"; break;
		case GL_DEBUG_SOURCE_OTHER: src = "other"; break;
	}

	const char *type_str = "?";
	switch (type) {
		case GL_DEBUG_TYPE_ERROR: type_str = "error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "deprecated behavior"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type_str = "undefined behavior"; break;
		case GL_DEBUG_TYPE_PORTABILITY: type_str = "portability"; break;
		case GL_DEBUG_TYPE_MARKER: type_str = "marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP: type_str = "push group"; break;
		case GL_DEBUG_TYPE_POP_GROUP: type_str = "pop group"; break;
		case GL_DEBUG_TYPE_OTHER: type_str = "other"; break;
	}

	fprintf(stderr, "debug:type: %s, source: %s, message: \"%.*s\"\n", type_str, src, length, message);

	if (type == GL_DEBUG_TYPE_ERROR)
		window::report_fatal("GL error: source: %s, message: \"%.*s\"", src, length, message);
}

int main(int argc, char *argv[]) {
	window _wnd;

	glEnable(GL_DEBUG_OUTPUT);
	//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // disable if in release
	glDebugMessageCallback(gl_debug_callback, nullptr);
	glDebugMessageControl(GL_DEBUG_SOURCE_SHADER_COMPILER, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

	printf("opengl ver is %d\n", epoxy_gl_version());

	shader_prog prog;
	prog.with_vertex_shader("res/vertex.glsl");
	prog.with_fragment_shader("res/fragment.glsl");
	prog.link();

	int tiles1[chunk_view::width * chunk_view::height];
	int tiles2[chunk_view::width * chunk_view::height];

	srand(time(nullptr));

	for (size_t i = 0; i < sizeof(tiles1) / sizeof(*tiles1); i++) {
		int n = static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * 16;
		tiles1[i] = n;
		n = static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * 16;
		tiles2[i] = n;
	}

	imgui_drawer _imgui_drawer{&prog, &_wnd};

	auto view1 = std::make_unique<chunk_view>(&prog, tiles1);
	auto view2 = std::make_unique<chunk_view>(&prog, tiles2);
	view1->update_mesh();
	view2->update_mesh();

	glm::mat4 ortho = glm::ortho(0.f, 1280.f, 720.f, 0.f);

	texture tex{};
	tex.load("res/tiles.png");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	bool loop = true;
	while(loop) {
		glClearColor(0.364f, 0.737f, 0.823f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SDL_Event ev;
		while(SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				loop = false;
		}

		prog.use();

		glm::mat4 model = glm::translate(glm::vec3{0.f, 0.f, 0.f});

		prog.set_uniform("ortho", ortho);
		prog.set_uniform("model", model);
		//prog.set_uniform("tex", 0);
		view1->render(tex);
		model = glm::translate(model, glm::vec3{512,0,0});
		prog.set_uniform("model", model);
		view2->render(tex);

		ImGui::NewFrame();

		bool foo;
		ImGui::ShowDemoWindow(&foo);

		ImGui::Render();
		_imgui_drawer.update_mesh();
		_imgui_drawer.render();

		_wnd.swap();
	}

	return 0;
}
