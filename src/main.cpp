#include "app.hpp"

#include <iostream>
#include <memory>

const int EC_OK = 0;
const int EC_FAIL = 1;

static int ec(robikzinputtest::AppRunResult result)
{
	using namespace robikzinputtest;

	switch (result) {
	case AppRunResult::SUCCESS:
		return EC_OK;
	case AppRunResult::FAILURE:
	default:
		return EC_FAIL;
	}
}

int main(int argc, char **argv)
{
	using namespace robikzinputtest;

	AppRunResult app_run_result;
	std::unique_ptr<App> app = std::make_unique<App>();
	app_run_result = app->init(argc, argv);
	if (app_run_result != AppRunResult::CONTINUE) {
		std::cerr << "Stopping after init." << std::endl;
		app->close();
		return ec(app_run_result);
	}

	app_run_result = app->run();
	app->close();

	if (app_run_result != AppRunResult::SUCCESS) {
		std::cerr << "Error during run." << std::endl;
		return EC_FAIL;
	}

	app.reset();
	return EC_OK;
}
