#pragma once

#include <SDL3/SDL.h>

namespace robikzinputtest::sdl {

/**
 * Check if the given window is a normal window.
 *
 * A normal window is not fullscreen, nor minimized, nor maximized,
 * nor hidden, nor borderless fullscreen.
 */
inline bool is_window_normal(SDL_Window *window) {
	auto flags = SDL_GetWindowFlags(window);
	return !(
		flags & (
			SDL_WINDOW_FULLSCREEN
			| SDL_WINDOW_MAXIMIZED
			| SDL_WINDOW_MINIMIZED
			| SDL_WINDOW_HIDDEN
		)
	);
}

/**
 * Check if the given window is maximized.
 */
inline bool is_window_maximized(SDL_Window *window) {
	auto flags = SDL_GetWindowFlags(window);
	return (flags & SDL_WINDOW_MAXIMIZED) != 0;
}

}
