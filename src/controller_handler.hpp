#pragma once

#include <SDL3/SDL.h>

namespace robikzinputtest {

class Controller;

class ControllerHandler {
public:
	virtual bool handle_event(
		Controller &controller,
		const SDL_Event &event
	) = 0;
};

class JoystickControllerHandler : public ControllerHandler {
public:
	bool handle_event(
		Controller &controller,
		const SDL_Event &event
	) override;
};

class KeyboardControllerHandler : public ControllerHandler {
public:
	bool handle_event(
		Controller &controller,
		const SDL_Event &event
	) override;
};

} // namespace robikzinputtest
