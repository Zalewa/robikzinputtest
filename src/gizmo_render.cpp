#include "gizmo_render.hpp"

#include "gizmo.hpp"

namespace robikzinputtest {

void GizmoRender::load_render(Renderer &renderer) {
	// no-op
}

void GizmoRender::render(Renderer &renderer) {
	SDL_FPoint &pos = m_gizmo.m_position;

	const SDL_Color &color = m_gizmo.is_active() ? m_active_color : m_color;
	const SDL_Color &frame_color = m_gizmo.is_active() ? m_active_frame_color : m_frame_color;

	SDL_FPoint origin = { pos.x - m_size.x / 2, pos.y - m_size.y / 2 };
	SDL_FRect rect = { origin.x, origin.y, m_size.x, m_size.y };

	SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(&renderer, &rect);

	SDL_SetRenderDrawColor(&renderer, frame_color.r, frame_color.g, frame_color.b, frame_color.a);
	SDL_RenderRect(&renderer, &rect);
}

} // namespace robikzinputtest
