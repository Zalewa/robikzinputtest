#pragma once

namespace robikzinputtest::gui {

struct GuiContext;

class WindowAbout {
public:
	void draw(const GuiContext &guictx, bool *p_open = nullptr);
};

} // namespace robikzinputtest::gui
