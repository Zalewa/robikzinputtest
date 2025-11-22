#include "settings.hpp"

#include "properties_file.hpp"

namespace robikzinputtest {

void Settings::import_properties(const std::vector<Property> &properties) {
	for (const Property &property : properties) {
		if (property.name == "show_fps") {
			this->show_fps = property.value;
		} else if (property.name == "show_help") {
			this->show_help = property.value;
		} else if (property.name == "show_help_at_start") {
			this->show_help_at_start = property.value;
		} else if (property.name == "show_settings_at_start") {
			this->show_settings_at_start = property.value;
		} else if (property.name == "gizmo_width") {
			this->gizmo_width = property.value;
		} else if (property.name == "gizmo_height") {
			this->gizmo_height = property.value;
		}
	}
}

std::vector<Property> Settings::export_properties() const {
	std::vector<Property> props;
	props.push_back({ "show_fps", show_fps });
	props.push_back({ "show_help", show_help });
	props.push_back({ "show_help_at_start", show_help_at_start });
	props.push_back({ "show_settings_at_start", show_settings_at_start });
	props.push_back({ "gizmo_width", gizmo_width });
	props.push_back({ "gizmo_height", gizmo_height });
	return props;
}

} // namespace robikzinputtest
