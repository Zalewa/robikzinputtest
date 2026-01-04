#pragma once

#include "color.hpp"
#include "video.hpp"

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

	bool limit_fps = true;
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
	float program_log_opacity = 1.0f;

	// Display settings
	int display_mode = static_cast<int>(DisplayMode::WINDOWED);
	int windowed_x = SDL_WINDOWPOS_CENTERED;
	int windowed_y = SDL_WINDOWPOS_CENTERED;
	int windowed_width = 800;
	int windowed_height = 600;
	bool windowed_maximized = false;
	int fullscreen_width = 0;
	int fullscreen_height = 0;
	int fullscreen_refresh_rate_numerator = 0;
	int fullscreen_refresh_rate_denominator = 0;
	uint32_t fullscreen_pixel_format = SDL_PIXELFORMAT_UNKNOWN;
	float fullscreen_pixel_density = 0.0f;
	std::string fullscreen_display_name;
	int vsync = SDL_RENDERER_VSYNC_DISABLED;

	// Log settings
	bool log_joystick_axis_events = false;
	bool log_joystick_button_events = false;
	bool log_joystick_hat_events = false;

	void import_properties(const std::vector<Property> &properties);
	std::vector<Property> export_properties() const;
};

class SettingsIO {
public:
	virtual bool save(const Settings &settings) = 0;
	virtual std::pair<bool, Settings> load() = 0;
};

} // namespace robikzinputtest
