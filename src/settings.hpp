#pragma once

#include "color.hpp"

#include <utility>
#include <vector>

namespace robikzinputtest {

struct Property;

struct Settings {
	bool show_fps = true;
	bool show_ui_frame_counter = true;
	bool show_help = false;
	bool show_help_at_start = true;
	bool show_settings_at_start = false;
	bool show_program_log = false;
	bool show_joystick_info = false;

	float target_fps = 60.0f;

	int gizmo_width = 50;
	int gizmo_height = 50;

	/**
	 * Value below which the axis motion is not triggered.
	 *
	 * The default value has been picked basing on tests with
	 * XBox Series Elite 2 plus some safety padding.
	 */
	int joystick_deadzone = static_cast<int>(32767.0f * 0.2f);

	Color background_color = { 0.0f, 0.20f, 0.0f, 1.0f };
	bool background_animate = true;
	Color background_flash_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	bool background_flash_on_gizmo_action = false;

	void import_properties(const std::vector<Property> &properties);
	std::vector<Property> export_properties() const;
};

class SettingsIO {
public:
	virtual bool save(const Settings &settings) = 0;
	virtual std::pair<bool, Settings> load() = 0;
};

} // namespace robikzinputtest
