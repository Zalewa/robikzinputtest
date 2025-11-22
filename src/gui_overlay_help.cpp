#include "gui_overlay_help.hpp"

#include "gui_context.hpp"
#include "imgui_defs.hpp"

#include <imgui.h>

namespace robikzinputtest::gui {

void overlay_help(const GuiContext &guictx) {
	ImGuiIO &imgui_io = ImGui::GetIO();
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::Begin("HELP Overlay", nullptr, imgui::overlay_flags);
	ImGui::Text(
		"Press F5 or START to open settings\n"
		"Press ENTER or SPACE to spawn keyboard Gizmo\n"
		"Press any button on controller to spawn controller Gizmo\n"
		"Press CTRL+Q to quit\n"
	);
	ImGui::End();
}

} // namespace robikzinputtest::gui
