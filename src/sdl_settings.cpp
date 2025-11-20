#include "sdl_settings.hpp"

#include "properties_file.hpp"
#include "sdl_storage.hpp"

#include <iostream>
#include <string>

namespace robikzinputtest::sdl {

static const std::string CONFIG_FILENAME = "robikzinputtest.cfg";

bool SettingsSdlIO::save(const Settings &settings) {
	std::shared_ptr<SDL_Storage> storage = user_storage();

	std::vector<Property> properties = settings.export_properties();
	std::string text = write_properties(properties);
	return write_text_file(storage, CONFIG_FILENAME, text);
}

std::pair<bool, Settings> SettingsSdlIO::load() {
	Settings settings;

	std::shared_ptr<SDL_Storage> storage = user_storage();
	auto file_load_result = load_text_file(storage, CONFIG_FILENAME);
	if (file_load_result.first) {
		const std::vector<Property> properties =
			read_properties(file_load_result.second);
		settings.import_properties(properties);
		return { true, settings };
	} else {
		std::cerr << "Failed to read config file: " << CONFIG_FILENAME << std::endl;
	}

	return { false, settings };
}

} // namespace robikzinputtest::sdl
