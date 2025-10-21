#pragma once

#include "controller.hpp"
#include "clock.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <optional>

namespace robikzinputtest {

class GizmoRender;

/**
 * Gizmo is the controllable entity that can be moved around in the scene.
 */
class Gizmo {
public:
	// Properties
	float m_speed = 200.0f;
	/// Action time should be *very* short in order to appear instantaneous.
	Seconds m_action_time = 1 / 60.0f;

	// State
	SDL_FPoint m_position;
	std::optional<FrameTime> m_action_started_at;

	Gizmo();

	bool is_active() const { return m_action_started_at.has_value(); }

	const ControllerId &controller() const { return m_controller; }
	void set_controller(const ControllerId &controller) { m_controller = controller; }

	GizmoRender &renderer() {
		return *m_renderer;
	}

private:
	ControllerId m_controller;
	std::unique_ptr<GizmoRender> m_renderer;
};

} // namespace robikzinputtest
