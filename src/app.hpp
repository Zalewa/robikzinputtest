#pragma once

#include <memory>

namespace robikzinputtest {

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
	AppRunResult iterate();
	AppRunResult handleEvents();
	void close();

private:
	struct D;
	std::unique_ptr<D> d;
};

}
