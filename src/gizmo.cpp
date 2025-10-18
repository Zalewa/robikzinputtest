#include "gizmo.hpp"

#include "gizmo_render.hpp"

namespace robikzinputtest {

Gizmo::Gizmo()
	: m_renderer(std::make_unique<GizmoRender>(*this)),
	  m_position({0.0f, 0.0f}) {}

} // namespace robikzinputtest
