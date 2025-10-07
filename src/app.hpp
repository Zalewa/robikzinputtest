#pragma once

#include <SDL.h>

namespace robikzinputtest {

class App {
public:
	App();
	~App();

	void run();

private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	bool running;
}

}
