#include "gui_window_about.hpp"

#include "gui_context.hpp"
#include "version.hpp"

#include <imgui.h>

#include <string>

namespace robikzinputtest::gui {

using namespace std::literals;

void WindowAbout::draw(const GuiContext &guictx, bool *p_open) {
	const std::string title = "About "s + app_name();
	if (ImGui::Begin(title.c_str(), p_open)) {
		ImGui::Text("%s", app_full_signature().c_str());
		if (ImGui::Button("Copy")) {
			ImGui::SetClipboardText(app_full_signature().c_str());
		}
		ImGui::Separator();
		ImGui::Text("%s", app_copyright().c_str());
		ImGui::Text(
			"%s is licensed under %s.",
			app_name().c_str(),
			app_license().c_str()
		);
	}
	ImGui::End();
}

} // namespace robikzinputtest::gui
