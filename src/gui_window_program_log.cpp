#include "gui_window_program_log.hpp"

#include "app.hpp"
#include "gui_context.hpp"
#include "gui_logbox.hpp"
#include "settings.hpp"
#include <imgui.h>

namespace robikzinputtest::gui {

void WindowProgramLog::draw(const GuiContext &guictx) {
	ImGui::SetNextWindowSize(
		ImVec2(guictx.window_size.x, 200),
		ImGuiCond_FirstUseEver
	);
	ImGui::SetNextWindowPos(
		{
			static_cast<float>(guictx.window_size.x),
			static_cast<float>(guictx.window_size.y),
		},
		ImGuiCond_Once,
		{ 1, 1 }
	);
	m_logbox.draw(this->m_log, "Program Log", &guictx.app.settings().show_program_log);
}

} // namespace robikzinputtest::gui
