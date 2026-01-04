#include "controller_handler.hpp"

#include "app.hpp"
#include "controller.hpp"
#include "sdl_math.hpp"
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

/**
 * Get movement direction as indicated by joystick's axes (thumbsticks).
 *
 * Deadzone is taken into account. An axis with an absolute value below the
 * deadzone is ignored. Axis value gets rescaled linearly in accordance to
 * the deadzone.
 *
 * The movement direction is not normalized (it may exceed a unit vector).
 */
static SDL_FPoint get_joystick_axis_direction(SDL_Joystick *joystick, int32_t deadzone) {
	SDL_FPoint total_direction = { 0, 0 };
	SDL_LockJoysticks();
	const int32_t n_axes = SDL_GetNumJoystickAxes(joystick);
	for (int32_t n_axis = 0; n_axis < n_axes; ++n_axis) {
		const int32_t axis = SDL_GetJoystickAxis(joystick, n_axis);
		if (axis != 0 && std::abs(axis) >= deadzone) {
			float axis_magnitude = 0.0f;
			if (axis > 0 && deadzone < JOYSTICK_AXIS_MAX32) {
				axis_magnitude =
					static_cast<float>(axis - deadzone)
					/ static_cast<float>(JOYSTICK_AXIS_MAX32 - deadzone);
			} else if (axis < 0 && -deadzone > JOYSTICK_AXIS_MIN32) {
				axis_magnitude =
					static_cast<float>(axis + deadzone)
					/ static_cast<float>(JOYSTICK_AXIS_MIN32 + deadzone);
				axis_magnitude *= -1;
			}
			if (is_joystick_horizontal_axis(n_axis)) {
				total_direction.x += axis_magnitude;
			}
			if (is_joystick_vertical_axis(n_axis)) {
				total_direction.y += axis_magnitude;
			}
		}
	}
	SDL_UnlockJoysticks();
	return total_direction;
}

/**
 * Get movement direction as indicated by joystick's d-pads.
 *
 * The movement direction is not normalized (it may exceed a unit vector).
 */
static SDL_FPoint get_joystick_dpad_direction(SDL_Joystick *joystick) {
	SDL_FPoint total_direction = { 0, 0 };
	SDL_LockJoysticks();
	// The joystick's "hat" (or "d-pad") is also recognized as an axis of movement.
	const int32_t n_hats = SDL_GetNumJoystickHats(joystick);
	for (int32_t n_hat = 0; n_hat < n_hats; ++n_hat) {
		uint8_t hat = SDL_GetJoystickHat(joystick, n_hat);
		SDL_FPoint hat_direction = { 0, 0 };
		total_direction.x += (hat & SDL_HAT_LEFT) ? -1.0f : 0.0f;
		total_direction.x += (hat & SDL_HAT_RIGHT) ? +1.0f : 0.0f;
		total_direction.y += (hat & SDL_HAT_UP) ? -1.0f : 0.0f;
		total_direction.y += (hat & SDL_HAT_DOWN) ? +1.0f : 0.0f;
	}
	SDL_UnlockJoysticks();
	return total_direction;
}

/**
 * Sum up all joystick movements buttons into a movement vector.
 */
static SDL_FPoint get_joystick_complete_normalized_movement_direction(SDL_Joystick *joystick, int32_t deadzone) {
	SDL_FPoint total_direction = { 0, 0 };
	sdl::addi_fpoint(total_direction, get_joystick_axis_direction(joystick, deadzone));
	sdl::addi_fpoint(total_direction, get_joystick_dpad_direction(joystick));
	if (sdl::magvec_fpoint(total_direction) > 1.0f) {
		// If overall magnitude is higher than a unit, clamp it.
		return sdl::normveci_fpoint(total_direction);
	} else {
		// Magnitudes lower than a unit are allowed.
		return total_direction;
	}
}

#if 0
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
#endif

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
		} else {
			SDL_Joystick *joystick = SDL_GetJoystickFromID(event.jaxis.which);
			if (joystick != nullptr) {
				state.direction_vec2 = get_joystick_complete_normalized_movement_direction(
					joystick,
					app.settings().joystick_deadzone
				);
			}
		}
	} else if (event.type == SDL_EVENT_JOYSTICK_HAT_MOTION) {
		if (event.jhat.which != controller.id.index)
			return false;

		SDL_Joystick *joystick = SDL_GetJoystickFromID(event.jhat.which);
		if (joystick != nullptr) {
			state.direction_vec2 = get_joystick_complete_normalized_movement_direction(
				joystick,
				app.settings().joystick_deadzone
			);
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
