#pragma once

#include "renderable.hpp"

namespace robikzinputtest {

class Gizmo;

/**
 * GizmoRender is responsible for rendering a Gizmo.
 */
class GizmoRender : public Renderable {
public:
	SDL_Color m_color = { 255, 0, 0, 255 };
	SDL_Color m_frame_color = { 255, 96, 96, 255 };
	SDL_Color m_active_color = { 0, 255, 0, 255 };
	SDL_Color m_active_frame_color = { 96, 255, 96, 255 };

	GizmoRender(Gizmo &gizmo)
		: m_gizmo(gizmo) {}

	void load_render(Renderer &renderer) override;
	void render(Renderer &renderer) override;

private:
	Gizmo &m_gizmo;
};

} // namespace robikzinputtest
