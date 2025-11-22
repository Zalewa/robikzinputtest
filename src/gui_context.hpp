#pragma once

#include <SDL3/SDL.h>

namespace robikzinputtest {

struct FrameTime;

namespace gui {

struct GuiContext {
	const FrameTime &frameTime;
	SDL_Point window_size = {0, 0};
};

} // namespace gui
} // namespace robikzinputtest
