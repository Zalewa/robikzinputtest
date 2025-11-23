#pragma once

#include <functional>
#include <sstream>
#include <string>

namespace robikzinputtest {

class CallbackBuffer : public std::stringbuf {
public:
	using callback_t = std::function<void(const std::string&)>;

	explicit CallbackBuffer(callback_t cb) : callback(std::move(cb)) {}

protected:
	int sync() override {
		// Text accumulated to far.
		std::string data = str();
		if (!data.empty()) {
			// Deliver and clear the buffer.
			callback(data);
			str("");
		}
		return 0;
	}

private:
	callback_t callback;
};

class CallbackOstream : public std::ostream {
public:
	explicit CallbackOstream(CallbackBuffer::callback_t cb)
		: std::ostream(nullptr),
		  buf(std::move(cb)) {
		rdbuf(&buf);
	}

private:
	CallbackBuffer buf;
};

} // namespace robikzinputtest
