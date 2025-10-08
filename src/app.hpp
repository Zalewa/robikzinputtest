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

class App {
public:
	App();
	~App();

	std::pair<ExitCode, std::string> init(int argc, char *argv[]);
	std::pair<ExitCode, std::string> run();

private:
	struct D;
	std::unique_ptr<D> d;
};

}
