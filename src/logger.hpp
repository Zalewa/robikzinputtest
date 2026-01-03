#pragma once

#include "callback_stream.hpp"
#include "handler.hpp"

namespace robikzinputtest {

struct LogRecord {
	std::string text;
	int64_t gui_frame;
};

using LogCallback = std::function<void(const LogRecord &)>;

class Logger {
public:
	Logger();

	Handler<LogRecord> on_logrecord;

	std::ostream &info() { return m_stream; }
	std::ostream &error() { return m_stream; }

private:
	CallbackOstream m_stream;

	void accept_text(const std::string &text);
};

} // namespace robikzinputtest
