#include "app.hpp"

#include <iostream>
#include <memory>

int main(int argc, char **argv)
{
	using namespace robikzinputtest;

	std::cerr << "hello, world!" << std::endl;

	std::unique_ptr<App> app = std::make_unique<App>();
	auto [code, msg] = app->init(argc, argv);
	if (code != ExitCode::OK) {
		std::cerr << "Error during initialization: " << msg << std::endl;
		return int(code);
	}

	auto runResult = app->run();
	code = runResult.first;
	msg = runResult.second;
	if (code != ExitCode::OK) {
		std::cerr << "Error during run: " << msg << std::endl;
		return int(code);
	}

	app.reset();
	return 0;
}
