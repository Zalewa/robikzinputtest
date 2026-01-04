#pragma once

#include <imgui.h>

#include <functional>
#include <sstream>

namespace robikzinputtest::gui {

class Log;

class LogBox {
public:
	LogBox();

	void draw(
		Log &log,
		const char *title,
		bool *p_open = nullptr,
		float *opacity = nullptr,
		std::function<void()> extra_options_callback = [](){}
	);

private:
	ImGuiTextFilter m_filter;
	bool m_auto_scroll;
	std::stringstream m_clipboard;

	void draw_body(Log &log, float *opacity, std::function<void()> extra_options_callback);
};

} // namespace robikzinputtest::gui
