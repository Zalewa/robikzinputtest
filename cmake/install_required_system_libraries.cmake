# install_required_system_libraries.cmake - Wrapper and extension
# around InstallRequiredSystemLibraries.
#
# Written in 2026 by Robert "Robikz" Zalewski <zalewapl@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see
# <http://creativecommons.org/publicdomain/zero/1.0/>.
#
# SPDX-License-Identifier: CC0-1.0
# -
#
# Including this file is akin to including InstallRequiredSystemLibraries.
# It takes care of some outliers that the regular InstallRequiredSystemLibraries
# doesn't seem to function with.
if(MINGW)  # Based on: https://winlibs.com/
	get_filename_component(MINGW_PATH ${CMAKE_CXX_COMPILER} PATH)
	# Collecting the files is tricky because there is no one MinGW
	# compiler. It's best to consider this list as always incomplete
	# and fill it in as it goes.
	set(
		CANDIDATE_FILES
		libstdc++-6.dll      # constant among all compilers, I think...
		libgcc_s_dw2-1.dll   # i686-posix-dwarf
		libgcc_s_seh-1.dll   # x86_64-posix-seh
		libwinpthread-1.dll  # POSIX threads
	)
	foreach(file ${CANDIDATE_FILES})
		if(EXISTS "${MINGW_PATH}/${file}")
			list(APPEND CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS "${MINGW_PATH}/${file}")
		endif()
	endforeach()
endif()
include(InstallRequiredSystemLibraries)
