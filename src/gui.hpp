#pragma once

#include <SDL3/SDL.h>
#include <memory>

namespace robikzinputtest {

class App;
struct FrameTime;

namespace gui {

class Gui {
public:
	Gui(App &app, SDL_Window &window, SDL_Renderer &renderer);
	~Gui();

	bool init();
	void close();

	void clear_focus();
	void grab_focus();

	bool is_demo_enabled() const;
	void set_demo_enabled(bool enabled);

	bool handle_event(SDL_Event &event);
	void iterate(const FrameTime &frame_time);

private:
	struct D;
	std::unique_ptr<D> d;
};

} // namespace gui

} // namespace robikzinputtest
