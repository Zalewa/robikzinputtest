#include "gui.hpp"
#include "app.hpp"
#include "logger.hpp"
#include "gui_context.hpp"
#include "gui_log.hpp"
#include "gui_overlay_fps.hpp"
#include "gui_overlay_help.hpp"
#include "gui_overlay_joystick.hpp"
#include "gui_window_program_log.hpp"
#include "gui_window_settings.hpp"
#include "sdl_event.hpp"
#include "settings.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

namespace robikzinputtest::gui {

using namespace std::literals;

static const auto DURATION_HELP_OVERLAY_SHOW = 5s;

static bool is_gui_demo_key(const SDL_KeyboardEvent &event) {
	return (
		event.type == SDL_EVENT_KEY_DOWN
		&& event.key == SDLK_F1
	);
}

static bool is_gui_demo_event(const SDL_Event &event) {
	return is_gui_demo_key(event.key);
}

static bool is_gui_settings_key(const SDL_KeyboardEvent &event) {
	return (
		event.type == SDL_EVENT_KEY_DOWN
		&& event.key == SDLK_F5
	);
}

static bool is_gui_settings_joystick_button(const SDL_JoyButtonEvent &event) {
	return (
		event.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN
		&& event.button == 7
	);
}

static bool is_gui_settings_event(const SDL_Event &event) {
	return is_gui_settings_key(event.key)
		|| is_gui_settings_joystick_button(event.jbutton);
}

static bool is_gui_defocus_button(const SDL_JoyButtonEvent &event) {
	return (
		event.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN
		&& event.button == 6 // options on xbox gamepad
	);
}

static bool is_gui_defocus_key(const SDL_KeyboardEvent &event) {
	return (
		event.type == SDL_EVENT_KEY_DOWN
		&& event.key == SDLK_ESCAPE
	);
}

static bool is_gui_defocus_event(const SDL_Event &event) {
	return is_gui_defocus_button(event.jbutton)
		|| is_gui_defocus_key(event.key);
}

static bool is_gui_focus_key(const SDL_KeyboardEvent &event) {
	return (
		event.type == SDL_EVENT_KEY_DOWN
		&& event.key == SDLK_ESCAPE
	);
}

static bool is_gui_focus_event(const SDL_Event &event) {
	return is_gui_focus_key(event.key);
}

static bool is_imgui_swallowing_event(const SDL_Event &event) {
	ImGuiIO &io = ImGui::GetIO();
	return (sdl::is_keyboard_event(event) && io.WantCaptureKeyboard)
		|| (sdl::is_joystick_event(event) && io.WantCaptureKeyboard)
		|| (sdl::is_mouse_event(event) && io.WantCaptureMouse);
}

struct Gui::D {
	App &app;
	SDL_Window &window;
	SDL_Renderer &renderer;

	HandlerId log_handler_id = 0;
	Log log;

	std::unique_ptr<WindowProgramLog> window_program_log;
	std::unique_ptr<WindowSettings> window_settings;

	bool imgui_init_context = false;
	bool imgui_init_platform = false;
	bool imgui_init_renderer = false;

	bool show_imgui_demo = false;
	bool show_settings_window = false;

	ExpirableValue<bool> show_help_overlay = { {}, false };

	D(
		App &app,
		SDL_Window &window,
		SDL_Renderer &renderer
	) : app(app),
		window(window),
		renderer(renderer) {}
};

Gui::Gui(
	App &app,
	SDL_Window &window,
	SDL_Renderer &renderer
) : d(std::make_unique<D>(app, window, renderer)) {
}

Gui::~Gui() {
	close();
}

bool Gui::init() {
	d->log_handler_id = d->app.logger().on_logrecord.add(
		[&](const LogRecord &record) {
			d->log.add(record);
		}
	);

	IMGUI_CHECKVERSION();
	d->imgui_init_context = ImGui::CreateContext() != nullptr;
	if (!d->imgui_init_context)
		return false;

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.IniFilename = nullptr; // disable spawn of imgui.ini

	d->imgui_init_platform = ImGui_ImplSDL3_InitForSDLRenderer(&d->window, &d->renderer);
	if (!d->imgui_init_platform)
		return false;
	d->imgui_init_renderer =  ImGui_ImplSDLRenderer3_Init(&d->renderer);
	if (!d->imgui_init_renderer)
		return false;

	if (d->app.settings().show_help_at_start) {
		d->show_help_overlay = {
			static_cast<double>(std::chrono::seconds(DURATION_HELP_OVERLAY_SHOW).count()),
			true,
		};
	}
	d->show_settings_window = d->app.settings().show_settings_at_start;

	d->window_settings = std::make_unique<WindowSettings>();
	d->window_program_log = std::make_unique<WindowProgramLog>(d->log);

	return true;
}

void Gui::close() {
	d->app.logger().on_logrecord.remove(d->log_handler_id);
	d->window_program_log.reset();
	d->window_settings.reset();
	if (d->imgui_init_renderer) {
		ImGui_ImplSDLRenderer3_Shutdown();
		d->imgui_init_renderer = false;
	}
	if (d->imgui_init_platform) {
		ImGui_ImplSDL3_Shutdown();
		d->imgui_init_platform = false;
	}
	if (d->imgui_init_context) {
		ImGui::DestroyContext();
		d->imgui_init_context = false;
	}
}

void Gui::clear_focus() {
	ImGui::SetWindowFocus(nullptr);
}

void Gui::grab_focus() {
	ImGui::SetWindowFocus(WINDOW_SETTINGS_TITLE.c_str());
}

bool Gui::is_demo_enabled() const {
	return d->show_imgui_demo;
}

void Gui::set_demo_enabled(bool enabled) {
	d->show_imgui_demo = enabled;
}

bool Gui::handle_event(SDL_Event &event) {
	if (
		is_gui_focus_event(event)
		&& !is_imgui_swallowing_event(event)
	) {
		grab_focus();
		return true;
	}

	if (
		is_gui_defocus_event(event)
		&& is_imgui_swallowing_event(event)
	) {
		clear_focus();
		return true;
	}

	if (is_gui_demo_event(event)) {
		d->show_imgui_demo = !d->show_imgui_demo;
		return true;
	} else if (is_gui_settings_event(event)) {
		d->show_settings_window = !d->show_settings_window;
		return true;
	}

	ImGui_ImplSDL3_ProcessEvent(&event);
	return is_imgui_swallowing_event(event);
}

void Gui::iterate(
	const FrameTime &frame_time
) {
	GuiContext guictx = { d->app, frame_time };
	SDL_GetWindowSize(&d->window, &guictx.window_size.x, &guictx.window_size.y);

	ImGuiIO &imgui_io = ImGui::GetIO();
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// Overlays
	if (d->show_help_overlay || d->app.settings().show_help) {
		if (
			!d->app.settings().show_help &&
			d->show_help_overlay.countdown(frame_time.delta_seconds)
		) {
			d->show_help_overlay.value = false;
		} else {
			overlay_help(guictx);
		}
	}
	overlay_fps(guictx);
	if (d->app.settings().show_joystick_info) {
		overlay_joystick(guictx);
	}

	// Windows
	if (d->app.settings().show_program_log) {
		d->window_program_log->draw(guictx);
	}
	if (d->show_settings_window) {
		d->window_settings->draw(guictx, &d->show_settings_window);
	}

	if (d->show_imgui_demo)
		ImGui::ShowDemoWindow(&d->show_imgui_demo);

	ImGui::Render();

	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), &d->renderer);
}

} // namespace robikzinputtest::gui
