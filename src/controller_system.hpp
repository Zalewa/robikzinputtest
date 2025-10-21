#pragma once

#include <SDL3/SDL_events.h>
#include <memory>

namespace robikzinputtest {

class Controller;
struct ControllerId;

class ControllerSystem {
public:
	ControllerSystem();
	~ControllerSystem();

	std::shared_ptr<Controller> find_controller_by_id(const ControllerId &id);

	Controller &for_joystick(SDL_JoystickID which);
	Controller &for_keyboard();

	bool handle_event(const SDL_Event &event);

private:
	struct D;
	std::unique_ptr<D> d;
};

} // namespace robikzinputtest
