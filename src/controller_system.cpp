#include "controller_system.hpp"

#include "controller.hpp"
#include <map>
#include <sstream>

namespace robikzinputtest {

struct ControllerSystem::D {
	std::shared_ptr<Controller> m_keyboard_controller;
	std::map<SDL_JoystickID, std::shared_ptr<Controller>> m_joystick_controllers;
};

ControllerSystem::ControllerSystem()
	: d(std::make_unique<D>()) {
	// Initialize keyboard
	const ControllerId controller_id = {
		.type = ControllerId::TYPE_KEYBOARD,
		.identifier = "default_keyboard",
		.index = 0,
	};
	d->m_keyboard_controller = std::make_shared<Controller>(controller_id);
	d->m_keyboard_controller->set_handler(std::make_shared<KeyboardControllerHandler>());
}

ControllerSystem::~ControllerSystem() = default;

std::shared_ptr<Controller> ControllerSystem::find_controller_by_id(
	const ControllerId &id
) {
	if (d->m_keyboard_controller->id == id) {
		return d->m_keyboard_controller;
	}

	for (auto it : d->m_joystick_controllers) {
		if (it.second->id == id) {
			return it.second;
		}
	}
	return nullptr;
}

Controller &ControllerSystem::for_joystick(SDL_JoystickID which) {
	auto it = d->m_joystick_controllers.find(which);
	if (it == d->m_joystick_controllers.end()) {
		std::stringstream ss;
		ss << "joystick_" << which;
		const ControllerId controller_id = {
			.type = ControllerId::TYPE_JOY,
			.identifier = ss.str(),
			.index = which,
		};
		auto joystick_controller = std::make_shared<Controller>(controller_id);
		joystick_controller->set_handler(std::make_shared<JoystickControllerHandler>());
		it = d->m_joystick_controllers.insert({which, joystick_controller}).first;
	}
	return *it->second;
}

Controller &ControllerSystem::for_keyboard() {
	return *d->m_keyboard_controller;
}

bool ControllerSystem::handle_event(const SDL_Event &event) {
	// Pass the event to all controllers
	bool handled = false;
	if (d->m_keyboard_controller->handle_event(event))
		return true;
	for (auto it : d->m_joystick_controllers) {
		if (it.second->handle_event(event))
			return true;
	}
	return false;
}

} // namespace robikzinputtest
