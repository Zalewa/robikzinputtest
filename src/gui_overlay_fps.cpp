#include "gui_overlay_fps.hpp"

#include "gui_context.hpp"
#include "imgui_defs.hpp"

#include <imgui.h>

namespace robikzinputtest::gui {

void overlay_fps(const GuiContext &guictx) {
	ImGuiIO &imgui_io = ImGui::GetIO();
	ImGui::SetNextWindowPos(
		{ static_cast<float>(guictx.window_size.x), 0 },
		0,
		{ 1.0, 0 }
	);
	ImGui::Begin("FPS Overlay", nullptr, imgui::overlay_flags);
	ImGui::Text(
		"%.3f ms/frame (%.1f FPS)",
		1000.0f / imgui_io.Framerate,
		imgui_io.Framerate
	);
	ImGui::End();
}

} // namespace robikzinputtest::gui
