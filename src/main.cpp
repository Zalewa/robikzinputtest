#include <array>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

int main(int argc, char **argv)
{
	std::cout << "hello, world!" << std::endl;

	SDL_Init(SDL_INIT_VIDEO);

	// Open a black window
	SDL_Window *window = SDL_CreateWindow(
		"Hello World!",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
		);
	if (window == nullptr) {
		std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
		return 1;
	}
	SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
	SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0, 0, 0));
	SDL_UpdateWindowSurface(window);

	// Color bank
	const std::vector<std::array<int, 3>> colors = {
		{0, 198, 0},
		{0, 170, 0},
		{0, 146, 0},
	};
	int current_color = 0;

	// Main loop
	bool running = true;
	while (running) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if (is_quit_key(event.key.keysym)) {
					running = false;
				} else if (is_alt_enter(event.key.keysym)) {
					Uint32 flags = SDL_GetWindowFlags(window);
					if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
						SDL_SetWindowFullscreen(window, 0);
					} else {
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
					}
				}
				break;
			}
		}

		// Rendering
		current_color = (current_color + 1) % (60);
		const auto &color = colors[current_color / 20 % colors.size()];
		SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, color[0], color[1], color[2]));
		SDL_UpdateWindowSurface(window);

		SDL_Delay(16); // Roughly 60 FPS
	}
	SDL_Quit();

	return 0;
}
