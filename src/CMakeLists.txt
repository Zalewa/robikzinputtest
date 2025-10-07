# Add SDL2 to the project
find_package(SDL2 REQUIRED)

add_executable(
	robikzinputtest
	main.cpp
)

target_link_libraries(
	robikzinputtest
	PRIVATE
	SDL2::SDL2
	SDL2::SDL2main
)
