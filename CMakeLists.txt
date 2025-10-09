cmake_minimum_required(VERSION 3.28)

include(cmake/git.cmake)
git_describe(robikzinputtest v)

project(
	robikzinputtest
	VERSION ${robikzinputtest_VERSION_MAJOR}.${robikzinputtest_VERSION_MINOR}.${robikzinputtest_VERSION_PATCH}
	DESCRIPTION "Robikz's Input Tester"
)

add_subdirectory(src)
