#pragma once

#include "clock.hpp"

#include <SDL3/SDL.h>

#include <functional>
#include <map>
#include <memory>

namespace robikzinputtest {

class Arena;
class Logger;
struct Settings;
struct VideoModeSettings;

using JoystickUPtr = std::unique_ptr<SDL_Joystick, std::function<void(SDL_Joystick *)>>;
using OpenedJoysticksMap = std::map<SDL_JoystickID, JoystickUPtr>;

enum class AppRunResult {
	SUCCESS,
	FAILURE,
	CONTINUE,
};

class App {
public:
	App();
	~App();

	/// Initialize the systems and the application internal state.
	AppRunResult init(int argc, char *argv[]);
	/// Where's the main loop.
	AppRunResult run();
	/// "Tick"
	AppRunResult iterate(const FrameTime &frame_time);
	/// Handle incoming system events.
	AppRunResult handleEvents(const FrameTime &frame_time);
	/// Cleanup after app was quit.
	void close();

	/// Trigger graceful app exit.
	void quit();
	/// Limit the tickrate clock to target FPS.
	void recalculate_fps_clock();

	Arena &arena();
	Logger &logger();
	Settings &settings();
	const OpenedJoysticksMap &joysticks() const;
	SDL_Renderer *renderer() const;
	SDL_Window *window() const;

private:
	struct D;
	std::unique_ptr<D> d;
};

}
