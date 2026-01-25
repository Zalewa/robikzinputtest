#include "video_settings.hpp"

#include "settings.hpp"
#include "video.hpp"

namespace robikzinputtest {

std::string encode_display_id_memo(const DisplayIdMemo &display_id_memo) {
	return std::to_string(display_id_memo.id) + "|" + display_id_memo.name;
}

DisplayIdMemo decode_display_id_memo(const std::string &encoded_display_id_memo) {
	DisplayIdMemo decoded_display_id_memo = {
		// SDL functions such as SDL_GetPrimaryDisplay() return 0 when
		// display cannot be established.
		0,
		"Unknown Display",
	};
	const size_t separator_pos = encoded_display_id_memo.find('|');
	if (separator_pos == std::string::npos) {
		return decoded_display_id_memo;
	}
	const SDL_DisplayID display_id = static_cast<SDL_DisplayID>(
		std::stoll(encoded_display_id_memo.substr(0, separator_pos))
	);
	const std::string display_name = encoded_display_id_memo.substr(separator_pos + 1);
	decoded_display_id_memo.id = display_id;
	decoded_display_id_memo.name = display_name;
	return decoded_display_id_memo;
}

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
			settings.fullscreen_display = get_display_info(
				SDL_GetDisplayForWindow(window)
			).to_memo();
		}
		break;
	}
	case DisplayMode::BORDERLESS_FULLSCREEN:
		settings.fullscreen_display = get_display_info(
			SDL_GetDisplayForWindow(window)
		).to_memo();
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
		video_mode_settings.display_id = get_display_id_by_memo(settings.fullscreen_display);
		break;
	case DisplayMode::BORDERLESS_FULLSCREEN:
		video_mode_settings.display_id = get_display_id_by_memo(settings.fullscreen_display);
		break;
	default:
		// no extra info to load
		break;
	}
	set_window_video_mode_settings(window, video_mode_settings);
}

} // namespace robikzinputtest
