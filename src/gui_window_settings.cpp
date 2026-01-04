#include "gui_window_settings.hpp"

#include "app.hpp"
#include "arena.hpp"
#include "gui_context.hpp"
#include "gui_window_about.hpp"
#include "gui_window_resolution_popup.hpp"
#include "logger.hpp"
#include "settings.hpp"
#include "version.hpp"
#include "video.hpp"
#include "video_settings.hpp"

#include <imgui.h>
#include <SDL3/SDL.h>

#include <chrono>
#include <string>

namespace robikzinputtest::gui {

using namespace std::literals;

const std::string WINDOW_SETTINGS_TITLE = "Settings";

static std::string get_resolution_label(const DisplaySettings &display_settings) {
	return
		std::to_string(display_settings.width) + "x"
		+ std::to_string(display_settings.height) + " @ "
		+ std::to_string(display_settings.refresh_rate) + "Hz"
		;
}

struct WindowSettings::D {
	std::unique_ptr<WindowAbout> window_about;
	std::unique_ptr<WindowResolutionPopup> window_resolution_popup;

	VideoModeSettings video_mode_settings {};
	VideoModeSettings original_video_mode_settings {};
	bool resolution_just_changed = false;
	bool resolution_needs_confirmation = false;
	bool show_about = false;

	D()
		:
		window_about(std::make_unique<WindowAbout>()),
		window_resolution_popup(std::make_unique<WindowResolutionPopup>()) {
	}

	void reset_display_settings_to_factual(SDL_Window *window) {
		video_mode_settings = get_window_video_mode_settings(window);
		original_video_mode_settings = video_mode_settings;
	}

	VideoModeSettings merge_factual_window_settings_with_settings_from_ui_with(
		Settings &app_settings,
		SDL_Window *window
	) const {
		const VideoModeSettings factual_settings = get_window_video_mode_settings(window);
		VideoModeSettings settings = factual_settings;
		settings.display_mode = this->video_mode_settings.display_mode;
		switch (this->video_mode_settings.display_mode) {
		case DisplayMode::WINDOWED:
			if (factual_settings.display_mode != DisplayMode::WINDOWED) {
				// When going to windowed mode from non-windowed,
				// load the window settings from app settings.
				settings.display_settings.x = app_settings.windowed_x;
				settings.display_settings.y = app_settings.windowed_y;
				settings.display_settings.width = app_settings.windowed_width;
				settings.display_settings.height = app_settings.windowed_height;
				settings.display_settings.maximized = app_settings.windowed_maximized;
			}
			break;
		case DisplayMode::FULLSCREEN:
			settings.display_id = this->video_mode_settings.display_id;
			settings.display_settings = this->video_mode_settings.display_settings;
			break;
		case DisplayMode::BORDERLESS_FULLSCREEN:
			settings.display_id = this->video_mode_settings.display_id;
			// No extra settings to merge
			break;
		}
		return settings;
	}

	void request_display_settings_confirmation(std::chrono::seconds timeout_duration = 10s) {
		this->resolution_needs_confirmation = true;
		this->window_resolution_popup->set_deadline(
			std::chrono::steady_clock::now() + timeout_duration
		);
	}
};

WindowSettings::WindowSettings()
	: d(std::make_unique<D>()) {
}

WindowSettings::~WindowSettings() = default;

void WindowSettings::draw(const GuiContext &guictx, bool *p_open) {
	SDL_Window *const main_window = guictx.app.window();

	ImGui::SetNextWindowSize({ 300.0f, 0.0f }, ImGuiCond_FirstUseEver);
	ImGui::Begin(WINDOW_SETTINGS_TITLE.c_str(), p_open);
	if (ImGui::IsWindowAppearing()) {
		guictx.app.logger().info() << "Settings window opened" << std::endl;
		d->reset_display_settings_to_factual(main_window);
	}

	draw_app_info(guictx);
	ImGui::Separator();
	draw_display_settings(guictx);
	ImGui::Separator();
	draw_fps_settings(guictx);
	ImGui::Separator();
	draw_ui_settings(guictx);
	ImGui::Separator();
	draw_gizmo_settings(guictx);
	ImGui::Separator();
	draw_background_settings(guictx);

	// Display confirmation pop-up
	if (d->resolution_needs_confirmation) {
		draw_display_confirmation_popup(guictx);
	}

	if (d->resolution_just_changed) {
		d->request_display_settings_confirmation();
		d->resolution_just_changed = false;
	}
	ImGui::End();
}

void WindowSettings::draw_app_info(const GuiContext &guictx) {
	// Application info
	if (ImGui::Button("Quit")) {
		guictx.app.quit();
	}
	ImGui::SameLine();
	if (ImGui::Button("About")) {
		d->show_about = true;
	}
	// About
	if (d->show_about) {
		const std::string title = "About"s + app_name();
		d->window_about->draw(guictx, &d->show_about);
	}
}

void WindowSettings::draw_display_settings(const GuiContext &guictx) {
	SDL_Window *const main_window = guictx.app.window();

	// Window display mode selection
	static const std::array<DisplayMode, 3> available_display_modes = {
		DisplayMode::WINDOWED,
		DisplayMode::FULLSCREEN,
		DisplayMode::BORDERLESS_FULLSCREEN,
	};
	if (
		ImGui::BeginCombo(
			"Display mode",
			get_display_mode_label(d->video_mode_settings.display_mode).c_str()
		)
	) {
		for (const auto &mode : available_display_modes) {
			const bool is_selected = (mode == d->video_mode_settings.display_mode);
			const std::string label = get_display_mode_label(mode);
			if (ImGui::Selectable(label.c_str(), is_selected)) {
				d->video_mode_settings.display_mode = mode;
				if (!is_selected && mode == DisplayMode::FULLSCREEN) {
					// When switching to fullscreen, pick the best available mode
					d->video_mode_settings = get_best_fullscreen_settings_for_window_and_display(
						main_window,
						d->video_mode_settings.display_id
					);
				}
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	// Display selection
	if (d->video_mode_settings.display_mode != DisplayMode::WINDOWED) {
		const DisplayInfo current_display_info = get_display_info(d->video_mode_settings.display_id);
		if (ImGui::BeginCombo("Display", current_display_info.name.c_str())) {
			std::vector<DisplayInfo> displays_info = get_available_displays_info();
			for (const auto &display_info : displays_info) {
				const bool is_selected = (display_info.id == current_display_info.id);
				if (ImGui::Selectable(display_info.name.c_str(), is_selected)) {
					d->video_mode_settings.display_id = display_info.id;
					if (!is_selected && d->video_mode_settings.display_mode == DisplayMode::FULLSCREEN) {
						// When changing display in fullscreen mode, pick the best available mode
						d->video_mode_settings = get_best_fullscreen_settings_for_window_and_display(
							main_window,
							d->video_mode_settings.display_id
						);
					}
				}
			}
			ImGui::EndCombo();
		}
	}
	// Fullscreen display mode selection
	if (d->video_mode_settings.display_mode == DisplayMode::FULLSCREEN) {
		SDL_DisplayID display_id = d->video_mode_settings.display_id;
		if (display_id == 0) {
			display_id = SDL_GetDisplayForWindow(main_window);
		}
		const std::vector<VideoModeSettings> video_mode_settings_list =
			get_available_display_modes(display_id);
		const std::string current_resolution_label =
			get_resolution_label(d->video_mode_settings.display_settings);
		if (ImGui::BeginCombo("Resolution", current_resolution_label.c_str())) {
			for (const auto &video_mode_settings : video_mode_settings_list) {
				const bool is_selected =
					video_mode_settings.display_settings == d->video_mode_settings.display_settings;
				const DisplaySettings &display_settings = video_mode_settings.display_settings;
				const std::string label = get_resolution_label(display_settings);
				if (ImGui::Selectable(label.c_str(), is_selected)) {
					d->video_mode_settings.display_settings = display_settings;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}
	// The "Set" button
	if (
		ImGui::Button("Set")
	) {
		const VideoModeSettings factual_video_mode_settings =
			get_window_video_mode_settings(main_window);
		const VideoModeSettings new_video_mode_settings =
			d->merge_factual_window_settings_with_settings_from_ui_with(guictx.app.settings(), main_window);
		if (new_video_mode_settings != factual_video_mode_settings) {
			const std::string display_mode_label = get_display_mode_label(new_video_mode_settings.display_mode);
			const DisplayInfo display_info = get_display_info(new_video_mode_settings.display_id);
			guictx.app.logger().info() << "Changing display mode to " << display_mode_label << std::endl;
			if (new_video_mode_settings.display_id != factual_video_mode_settings.display_id) {
				guictx.app.logger().info() << "Changing display to " << display_info.name << std::endl;
			}
			if (new_video_mode_settings.display_settings != factual_video_mode_settings.display_settings) {
				guictx.app.logger().info() << "Changing resolution to "
					<< get_resolution_label(new_video_mode_settings.display_settings)
					<< std::endl;
			}
			d->original_video_mode_settings = factual_video_mode_settings;
			if (
				set_window_video_mode_settings(
					main_window,
					new_video_mode_settings
				)
			) {
				d->resolution_just_changed = true;
				d->request_display_settings_confirmation();
			} else {
				guictx.app.logger().error() << "Failed to set display mode to " << display_mode_label << std::endl;
			}
		}
	}
	// The "Reload" button
	ImGui::SameLine();
	if (ImGui::Button("Reload")) {
		d->reset_display_settings_to_factual(main_window);
		guictx.app.logger().info() << "Reloaded factual display settings" <<
			std::endl;
	}
	ImGui::SetItemTooltip("Reload display settings from the actual window settings");
}

void WindowSettings::draw_display_confirmation_popup(const GuiContext &guictx) {
	WindowResolutionPopup::Result confirmation_result = d->window_resolution_popup->draw(guictx);
	if (confirmation_result == WindowResolutionPopup::Result::CONTINUE)
		return;

	d->resolution_needs_confirmation = false;
	if (confirmation_result == WindowResolutionPopup::Result::KEEP) {
		guictx.app.logger().info() << "Accepted new resolution" << std::endl;
		save_window_video_settings(
			guictx.app.settings(),
			guictx.app.window()
		);
	} else if (confirmation_result == WindowResolutionPopup::Result::REVERT) {
		guictx.app.logger().info() << "Reverting to original resolution" << std::endl;
		if (
			!set_window_video_mode_settings(
				guictx.app.window(),
				d->original_video_mode_settings
			)
		) {
			guictx.app.logger().error() << "Failed to reset display mode to original settings" << std::endl;
		}
	}
	d->reset_display_settings_to_factual(guictx.app.window());
}

void WindowSettings::draw_fps_settings(const GuiContext &guictx) {
	ImGui::Checkbox("Show FPS", &guictx.app.settings().show_fps);
	ImGui::Checkbox("Show UI frame counter", &guictx.app.settings().show_ui_frame_counter);
	ImGui::SetNextItemWidth(80.0f);
	if (
		ImGui::DragFloat(
			"Target FPS", &guictx.app.settings().target_fps,
			1.0, 10.0f, 10000.0f, "%.3fFPS", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp
		)
	) {
		guictx.app.recalculate_fps_clock();
	}
}

void WindowSettings::draw_ui_settings(const GuiContext &guictx) {
	ImGui::Checkbox("Show help", &guictx.app.settings().show_help);
	ImGui::Checkbox("Show help at start", &guictx.app.settings().show_help_at_start);
	ImGui::Checkbox("Show settings at start", &guictx.app.settings().show_settings_at_start);
	ImGui::Checkbox("Show program log", &guictx.app.settings().show_program_log);
	ImGui::Checkbox("Show joystick info", &guictx.app.settings().show_joystick_info);
}

void WindowSettings::draw_gizmo_settings(const GuiContext &guictx) {
	// Gizmos
	const std::string remove_all_gizmos_label =
		"Remove all gizmos ("s + std::to_string(guictx.app.arena().gizmos().size()) + ")"s;
	if (ImGui::Button(remove_all_gizmos_label.c_str()))
		guictx.app.arena().remove_all_gizmos();
	ImGui::SetNextItemWidth(60.0f);
	if (
		ImGui::DragInt(
			"Gizmo width", &guictx.app.settings().gizmo_width,
			1, 1, 1000, "%dpx", ImGuiSliderFlags_AlwaysClamp
		)
	) {
		guictx.app.arena().set_gizmos_width(guictx.app.settings().gizmo_width);
	}
	ImGui::SetNextItemWidth(60.0f);
	if (
		ImGui::DragInt(
			"Gizmo height", &guictx.app.settings().gizmo_height,
			1, 1, 1000, "%dpx", ImGuiSliderFlags_AlwaysClamp
		)
	) {
		guictx.app.arena().set_gizmos_height(guictx.app.settings().gizmo_height);
	}
	// Joystick configuration
	ImGui::SetNextItemWidth(120.0f);
	ImGui::DragInt(
		"Joystick deadzone", &guictx.app.settings().joystick_deadzone,
		50.0f, 0, SDL_JOYSTICK_AXIS_MAX, "%d", ImGuiSliderFlags_AlwaysClamp
	);
}

void WindowSettings::draw_background_settings(const GuiContext &guictx) {
	ImGui::SetNextItemWidth(150.0f);
	ImGui::ColorEdit3("Background", &guictx.app.settings().background_color.value[0]);
	ImGui::Checkbox("Cycle background", &guictx.app.settings().background_animate);
	ImGui::Checkbox("Flash on gizmo action", &guictx.app.settings().background_flash_on_gizmo_action);
	ImGui::SetNextItemWidth(150.0f);
	ImGui::ColorEdit3("Background flash", &guictx.app.settings().background_flash_color[0]);
}

}
