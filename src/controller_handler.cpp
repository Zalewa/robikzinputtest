#include "controller_handler.hpp"

#include "controller.hpp"

namespace robikzinputtest {

/*
  KeyboardControllerHandler
*/

bool KeyboardControllerHandler::handle_event(Controller &controller, const SDL_Event &event)
{
	ControllerState &state = controller.state;
	if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
		bool is_pressed = (event.type == SDL_EVENT_KEY_DOWN);
		switch (event.key.key) {
		case SDLK_UP:
		case SDLK_W:
			state.direction_vec2d.y = is_pressed ? -1 : 0;
			return true;
		case SDLK_DOWN:
		case SDLK_S:
			state.direction_vec2d.y = is_pressed ? 1 : 0;
			return true;
		case SDLK_LEFT:
		case SDLK_A:
			state.direction_vec2d.x = is_pressed ? -1 : 0;
			return true;
		case SDLK_RIGHT:
		case SDLK_D:
			state.direction_vec2d.x = is_pressed ? 1 : 0;
			return true;
		case SDLK_SPACE:
			state.button_primary = is_pressed ? ButtonState::PRESSED : ButtonState::RELEASED;
			return true;
		default:
			break;
		}
	}
	return false;
}

} // namespace robikzinputtest
