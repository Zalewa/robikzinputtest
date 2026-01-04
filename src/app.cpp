#include "app.hpp"
#include "arena.hpp"
#include "clock.hpp"
#include "controller.hpp"
#include "controller_system.hpp"
#include "gizmo.hpp"
#include "gui.hpp"
#include "logger.hpp"
#include "sdl_event.hpp"
#include "sdl_settings.hpp"
#include "sdl_window.hpp"
#include "settings.hpp"
#include "version.hpp"
#include "video.hpp"
#include "video_settings.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <SDL3/SDL.h>

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace robikzinputtest {

namespace {
constexpr int DEFAULT_WINDOW_WIDTH = 800;
constexpr int DEFAULT_WINDOW_HEIGHT = 600;

bool is_quit_key(const SDL_KeyboardEvent &key)
{
	return (key.key == SDLK_Q && (key.mod & SDL_KMOD_CTRL));
}

bool is_alt_enter(const SDL_KeyboardEvent &key)
{
	return key.key == SDLK_RETURN
		&& (key.mod & SDL_KMOD_ALT);
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

bool is_keyboard_priority_event(const SDL_Event &event)
{
	using namespace robikzinputtest::sdl;
	return is_keyboard_event(event)
		&& (
			is_quit_key(event.key)
			|| is_alt_enter(event.key)
		);
}

bool is_app_input_priority_event(const SDL_Event &event) {
	return is_keyboard_priority_event(event);
}

} // namespace

struct App::D
{
	AppRunResult main_loop_result = AppRunResult::CONTINUE;
	Logger logger;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	Settings settings;

	OpenedJoysticksMap joysticks;

	std::unique_ptr<Arena> arena;
	std::unique_ptr<ControllerSystem> controller_system;
	std::unique_ptr<gui::Gui> gui;

	EngineClock clock;

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

	// Load settings
	sdl::SettingsSdlIO settings_io;
	auto settings_load_result = settings_io.load();
	d->settings = settings_load_result.second;

	// Initialize controller system
	d->controller_system = std::make_unique<ControllerSystem>(*this);

	// Limit clock to target FPS
	recalculate_fps_clock();

	// Create the window
	// Correct the window size first if the saved settings are invalid.
	if (d->settings.windowed_width <= 0) {
		d->settings.windowed_width = DEFAULT_WINDOW_WIDTH;
	}
	if (d->settings.windowed_height <= 0) {
		d->settings.windowed_height = DEFAULT_WINDOW_HEIGHT;
	}
	const std::string title = app_full_signature();
	d->window = SDL_CreateWindow(
		title.c_str(),
		d->settings.windowed_width,
		d->settings.windowed_height,
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

	// Load video settings into the window.
	// Must happen after the renderer is created, otherwise the 'y' position
	// of the window in WINDOWED mode is not restored properly for some reason.
	load_window_video_settings(d->settings, d->window);

	// Load VSync setting
	if (!SDL_SetRenderVSync(d->renderer, d->settings.vsync)) {
		std::cerr << "Failed to initialize VSync: " << SDL_GetError() << std::endl;
		// non-fatal error; continue
	}

	// Create GUI
	d->gui = std::make_unique<gui::Gui>(*this, *d->window, *d->renderer);
	if (!d->gui->init()) {
		return AppRunResult::FAILURE;
	}

	// Create the arena
	d->arena = std::make_unique<Arena>(*this);
	d->arena->load_render(*d->renderer);
	// Set arena bounds to the actual window size
	SDL_Point window_size;
	SDL_GetWindowSize(d->window, &window_size.x, &window_size.y);
	d->arena->set_bounds({ 0, 0, window_size.x, window_size.y });

	return AppRunResult::CONTINUE;
}

AppRunResult App::run()
{
	while (d->main_loop_result == AppRunResult::CONTINUE) {
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
	return d->main_loop_result;
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

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		// Pass the events to ImGUI first
		const bool handled_by_gui = d->gui->handle_event(event);
		if (handled_by_gui && !is_app_input_priority_event(event)) {
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
				const auto flags = SDL_GetWindowFlags(d->window);
				if (!(flags & SDL_WINDOW_FULLSCREEN)) {
					d->logger.info() << "Switching to fullscreen" << std::endl;
					SDL_SetWindowFullscreen(d->window, true);
				} else {
					d->logger.info() << "Switching to windowed" << std::endl;
					SDL_SetWindowFullscreen(d->window, false);
					SDL_SetWindowBordered(d->window, true);
				}
				settings().display_mode = static_cast<int>(get_window_display_mode(d->window));
				continue;
			} else if (is_keyboard_gizmo_create_key(event.key)) {
				// Create a gizmo for the keyboard
				Controller &controller = d->controller_system->for_keyboard();
				spawn_controller_gizmo(controller);
			}
			break;
		case SDL_EVENT_WINDOW_MOVED:
			if (sdl::is_window_normal(d->window)) {
				settings().windowed_x = event.window.data1;
				settings().windowed_y = event.window.data2;
			}
			break;
		case SDL_EVENT_WINDOW_RESIZED:
			// Update arena bounds
			d->arena->set_bounds({ 0, 0, event.window.data1, event.window.data2 });
			if (sdl::is_window_normal(d->window)) {
				settings().windowed_width = event.window.data1;
				settings().windowed_height = event.window.data2;
			}
			break;
		case SDL_EVENT_WINDOW_MAXIMIZED:
		case SDL_EVENT_WINDOW_RESTORED:
			settings().windowed_maximized = sdl::is_window_maximized(d->window);
			break;
		case SDL_EVENT_JOYSTICK_ADDED:
		case SDL_EVENT_JOYSTICK_REMOVED:
			d->logger.info() <<
				(
					event.jdevice.type == SDL_EVENT_JOYSTICK_ADDED
					? "JOYSTICK_ADDED "
					: "JOYSTICK_REMOVED "
				)
				<< "timestamp=" << event.jdevice.timestamp << ", "
				<< "which=" << event.jdevice.which << ", "
				<< "type=" << event.jdevice.type
				<< std::endl;
			if (event.type == SDL_EVENT_JOYSTICK_ADDED) {
				SDL_Joystick *joystick = SDL_OpenJoystick(event.jdevice.which);
				if (joystick) {
					d->joysticks.insert(
						{
							event.jdevice.which,
							JoystickUPtr(joystick, &SDL_CloseJoystick)
						}
					);
				}
			} else if (event.type == SDL_EVENT_JOYSTICK_REMOVED) {
				d->joysticks.erase(event.jdevice.which);
			}
			break;
		case SDL_EVENT_JOYSTICK_AXIS_MOTION: {
			if (d->settings.log_joystick_axis_events) {
				d->logger.info()
					<< "JOYSTICK_AXIS_MOTION "
					<< "timestamp=" << event.jaxis.timestamp << ", "
					<< "which=" << event.jaxis.which << ", "
					<< "axis=" << static_cast<int32_t>(event.jaxis.axis) << ", "
					<< "value=" << event.jaxis.value
					<< std::endl;
			}
			break;
		}
		case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
			if (d->settings.log_joystick_button_events) {
				d->logger.info()
					<< "JOYSTICK_BUTTON_DOWN "
					<< "timestamp=" << event.jaxis.timestamp << ", "
					<< "which=" << event.jaxis.which << ", "
					<< "button=" << static_cast<int32_t>(event.jbutton.button) << ", "
					<< "down=" << static_cast<int32_t>(event.jbutton.down)
					<< std::endl;
			}
			if (is_joystick_gizmo_create_key(event.jbutton)) {
				Controller &controller = d->controller_system->for_joystick(event.jbutton.which);
				spawn_controller_gizmo(controller);
			}
			break;
		case SDL_EVENT_JOYSTICK_BUTTON_UP:
			if (d->settings.log_joystick_button_events) {
				d->logger.info()
					<< "JOYSTICK_BUTTON_UP "
					<< "timestamp=" << event.jaxis.timestamp << ", "
					<< "which=" << event.jaxis.which << ", "
					<< "button=" << static_cast<int32_t>(event.jbutton.button) << ", "
					<< "down=" << static_cast<int32_t>(event.jbutton.down)
					<< std::endl;
			}
			break;
		case SDL_EVENT_JOYSTICK_HAT_MOTION:
			if (d->settings.log_joystick_button_events) {
				d->logger.info()
					<< "JOYSTICK_HAT_MOTION "
					<< "timestamp=" << event.jhat.timestamp << ", "
					<< "which=" << event.jhat.which << ", "
					<< "hat=" << static_cast<int32_t>(event.jhat.hat) << ", "
					<< "value=" << static_cast<int32_t>(event.jhat.value)
					<< std::endl;
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
	// Background color setup.
	const Color base_bgcolor = d->settings.background_color;
	const bool is_bgcolor_light = base_bgcolor.is_light();
	const std::array<Color, 4> colors = {
		base_bgcolor,
		base_bgcolor.adjust_brightness(is_bgcolor_light ? -0.02f : 0.01f),
		base_bgcolor.adjust_brightness(is_bgcolor_light ? -0.04f : 0.0125f),
		base_bgcolor.adjust_brightness(is_bgcolor_light ? -0.06f : 0.0175f),
	};
	static int color_cycle_index = 0;
	const static Seconds time_color_change_rate = 0.5;
	static Seconds time_accumulator = 0.0;
	// Cycle the background color
	if (d->settings.background_animate) {
		time_accumulator += frame_time.delta_seconds;
		while (time_accumulator >= time_color_change_rate) {
			time_accumulator -= time_color_change_rate;
			color_cycle_index = (++color_cycle_index) % (colors.size());
		}
	} else {
		color_cycle_index = 0;
	}

	// Update arena
	d->arena->update(*d->controller_system, frame_time);

	// Clear the screen with a color
	auto bgcolor = ColorU8<uint8_t>::from(colors[color_cycle_index]);
	if (
		d->settings.background_flash_on_gizmo_action &&
		std::any_of(
			d->arena->gizmos().begin(), d->arena->gizmos().end(),
			[](auto &it) { return it->is_active(); }
		)
	) {
		bgcolor = ColorU8<uint8_t>::from(d->settings.background_flash_color);
	}
	SDL_SetRenderDrawColor(d->renderer, bgcolor[0], bgcolor[1], bgcolor[2], 255);
	SDL_RenderClear(d->renderer);

	// Draw the arena
	d->arena->render(*d->renderer);

	// Draw GUI
	d->gui->iterate(frame_time);

	// Present the backbuffer
	SDL_RenderPresent(d->renderer);

	return AppRunResult::CONTINUE;
}

void App::close()
{
	d->arena.reset();
	d->controller_system.reset();
	d->gui.reset();

	if (d->renderer) {
		SDL_DestroyRenderer(d->renderer);
		d->renderer = nullptr;
	}
	if (d->window) {
		SDL_DestroyWindow(d->window);
		d->window = nullptr;
	}

	// Save settings.
	sdl::SettingsSdlIO settings_io;
	if (!settings_io.save(d->settings)) {
		std::cerr << "Failed to save settings" << std::endl;
	}

	SDL_Quit();
}

void App::quit() {
	d->main_loop_result = AppRunResult::SUCCESS;
}

void App::recalculate_fps_clock() {
	if (d->settings.limit_fps) {
		const double reasonably_clamped_target_fps = std::max<double>(10.0, d->settings.target_fps);
		d->settings.target_fps = reasonably_clamped_target_fps;
		d->clock.set_resolution(
			std::chrono::round<std::chrono::nanoseconds>(
				std::chrono::duration<double>(1.0 / reasonably_clamped_target_fps)
			)
		);
	} else {
		d->clock.set_resolution(std::chrono::nanoseconds::zero());
	}
}

Arena &App::arena() {
	return *d->arena;
}

Logger &App::logger() {
	return d->logger;
}

Settings &App::settings() {
	return d->settings;
}

const OpenedJoysticksMap &App::joysticks() const {
	return d->joysticks;
}

SDL_Renderer *App::renderer() const {
	return d->renderer;
}

SDL_Window *App::window() const {
	return d->window;
}

} // namespace robikzinputtest
