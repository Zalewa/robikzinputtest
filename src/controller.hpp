#pragma once

#include "controller_handler.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <string>

namespace robikzinputtest {

struct ControllerId {
	enum Type {
		TYPE_NONE,
		TYPE_KEYBOARD,
		TYPE_MOUSE,
		TYPE_JOY,
	};

	Type type = TYPE_NONE;
	std::string identifier;
	uint32_t index;

	bool operator==(const ControllerId &other) const {
		return (type == other.type)
			&& (identifier == other.identifier)
			&& (index == other.index);
	}

	bool operator!=(const ControllerId &other) const {
		return !(*this == other);
	}
};

enum class ButtonState {
	CLEAR,
	PRESSED,
	RELEASED,
};

struct ControllerState {
	SDL_FPoint direction_vec2 = { 0.0f, 0.0f };
	ButtonState button_primary = ButtonState::CLEAR;
};

class Controller {
public:
	const ControllerId id;
	ControllerState state;

	Controller(const ControllerId &controller_id)
		: id(controller_id) {}

	bool handle_event(const SDL_Event &event) {
		if (m_handler) {
			return m_handler->handle_event(*this, event);
		}
		return false;
	}

	void set_handler(std::shared_ptr<ControllerHandler> handler) {
		m_handler = handler;
		this->state = ControllerState();
	}

private:
	std::shared_ptr<ControllerHandler> m_handler;
};

} // namespace robikzinputtest
