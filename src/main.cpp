#include <iostream>
#include <window.h>
#include <shader_prog.h>
#include <texture.h>
#include <level.h>
#include <imgui_drawer.h>
#include <build_tile_view.h>

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
#include <chrono>

#include <SDL2/SDL_mixer.h>

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

bool is_hard_mode = false;

struct missile {
	glm::vec3 position;
	glm::vec3 velocity;
	float angle;
	float d = 2.f;
	float speed = rng_between<float>(1.8f, 2.2f)
		* (is_hard_mode ? 2 : 1);

	int i = 0;
	constexpr static int n = 50;

	bool do_remove = false;

	void render(glm::vec3 world_transform, sprite &sp) {
		sp.position() = position + glm::vec3{12, 20, 0};
		sp.render(world_transform, angle, true);
	}

	void update() {
		auto tx = 640 - position.x;
		auto ty = -(362 - position.y);
		angle = std::atan2(tx, ty);
		velocity *= 0.1;
		if (i++ > n) {
			d = rng_between<float>(1.8f, 2.2f);
			i = 0;
		}
		velocity += glm::vec3{std::cos(angle - M_PI / d) * speed,
			std::sin(angle - M_PI / d) * speed, 0};
		position += velocity;
	}
};

extern "C" int main(int, char **) {
	using namespace std::literals::chrono_literals;
	window _wnd;

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512) < 0)
		window::report_fatal("main: failed to open mixer: %s", SDL_GetError());

	if (Mix_AllocateChannels(4) < 0)
		window::report_fatal("main: failed to allocate channels: %s", SDL_GetError());

	Mix_Chunk *explosion_sound, *death_sound;
	explosion_sound = Mix_LoadWAV("res/explosion.wav");
	death_sound = Mix_LoadWAV("res/death.wav");

	if (!explosion_sound)
		window::report_fatal("main: failed to open explosion sound: %s", SDL_GetError());

	if (!death_sound)
		window::report_fatal("main: failed to open death sound: %s", SDL_GetError());

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
	sprite bg2_sprite{"res/bg2.png", &prog, 1280, 720};
	sprite cur_sprite{"res/cur.png", &prog, 32, 32};
	sprite gameover_sprite{"res/gameover.png", &prog, 643, 99};
	sprite toexit_sprite{"res/toexit.png", &prog, 957, 237};
	sprite title_sprite{"res/title.png", &prog, 624, 104};
	sprite info_sprite{"res/info.png", &prog, 1081, 227};
	sprite howto_sprite{"res/howto.png", &prog, 885, 178};
	sprite rain_sprite{"res/rain.png", &prog, 200, 210, 2000, 2000, 0, 0};
	auto base_pos = rain_sprite.position() = {-500, -500, 0};

	gameover_sprite.position() = {318, 310, 0};
	toexit_sprite.position() = {161, 410, 0};

	title_sprite.position() = {328, 100, 0};
	info_sprite.position() = {99, 220, 0};
	howto_sprite.position() = {10, 532, 0};

	sprite ufo{"res/tiles.png", &prog, 888, 780, 72, 68, 296, 473};
	sprite missile_spr{"res/missile.png", &prog, 24, 56, 24, 56, 0, 0};
	sprite crate_spr{"res/tiles.png", &prog, 888, 780, 32, 32, 485, 448};
	std::vector<glm::vec3> ufos;
	std::vector<glm::vec3> ufo_speeds;

	std::vector<missile> missiles;

	glm::vec3 crate_pos = {-32, -32, 0};
	bool crate_visible = false;

	build_tile_view tiles{&prog};

	for (int x = 0; x < build_tile_view::width; x++) {
		for (int y = 0; y < build_tile_view::height; y++) {
			if (tiles.is_valid_spot(x, y))
				tiles.set(x, y, false);
		}
	}

	tiles.upload_texture();

	auto add_ufo = [&](){
		auto pos = glm::vec3{
			rng_between<int>(0, 1) ? -72 : 1280,
			rng_between<int>(16, 64),
			0
		} + glm::vec3{rng_between<int>(-1000, 1000), 0, 0};

		while (pos.x < -72) pos.x += 1280;
		while (pos.x > 1280) pos.x -= 1280;

		ufos.push_back(pos);
		ufo_speeds.push_back(glm::vec3{rng_between<int>(0, 1) ? 2 : -2, 0, 0});
	};

	add_ufo();

	glm::mat4 ortho = glm::ortho(0.f, 1280.f, 720.f, 0.f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double screenshake_time = 0;
	double magnitude = 0;

	auto trigger_screenshake = [&](double mag){
		screenshake_time = M_PI / 4;
		magnitude = mag + magnitude;
	};

	auto break_around = [&](int x, int y, int r){
		trigger_screenshake(1);
		Mix_PlayChannel(-1, explosion_sound, 0);
		for (int px = x - r; px <= x + r; px++) {
			for (int py = y - r; py <= y + r; py++) {
				auto dist = std::hypot(px - x, py - y)
					+ rng_between<int>(0, 4);
				if (dist < r && tiles.is_valid_spot(px, py)) {
					tiles.set(px, py, false);
				}
			}
		}
		tiles.upload_texture();
	};

	auto check_rect = [&](int x, int y, int side) -> int {
		int n = 0;
		for (int px = x; px < x + side; px++) {
			for (int py = y; py < y + side; py++) {
				if (!tiles.is_valid_spot(px, py))
					return 200000000;
				if (tiles.get(px, py))
					n++;
			}
		}

		return n;
	};

	auto check_circle = [&](int x, int y, int r) -> glm::vec2 {
		for (int px = x - r; px <= x + r; px++) {
			for (int py = y - r; py <= y + r; py++) {
				auto dist = std::hypot(px - x, py - y);
				if (dist < r && 
					tiles.is_valid_spot(px, py) &&
					tiles.get(px, py)) {
					return {px, py};
				}
			}
		}

		return {-1, -1};
	};

	constexpr int block_size_min = 10;
	constexpr int block_size_max = 20;
	int next_size = rng_between<int>(block_size_min, block_size_max) * 2;

	auto place_rect = [&](int x, int y, int side) -> bool {
		if (int n = check_rect(x, y, next_size); n > (next_size * next_size * 2) / 3)
			return false;
		for (int px = x; px < x + side; px++) {
			for (int py = y; py < y + side; py++) {
				if (tiles.is_valid_spot(px, py)) {
					tiles.set(px, py, true);
				}
			}
		}

		tiles.upload_texture();

		return true;
	};

	glm::vec4 cursor_color = {1,1,1,1};

	int hp = 2;
	int blocks = 5;

	float rate = 0;
	auto time1 = std::chrono::high_resolution_clock::now();
	auto time2 = std::chrono::high_resolution_clock::now();
	auto time3 = std::chrono::high_resolution_clock::now();
	auto time4 = std::chrono::high_resolution_clock::now();
	bool in_menu = true;
	bool is_main_menu = true;
	bool is_game_over = false;
	bool draw_console = false;
	bool loop = true;

	auto reset_state = [&]() {
		rate = 0;
		hp = 2;
		blocks = 5;
		time1 = std::chrono::high_resolution_clock::now();
		time2 = std::chrono::high_resolution_clock::now();
		time3 = std::chrono::high_resolution_clock::now();
		time4 = std::chrono::high_resolution_clock::now();

		for (int x = 0; x < build_tile_view::width; x++) {
			for (int y = 0; y < build_tile_view::height; y++) {
				if (tiles.is_valid_spot(x, y))
					tiles.set(x, y, false);
			}
		}

		tiles.upload_texture();

		level1.object("egg").enabled = true;
		level1.swap_object_with_replacement("egg");
		level1.object("egg").enabled = true;

		ufos.clear();
		ufo_speeds.clear();
		missiles.clear();

		add_ufo();

		crate_visible = false;
		is_game_over = false;
	};

	while(loop) {
		glClearColor(0.63921f, 0.97647f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SDL_Event ev;
		while(SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				loop = false;
			if (_imgui_drawer.process_event(&ev))
				continue;
			if (ev.type == SDL_MOUSEBUTTONDOWN && !in_menu) {
				int x = ev.button.x - 72 - next_size / 2,
					y = ev.button.y - 140 - next_size / 2;
				bool h = true;
				if (crate_visible && ev.button.x >= crate_pos.x
					&& ev.button.x < crate_pos.x + 32
					&& ev.button.y >= crate_pos.y
					&& ev.button.y < crate_pos.y + 32) {
					blocks += rng_between<int>(5, 15);
					crate_visible = false;
					h = false;
				} else if (tiles.is_valid_spot(x, y) && blocks) {
					if (place_rect(x, y, next_size))
						blocks--;
				}

				if (h)
					next_size = rng_between<int>(block_size_min, block_size_max) * 2;
			} else if (ev.type == SDL_MOUSEBUTTONDOWN && is_main_menu) {
				is_hard_mode = ev.button.button == SDL_BUTTON_RIGHT;
				console::dbg("is hard mode? %s", is_hard_mode ? "yes" : "no");
				in_menu = false;
				is_main_menu = false;

				time1 = std::chrono::high_resolution_clock::now();
				time2 = std::chrono::high_resolution_clock::now();
				time3 = std::chrono::high_resolution_clock::now();
				time4 = std::chrono::high_resolution_clock::now();
			} else if (ev.type == SDL_MOUSEBUTTONDOWN && is_game_over) {
				console::dbg("here");
				reset_state();
				in_menu = is_main_menu = ev.button.button == SDL_BUTTON_RIGHT;
			}
			if (ev.type == SDL_MOUSEMOTION && !in_menu) {
				int x = ev.motion.x - 16, y = ev.motion.y - 16;

				cur_sprite.position() = {x, y, 0};
				if (!blocks)
					cursor_color = {1,0,1,1};
				else if (int n = check_rect(x - 72, y - 140, next_size); n > (next_size * next_size * 2) / 3)
					cursor_color = {1,0,0,1};
				else
					cursor_color = {1,1,1,1};
			}
		}

		_imgui_drawer.update();

		prog.use();

		double mag = sin(screenshake_time) * magnitude;

		glm::vec3 screen_shake_offset{0, 0, 0};
		screen_shake_offset = {rng_between<double>(-1, 1),
					rng_between<double>(-1, 1), 0};

		screen_shake_offset *= mag;

		prog.set_uniform("ortho", ortho);
		prog.set_uniform("model_color", glm::vec4({1,1,1,1}));

		bg1_sprite.render({0,0,0});
		level1.render(screen_shake_offset);
		tiles.render();
		for (size_t i = 0; i < ufos.size(); i++) {
			ufo.position() = ufos[i];
			ufo.render(screen_shake_offset);
			if (!in_menu)
				ufos[i] += ufo_speeds[i] * glm::vec3{(is_hard_mode ? 2 : 1), 0, 0};

			auto &pos = ufos[i];
			if (pos.x < -72)
				pos.x = 1280;
			if (pos.x > 1280)
				pos.x = -72;
		}

		auto now = std::chrono::high_resolution_clock::now();

		if ((now - time4 >= 4s) && !in_menu && rng_between<float>(1, 100) > (is_hard_mode ? 98.f : 99.f) - rate) {
			missiles.push_back(missile{ufos[rng_between<size_t>(0, ufos.size() - 1)], {}, {}});
		}

		for (auto &_missile : missiles) {
			_missile.render(screen_shake_offset, missile_spr);

			if (!in_menu) {
				_missile.update();

				int x = _missile.position.x - 72;
				int y = _missile.position.y - 140;

				if (auto impact = check_circle(x, y, 10); impact != glm::vec2{-1, -1}) {
					break_around(impact.x, impact.y, rng_between<int>(16, 24) * (is_hard_mode ? 2 : 1));
					_missile.do_remove = true;
				}

				if (std::hypot(x + 72 - 640, y + 140 - 362) < 40) {
					// hit
					hp--;
					if (hp == 1) {
						Mix_PlayChannel(-1, explosion_sound, 0);
						level1.swap_object_with_replacement("egg");
						_missile.do_remove = true;
					}

					if (!hp) {
						Mix_PlayChannel(-1, death_sound, 0);
						level1.object("egg").enabled = false;
						level1.generate_mesh();
						is_game_over = true;
						in_menu = true;
					}
				}
			}
		}

		size_t i = 0;
		while (i < missiles.size()) {
			if (missiles[i].do_remove)
				missiles.erase(missiles.begin() + i);
			else
				i++;
		}

		if (crate_visible) {
			crate_spr.position() = crate_pos;
			crate_spr.render(screen_shake_offset);
			if (!in_menu)
				crate_pos += glm::vec3{0, 2, 0};

			if (crate_pos.y >= 720)
				crate_visible = false;
		}

		if (!in_menu) {
			if (now - time1 >= 10s) {
				add_ufo();
				time1 = now;
			}

			if (now - time2 >= 2s && !crate_visible) {
				if (rng_between<int>(1, 100) > 40) {
					crate_pos = {
						rng_between<int>(64, 1216),
						-32, 0};
					crate_visible = true;
				}
				time2 = now;
			} else if (crate_visible) {
				time2 = now;
			}

			if (now - time3 >= 6s) {
				rate += 0.2f;
				time3 = now;
			}
		}

		rain_sprite.render({0,0,0});
		rain_sprite.position() += glm::vec3{-6.66666, 7, 0};

		auto dr = base_pos - rain_sprite.position();
		if (std::round(dr.x) == 200 && std::round(dr.y) == -210) {
			rain_sprite.position() = base_pos;
		}

		prog.set_uniform("model_color", cursor_color);
		cur_sprite.render(screen_shake_offset);

		prog.set_uniform("model_color", glm::vec4({1,1,1,1}));

		if (in_menu)
			bg2_sprite.render({0,0,0});

		if (is_game_over) {
			gameover_sprite.render({0,0,0});
			toexit_sprite.render({0,0,0});
		}

		if (is_main_menu) {
			title_sprite.render({0,0,0});
			info_sprite.render({0,0,0});
			howto_sprite.render({0,0,0});
		}

		ImGui::NewFrame();

		if (screenshake_time > 0)
			screenshake_time += 0.1;
		if (screenshake_time > M_PI) {
			magnitude = 0;
			screenshake_time = 0;
		}

		if (draw_console)
			console::get().draw(&draw_console);

		ImGui::Render();
		_imgui_drawer.render();

		ImGui::EndFrame();

		_wnd.swap();
	}

	Mix_FreeChunk(explosion_sound);
	Mix_FreeChunk(death_sound);
	Mix_CloseAudio();

	return 0;
}
