#pragma once

#include <imgui.h>

namespace robikzinputtest::imgui {

static constexpr ImGuiWindowFlags inert_window_flags = 0
	| ImGuiWindowFlags_NoDecoration
	| ImGuiWindowFlags_NoBackground
	| ImGuiWindowFlags_NoInputs
	| ImGuiWindowFlags_NoMove
	| ImGuiWindowFlags_NoSavedSettings
	;

static constexpr ImGuiWindowFlags overlay_flags = inert_window_flags;

}
