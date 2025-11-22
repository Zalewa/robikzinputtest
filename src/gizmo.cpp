#include "gizmo.hpp"

#include "controller.hpp"
#include "gizmo_render.hpp"

#include <sstream>

namespace robikzinputtest {

Gizmo::Gizmo()
	: m_renderer(std::make_unique<GizmoRender>(*this)),
	m_position({0.0f, 0.0f}) {}

std::string Gizmo::name() const {
	std::ostringstream ss;
	switch (controller().type) {
	case ControllerId::TYPE_JOY:
		ss << "J";
		break;
	case ControllerId::TYPE_KEYBOARD:
		ss << "K";
		break;
	case ControllerId::TYPE_MOUSE:
		ss << "M";
		break;
	default:
		ss << "?";
		break;
	}
	ss << controller().index;
	return ss.str();
}

} // namespace robikzinputtest
