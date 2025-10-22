/**
 * @file
 * Utility functions for working with SDL events.
 */
#pragma once

#include <SDL3/SDL.h>

namespace robikzinputtest::sdl {

inline bool is_keyboard_event(const SDL_Event &event)
{
	return event.type == SDL_EVENT_KEY_DOWN
		|| event.type == SDL_EVENT_KEY_UP
		;
}

inline bool is_joystick_event(const SDL_Event &event)
{
	return event.type == SDL_EVENT_JOYSTICK_AXIS_MOTION
		|| event.type == SDL_EVENT_JOYSTICK_BALL_MOTION
		|| event.type == SDL_EVENT_JOYSTICK_HAT_MOTION
		|| event.type == SDL_EVENT_JOYSTICK_BUTTON_UP
		|| event.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN
		;
}

inline bool is_mouse_event(const SDL_Event &event)
{
	return event.type == SDL_EVENT_MOUSE_BUTTON_DOWN
		|| event.type == SDL_EVENT_MOUSE_BUTTON_UP
		|| event.type == SDL_EVENT_MOUSE_MOTION
		|| event.type == SDL_EVENT_MOUSE_WHEEL
		;
}

} // namespace robikzinputtest::sdl
