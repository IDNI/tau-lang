# cmake/tau-shared-helpers.provenance.cmake
#
# Provenance of the shared build helpers vendored into Tau's cmake/. The copied
# helpers only warn when the parser's copy has moved on.
#
# Each entry is "<vendored>|<parser path>|<sha256 of the parser file>|<mode>".
# mode is "strict" or "warn".
set(TAU_SHARED_HELPERS_DIR "${CMAKE_CURRENT_LIST_DIR}")

set(TAU_SHARED_HELPER_PROVENANCE
	"tau-shared-resolve.cmake|cmake/tau-resolve.cmake|61fce476f1c8ed87116d07a33f686331d0a79730ca3cf77ade4ce870acb2f7ed|warn"
	"tau-shared-ccache.cmake|cmake/use-ccache.cmake|0e178c553711db49560551f7e123322284824f7124e51a1759e3f912bb71105a|warn"
	"tau-shared-emscripten.cmake|cmake/use-emscripten.cmake|981a7a80f819a413cc56b88b1b626187851a008f28ec718cae6a52d82e7d52f5|warn"
	"tau-shared-version-license.cmake|cmake/version_license.cmake|feb5407b9cd874f5ec786145d9c7af3d361f221be6576e77f9e9e03b7fa12ab2|warn"
	"tau-shared-ctest-build-tree.cmake|cmake/ctest-build-tree.cmake|714a581f7e63588c1626ca05fab4260cb543072a011cceb0fa1ecb8b3b20481e|warn"
)

# Tau-side files that choose which helper library a producer loads. They are not
# part of any package identity -- editing one would not invalidate a package --
# so a strict content guard is what stops them from drifting. Each entry is
# "<repo-relative path>|<sha256>".
set(TAU_SHIM_PROVENANCE
	"scripts/devrc|dfa47e774df68bf0895cf5fda7474a10c14d3f6ae2252d97a97589e6f29535f9"
)

# Check the vendored helpers against a parser cmake directory. Defaults to the
# co-located parser tree; the hermetic self-check passes a tampered directory.
# A strict mismatch is a hard error; a warn mismatch is a warning.
function(tau_shared_helpers_check_provenance)
	set(_parser_cmake "${PROJECT_SOURCE_DIR}/external/parser/cmake")
	if(ARGC GREATER 0 AND NOT ARGV0 STREQUAL "")
		set(_parser_cmake "${ARGV0}")
	endif()
	if(NOT EXISTS "${_parser_cmake}")
		return()
	endif()
	foreach(_entry IN LISTS TAU_SHARED_HELPER_PROVENANCE)
		string(REPLACE "|" ";" _parts "${_entry}")
		list(GET _parts 0 _vendored)
		list(GET _parts 1 _source_rel)
		list(GET _parts 2 _expected)
		get_filename_component(_source_name "${_source_rel}" NAME)
		set(_source "${_parser_cmake}/${_source_name}")
		if(NOT EXISTS "${_source}")
			continue()
		endif()
		file(SHA256 "${_source}" _actual)
		if(NOT _actual STREQUAL _expected)
			message(WARNING
				"vendored helper ${_vendored} is behind its parser source "
				"${_source_rel}: vendored-from ${_expected}, parser now ${_actual}")
		endif()
	endforeach()
	# Guard the Tau-side selector shims against a content change. ARGV1 may name
	# the repo root (the self-check passes a tampered copy); otherwise
	# PROJECT_SOURCE_DIR. In script mode with neither, this is skipped.
	set(_tau_shim_root "${PROJECT_SOURCE_DIR}")
	if(ARGC GREATER 1 AND NOT ARGV1 STREQUAL "")
		set(_tau_shim_root "${ARGV1}")
	endif()
	if(NOT _tau_shim_root STREQUAL "")
		foreach(_entry IN LISTS TAU_SHIM_PROVENANCE)
			string(REPLACE "|" ";" _parts "${_entry}")
			list(GET _parts 0 _rel)
			list(GET _parts 1 _expected)
			set(_file "${_tau_shim_root}/${_rel}")
			if(NOT EXISTS "${_file}")
				message(FATAL_ERROR "provenance: ${_rel} is missing under ${_tau_shim_root}")
			endif()
			file(SHA256 "${_file}" _actual)
			if(NOT _actual STREQUAL _expected)
				message(FATAL_ERROR
					"provenance: ${_rel} changed (${_actual}); it selects the "
					"helper library the cvc5/Boost producers load but is not part of "
					"any package identity")
			endif()
		endforeach()
	endif()
endfunction()
