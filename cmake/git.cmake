# Extract version information from git tags.
#
# It expects the git tags to be in the SEMANTIC VERSIONING format with mandatory
# MAJOR, MINOR and PATCH numbers, even if all are 0, but with optional prefixes
# e.g. "v1.2.3", or "app-v0.1.0".
#
# Arguments:
# var_prefix - Variable name prefix for the variables to set.
# tag_prefix - Prefix for git tags to match (e.g. "v" for simple repos, or "app-v" for monorepos)
#
# This function sets the following variables with the given prefix:
# <prefix>_VERSION_MAJOR
# <prefix>_VERSION_MINOR
# <prefix>_VERSION_PATCH
#     The major, minor, and patch version numbers from the latest matching tag
# <prefix>_VERSION_SUFFIX
#     The suffix from `git describe`, e.g. "-dirty" or "-5-gabcdef"
# <prefix>_VERSION_FULL
#     Full version string without the tag prefix,
#     e.g. "1.2.3-dirty" or "1.2.3-5-gabcdef"
# <prefix>_VERSION_DESCRIBE - Unfiltered full output of `git describe` including the tag prefix
function(git_describe var_prefix tag_prefix)
	set(${var_prefix}_VERSION_MAJOR 0 PARENT_SCOPE)
	set(${var_prefix}_VERSION_MINOR 0 PARENT_SCOPE)
	set(${var_prefix}_VERSION_PATCH 0 PARENT_SCOPE)
	set(${var_prefix}_VERSION_SUFFIX "unknown-version" PARENT_SCOPE)
	set(${var_prefix}_VERSION_FULL "unknown-version" PARENT_SCOPE)

	find_package(Git)
	if (Git_FOUND)
		execute_process(
			COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty=-M --match ${tag_prefix}*
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			OUTPUT_VARIABLE GIT_REPO_VERSION
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)

		string(REGEX MATCH "${tag_prefix}([0-9]+)\\.([0-9]+)\\.([0-9]+)(.*)"
			_ ${GIT_REPO_VERSION})
		if (CMAKE_MATCH_COUNT EQUAL 4)
			# Extract version components from `git describe` output, stripping
			# the tag prefix.
			set(${var_prefix}_VERSION_MAJOR ${CMAKE_MATCH_1} PARENT_SCOPE)
			set(${var_prefix}_VERSION_MINOR ${CMAKE_MATCH_2} PARENT_SCOPE)
			set(${var_prefix}_VERSION_PATCH ${CMAKE_MATCH_3} PARENT_SCOPE)
			set(${var_prefix}_VERSION_SUFFIX ${CMAKE_MATCH_4} PARENT_SCOPE)
			# ${var_prefix}_VERSION_FULL is like VERSION_DESCRIBE but without the tag prefix
			set(${var_prefix}_VERSION_FULL
				"${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}${CMAKE_MATCH_4}"
				PARENT_SCOPE)
			# ${var_prefix}_VERSION_DESCRIBE is the full git describe output without modification
			set(${var_prefix}_VERSION_DESCRIBE ${GIT_REPO_VERSION} PARENT_SCOPE)
		else()
			message(WARNING "Git describe output did not match expected format: ${GIT_REPO_VERSION}")
		endif()
	else()
		message(WARNING "Git was not found. Program version cannot be established.")
	endif()
endfunction()
