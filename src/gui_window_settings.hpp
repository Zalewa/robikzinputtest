#pragma once

#include <string>

namespace robikzinputtest::gui {

struct GuiContext;

extern const std::string WINDOW_SETTINGS_TITLE;

void window_settings(const GuiContext &guictx, bool *p_open = nullptr);

} // namespace robikzinputtest::gui
