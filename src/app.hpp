#pragma once

#include "clock.hpp"

#include <memory>

namespace robikzinputtest {

class Arena;
class Logger;
struct Settings;

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

private:
	struct D;
	std::unique_ptr<D> d;
};

}
