#pragma once

#include <utility>
#include <vector>

namespace robikzinputtest {

struct Property;

struct Settings {
	bool show_fps = true;

	void import_properties(const std::vector<Property> &properties);
	std::vector<Property> export_properties() const;
};

class SettingsIO {
public:
	virtual bool save(const Settings &settings) = 0;
	virtual std::pair<bool, Settings> load() = 0;
};

} // namespace robikzinputtest
