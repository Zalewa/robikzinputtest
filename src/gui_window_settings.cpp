#include "gui_window_settings.hpp"

#include "app.hpp"
#include "arena.hpp"
#include "gui_context.hpp"
#include "settings.hpp"
#include "version.hpp"

#include <imgui.h>

#include <string>

namespace robikzinputtest::gui {

const std::string WINDOW_SETTINGS_TITLE = "Settings";

void window_settings(const GuiContext &guictx) {
	using namespace std::literals;

	ImGui::SetNextWindowSize({ 300.0f, 0.0f }, ImGuiCond_FirstUseEver);
	ImGui::Begin(WINDOW_SETTINGS_TITLE.c_str());

	// Application info
	ImGui::Text("%s", app_name().c_str());
	ImGui::Text("v%s", app_version().c_str());
	if (ImGui::Button("Quit")) {
		guictx.app.quit();
	}
	ImGui::Separator();
	// FPS settings
	ImGui::Checkbox("Show FPS", &guictx.app.settings().show_fps);
	ImGui::SetNextItemWidth(80.0f);
	if (
		ImGui::DragFloat(
			"Target FPS", &guictx.app.settings().target_fps,
			1.0, 10.0f, 10000.0f, "%.3fFPS", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp
		)
	) {
		guictx.app.recalculate_fps_clock();
	}
	ImGui::Separator();

	// UI settings
	ImGui::Checkbox("Show help", &guictx.app.settings().show_help);
	ImGui::Checkbox("Show help at start", &guictx.app.settings().show_help_at_start);
	ImGui::Checkbox("Show settings at start", &guictx.app.settings().show_settings_at_start);
	ImGui::Separator();

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
	ImGui::SetNextItemWidth(120.0f);
	ImGui::DragInt(
		"Joystick deadzone", &guictx.app.settings().joystick_deadzone,
		50.0f, 0, SDL_JOYSTICK_AXIS_MAX, "%d", ImGuiSliderFlags_AlwaysClamp
	);

	ImGui::Separator();
	// Background
	ImGui::SetNextItemWidth(150.0f);
	ImGui::ColorEdit3("Background", &guictx.app.settings().background_color.value[0]);
	ImGui::Checkbox("Cycle background", &guictx.app.settings().background_animate);
	ImGui::Checkbox("Flash on gizmo action", &guictx.app.settings().background_flash_on_gizmo_action);
	ImGui::SetNextItemWidth(150.0f);
	ImGui::ColorEdit3("Background flash", &guictx.app.settings().background_flash_color[0]);

	ImGui::End();
}

}
