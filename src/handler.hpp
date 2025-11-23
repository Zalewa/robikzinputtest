#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <vector>

namespace robikzinputtest {

using HandlerId = int32_t;

template<typename... Args>
class Handler {
public:
	using Fn = std::function<void(Args...)>;

	HandlerId add(Fn fn) {
		auto handler_id = m_next_id++;
		m_fns.emplace_back(handler_id, std::move(fn));
		return handler_id;
	}

	void remove(HandlerId handler_id) {
		m_fns.erase(
			std::remove_if(
				m_fns.begin(), m_fns.end(),
				[&](auto &p){ return p.first == handler_id; }
			),
			m_fns.end()
		);
	}

	void operator()(Args... args) {
		for (auto &fn : m_fns)
			fn.second(args...);
	}

private:
	HandlerId m_next_id = 1;
	std::vector<std::pair<HandlerId, Fn>> m_fns;
};

}
