#pragma once

#include <memory>
#include <string>

namespace robikzinputtest::gui {

struct GuiContext;

extern const std::string WINDOW_SETTINGS_TITLE;

class WindowSettings {
public:
	WindowSettings();
	~WindowSettings();

	void draw(const GuiContext &guictx, bool *p_open = nullptr);

private:
	struct D;
	std::unique_ptr<D> d;

	void draw_app_info(const GuiContext &guictx);
	void draw_display_settings(const GuiContext &guictx);
	void draw_display_confirmation_popup(const GuiContext &guictx);
	void draw_vsync_settings(const GuiContext &guictx);
	void draw_fps_settings(const GuiContext &guictx);
	void draw_ui_settings(const GuiContext &guictx);
	void draw_gizmo_settings(const GuiContext &guictx);
	void draw_background_settings(const GuiContext &guictx);
};

} // namespace robikzinputtest::gui
