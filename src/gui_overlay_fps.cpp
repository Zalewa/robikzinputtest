#include "gui_overlay_fps.hpp"

#include "app.hpp"
#include "gui_context.hpp"
#include "imgui_defs.hpp"
#include "settings.hpp"

#include <imgui.h>

namespace robikzinputtest::gui {

void overlay_fps(const GuiContext &guictx) {
	const bool show_fps = guictx.app.settings().show_fps;
	const bool show_ui_frame_counter = guictx.app.settings().show_ui_frame_counter;

	if (
		!show_fps
		&& !show_ui_frame_counter
	) {
		return;
	}

	ImGuiIO &imgui_io = ImGui::GetIO();
	ImGui::SetNextWindowPos(
		{ static_cast<float>(guictx.window_size.x), 0 },
		0,
		{ 1.0, 0 }
	);
	ImGui::Begin("FPS Overlay", nullptr, imgui::overlay_flags);
	if (show_fps) {
		ImGui::Text(
			"%.3f ms/frame (%.1f FPS)",
			1000.0f / imgui_io.Framerate,
			imgui_io.Framerate
		);
	}
	if (show_ui_frame_counter) {
		ImGui::Text("Frame: %d", ImGui::GetFrameCount());
	}
	ImGui::End();
}

} // namespace robikzinputtest::gui
