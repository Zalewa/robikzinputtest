#pragma once

#include "settings.hpp"

namespace robikzinputtest::sdl {

class SettingsSdlIO : public SettingsIO {
public:
	bool save(const Settings &settings) override;
	std::pair<bool, Settings> load() override;
};

} // namespace robikzinputtest::sdl
