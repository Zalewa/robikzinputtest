#pragma once

#include <SDL3/SDL.h>

namespace robikzinputtest {

using Renderer = SDL_Renderer;

class Renderable {
public:
	virtual void load_render(Renderer &renderer) = 0;
	virtual void render(Renderer &renderer) = 0;
};

} // namespace robikzinputtest
