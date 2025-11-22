#pragma once

#include <utility>
#include <vector>

namespace robikzinputtest {

struct Property;

struct Settings {
	bool show_fps = true;
	bool show_help = false;
	bool show_help_at_start = true;
	bool show_settings_at_start = false;

	int gizmo_width = 20;
	int gizmo_height = 20;

	void import_properties(const std::vector<Property> &properties);
	std::vector<Property> export_properties() const;
};

class SettingsIO {
public:
	virtual bool save(const Settings &settings) = 0;
	virtual std::pair<bool, Settings> load() = 0;
};

} // namespace robikzinputtest
