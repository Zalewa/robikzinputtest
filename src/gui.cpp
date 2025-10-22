#include "gui.hpp"
#include "sdl_event.hpp"
#include "version.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

namespace robikzinputtest {

static bool is_gui_demo_key(const SDL_KeyboardEvent &event) {
	return (
		event.type == SDL_EVENT_KEY_DOWN
		&& event.key == SDLK_F1
	);
}

static bool is_gui_demo_event(const SDL_Event &event) {
	return is_gui_demo_key(event.key);
}

static bool is_gui_defocus_button(const SDL_JoyButtonEvent &event) {
	return (
		event.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN
		&& event.button == 6 // options on xbox gamepad
	);
}

static bool is_gui_defocus_event(const SDL_Event &event) {
	return is_gui_defocus_button(event.jbutton);
}

static bool is_imgui_swallowing_event(const SDL_Event &event) {
	ImGuiIO &io = ImGui::GetIO();
	return (sdl::is_keyboard_event(event) && io.WantCaptureKeyboard)
		|| (sdl::is_joystick_event(event) && io.WantCaptureKeyboard)
		|| (sdl::is_mouse_event(event) && io.WantCaptureMouse);
}

struct Gui::D {
	SDL_Window &window;
	SDL_Renderer &renderer;

	bool imgui_init_context = false;
	bool imgui_init_platform = false;
	bool imgui_init_renderer = false;

	bool gui_demo_enabled = false;

	D(
		SDL_Window &window,
		SDL_Renderer &renderer
	) : window(window),
		renderer(renderer) {}
};

Gui::Gui(
	SDL_Window &window,
	SDL_Renderer &renderer
) : d(std::make_unique<D>(window, renderer)) {
}

Gui::~Gui() {
	close();
}

bool Gui::init() {
	IMGUI_CHECKVERSION();
	d->imgui_init_context = ImGui::CreateContext() != nullptr;
	if (!d->imgui_init_context)
		return false;

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	d->imgui_init_platform = ImGui_ImplSDL3_InitForSDLRenderer(&d->window, &d->renderer);
	if (!d->imgui_init_platform)
		return false;
	d->imgui_init_renderer =  ImGui_ImplSDLRenderer3_Init(&d->renderer);
	if (!d->imgui_init_renderer)
		return false;

	return true;
}

void Gui::close() {
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

bool Gui::is_demo_enabled() const {
	return d->gui_demo_enabled;
}

void Gui::set_demo_enabled(bool enabled) {
	d->gui_demo_enabled = enabled;
}

bool Gui::handle_event(SDL_Event &event) {
	if (
		is_gui_defocus_event(event)
		&& is_imgui_swallowing_event(event)
	) {
		clear_focus();
		return true;
	}

	if (is_gui_demo_event(event)) {
		d->gui_demo_enabled = !d->gui_demo_enabled;
		return true;
	}

	ImGui_ImplSDL3_ProcessEvent(&event);
	return is_imgui_swallowing_event(event);
}

void Gui::iterate(
	const FrameTime &frame_time
) {
	ImGuiIO &imgui_io = ImGui::GetIO();
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin(app_full_signature().c_str());
	ImGui::Text("Welcome to Dear ImGui with SDL3!");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / imgui_io.Framerate, imgui_io.Framerate);
	ImGui::End();

	if (d->gui_demo_enabled)
		ImGui::ShowDemoWindow(&d->gui_demo_enabled);

	ImGui::Render();

	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), &d->renderer);
}

}
