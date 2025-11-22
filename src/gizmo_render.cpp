#include "gizmo_render.hpp"

#include "SDL3/SDL_render.h"
#include "gizmo.hpp"

namespace robikzinputtest {

static const float SDL_DEBUG_FONT_SIZE = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;

void GizmoRender::load_render(Renderer &renderer) {
	// no-op
}

void GizmoRender::render(Renderer &renderer) {
	SDL_FPoint &pos = m_gizmo.m_position;
	SDL_FPoint &size = m_gizmo.m_size;

	const SDL_Color &color = m_gizmo.is_active() ? m_active_color : m_color;
	const SDL_Color &frame_color = m_gizmo.is_active() ? m_active_frame_color : m_frame_color;

	SDL_FPoint origin = { pos.x - size.x / 2, pos.y - size.y / 2 };
	SDL_FRect rect = { origin.x, origin.y, size.x, size.y };

	SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(&renderer, &rect);

	SDL_SetRenderDrawColor(&renderer, frame_color.r, frame_color.g, frame_color.b, frame_color.a);
	SDL_RenderRect(&renderer, &rect);

	SDL_FPoint original_scale;
	SDL_GetRenderScale(&renderer, &original_scale.x, &original_scale.y);
	const std::string gizmo_name = m_gizmo.name();
	const SDL_FPoint text_scale = {
		original_scale.x * size.x / std::max(1.0f, (SDL_DEBUG_FONT_SIZE * gizmo_name.size())),
		original_scale.y * size.y / SDL_DEBUG_FONT_SIZE,
	};
	SDL_SetRenderScale(&renderer, text_scale.x, text_scale.y);
	SDL_SetRenderDrawColor(&renderer, 255, 255, 255, 224);
	SDL_RenderDebugText(
		&renderer,
		origin.x / text_scale.x,
		origin.y / text_scale.y,
		gizmo_name.c_str()
	);
	SDL_SetRenderScale(&renderer, original_scale.x, original_scale.y);
}

} // namespace robikzinputtest
