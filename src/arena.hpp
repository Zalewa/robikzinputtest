#pragma once

#include "clock.hpp"
#include "renderable.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <queue>
#include <vector>

namespace robikzinputtest {

class App;
class ControllerSystem;
class Gizmo;
struct ControllerId;

class Arena : public Renderable
{
public:
	Arena(App &app);

	std::shared_ptr<Gizmo> create_gizmo(const ControllerId &controller);
	std::shared_ptr<Gizmo> find_gizmo_for_controller(const ControllerId &controller);
	void remove_gizmo(std::shared_ptr<Gizmo> gizmo);
	void remove_all_gizmos();
	const std::vector<std::shared_ptr<Gizmo>> &gizmos() const { return m_gizmos; }
	void set_gizmos_width(int px);
	void set_gizmos_height(int px);

	void set_bounds(const SDL_Rect &bounds);

	void load_render(Renderer &renderer) override;
	void render(Renderer &renderer) override;

	void update(ControllerSystem &controller_system, const FrameTime &frame_time);

private:
	App &m_app;

	std::vector<std::shared_ptr<Gizmo>> m_gizmos;
	std::queue<std::shared_ptr<Gizmo>> m_gizmos_to_load;
	SDL_FRect m_bounds;

	SDL_FPoint clamp_to_bounds(const SDL_FPoint &point) const;
	SDL_FPoint find_free_position() const;
};

} // namespace robikzinputtest
