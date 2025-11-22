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

	ImGui::Begin(WINDOW_SETTINGS_TITLE.c_str());
	ImGui::Text("%s", app_name().c_str());
	ImGui::Text("v%s", app_version().c_str());
	ImGui::Separator();
	ImGui::Checkbox("Show FPS", &guictx.app.settings().show_fps);
	ImGui::Checkbox("Show help", &guictx.app.settings().show_help);
	ImGui::Checkbox("Show help at start", &guictx.app.settings().show_help_at_start);
	ImGui::Checkbox("Show settings at start", &guictx.app.settings().show_settings_at_start);
	ImGui::Separator();
	const std::string remove_all_gizmos_label =
		"Remove all Gizmos ("s + std::to_string(guictx.app.arena().gizmos().size()) + ")"s;
	if (ImGui::Button(remove_all_gizmos_label.c_str()))
		guictx.app.arena().remove_all_gizmos();
	ImGui::End();
}

}
