#pragma once

#include "clock.hpp"

#include <memory>

namespace robikzinputtest {

class Arena;
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

	AppRunResult init(int argc, char *argv[]);
	AppRunResult run();
	AppRunResult iterate(const FrameTime &frame_time);
	AppRunResult handleEvents(const FrameTime &frame_time);
	void close();

	Arena &arena();
	Settings &settings();

private:
	struct D;
	std::unique_ptr<D> d;
};

}
