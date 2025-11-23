#include "controller_handler.hpp"

#include "app.hpp"
#include "controller.hpp"
#include "settings.hpp"
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

static const int32_t JOYSTICK_AXIS_MAX32 = SDL_JOYSTICK_AXIS_MAX;
static const int32_t JOYSTICK_AXIS_MIN32 = SDL_JOYSTICK_AXIS_MIN;

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

static int32_t sum_joystick_thumbsticks_for_axis(SDL_Joystick *joystick, int32_t axis, int32_t deadzone) {
	SDL_LockJoysticks();
	const int32_t n_axes = SDL_GetNumJoystickAxes(joystick);
	std::function<bool(int)> same_axis;
	if (is_joystick_horizontal_axis(axis)) {
		same_axis = &is_joystick_horizontal_axis;
	} else if (is_joystick_vertical_axis(axis)) {
		same_axis = &is_joystick_vertical_axis;
	} else {
		same_axis = [](int axis) { return false; };
	}
	int32_t summed_axis_value = 0;
	for (int32_t n_axis = 0; n_axis < n_axes; ++n_axis) {
		if (same_axis(n_axis)) {
			const int32_t current_axis_value = SDL_GetJoystickAxis(joystick, n_axis);
			if (std::abs(current_axis_value) >= deadzone) {
				const bool same_sign = (summed_axis_value < 0) == (current_axis_value < 0);
				if (same_sign) {
					if (summed_axis_value >= 0) {
						summed_axis_value = std::min(JOYSTICK_AXIS_MAX32, summed_axis_value + current_axis_value);
					} else {
						summed_axis_value = std::max(JOYSTICK_AXIS_MIN32, summed_axis_value + current_axis_value);
					}
				} else {
					summed_axis_value += current_axis_value;
				}
			}
		}
	}
	SDL_UnlockJoysticks();
	return summed_axis_value;
}

bool JoystickControllerHandler::handle_event(
	App &app,
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
		const int32_t stick_deadzone = app.settings().joystick_deadzone;
		// Increase the space to 32-bit for calculations.
		int32_t axis_value =  event.jaxis.value;
		// Since multiple thumbsticks can control movement, it's necessary
		// to sum their values to see if they cancel each other out.
		auto joystick_it = app.joysticks().find(event.jaxis.which);
		if (joystick_it != app.joysticks().end()) {
			axis_value = sum_joystick_thumbsticks_for_axis(
				joystick_it->second.get(),
				event.jaxis.axis,
				stick_deadzone
			);
		}

		int32_t axis_value_in_deadzone = 0;
		int32_t axis_max_in_deadzone = 0;
		if (axis_value >= 0) {
			axis_value_in_deadzone = axis_value - stick_deadzone;
			axis_max_in_deadzone = JOYSTICK_AXIS_MAX32 - stick_deadzone;
		} else {
			axis_value_in_deadzone = (axis_value * -1) - stick_deadzone;
			axis_max_in_deadzone = (JOYSTICK_AXIS_MIN32 * -1) - stick_deadzone;
		}

		if (axis_value_in_deadzone > 0 && axis_max_in_deadzone > 0) {
			double normalized = 0.0;
			normalized = static_cast<double>(axis_value_in_deadzone) / axis_max_in_deadzone;
			if (axis_value < 0) {
				normalized *= -1.0f;
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
	App &app,
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
