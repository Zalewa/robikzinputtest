#pragma once

#include <SDL3/SDL.h>

#include <string>
#include <vector>

namespace robikzinputtest {

enum class DisplayMode : int {
	WINDOWED = 0,
	FULLSCREEN = 1,
	BORDERLESS_FULLSCREEN = 2,
};

struct DisplayInfo {
	SDL_DisplayID id;
	std::string name;
};

struct DisplaySettings {
	int x = SDL_WINDOWPOS_UNDEFINED;
	int y = SDL_WINDOWPOS_UNDEFINED;
	int width = 0;
	int height = 0;
	bool maximized = false;
	SDL_PixelFormat pixel_format = SDL_PIXELFORMAT_UNKNOWN;
	float pixel_density = 1.0f;
	float refresh_rate = 0.0f;
	int refresh_rate_numerator = 0;
	int refresh_rate_denominator = 0;

	bool operator==(const DisplaySettings &other) const {
		return
			x == other.x
			&& y == other.y
			&& width == other.width
			&& height == other.height
			&& pixel_format == other.pixel_format
			&& pixel_density == other.pixel_density
			&& refresh_rate == other.refresh_rate
			&& refresh_rate_numerator == other.refresh_rate_numerator
			&& refresh_rate_denominator == other.refresh_rate_denominator
			;
	}

	bool operator!=(const DisplaySettings &other) const {
		return !(*this == other);
	}
};

struct VideoModeSettings {
	DisplayMode display_mode = DisplayMode::WINDOWED;
	SDL_DisplayID display_id = 0;
	DisplaySettings display_settings;

	bool operator==(const VideoModeSettings &other) const {
		return
			display_mode == other.display_mode
			&& display_id == other.display_id
			&& display_settings == other.display_settings
			;
	}

	bool operator!=(const VideoModeSettings &other) const {
		return !(*this == other);
	}
};

DisplayInfo get_display_info(SDL_DisplayID display_id);
SDL_DisplayID get_display_id_by_name(const std::string &display_name);
std::vector<DisplayInfo> get_available_displays_info();
std::vector<VideoModeSettings> get_available_display_modes(SDL_DisplayID display_id);

/**
 * Pick the best display mode from the available ones.
 *
 * The best mode is the one with the highest resolution and refresh rate.
 */
VideoModeSettings pick_best_display_mode(
	const std::vector<VideoModeSettings> &available_modes
);

/**
 * Pick the best available fullscreen video mode settings
 * for the given window and display.
 *
 * This is attempted in order:
 *
 * 1. Match the current window settings; if the window is already fullscreen, it's
 *    on the display and exact settings match is found, return it.
 * 2. Otherwise, pick_best_display_mode() is used on display's available modes.
 */
VideoModeSettings get_best_fullscreen_settings_for_window_and_display(
	SDL_Window *window,
	SDL_DisplayID display_id
);

std::string get_display_mode_label(DisplayMode display_mode);

inline SDL_DisplayID get_window_display(SDL_Window *window) {
	return SDL_GetDisplayForWindow(window);
}
DisplayMode get_window_display_mode(SDL_Window *window);
VideoModeSettings get_window_video_mode_settings(SDL_Window *window);
bool set_window_video_mode_settings(SDL_Window *window, const VideoModeSettings &video_mode_settings);

} // namespace robikzinputtest
