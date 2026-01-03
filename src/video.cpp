#include "video.hpp"

#include <SDL3/SDL.h>

namespace robikzinputtest {

/*
  Screen and display mode functions
*/

DisplayInfo get_display_info(SDL_DisplayID display_id) {
	const char *name = SDL_GetDisplayName(display_id);
	DisplayInfo info = {
		.id = display_id,
		.name = name != nullptr ? std::string(name) : "Unknown Display",
	};
	return info;
}

SDL_DisplayID get_display_id_by_name(const std::string &display_name) {
	int num_displays = 0;
	SDL_DisplayID *displays = SDL_GetDisplays(&num_displays);
	if (displays == nullptr) {
		return 0;
	}
	SDL_DisplayID found_display_id = 0;
	for (int i = 0; i < num_displays; ++i) {
		DisplayInfo info = get_display_info(displays[i]);
		if (info.name == display_name) {
			found_display_id = displays[i];
			break;
		}
	}
	SDL_free(displays);
	return found_display_id;
}

std::vector<DisplayInfo> get_available_displays_info() {
	int num_displays = 0;
	SDL_DisplayID *displays = SDL_GetDisplays(&num_displays);
	if (displays == nullptr) {
		return {};
	}
	std::vector<DisplayInfo> displays_info;
	for (int i = 0; i < num_displays; ++i) {
		displays_info.push_back(get_display_info(displays[i]));
	}
	SDL_free(displays);
	return displays_info;
}

std::vector<VideoModeSettings> get_available_display_modes(SDL_DisplayID display_id) {
	int num_display_modes = 0;
	SDL_DisplayMode **display_modes = SDL_GetFullscreenDisplayModes(
		display_id,
		&num_display_modes
	);
	std::vector<VideoModeSettings> video_mode_settings_list;
	for (int i = 0; i < num_display_modes; ++i) {
		SDL_DisplayMode *mode = display_modes[i];
		video_mode_settings_list.push_back(VideoModeSettings {
			.display_mode = DisplayMode::FULLSCREEN,
			.display_id = display_id,
			.display_settings =
				DisplaySettings {
					.width = mode->w,
					.height = mode->h,
					.pixel_format = mode->format,
					.refresh_rate = mode->refresh_rate,
					.refresh_rate_numerator = mode->refresh_rate_numerator,
					.refresh_rate_denominator = mode->refresh_rate_denominator,
				},
		});
	}
	SDL_free(display_modes);
	return video_mode_settings_list;
}

VideoModeSettings pick_best_display_mode(
	const std::vector<VideoModeSettings> &available_modes
) {
	if (available_modes.empty()) {
		return VideoModeSettings{};
	}
	VideoModeSettings best_mode = available_modes[0];
	for (const auto &mode : available_modes) {
		if (mode.display_settings.width > best_mode.display_settings.width) {
			best_mode = mode;
		} else if (
			mode.display_settings.width == best_mode.display_settings.width
			&& mode.display_settings.height > best_mode.display_settings.height
		) {
			best_mode = mode;
		} else if (
			mode.display_settings.width == best_mode.display_settings.width
			&& mode.display_settings.height == best_mode.display_settings.height
			&& mode.display_settings.refresh_rate > best_mode.display_settings.refresh_rate
		) {
			best_mode = mode;
		}
	}
	return best_mode;
}

VideoModeSettings get_best_fullscreen_settings_for_window_and_display(
	SDL_Window *window,
	SDL_DisplayID display_id
) {
	const std::vector<VideoModeSettings> available_modes =
		get_available_display_modes(display_id);

	const VideoModeSettings factual_settings =
		get_window_video_mode_settings(window);

	if (
		factual_settings.display_mode == DisplayMode::FULLSCREEN
		&& factual_settings.display_id == display_id
	) {
		// Try to find an exact match for the current fullscreen settings
		for (const auto &mode : available_modes) {
			if (mode.display_settings == factual_settings.display_settings) {
				return mode;
			}
		}
	}

	// Fallback to picking the best available mode
	return pick_best_display_mode(available_modes);
}

std::string get_display_mode_label(DisplayMode display_mode) {
	switch (display_mode) {
	case DisplayMode::WINDOWED:
		return "Windowed";
	case DisplayMode::FULLSCREEN:
		return "Fullscreen";
	case DisplayMode::BORDERLESS_FULLSCREEN:
		return "Borderless Fullscreen";
	default:
		return "Unknown";
	}
}


static bool is_matching_mode_exact(
	const SDL_DisplayMode &mode,
	const DisplaySettings &display_settings
) {
	return
		mode.w == display_settings.width
		&& mode.h == display_settings.height
		&& mode.format == display_settings.pixel_format
		&& mode.pixel_density == display_settings.pixel_density
		&& mode.refresh_rate_numerator == display_settings.refresh_rate_numerator
		&& mode.refresh_rate_denominator == display_settings.refresh_rate_denominator
		;
}

static bool is_matching_mode_resolution_and_refresh_rate(
	const SDL_DisplayMode &mode,
	const DisplaySettings &display_settings
) {
	return
		mode.w == display_settings.width
		&& mode.h == display_settings.height
		&& mode.refresh_rate_numerator == display_settings.refresh_rate_numerator
		&& mode.refresh_rate_denominator == display_settings.refresh_rate_denominator
		;
}

static bool is_matching_mode_resolution(
	const SDL_DisplayMode &mode,
	const DisplaySettings &display_settings
) {
	return
		mode.w == display_settings.width
		&& mode.h == display_settings.height
		;
}

static SDL_DisplayMode *select_best_matching_mode(
	SDL_DisplayMode **available_modes,
	int num_available_modes,
	const DisplaySettings &display_settings
) {
	for (int i = 0; i < num_available_modes; ++i) {
		SDL_DisplayMode *mode = available_modes[i];
		if (is_matching_mode_exact(*mode, display_settings)) {
			return mode;
		}
	}
	for (int i = 0; i < num_available_modes; ++i) {
		SDL_DisplayMode *mode = available_modes[i];
		if (is_matching_mode_resolution_and_refresh_rate(*mode, display_settings)) {
			return mode;
		}
	}
	for (int i = 0; i < num_available_modes; ++i) {
		SDL_DisplayMode *mode = available_modes[i];
		if (is_matching_mode_resolution(*mode, display_settings)) {
			return mode;
		}
	}
	return nullptr;
}

/*
  SDL_Window display mode functions
*/

DisplayMode get_window_display_mode(SDL_Window *window) {
	Uint32 flags = SDL_GetWindowFlags(window);
	if (flags & SDL_WINDOW_FULLSCREEN) {
		if (SDL_GetWindowFullscreenMode(window) != nullptr) {
			return DisplayMode::FULLSCREEN;
		} else {
			return DisplayMode::BORDERLESS_FULLSCREEN;
		}
	} else {
		return DisplayMode::WINDOWED;
	}
}

VideoModeSettings get_window_video_mode_settings(SDL_Window *window) {
	VideoModeSettings video_mode_settings;
	video_mode_settings.display_mode = get_window_display_mode(window);
	video_mode_settings.display_id = SDL_GetDisplayForWindow(window);
	DisplaySettings &display_settings = video_mode_settings.display_settings;
	SDL_GetWindowPosition(window, &display_settings.x, &display_settings.y);
	SDL_GetWindowSize(window, &display_settings.width, &display_settings.height);
	display_settings.maximized =
		(SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED) != 0;
	const SDL_DisplayMode *fullscreen_mode = SDL_GetWindowFullscreenMode(window);
	if (fullscreen_mode != nullptr) {
		display_settings.pixel_format = fullscreen_mode->format;
		display_settings.refresh_rate = fullscreen_mode->refresh_rate;
		display_settings.refresh_rate_numerator = fullscreen_mode->refresh_rate_numerator;
		display_settings.refresh_rate_denominator = fullscreen_mode->refresh_rate_denominator;
	} else {
		display_settings.pixel_format = SDL_PIXELFORMAT_UNKNOWN;
		display_settings.refresh_rate = 0.0f;
		display_settings.refresh_rate_numerator = 0;
		display_settings.refresh_rate_denominator = 0;
	}
	return video_mode_settings;
}

static const int MIN_REASONABLE_WINDOW_DIMENSION = 10;
static const int MAX_REASONABLE_WINDOW_DIMENSION = 20000;

static bool set_windowed_video_mode(SDL_Window *window, const DisplaySettings &display_settings) {
	// There is no guarantee that the window will not be placed
	// completely out of screen bounds, or become overstretched in a way
	// that becomes unusable. This can happen when incorrect values
	// appear in the config file, but also when a window is placed on a
	// display that gets disconnected. The routine should enforce
	// window positioning to the screen bounds, even if it will
	// sometimes perform undesireably by moving the window to a location
	// it wasn't in when the settings were saved.
	SDL_Rect target_rect = {
		display_settings.x,
		display_settings.y,
		// Unreasonable window size will screw up the display canvas.
		SDL_clamp(display_settings.width, MIN_REASONABLE_WINDOW_DIMENSION, MAX_REASONABLE_WINDOW_DIMENSION),
		SDL_clamp(display_settings.height, MIN_REASONABLE_WINDOW_DIMENSION, MAX_REASONABLE_WINDOW_DIMENSION),
	};
	SDL_DisplayID target_display_id = SDL_GetDisplayForRect(&target_rect);
	if (target_display_id == 0) {
		target_display_id = SDL_GetPrimaryDisplay();
	}
	// If the display ID is zero at this point (unknown), there's no way to clamp anything,
	// so let's just roll with whatever settings were loaded.
	if (target_display_id != 0) {
		SDL_Rect usable_bounds;
		if (SDL_GetDisplayUsableBounds(target_display_id, &usable_bounds)) {
			target_rect.x = SDL_clamp(
				target_rect.x,
				usable_bounds.x,
				std::max(usable_bounds.x, usable_bounds.x + usable_bounds.w - target_rect.w)
			);
			target_rect.y = SDL_clamp(
				target_rect.y,
				usable_bounds.y + 1, // +1 because window top decoration should remain visible
				std::max(usable_bounds.y, usable_bounds.y + usable_bounds.h - target_rect.h)
			);
			if (target_rect.x + target_rect.w > usable_bounds.x + usable_bounds.w) {
				target_rect.w = usable_bounds.x + usable_bounds.w - target_rect.x;
			}
			if (target_rect.y + target_rect.h > usable_bounds.y + usable_bounds.h) {
				target_rect.h = usable_bounds.y + usable_bounds.h - target_rect.y;
			}
		}
	}

	SDL_SetWindowFullscreen(window, false);
	SDL_SetWindowBordered(window, true);
	SDL_RestoreWindow(window);
	SDL_SetWindowFullscreenMode(window, nullptr);
	SDL_SetWindowPosition(window, target_rect.x, target_rect.y);
	SDL_SetWindowSize(window, target_rect.w, target_rect.h);
	if (display_settings.maximized)
		SDL_MaximizeWindow(window);
	SDL_SyncWindow(window);
	return true;
}

static bool set_fullscreen_video_mode(
	SDL_Window *window,
	SDL_DisplayID display_id,
	const DisplaySettings &display_settings
) {
	int num_available_display_modes = 0;
	SDL_DisplayMode **available_display_modes = SDL_GetFullscreenDisplayModes(
		display_id,
		&num_available_display_modes
	);

	if (num_available_display_modes == 0) {
		SDL_free(available_display_modes);
		return false;
	}

	SDL_DisplayMode *matching_mode = select_best_matching_mode(
		available_display_modes,
		num_available_display_modes,
		display_settings
	);
	bool resolution_set = false;
	if (matching_mode != nullptr) {
		if (SDL_SetWindowFullscreen(window, true)) {
			SDL_SyncWindow(window);
			resolution_set = SDL_SetWindowFullscreenMode(window, matching_mode);
		}
	}
	SDL_free(available_display_modes);
	return resolution_set;
}

static bool set_borderless_fullscreen_video_mode(
	SDL_Window *window,
	SDL_DisplayID display_id
) {
	// Get the display bounds to move the window to the correct display.
	SDL_Rect display_bounds;
	bool reposition_allowed = true;
	if (!SDL_GetDisplayBounds(display_id, &display_bounds)) {
		reposition_allowed = false;
	}

	if (reposition_allowed) {
		// It's possible that the window is fullscreen already, but on another display.
		// Window may not move if it's already fullscreen, so unset fullscreen first.
		SDL_SetWindowFullscreen(window, false);
		// Move the window to the target display.
		SDL_SetWindowPosition(window, display_bounds.x, display_bounds.y);
	}
	SDL_SetWindowBordered(window, false);
	SDL_SetWindowFullscreen(window, true);
	SDL_SetWindowFullscreenMode(window, nullptr);
	SDL_SyncWindow(window);
	return true;
}

bool set_window_video_mode_settings(SDL_Window *window, const VideoModeSettings &video_mode_settings) {
	switch (video_mode_settings.display_mode) {
	case DisplayMode::WINDOWED:
		return set_windowed_video_mode(
			window,
			video_mode_settings.display_settings
		);
	case DisplayMode::FULLSCREEN:
		return set_fullscreen_video_mode(
			window,
			video_mode_settings.display_id,
			video_mode_settings.display_settings
		);
	case DisplayMode::BORDERLESS_FULLSCREEN:
		return set_borderless_fullscreen_video_mode(
			window,
			video_mode_settings.display_id
		);
	default:
		// Unknown display mode; do nothing.
		return false;
	}
	return true;
}

} // namespace robikzinputtest
