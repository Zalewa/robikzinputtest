#include "gui_window_about.hpp"

#include "gui_context.hpp"
#include "version.hpp"

#include <SDL3/SDL.h>
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
		ImGui::SeparatorText("Dear ImGUI");
		ImGui::Text(
			"This software uses Dear ImGUI version %s.",
			ImGui::GetVersion()
		);
		ImGui::TextLinkOpenURL(
			"Dear ImGUI Homepage",
			"https://github.com/ocornut/imgui"
		);
		ImGui::SeparatorText("Simple DirectMedia Layer");
		const auto sdl_version = SDL_GetVersion();
		ImGui::Text(
			"This software uses Simple DirectMedia Layer version %d.%d.%d.",
			SDL_VERSIONNUM_MAJOR(sdl_version),
			SDL_VERSIONNUM_MINOR(sdl_version),
			SDL_VERSIONNUM_MICRO(sdl_version)
		);
		ImGui::TextLinkOpenURL(
			"Simple DirectMedia Layer Homepage",
			"https://www.libsdl.org/"
		);
	}
	ImGui::End();
}

} // namespace robikzinputtest::gui
