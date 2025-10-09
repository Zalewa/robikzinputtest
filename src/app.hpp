#pragma once

#include <memory>
#include <string>

namespace robikzinputtest {

enum class ExitCode {
	OK = 0,
	SDL_INIT_ERROR = 1,
	WINDOW_INIT_ERROR = 2,
	RENDERER_INIT_ERROR = 3,
	UNKNOWN_ERROR = 4
};

struct AppRunResult {
	ExitCode code;
	std::string message;

	bool ok() const { return code == ExitCode::OK; }
};

class App {
public:
	App();
	~App();

	AppRunResult init(int argc, char *argv[]);
	AppRunResult run();

private:
	struct D;
	std::unique_ptr<D> d;
};

}
