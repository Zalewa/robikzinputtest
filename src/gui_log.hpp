#pragma once

#include "logger.hpp"
#include <imgui.h>
#include <sstream>

namespace robikzinputtest::gui {

class Log {
public:
	Log() {
		clear();
	}

	void add(const LogRecord &record) {
		std::ostringstream ss;
		ss << "[" << record.gui_frame << "] " << record.text;
		m_buffer.append(ss.str().c_str());
		m_record_offsets.push_back(m_buffer.size());
	}

	void clear() {
		m_buffer.clear();
		m_record_offsets.clear();
		m_record_offsets.push_back(0);
	}

	const ImGuiTextBuffer &buffer() const { return m_buffer; }
	const std::vector<int64_t> record_offsets() const { return m_record_offsets; }

private:
	ImGuiTextBuffer m_buffer;
	std::vector<int64_t> m_record_offsets;
};

} // namespace robikzinputtest::gui
