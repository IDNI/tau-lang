# cmake/tau-shared-helpers.provenance.cmake
#
# Provenance of the shared build helpers vendored into Tau's cmake/. The copied
# helpers only warn when the parser's copy has moved on.
#
# Each entry is "<vendored>|<parser path>|<sha256 of the parser file>|<mode>".
# mode is "strict" or "warn".
set(TAU_SHARED_HELPERS_DIR "${CMAKE_CURRENT_LIST_DIR}")

set(TAU_SHARED_HELPER_PROVENANCE
	"tau-shared-resolve.cmake|cmake/tau-resolve.cmake|8c68acb3536ea71ea63a3f34791087b68ede2b4e55f5eb4a85fcbd786dacf566|warn"
	"tau-shared-ccache.cmake|cmake/use-ccache.cmake|0e178c553711db49560551f7e123322284824f7124e51a1759e3f912bb71105a|warn"
	"tau-shared-emscripten.cmake|cmake/use-emscripten.cmake|981a7a80f819a413cc56b88b1b626187851a008f28ec718cae6a52d82e7d52f5|warn"
	"tau-shared-version-license.cmake|cmake/version_license.cmake|feb5407b9cd874f5ec786145d9c7af3d361f221be6576e77f9e9e03b7fa12ab2|warn"
	"cmake/toolchains/mingw-w64-x86_64.cmake|cmake/mingw-w64-x86_64.cmake|a61fe1d3ffbf2d259844da1b5cff716bada0cd1066655b11ee4889bc4168a0af|warn"
	"cmake/toolchains/aarch64-linux-gnu.cmake|cmake/aarch64-linux-gnu.cmake|d8ce21f137273a24b1a710049c32793148dad3a31bcfafce81a3bbd1c6070e62|warn"
)

# Files that choose which helper library a producer loads. A strict content
# guard catches a change a package identity would miss. Each entry is
# "<repo-relative path>|<sha256>".
set(TAU_SHIM_PROVENANCE
	"external/parser/scripts/devrc|2857ea1e205f03cbc1d6a0c52d056cb93e520c1b932d6d447745bf90d2b17a6a"
	"scripts/devrc|1a31e751f7dfaa5a65c83612c7429cf89a35dcda74b2890fbecdd69624b62eb6"
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
