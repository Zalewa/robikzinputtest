#pragma once

#include <SDL3/SDL.h>

#include <cmath>

namespace robikzinputtest::sdl {

inline SDL_FPoint add_fpoint(SDL_FPoint a, SDL_FPoint b) {
	return { a.x + b.x, a.y + b.y };
}

inline SDL_FPoint &addi_fpoint(SDL_FPoint &a, const SDL_FPoint &b) {
	a.x += b.x;
	a.y += b.y;
	return a;
}

inline float magvec2_fpoint(const SDL_FPoint &vec) {
	return vec.x * vec.x + vec.y * vec.y;
}

inline float magvec_fpoint(const SDL_FPoint &vec) {
	return std::sqrt(magvec2_fpoint(vec));
}

inline SDL_FPoint &normveci_fpoint(SDL_FPoint &vec) {
	const float magnitude = magvec_fpoint(vec);
	if (magnitude != 0.0f) {
		vec.x /= magnitude;
		vec.y /= magnitude;
	} // else: vec must be zero
	return vec;
}

}
