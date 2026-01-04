#include "imgui_style.hpp"

#include <imgui.h>

namespace robikzinputtest::imgui {

static void push_color_alpha(ImGuiCol idx, float alpha) {
	ImVec4 color = ImGui::GetStyleColorVec4(idx);
	color.w = alpha;
	ImGui::PushStyleColor(idx, color);
}

void push_window_opacity(float opacity) {
	push_color_alpha(ImGuiCol_WindowBg, opacity);
	push_color_alpha(ImGuiCol_TitleBg, opacity);
	push_color_alpha(ImGuiCol_TitleBgActive, opacity);
	push_color_alpha(ImGuiCol_TitleBgCollapsed, opacity);
}

void pop_window_opacity() {
	// amount must be in sync with colors from push_window_opacity()
	ImGui::PopStyleColor(4);
}

} // namespace robikzinputtest::imgui
