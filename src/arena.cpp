#include "arena.hpp"

#include "app.hpp"
#include "controller_system.hpp"
#include "gizmo.hpp"
#include "gizmo_render.hpp"
#include "settings.hpp"

#include <SDL3/SDL.h>

#include <algorithm>

namespace robikzinputtest {

Arena::Arena(App &app)
	: m_app(app), m_bounds({0, 0, 100, 100}) {}

std::shared_ptr<Gizmo> Arena::create_gizmo(const ControllerId &controller) {
	auto gizmo = std::make_shared<Gizmo>();
	gizmo->set_controller(controller);
	gizmo->m_position = find_free_position();
	gizmo->m_size = {
		static_cast<float>(m_app.settings().gizmo_width),
		static_cast<float>(m_app.settings().gizmo_height),
	};
	gizmo->m_speed = m_app.settings().gizmo_speed;
	m_gizmos_to_load.push(gizmo);
	m_gizmos.push_back(gizmo);
	return gizmo;
}

std::shared_ptr<Gizmo> Arena::find_gizmo_for_controller(const ControllerId &controller) {
	for (const auto &gizmo : m_gizmos) {
		const ControllerId &gizmo_controller = gizmo->controller();
		if (gizmo_controller == controller) {
			return gizmo;
		}
	}
	return nullptr;
}

void Arena::remove_gizmo(std::shared_ptr<Gizmo> gizmo) {
	auto it = std::find(m_gizmos.begin(), m_gizmos.end(), gizmo);
	if (it != m_gizmos.end()) {
		m_gizmos.erase(it);
	}
}

void Arena::remove_all_gizmos() {
	for (auto it = m_gizmos.begin(); it != m_gizmos.end(); ) {
		remove_gizmo(*it);
		it = m_gizmos.begin();
	}
}

void Arena::set_gizmos_width(int px) {
	for (auto &gizmo : m_gizmos)
		gizmo->m_size.x = static_cast<float>(px);
}

void Arena::set_gizmos_height(int px) {
	for (auto &gizmo : m_gizmos)
		gizmo->m_size.y = static_cast<float>(px);
}

void Arena::set_gizmos_speed(float speed) {
	for (auto &gizmo : m_gizmos)
		gizmo->m_speed = speed;
}

void Arena::set_bounds(const SDL_Rect &bounds) {
	SDL_RectToFRect(&bounds, &m_bounds);
	// Clamp all gizmos to the new bounds
	for (const auto &gizmo : m_gizmos) {
		gizmo->m_position = clamp_to_bounds(gizmo->m_position);
	}
}

SDL_FPoint Arena::clamp_to_bounds(const SDL_FPoint &point) const {
	SDL_FPoint clamped = point;
	if (clamped.x < m_bounds.x) {
		clamped.x = m_bounds.x;
	} else if (clamped.x > m_bounds.x + m_bounds.w) {
		clamped.x = m_bounds.x + m_bounds.w;
	}
	if (clamped.y < m_bounds.y) {
		clamped.y = m_bounds.y;
	} else if (clamped.y > m_bounds.y + m_bounds.h) {
		clamped.y = m_bounds.y + m_bounds.h;
	}
	return clamped;
}

SDL_FPoint Arena::find_free_position() const {
	// TODO: well, this just picks the bounds center for now
	return {
		m_bounds.x + m_bounds.w / 2.0f,
		m_bounds.y + m_bounds.h / 2.0f,
	};
}

void Arena::load_render(Renderer &renderer) {
	// no-op
}

void Arena::render(Renderer &renderer) {
	// Load any pending gizmos
	while (!m_gizmos_to_load.empty()) {
		auto gizmo = m_gizmos_to_load.front();
		gizmo->renderer().load_render(renderer);
		m_gizmos_to_load.pop();
	}

	// Draw arena bounds
	const Color bgcolor = m_app.settings().background_color;
	const ColorU8 border_color = ColorU8<uint8_t>::from(
		bgcolor.adjust_brightness(bgcolor.is_light() ? -0.3f : 0.3f)
	);
	SDL_SetRenderDrawColor(&renderer, border_color.r, border_color.g, border_color.b, 255);
	SDL_RenderRect(&renderer, &m_bounds);

	// Render all gizmos
	for (const auto &gizmo : m_gizmos) {
		gizmo->renderer().render(renderer);
	}
}

void Arena::update(ControllerSystem &controller_system, const FrameTime &frame_time) {
	for (const auto &gizmo : m_gizmos) {
		// Find the Controller instance for this gizmo.
		std::shared_ptr<Controller> controller = controller_system.find_controller_by_id(
			gizmo->controller()
		);
		if (controller) {
			// Update gizmo position based on controller state.
			SDL_FPoint &pos = gizmo->m_position;
			const SDL_FPoint &dir = controller->state.direction_vec2;
			pos.x += dir.x * gizmo->m_speed * frame_time.delta_seconds;
			pos.y += dir.y * gizmo->m_speed * frame_time.delta_seconds;
			// Clamp to arena bounds.
			pos = clamp_to_bounds(pos);
			// If button is (continuously) PRESSED, update action time.
			if (controller->state.button_primary == ButtonState::PRESSED) {
				gizmo->m_action_started_at = frame_time;
			}
			// If button was RELEASED, also set action time, but only if it's unset.
			if (controller->state.button_primary == ButtonState::RELEASED) {
				if (!gizmo->m_action_started_at.has_value()) {
					gizmo->m_action_started_at = frame_time;
				}
				// And clear the button state so that it doesn't get processed again.
				controller->state.button_primary = ButtonState::CLEAR;
			}
		}
		// Clear action time if set and enough time has passed.
		if (gizmo->m_action_started_at.has_value()) {
			FrameTime action_duration = FrameTime::delta(
				gizmo->m_action_started_at->currtick,
				frame_time.currtick
			);
			if (action_duration.delta_seconds > gizmo->m_action_time) {
				gizmo->m_action_started_at.reset();
			}
		}
	}
}

} // namespace robikzinputtest
