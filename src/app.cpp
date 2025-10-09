#include "app.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_keycode.h>

#include <array>
#include <string>
#include <vector>

namespace {
bool is_quit_key(const SDL_KeyboardEvent &key)
{
	return key.key == SDLK_ESCAPE
		|| (key.key == SDLK_Q && (key.mod & SDL_KMOD_CTRL));
}

bool is_alt_enter(const SDL_KeyboardEvent &key)
{
	return key.key == SDLK_RETURN
		&& (key.mod & SDL_KMOD_ALT);
}
} // namespace

namespace robikzinputtest {

struct App::D
{
	SDL_Window* window;
	SDL_Renderer* renderer;

	std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> windowPtr;

	D() : window(nullptr), renderer(nullptr), windowPtr(nullptr, SDL_DestroyWindow)
	{
	}
};

App::App()
	: d(std::make_unique<D>())
{
}

App::~App()
{
	if (d->renderer)
		SDL_DestroyRenderer(d->renderer);
	if (d->window)
		SDL_DestroyWindow(d->window);
	SDL_Quit();
}

AppRunResult App::init(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	// Initialize SDL
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
		return {
			ExitCode::SDL_INIT_ERROR,
			std::string("SDL_Init Error: ") + SDL_GetError()
		};
	}

	// Create a window
	d->window = SDL_CreateWindow(
		"Robikz's Input Test",
		800, 600,
		SDL_WINDOW_RESIZABLE
	);
	if (d->window == nullptr) {
		return {
			ExitCode::WINDOW_INIT_ERROR,
			std::string("SDL_CreateWindow Error: ") + SDL_GetError()
		};
	}

	// Create a renderer
	d->renderer = SDL_CreateRenderer(d->window, nullptr);
	if (d->renderer == nullptr) {
		return {
			ExitCode::RENDERER_INIT_ERROR,
			std::string("SDL_CreateRenderer Error: ") + SDL_GetError()
		};
	}

	return {ExitCode::OK, ""};
}

AppRunResult App::run()
{
	std::vector<std::array<uint8_t, 3>> colors = {
		// {255, 0, 0},   // Red
		// {0, 255, 0},   // Green
		// {0, 0, 255},   // Blue
		// {255, 255, 0}, // Yellow
		// {0, 255, 255}, // Cyan
		// {255, 0, 255}  // Magenta
		// Shades of green:
		{0, 50, 0},
		{0, 100, 0},
		{0, 150, 0},
		{0, 200, 0},
	};
	int color_cycle_speed = 20; // Change color every 20 frames
	int color_cycle_index = 0;

	bool running = true;
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				running = false;
				break;
			case SDL_EVENT_KEY_DOWN:
				if (is_quit_key(event.key)) {
					running = false;
				} else if (is_alt_enter(event.key)) {
					// Toggle fullscreen
					Uint32 flags = SDL_GetWindowFlags(d->window);
					if (flags & SDL_WINDOW_FULLSCREEN) {
						SDL_SetWindowFullscreen(d->window, 0);
					} else {
						SDL_SetWindowFullscreen(d->window, SDL_WINDOW_FULLSCREEN);
					}
				}
				break;
			}
		}

		// Rendering
		color_cycle_index = (color_cycle_index + 1) % (color_cycle_speed * colors.size());
		const auto &color = colors[color_cycle_index / color_cycle_speed % colors.size()];

		// Clear the screen with a color
		SDL_SetRenderDrawColor(d->renderer, color[0], color[1], color[2], 255);
		SDL_RenderClear(d->renderer);

		// Present the backbuffer
		SDL_RenderPresent(d->renderer);

		SDL_Delay(16); // Roughly 60 FPS
	}

	return {ExitCode::OK, ""};
}

} // namespace robikzinputtest
