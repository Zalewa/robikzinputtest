#include "app.hpp"
#include "clock.hpp"
#include "version.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_keycode.h>

#include <array>
#include <iostream>
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

	EngineClock clock;

	D() : window(nullptr), renderer(nullptr)
	{
	}
};

App::App()
	: d(std::make_unique<D>())
{
}

App::~App()
{
	close();
}

AppRunResult App::init(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	// Initialize SDL
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return AppRunResult::FAILURE;
	}

	// Create a window
	std::string title = "Robikz's Input Test v" + app_version();
	d->window = SDL_CreateWindow(
		title.c_str(),
		800, 600,
		SDL_WINDOW_RESIZABLE
	);
	if (d->window == nullptr) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return AppRunResult::FAILURE;
	}

	// Create a renderer
	d->renderer = SDL_CreateRenderer(d->window, nullptr);
	if (d->renderer == nullptr) {
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return AppRunResult::FAILURE;
	}

	return AppRunResult::CONTINUE;
}

AppRunResult App::run()
{
	while (1) {
		FrameTime frame_time = d->clock.tick();
		const AppRunResult event_result = handleEvents(frame_time);
		if (event_result != AppRunResult::CONTINUE) {
			return event_result;
		}
		const AppRunResult iterate_result = iterate(frame_time);
		if (iterate_result != AppRunResult::CONTINUE) {
			return iterate_result;
		}
	}
	return AppRunResult::SUCCESS;
}

AppRunResult App::handleEvents(const FrameTime &frame_time)
{
	(void) frame_time;

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_EVENT_QUIT:
			return AppRunResult::SUCCESS;
		case SDL_EVENT_KEY_DOWN:
			if (is_quit_key(event.key)) {
				return AppRunResult::SUCCESS;
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
	return AppRunResult::CONTINUE;
}

AppRunResult App::iterate(const FrameTime &frame_time)
{
	static const std::vector<std::array<uint8_t, 3>> colors = {
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
	static int color_cycle_index = 0;
	const static Seconds time_color_change_rate = 0.2;
	static Seconds time_accumulator = 0.0;

	// Rendering
	time_accumulator += frame_time.delta_seconds;
	while (time_accumulator >= time_color_change_rate) {
		time_accumulator -= time_color_change_rate;
		color_cycle_index = (++color_cycle_index) % (colors.size());
	}
	const auto &color = colors[color_cycle_index];

	// Clear the screen with a color
	SDL_SetRenderDrawColor(d->renderer, color[0], color[1], color[2], 255);
	SDL_RenderClear(d->renderer);

	// Present the backbuffer
	SDL_RenderPresent(d->renderer);

	return AppRunResult::CONTINUE;
}

void App::close()
{
	if (d->renderer) {
		SDL_DestroyRenderer(d->renderer);
		d->renderer = nullptr;
	}
	if (d->window) {
		SDL_DestroyWindow(d->window);
		d->window = nullptr;
	}
	SDL_Quit();
}

} // namespace robikzinputtest
