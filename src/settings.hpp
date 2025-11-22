#pragma once

#include "color.hpp"

#include <utility>
#include <vector>

namespace robikzinputtest {

struct Property;

struct Settings {
	bool show_fps = true;
	bool show_help = false;
	bool show_help_at_start = true;
	bool show_settings_at_start = false;

	float target_fps = 60.0f;

	int gizmo_width = 20;
	int gizmo_height = 20;

	Color background_color = { 0.0f, 0.20f, 0.0f, 1.0f };
	bool background_animate = true;

	void import_properties(const std::vector<Property> &properties);
	std::vector<Property> export_properties() const;
};

class SettingsIO {
public:
	virtual bool save(const Settings &settings) = 0;
	virtual std::pair<bool, Settings> load() = 0;
};

} // namespace robikzinputtest
