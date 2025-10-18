#include "controller_system.hpp"

#include "controller.hpp"

namespace robikzinputtest {

struct ControllerSystem::D {
	std::shared_ptr<Controller> m_keyboard_controller;
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
	return nullptr;
}

Controller &ControllerSystem::for_keyboard() {
	return *d->m_keyboard_controller;
}

bool ControllerSystem::handle_event(const SDL_Event &event) {
	// Pass the event to all controllers
	bool handled = false;
	handled |= d->m_keyboard_controller->handle_event(event);
	return handled;
}

} // namespace robikzinputtest
