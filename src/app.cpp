#include "app.hpp"
#include "arena.hpp"
#include "clock.hpp"
#include "controller.hpp"
#include "controller_system.hpp"
#include "version.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <SDL3/SDL.h>

#include <array>
#include <iostream>
#include <string>
#include <vector>

namespace {
constexpr int DEFAULT_WINDOW_WIDTH = 800;
constexpr int DEFAULT_WINDOW_HEIGHT = 600;

bool is_keyboard_event(const SDL_Event &event)
{
	return event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP;
}

bool is_mouse_event(const SDL_Event &event)
{
	return event.type == SDL_EVENT_MOUSE_BUTTON_DOWN
		|| event.type == SDL_EVENT_MOUSE_BUTTON_UP
		|| event.type == SDL_EVENT_MOUSE_MOTION
		|| event.type == SDL_EVENT_MOUSE_WHEEL;
}

bool is_quit_key(const SDL_KeyboardEvent &key)
{
	return (key.key == SDLK_Q && (key.mod & SDL_KMOD_CTRL));
}

bool is_alt_enter(const SDL_KeyboardEvent &key)
{
	return key.key == SDLK_RETURN
		&& (key.mod & SDL_KMOD_ALT);
}

bool is_gui_demo_key(const SDL_KeyboardEvent &key)
{
	return key.key == SDLK_F1;
}

bool is_keyboard_gizmo_create_key(const SDL_KeyboardEvent &key)
{
	return key.key == SDLK_RETURN || key.key == SDLK_SPACE;
}

bool is_joystick_gizmo_create_key(const SDL_JoyButtonEvent &jbutton)
{
	// Any button creates a gizmo
	return jbutton.down;
}

bool is_imgui_swallowing_event(const SDL_Event &event)
{
	ImGuiIO &io = ImGui::GetIO();
	return (is_keyboard_event(event) && io.WantCaptureKeyboard)
		|| (is_mouse_event(event) && io.WantCaptureMouse);
}

bool is_keyboard_priority_event(const SDL_Event &event)
{
	return is_keyboard_event(event)
		&& (
			is_quit_key(event.key)
			|| is_alt_enter(event.key)
		);
}
} // namespace

namespace robikzinputtest {

struct App::D
{
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	bool imgui_init_context = false;
	bool imgui_init_platform = false;
	bool imgui_init_renderer = false;

	std::unique_ptr<Arena> arena;
	std::unique_ptr<ControllerSystem> controller_system;

	EngineClock clock;
	bool gui_demo_enabled = false;

	D()
	{
	}
};

App::App()
	: d(std::make_unique<D>())
{
}

App::~App()
{
	close();
}

AppRunResult App::init(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	// Hello App.
	std::cerr << app_full_signature() << std::endl;

	// Set application metadata
	SDL_SetAppMetadata(
		app_name().c_str(),
		app_version().c_str(),
		app_identifier().c_str()
	);
	SDL_SetAppMetadataProperty(
		SDL_PROP_APP_METADATA_CREATOR_STRING,
		app_author().c_str()
	);
	SDL_SetAppMetadataProperty(
		SDL_PROP_APP_METADATA_COPYRIGHT_STRING,
		app_copyright().c_str()
	);

	// Initialize SDL
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return AppRunResult::FAILURE;
	}

	// Initialize controller system
	d->controller_system = std::make_unique<ControllerSystem>();

	// Limit clock to 60FPS (TODO: for now)
	d->clock.set_resolution(std::chrono::microseconds(16666));

	// Create a window
	const std::string title = app_full_signature();
	d->window = SDL_CreateWindow(
		title.c_str(),
		DEFAULT_WINDOW_WIDTH,
		DEFAULT_WINDOW_HEIGHT,
		SDL_WINDOW_RESIZABLE
	);
	if (d->window == nullptr) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return AppRunResult::FAILURE;
	}

	// Create a renderer
	d->renderer = SDL_CreateRenderer(d->window, nullptr);
	if (d->renderer == nullptr) {
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return AppRunResult::FAILURE;
	}

	// Create ImGUI
	IMGUI_CHECKVERSION();
	d->imgui_init_context = ImGui::CreateContext() != nullptr;
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	d->imgui_init_platform = ImGui_ImplSDL3_InitForSDLRenderer(d->window, d->renderer);
	d->imgui_init_renderer =  ImGui_ImplSDLRenderer3_Init(d->renderer);

	// Create the arena
	d->arena = std::make_unique<Arena>();
	d->arena->load_render(*d->renderer);
	// Set arena bounds to the actual window size
	SDL_Point window_size;
	SDL_GetWindowSize(d->window, &window_size.x, &window_size.y);
	d->arena->set_bounds({ 0, 0, window_size.x, window_size.y });

	return AppRunResult::CONTINUE;
}

AppRunResult App::run()
{
	while (1) {
		FrameTime frame_time = d->clock.tick();
		const AppRunResult event_result = handleEvents(frame_time);
		if (event_result != AppRunResult::CONTINUE) {
			return event_result;
		}
		const AppRunResult iterate_result = iterate(frame_time);
		if (iterate_result != AppRunResult::CONTINUE) {
			return iterate_result;
		}
	}
	return AppRunResult::SUCCESS;
}

AppRunResult App::handleEvents(const FrameTime &frame_time)
{
	(void) frame_time;

	auto spawn_controller_gizmo = [this](Controller &controller) {
		if (d->arena->find_gizmo_for_controller(controller.id) == nullptr) {
			std::cerr << "Creating gizmo for " << controller.id.identifier << " controller." << std::endl;
			d->arena->create_gizmo(controller.id);
		}
	};

#define RBKZIT_LOG_JOYSTICK_AXIS 0
#define RBKZIT_LOG_JOYSTICK_BALL 0
#define RBKZIT_LOG_JOYSTICK_HAT 0

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		// Pass the events to ImGUI first
		ImGui_ImplSDL3_ProcessEvent(&event);
		if (is_imgui_swallowing_event(event)
			&& !is_keyboard_priority_event(event)
		) {
			continue;
		}
		// Handle app events
		switch (event.type) {
		case SDL_EVENT_QUIT:
			return AppRunResult::SUCCESS;
		case SDL_EVENT_KEY_DOWN:
			if (is_quit_key(event.key)) {
				return AppRunResult::SUCCESS;
			} else if (is_alt_enter(event.key)) {
				// Toggle fullscreen
				Uint32 flags = SDL_GetWindowFlags(d->window);
				if (flags & SDL_WINDOW_FULLSCREEN) {
					SDL_SetWindowFullscreen(d->window, 0);
				} else {
					SDL_SetWindowFullscreen(d->window, SDL_WINDOW_FULLSCREEN);
				}
				continue;
			} else if (is_gui_demo_key(event.key)) {
				d->gui_demo_enabled = !d->gui_demo_enabled;
				continue;
			} else if (is_keyboard_gizmo_create_key(event.key)) {
				// Create a gizmo for the keyboard
				Controller &controller = d->controller_system->for_keyboard();
				spawn_controller_gizmo(controller);
				continue;
			}
			break;
		case SDL_EVENT_WINDOW_RESIZED:
			// Update arena bounds
			d->arena->set_bounds({ 0, 0, event.window.data1, event.window.data2 });
			break;
		case SDL_EVENT_JOYSTICK_AXIS_MOTION: {
#if RBKZIT_LOG_JOYSTICK_AXIS
			std::cerr << "SDL_EVENT_JOYSTICK_AXIS_MOTION "
				<< "timestamp=" << event.jaxis.timestamp << ", "
				<< "which=" << event.jaxis.which << ", "
				<< "axis=" << static_cast<int32_t>(event.jaxis.axis) << ", "
				<< "value=" << event.jaxis.value
				<< std::endl;
#endif
			break;
		}
		case SDL_EVENT_JOYSTICK_BALL_MOTION:
#if RBKZIT_LOG_JOYSTICK_BALL
			std::cerr << "SDL_EVENT_JOYSTICK_BALL_MOTION"<< std::endl;
#endif
			break;
		case SDL_EVENT_JOYSTICK_HAT_MOTION:
#if RBKZIT_LOG_JOYSTICK_HAT
			std::cerr << "SDL_EVENT_JOYSTICK_HAT_MOTION" << std::endl;
#endif
			break;
		case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
			if (is_joystick_gizmo_create_key(event.jbutton)) {
				Controller &controller = d->controller_system->for_joystick(event.jbutton.which);
				spawn_controller_gizmo(controller);
			}
			break;
		}
		// Now pass the event to controllers
		d->controller_system->handle_event(event);
	}
	return AppRunResult::CONTINUE;
}

AppRunResult App::iterate(const FrameTime &frame_time)
{
	static const std::vector<std::array<uint8_t, 3>> colors = {
		// {255, 0, 0},   // Red
		// {0, 255, 0},   // Green
		// {0, 0, 255},   // Blue
		// {255, 255, 0}, // Yellow
		// {0, 255, 255}, // Cyan
		// {255, 0, 255}  // Magenta
		// Shades of green:
		{0, 50, 0},
		{0, 52, 0},
		{0, 54, 0},
		{0, 56, 0},
	};
	static int color_cycle_index = 0;
	const static Seconds time_color_change_rate = 0.5;
	static Seconds time_accumulator = 0.0;

	// Update arena
	d->arena->update(*d->controller_system, frame_time);

	// Render ImGUI
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

	// Cycle the background color
	time_accumulator += frame_time.delta_seconds;
	while (time_accumulator >= time_color_change_rate) {
		time_accumulator -= time_color_change_rate;
		color_cycle_index = (++color_cycle_index) % (colors.size());
	}
	const auto &color = colors[color_cycle_index];

	// Clear the screen with a color
	SDL_SetRenderDrawColor(d->renderer, color[0], color[1], color[2], 255);
	SDL_RenderClear(d->renderer);

	// Draw the arena
	d->arena->render(*d->renderer);

	// Draw ImGUI
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), d->renderer);

	// Present the backbuffer
	SDL_RenderPresent(d->renderer);

	return AppRunResult::CONTINUE;
}

void App::close()
{
	d->arena.reset();
	d->controller_system.reset();

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

	if (d->renderer) {
		SDL_DestroyRenderer(d->renderer);
		d->renderer = nullptr;
	}
	if (d->window) {
		SDL_DestroyWindow(d->window);
		d->window = nullptr;
	}
	SDL_Quit();
}

} // namespace robikzinputtest
