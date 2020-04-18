#include <iostream>
#include <window.h>
#include <shader_prog.h>
#include <texture.h>
#include <level.h>
#include <imgui_drawer.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>

#include <stdlib.h>
#include <time.h>

#include <type_traits>

#include <console.h>

#include <sprite.h>

#include <random>

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

static std::mt19937 mt19937(std::random_device{}());

template <typename T>
static T rng_between(T min, T max) {
	if constexpr (std::is_floating_point_v<T>) {
		std::uniform_real_distribution dist{min, max};
		return dist(mt19937);
	} else {
		static_assert(std::is_integral_v<T>);
		std::uniform_int_distribution dist{min, max};
		return dist(mt19937);
	}
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

	imgui_drawer _imgui_drawer{&prog, &_wnd};

	level level1{"res/level1.json", &prog};

	sprite bg1_sprite{"res/bg1.png", &prog, 1280, 720};

	int n = rng_between<int>(8, 32);

	std::vector<sprite> ufos;
	std::vector<glm::vec3> ufo_speeds;
	ufos.reserve(n);
	ufo_speeds.reserve(n);

	for (int i = 0; i < n; i++) {
		ufos.push_back(sprite{"res/tiles.png", &prog, 2048, 2048, 72, 68, 296, 473});
		auto &pos = ufos.back().position();
		pos = glm::vec3{
			rng_between<int>(0, 1) ? -72 : 1280,
			rng_between<int>(16, 128),
			0
		} + glm::vec3{rng_between<int>(-1000, 1000), 0, 0};

		while (pos.x < -72) pos.x += 1280;
		while (pos.x > 1280) pos.x -= 1280;

		ufo_speeds.push_back(glm::vec3{rng_between<int>(0, 1) ? 1 : -1, 0, 0});
	}


	glm::mat4 ortho = glm::ortho(0.f, 1280.f, 720.f, 0.f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double i = 0;

	bool do_screen_shake = false;
	bool draw_console = true;
	bool loop = true;
	while(loop) {
		glClearColor(0.364f, 0.737f, 0.823f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SDL_Event ev;
		while(SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				loop = false;
			if (_imgui_drawer.process_event(&ev))
				continue;
			if (ev.type == SDL_TEXTINPUT) {
				if (*ev.text.text == '~' || *ev.text.text == '`') {
					draw_console = !draw_console;
				}
				if (*ev.text.text == '1' || *ev.text.text == '!') {
					do_screen_shake = !do_screen_shake;
				}
			}

		}

		_imgui_drawer.update();

		prog.use();

		double mag = sin(i);

		glm::vec3 screen_shake_offset{0, 0, 0};
		if (do_screen_shake)
			screen_shake_offset = {rng_between<double>(-1, 1),
					rng_between<double>(-1, 1), 0};

		screen_shake_offset *= mag * 5;

		prog.set_uniform("ortho", ortho);
		bg1_sprite.render({0, 0, 0});
		level1.render(screen_shake_offset);
		for (int i = 0; i < n; i++) {
			ufos[i].render(screen_shake_offset);
			ufos[i].position() += ufo_speeds[i];

			auto &pos = ufos[i].position();
			if (pos.x < -72)
				pos.x = 1280;
			if (pos.x > 1280)
				pos.x = -72;
		}

		ImGui::NewFrame();

		i += 0.1;

		if (draw_console)
			console::get().draw(&draw_console);

		ImGui::Render();
		_imgui_drawer.render();

		ImGui::EndFrame();

		_wnd.swap();
	}

	return 0;
}
