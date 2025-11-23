#pragma once

#include "gui_logbox.hpp"

namespace robikzinputtest::gui {

struct GuiContext;
class Log;

class WindowProgramLog {
public:
	WindowProgramLog(Log &log) : m_log(log) {}

	void draw(const GuiContext &guictx);

private:
	Log &m_log;
	LogBox m_logbox;
};

} // namespace robikzinputtest::gui
