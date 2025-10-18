#include "gizmo_render.hpp"

#include "gizmo.hpp"

namespace robikzinputtest {

void GizmoRender::load_render(Renderer &renderer) {
	// no-op
}

void GizmoRender::render(Renderer &renderer) {
	SDL_FPoint &pos = m_gizmo.m_position;

	SDL_SetRenderDrawColor(&renderer, m_color.r, m_color.g, m_color.b, m_color.a);
	SDL_FPoint origin = { pos.x - m_size.x / 2, pos.y - m_size.y / 2 };
	SDL_FRect rect = { origin.x, origin.y, m_size.x, m_size.y };
	SDL_RenderFillRect(&renderer, &rect);
}

} // namespace robikzinputtest
