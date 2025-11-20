#include "sdl_storage.hpp"

#include "SDL3/SDL_storage.h"
#include "version.hpp"
#include <iostream>

namespace robikzinputtest::sdl {

static std::shared_ptr<SDL_Storage> await_ready(std::shared_ptr<SDL_Storage> storage) {
	while (!SDL_StorageReady(storage.get()))
		SDL_Delay(1);
	return storage;
}

std::shared_ptr<SDL_Storage> user_storage() {
	SDL_Storage *storage = SDL_OpenUserStorage(
		app_identifier_organization().c_str(),
		app_identifier_appname().c_str(),
		0
	);
	return await_ready(std::shared_ptr<SDL_Storage>(storage, &SDL_CloseStorage));
}

std::pair<bool, std::vector<uint8_t>> load_binary_file(
	std::shared_ptr<SDL_Storage> storage,
	const std::string &name
) {
	uint64_t file_len = 0;
	if (
		SDL_GetStorageFileSize(
			storage.get(),
			name.c_str(),
			&file_len
		)
			&& file_len > 0
	) {
		std::vector<uint8_t> buffer(file_len);
		if (
			SDL_ReadStorageFile(
				storage.get(),
				name.c_str(),
				buffer.data(),
				file_len
			)
		) {
			return { true, buffer };
		} else {
			std::cerr << "Failed to read user file: " << name << std::endl;;
			return { false, {} };
		}
	} else {
		// No file?
		return { false, {} };
	}
}

std::pair<bool, std::string> load_text_file(
	std::shared_ptr<SDL_Storage> storage,
	const std::string &name
) {
	auto loadresult = load_binary_file(storage, name);
	if (loadresult.first) {
		auto &buffer = loadresult.second;
		return { true, std::string(buffer.begin(), buffer.end()) };
	} else {
		return { false, {} };
	}
}

bool write_binary_file(
	std::shared_ptr<SDL_Storage> storage,
	const std::string &name,
	const std::vector<uint8_t> &buffer
) {
	return SDL_WriteStorageFile(
		storage.get(),
		name.c_str(),
		buffer.data(),
		buffer.size()
	);
}

bool write_text_file(
	std::shared_ptr<SDL_Storage> storage,
	const std::string &name,
	const std::string &buffer
) {
	std::vector<uint8_t> bytebuffer(buffer.begin(), buffer.end());
	return write_binary_file(storage, name, bytebuffer);
}


} // namespace robikzinputtest::sdl
