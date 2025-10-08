include(FetchContent)

FetchContent_Declare(
	SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL
	GIT_TAG release-3.2.24
)
FetchContent_MakeAvailable(SDL3)

add_executable(
	robikzinputtest
	app.cpp
	main.cpp
)

target_link_libraries(
	robikzinputtest
	PRIVATE
	SDL3::SDL3
)

set_target_properties(
	robikzinputtest
	PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)
