cmake_minimum_required(VERSION 3.22)

# Project definition
include(cmake/git.cmake)
git_describe(robikzinputtest v)

project(
	robikzinputtest
	VERSION ${robikzinputtest_VERSION_MAJOR}.${robikzinputtest_VERSION_MINOR}.${robikzinputtest_VERSION_PATCH}
	DESCRIPTION "Robikz's Input Test"
)

# Configure install rules (must be done before subdirs are added)
include(GNUInstallDirs)
if(WIN32)
	# Flatten the installation structure on Windows
	set(CMAKE_INSTALL_BINDIR .)
	set(CMAKE_INSTALL_DOCDIR .)
	set(CMAKE_INSTALL_LIBDIR .)
endif()

# Source directories
add_subdirectory(src)

# Install rules
install(
	FILES
	ATTRIBUTION.md
	CHANGELOG.md
	LICENSE.txt
	DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

configure_file(README-bin.md.in README-bin.md)
install(
	FILES
	${CMAKE_CURRENT_BINARY_DIR}/README-bin.md
	DESTINATION ${CMAKE_INSTALL_DOCDIR}
	RENAME "README.md"
)

# Install compiler runtime
if(WIN32)
	# On MS Windows the software is installed in a flat directory structure.
	set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
endif()
include(cmake/install_required_system_libraries.cmake)

# Packaging rules
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "com.github.zalewa")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VERSION ${robikzinputtest_VERSION_FULL})
set(CPACK_STRIP_FILES TRUE)

set(CPACK_GENERATOR ZIP)

include(CPack)

# AppImage packaging
if(UNIX AND NOT APPLE)
	include(cmake/appimage.cmake)
	find_appimage_linuxdeploy()
	if(APPIMAGE_LINUXDEPLOY)
		message(STATUS "robikzinputtest: appimage target is enabled")
		add_custom_target(
			appimage
			COMMAND ${CMAKE_COMMAND} --install . --prefix AppDir/usr --strip
			COMMAND
				VERSION=${robikzinputtest_VERSION_FULL}
				${APPIMAGE_LINUXDEPLOY}
				--appdir AppDir
				--executable AppDir/usr/bin/robikzinputtest
				--desktop-file "${CMAKE_SOURCE_DIR}/packaging/linux/robikzinputtest.desktop"
				--icon-file "${CMAKE_SOURCE_DIR}/media/gamepad-silhouette-whiteborder-256px.png"
				--icon-filename robikzinputtest
				--output appimage
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	endif()
endif()
