#include "gui_overlay_joystick.hpp"

#include "SDL3/SDL_joystick.h"
#include "app.hpp"
#include "gui_context.hpp"
#include "imgui_defs.hpp"
#include "settings.hpp"

#include <imgui.h>

namespace robikzinputtest::gui {

void overlay_joystick(const GuiContext &guictx) {
	ImGuiIO &imgui_io = ImGui::GetIO();
	ImGui::SetNextWindowPos(
		{ static_cast<float>(guictx.window_size.x), 60.0f },
		0,
		{ 1.0, 0 }
	);
	ImGui::SetNextWindowSize({ 0, 0 }, ImGuiCond_Always);
	ImGui::Begin("Joystick Overlay", nullptr, imgui::overlay_flags);
	const OpenedJoysticksMap &joysticks = guictx.app.joysticks();
	ImGui::Text("Joystick Count: %zu", joysticks.size());
	ImGui::Text("Joystick Deadzone: %d", guictx.app.settings().joystick_deadzone);
	SDL_LockJoysticks();
	for (const auto &joypair : joysticks) {
		auto joy_id = joypair.first;
		auto joystick = joypair.second.get();

		std::ostringstream ss_title;
		ss_title << "Joystick " << joy_id;
		ImGui::SeparatorText(ss_title.str().c_str());

		const int n_axes = SDL_GetNumJoystickAxes(joystick);
		for (int axis = 0; axis < n_axes; ++axis) {
			const int axis_value = SDL_GetJoystickAxis(joystick, axis);
			ImGui::Text("Axis %d: %d", axis, axis_value);
		}
		const int n_buttons = SDL_GetNumJoystickButtons(joystick);
		std::ostringstream ss_buttons;
		for (int button = 0; button < n_buttons; ++button) {
			const bool down = SDL_GetJoystickButton(joystick, button);
			if (down) {
				ss_buttons << button << " ";
			}
		}
		ImGui::Text("Buttons: %s", ss_buttons.str().c_str());
	}
	SDL_UnlockJoysticks();
	ImGui::End();
}

} // namespace robikzinputtest::gui
