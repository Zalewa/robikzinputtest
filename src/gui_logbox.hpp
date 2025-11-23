#pragma once

#include <imgui.h>

namespace robikzinputtest::gui {

class Log;

class LogBox {
public:
	LogBox();

	void draw(Log &log, const char *title, bool *p_open = nullptr);

private:
	ImGuiTextFilter m_filter;
	bool m_auto_scroll;
};

} // namespace robikzinputtest::gui
