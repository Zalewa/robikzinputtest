#include "settings.hpp"

#include "properties_file.hpp"

namespace robikzinputtest {

void Settings::import_properties(const std::vector<Property> &properties) {
	for (const Property &property : properties) {
		if (property.name == "show_fps") {
			this->show_fps = property.value;
		}
	}
}

std::vector<Property> Settings::export_properties() const {
	std::vector<Property> props;
	props.push_back({ "show_fps", show_fps });
	return props;
}

} // namespace robikzinputtest
