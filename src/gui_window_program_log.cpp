#include "gui_window_program_log.hpp"

#include "app.hpp"
#include "gui_context.hpp"
#include "gui_logbox.hpp"
#include "settings.hpp"

#include <imgui.h>

#include <limits>

namespace robikzinputtest::gui {

void WindowProgramLog::draw(const GuiContext &guictx) {
	const float fixed_width = static_cast<float>(guictx.window_size.x);
	ImGui::SetNextWindowSize(
		{ fixed_width, 200.0f },
		ImGuiCond_Once
	);
	ImGui::SetNextWindowPos(
		{
			0,
			static_cast<float>(guictx.window_size.y),
		},
		ImGuiCond_Always,
		{ 0, 1 }
	);
	ImGui::SetNextWindowSizeConstraints(
		{ fixed_width, 0.0f },
		{ fixed_width, std::numeric_limits<float>::max() }
	);

	auto extra_options = [&guictx]() {
		ImGui::Checkbox(
			"Log joystick axis events",
			&guictx.app.settings().log_joystick_axis_events
		);
		ImGui::Checkbox(
			"Log joystick button events",
			&guictx.app.settings().log_joystick_button_events
		);
	};

	m_logbox.draw(
		this->m_log,
		"Program Log",
		&guictx.app.settings().show_program_log,
		&guictx.app.settings().program_log_opacity,
		extra_options
	);
}

} // namespace robikzinputtest::gui
