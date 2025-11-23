#include "logger.hpp"

#include <imgui.h>

namespace robikzinputtest {

Logger::Logger()
	: m_stream([&](const std::string &text) { this->accept_text(text); }) {}

void Logger::accept_text(const std::string &text) {
	LogRecord record;
	record.text = text;
	record.gui_frame = ImGui::GetFrameCount();
	this->on_logrecord(record);
}

} // namespace robikzinputtest
