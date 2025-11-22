#include "controller_handler.hpp"

#include "controller.hpp"
#include <SDL3/SDL.h>
#include <cstdlib>

namespace robikzinputtest {

/*
  JoystickControllerHandler
*/
static const int32_t JOYSTICK_GAMEPAD_LEFT_THUMBSTICK_HAXIS = 0;
static const int32_t JOYSTICK_GAMEPAD_LEFT_THUMBSTICK_VAXIS = 1;
static const int32_t JOYSTICK_GAMEPAD_RIGHT_THUMBSTICK_HAXIS = 3;
static const int32_t JOYSTICK_GAMEPAD_RIGHT_THUMBSTICK_VAXIS = 4;

static const int32_t JOYSTICK_GAMEPAD_LEFT_THROTTLE_AXIS = 2;
static const int32_t JOYSTICK_GAMEPAD_RIGHT_THROTTLE_AXIS = 5;

static const int16_t JOYSTICK_AXIS_THRESHOLD = SDL_JOYSTICK_AXIS_MAX / 10;

static bool is_joystick_horizontal_axis(int32_t axis) {
	return axis == JOYSTICK_GAMEPAD_LEFT_THUMBSTICK_HAXIS
		|| axis == JOYSTICK_GAMEPAD_RIGHT_THUMBSTICK_HAXIS;
}

static bool is_joystick_vertical_axis(int32_t axis) {
	return axis == JOYSTICK_GAMEPAD_LEFT_THUMBSTICK_VAXIS
		|| axis == JOYSTICK_GAMEPAD_RIGHT_THUMBSTICK_VAXIS;
}

static bool is_joystick_throttle_axis(int32_t axis) {
	return axis == JOYSTICK_GAMEPAD_LEFT_THROTTLE_AXIS
		|| axis == JOYSTICK_GAMEPAD_RIGHT_THROTTLE_AXIS;
}

bool JoystickControllerHandler::handle_event(
	Controller &controller,
	const SDL_Event &event
) {
	ControllerState &state = controller.state;
	if (event.type == SDL_EVENT_JOYSTICK_AXIS_MOTION) {
		if (event.jaxis.which != controller.id.index)
			return false;

		const uint32_t axis = event.jaxis.axis;
		if (is_joystick_throttle_axis(axis)) {
			if (event.jaxis.value >= SDL_JOYSTICK_AXIS_MIN + JOYSTICK_AXIS_THRESHOLD) {
				state.button_primary = ButtonState::PRESSED;
			} else if (state.button_primary == ButtonState::PRESSED) {
				state.button_primary = ButtonState::RELEASED;
			}
		}
		if (std::abs(event.jaxis.value) >= JOYSTICK_AXIS_THRESHOLD) {
			double normalized = 0.0;
			if (event.jaxis.value > 0) {
				normalized = static_cast<double>(event.jaxis.value) / SDL_JOYSTICK_AXIS_MAX;
			} else if (event.jaxis.value < 0) {
				normalized = static_cast<double>(event.jaxis.value) / SDL_JOYSTICK_AXIS_MIN;
				normalized *= -1.0;
			}

			if (is_joystick_horizontal_axis(axis)) {
				state.direction_vec2.x = normalized;
			} else if (is_joystick_vertical_axis(axis)) {
				state.direction_vec2.y = normalized;
			}
		} else {
			if (is_joystick_horizontal_axis(axis)) {
				state.direction_vec2.x = 0.0;
			} else if (is_joystick_vertical_axis(axis)) {
				state.direction_vec2.y = 0.0;
			}
		}
	} else if (
		event.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN
		|| event.type == SDL_EVENT_JOYSTICK_BUTTON_UP
	) {
		if (event.jbutton.which != controller.id.index)
			return false;

		const bool is_pressed = event.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN;
		state.button_primary = is_pressed ? ButtonState::PRESSED : ButtonState::RELEASED;
	}
	return false;
}

/*
  KeyboardControllerHandler
*/

bool KeyboardControllerHandler::handle_event(
	Controller &controller,
	const SDL_Event &event
) {
	ControllerState &state = controller.state;
	if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
		bool is_pressed = (event.type == SDL_EVENT_KEY_DOWN);
		switch (event.key.key) {
		case SDLK_UP:
		case SDLK_W:
			state.direction_vec2.y = is_pressed ? -1.0 : 0.0;
			return true;
		case SDLK_DOWN:
		case SDLK_S:
			state.direction_vec2.y = is_pressed ? 1.0 : 0.0;
			return true;
		case SDLK_LEFT:
		case SDLK_A:
			state.direction_vec2.x = is_pressed ? -1.0 : 0.0;
			return true;
		case SDLK_RIGHT:
		case SDLK_D:
			state.direction_vec2.x = is_pressed ? 1.0 : 0.0;
			return true;
		case SDLK_SPACE:
		case SDLK_RETURN:
			state.button_primary = is_pressed ? ButtonState::PRESSED : ButtonState::RELEASED;
			return true;
		default:
			break;
		}
	}
	return false;
}

} // namespace robikzinputtest
