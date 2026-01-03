#include "video_settings.hpp"

#include "settings.hpp"
#include "video.hpp"

namespace robikzinputtest {

void save_window_video_settings(Settings &settings, SDL_Window *window) {
	DisplayMode display_mode = get_window_display_mode(window);
	settings.display_mode = static_cast<int>(display_mode);
	switch (display_mode) {
	case DisplayMode::WINDOWED:
		SDL_GetWindowSize(
			window,
			&settings.windowed_width,
			&settings.windowed_height
		);
		SDL_GetWindowPosition(
			window,
			&settings.windowed_x,
			&settings.windowed_y
		);
		settings.windowed_maximized = (SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED) != 0;
		break;
	case DisplayMode::FULLSCREEN: {
		const SDL_DisplayMode *fullscreen_mode = SDL_GetWindowFullscreenMode(window);
		if (fullscreen_mode != nullptr) {
			settings.fullscreen_width = fullscreen_mode->w;
			settings.fullscreen_height = fullscreen_mode->h;
			settings.fullscreen_refresh_rate_numerator = fullscreen_mode->refresh_rate_numerator;
			settings.fullscreen_refresh_rate_denominator = fullscreen_mode->refresh_rate_denominator;
			settings.fullscreen_pixel_format = fullscreen_mode->format;
			settings.fullscreen_pixel_density = fullscreen_mode->pixel_density;
			settings.fullscreen_display_name = get_display_info(
				SDL_GetDisplayForWindow(window)
			).name;
		}
		break;
	}
	case DisplayMode::BORDERLESS_FULLSCREEN:
		settings.fullscreen_display_name = get_display_info(
			SDL_GetDisplayForWindow(window)
		).name;
		break;
	default:
		// no extra info to save
		break;
	}
}

void load_window_video_settings(const Settings &settings, SDL_Window *window) {
	VideoModeSettings video_mode_settings;
	video_mode_settings.display_mode = static_cast<DisplayMode>(settings.display_mode);
	switch (video_mode_settings.display_mode) {
	case DisplayMode::WINDOWED:
		video_mode_settings.display_settings.width = settings.windowed_width;
		video_mode_settings.display_settings.height = settings.windowed_height;
		video_mode_settings.display_settings.x = settings.windowed_x;
		video_mode_settings.display_settings.y = settings.windowed_y;
		video_mode_settings.display_settings.maximized = settings.windowed_maximized;
		break;
	case DisplayMode::FULLSCREEN:
		video_mode_settings.display_settings.width = settings.fullscreen_width;
		video_mode_settings.display_settings.height = settings.fullscreen_height;
		video_mode_settings.display_settings.refresh_rate = 0.0f;
		video_mode_settings.display_settings.refresh_rate_numerator = settings.fullscreen_refresh_rate_numerator;
		video_mode_settings.display_settings.refresh_rate_denominator = settings.fullscreen_refresh_rate_denominator;
		video_mode_settings.display_settings.pixel_format = static_cast<SDL_PixelFormat>(settings.fullscreen_pixel_format);
		video_mode_settings.display_settings.pixel_density = settings.fullscreen_pixel_density;
		video_mode_settings.display_id = get_display_id_by_name(settings.fullscreen_display_name);
		break;
	case DisplayMode::BORDERLESS_FULLSCREEN:
		video_mode_settings.display_id = get_display_id_by_name(settings.fullscreen_display_name);
		break;
	default:
		// no extra info to load
		break;
	}
	set_window_video_mode_settings(window, video_mode_settings);
}

} // namespace robikzinputtest
