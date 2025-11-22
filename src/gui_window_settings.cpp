#include "gui_window_settings.hpp"

#include "app.hpp"
#include "gui_context.hpp"
#include "settings.hpp"
#include "version.hpp"

#include <imgui.h>

namespace robikzinputtest::gui {

const std::string WINDOW_SETTINGS_TITLE = app_full_signature();

void window_settings(const GuiContext &guictx) {
	ImGui::Begin(WINDOW_SETTINGS_TITLE.c_str());
	ImGui::Checkbox("Show FPS", &guictx.app.settings().show_fps);
	ImGui::End();
}

}
