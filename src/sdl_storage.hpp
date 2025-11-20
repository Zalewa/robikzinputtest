#pragma once

#include <SDL3/SDL.h>
#include <cstdint>
#include <memory>
#include <vector>

namespace robikzinputtest::sdl {

/**
 * A shared_ptr to SDL_OpenUserStorage() with deleter
 * set to SDL_CloseStorage().
 */
std::shared_ptr<SDL_Storage> user_storage();

std::pair<bool, std::vector<uint8_t>> load_binary_file(
	std::shared_ptr<SDL_Storage> storage,
	const std::string &name
);
std::pair<bool, std::string> load_text_file(
	std::shared_ptr<SDL_Storage> storage,
	const std::string &name
);

bool write_binary_file(
	std::shared_ptr<SDL_Storage> storage,
	const std::string &name,
	const std::vector<uint8_t> &buffer
);
bool write_text_file(
	std::shared_ptr<SDL_Storage> storage,
	const std::string &name,
	const std::string &buffer
);

} // namespace robikzinputtest::sdl
