# Project setup
configure_file(version_defs.h.in version_defs.h)

# Dependencies
include(FetchContent)

# SDL3
FetchContent_Declare(
	SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL
	GIT_TAG release-3.2.24
)
FetchContent_MakeAvailable(SDL3)

# Dear ImGui
FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui
	GIT_TAG v1.92.5
)
FetchContent_MakeAvailable(imgui)

add_library(
	imgui_core
	STATIC
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_demo.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
)
target_include_directories(
	imgui_core
	PUBLIC
	${imgui_SOURCE_DIR}
)

add_library(
	imgui_sdl3_renderer
	STATIC
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)
target_include_directories(
	imgui_sdl3_renderer
	PUBLIC
	${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_sdl3_renderer PUBLIC imgui_core SDL3::SDL3)

# Main target
add_executable(
	robikzinputtest
	app.cpp
	arena.cpp
	color.cpp
	controller_handler.cpp
	controller_system.cpp
	imgui_style.cpp
	logger.cpp
	gizmo.cpp
	gizmo_render.cpp
	gui.cpp
	gui_logbox.cpp
	gui_overlay_fps.cpp
	gui_overlay_help.cpp
	gui_overlay_joystick.cpp
	gui_window_about.cpp
	gui_window_program_log.cpp
	gui_window_resolution_popup.cpp
	gui_window_settings.cpp
	main.cpp
	properties_file.cpp
	sdl_settings.cpp
	sdl_storage.cpp
	settings.cpp
	variant.cpp
	version.cpp
	version.rc
	video.cpp
	video_settings.cpp
)

target_include_directories(
	robikzinputtest
	PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
	robikzinputtest
	PRIVATE
	SDL3::SDL3
	imgui_sdl3_renderer
)

set_target_properties(
	robikzinputtest
	PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
	WIN32_EXECUTABLE YES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
