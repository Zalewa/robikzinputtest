#include "app.hpp"

namespace {
bool is_quit_key(const SDL_Keysym &keysym)
{
	return keysym.sym == SDLK_ESCAPE
		|| (keysym.sym == SDLK_q && (keysym.mod & KMOD_CTRL));
}

bool is_alt_enter(const SDL_Keysym &keysym)
{
	return keysym.sym == SDLK_RETURN
		&& (keysym.mod & KMOD_ALT);
}
} // namespace

namespace robikzinputtest {

App::App()
	: window(nullptr), renderer(nullptr), running(true)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	// Create a window
	window = SDL_CreateWindow("Robikz's Input Test",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600,
		SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		SDL_Quit();
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	// Create a renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		SDL_DestroyWindow(window);
		SDL_Quit();
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}
}

App::~App()
{
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);
	SDL_Quit();
}

void App::run()
{
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if (is_quit_key(event.key.keysym)) {
					running = false;
				} else if (is_alt_enter(event.key.keysym)) {
					// Toggle fullscreen
					Uint32 flags = SDL_GetWindowFlags(window);
					if (flags & SDL_WINDOW_FULLSCREEN) {
						SDL_SetWindowFullscreen(window, 0);
					} else {
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
					}
				}
				break;
			}
		}

		// Clear the screen with a black color
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Present the backbuffer
		SDL_RenderPresent(renderer);
	}
}
