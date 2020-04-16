#pragma once

#include <cstdio>
#include <vector>
#include <stdarg.h>
#include <imgui.h>

struct console {
#if !defined(_WIN32) && !defined(_WIN64)
	constexpr static const char *format_reset = "\x1B[0m";
	constexpr static const char *format_warn = "\x1B[33m";
	constexpr static const char *format_err = "\x1B[31m";
	constexpr static const char *format_dbg = "\x1B[35m";
#else
	constexpr static const char *format_reset = "";
	constexpr static const char *format_warn = "";
	constexpr static const char *format_err = "";
	constexpr static const char *format_dbg = "";
#endif

	static void info(const char *format, ...) {
		va_list va;
		va_start(va, format);

		char buf[2048];
		int size = vsnprintf(buf, 2048, format, va);

		va_end(va);

		fprintf(stderr, "%s%.*s\n", format_reset, size, buf);
		get().push_info(std::string{buf, static_cast<size_t>(size)});
	}

	static void warn(const char *format, ...) {
		va_list va;
		va_start(va, format);

		char buf[2048];
		int size = vsnprintf(buf, 2048, format, va);

		va_end(va);

		fprintf(stderr, "%s%.*s%s\n", format_warn, size, buf, format_reset);
		get().push_warn(std::string{buf, static_cast<size_t>(size)});
	}

	static void err(const char *format, ...) {
		va_list va;
		va_start(va, format);

		char buf[2048];
		int size = vsnprintf(buf, 2048, format, va);

		va_end(va);

		fprintf(stderr, "%s%.*s%s\n", format_err, size, buf, format_reset);
		get().push_err(std::string{buf, static_cast<size_t>(size)});
	}

	static void dbg(const char *format, ...) {
		va_list va;
		va_start(va, format);

		char buf[2048];
		int size = vsnprintf(buf, 2048, format, va);

		va_end(va);

		fprintf(stderr, "%s%.*s%s\n", format_dbg, size, buf, format_reset);
		get().push_dbg(std::string{buf, static_cast<size_t>(size)});
	}

	static console &get() {
		static console _inst;
		return _inst;
	}

	void push_info(std::string msg) {
		_messages.push_back({log_message::category::info, msg});
	}

	void push_warn(std::string msg) {
		_messages.push_back({log_message::category::warn, msg});
	}

	void push_err(std::string msg) {
		_messages.push_back({log_message::category::err, msg});
	}

	void push_dbg(std::string msg) {
		_messages.push_back({log_message::category::dbg, msg});
	}

	void draw(bool *open) {
		ImGui::SetNextWindowSize(ImVec2(520,600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Debug console", open)) {
			ImGui::End();
			return;
		}

		ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1));

		for (auto &msg : _messages) {
			ImVec4 color;
			switch(msg.cat) {
				case log_message::category::info:
					color = {1.f, 1.f, 1.f, 1.f};
					break;
				case log_message::category::warn:
					color = {1.f, 1.f, .4f, 1.f};
					break;
				case log_message::category::err:
					color = {1.f, .4f, .4f, 1.f};
					break;
				case log_message::category::dbg:
					color = {1.f, .4f, 1.f, 1.f};
					break;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(msg.msg.c_str());
			ImGui::PopStyleColor();
		}

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);


		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::End();
	}

private:
	console() = default;

	struct log_message {
		enum class category {
			info,
			warn,
			err,
			dbg
		} cat;
		std::string msg;
	};

	std::vector<log_message> _messages;
};
