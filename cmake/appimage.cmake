# appimage.cmake - Finds linuxdeploy tool
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
# Linuxdeploy can be obtained from:
#
#   https://github.com/linuxdeploy/linuxdeploy
#   (accessed: 2026-01-05)
#
# This script exports find_appimage_linuxdeploy() function

# Find program linuxdeploy in the system.
#
# Sets the following variables:
# APPIMAGE_LINUXDEPLOY
#     Path to the linuxdeploy tool if found.
# APPIMAGE_LINUXDEPLOY-NOTFOUND
#     If linuxdeploy program was not found.
function(find_appimage_linuxdeploy)
	find_program(
		APPIMAGE_LINUXDEPLOY
		linuxdeploy
	)
endfunction()
