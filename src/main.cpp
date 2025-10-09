#include "app.hpp"

#include <iostream>
#include <memory>

int main(int argc, char **argv)
{
	using namespace robikzinputtest;

	AppRunResult app_run_result;
	std::unique_ptr<App> app = std::make_unique<App>();
	app_run_result = app->init(argc, argv);
	if (!app_run_result.ok()) {
		std::cerr << "Error during initialization: " << app_run_result.message << std::endl;
		return int(app_run_result.code);
	}

	app_run_result = app->run();
	if (!app_run_result.ok()) {
		std::cerr << "Error during run: " << app_run_result.message << std::endl;
		return int(app_run_result.code);
	}

	app.reset();
	return 0;
}
