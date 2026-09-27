# The map from a tau preset name to the platform it builds in, and the platform
# names themselves. Derived from CMakePresets.json so no second copy of the
# list can drift: a platform is the build folder of a visible configure preset,
# restricted to the three build types and the toolchain families the artifact
# presets carry.
#
# One collector serves both consumers -- the cmake file written into an SDK
# (tau_write_platform_map) and the table compiled into tau
# (cmake/tau_bas.cmake) -- so the SDK map and the compiled map cannot drift.
# Script-mode safe: it reads PROJECT_SOURCE_DIR and needs no configured build,
# which lets a `cmake -P` driver print the table the header will carry.
include_guard(GLOBAL)

# Sets TAU_PLATFORM_NAMES (the platform names) and TAU_PRESET_PLATFORM_MAP
# (one `preset|platform` item each) in the caller's scope.
function(tau_collect_platform_map)
	file(READ "${PROJECT_SOURCE_DIR}/CMakePresets.json" _json)
	string(JSON _count LENGTH "${_json}" configurePresets)
	math(EXPR _last "${_count} - 1")
	set(_presets "")
	foreach(_i RANGE 0 ${_last})
		string(JSON _name GET "${_json}" configurePresets "${_i}" name)
		list(APPEND _presets "${_name}")
		# inherits is a string or an array of strings
		string(JSON _ninh ERROR_VARIABLE _err LENGTH
			"${_json}" configurePresets "${_i}" inherits)
		if("${_err}" STREQUAL "NOTFOUND")
			set(_inh "")
			math(EXPR _li "${_ninh} - 1")
			foreach(_k RANGE 0 ${_li})
				string(JSON _e GET "${_json}" configurePresets
					"${_i}" inherits "${_k}")
				list(APPEND _inh "${_e}")
			endforeach()
		else()
			string(JSON _e ERROR_VARIABLE _err GET "${_json}"
				configurePresets "${_i}" inherits)
			if("${_err}" STREQUAL "NOTFOUND")
				set(_inh "${_e}")
			else()
				set(_inh "")
			endif()
		endif()
		string(JSON _bd ERROR_VARIABLE _err GET "${_json}"
			configurePresets "${_i}" binaryDir)
		if(NOT "${_err}" STREQUAL "NOTFOUND")
			set(_bd "")
		endif()
		set(_tau_inherits_${_name} "${_inh}")
		set(_tau_binary_dir_${_name} "${_bd}")
	endforeach()

	set(_platforms "")
	set(_entries "")
	foreach(_name IN LISTS _presets)
		# Walk to the nearest preset that sets binaryDir: a pack variant
		# that overrides it lands on its own folder and is filtered out.
		set(_cur "${_name}")
		set(_seen "")
		set(_bd "")
		while(NOT "${_cur}" STREQUAL "")
			if(NOT "${_tau_binary_dir_${_cur}}" STREQUAL "")
				set(_bd "${_tau_binary_dir_${_cur}}")
				break()
			endif()
			if("${_cur}" IN_LIST _seen)
				break()
			endif()
			list(APPEND _seen "${_cur}")
			if("${_tau_inherits_${_cur}}" STREQUAL "")
				break()
			endif()
			list(GET _tau_inherits_${_cur} 0 _cur)
		endwhile()
		if("${_bd}" STREQUAL "")
			continue()
		endif()
		string(REPLACE "\${sourceDir}/build/" "" _platform "${_bd}")
		if(NOT _platform MATCHES
			"^(release|devel|debug)(-gcc|-w64|-msvc|-msvc-clang-cl|-wasm|-wasm-nothreads)?$")
			continue()
		endif()
		list(APPEND _entries "${_name}|${_platform}")
		if(NOT _platform IN_LIST _platforms)
			list(APPEND _platforms "${_platform}")
		endif()
	endforeach()
	list(SORT _platforms)
	set(TAU_PLATFORM_NAMES "${_platforms}" PARENT_SCOPE)
	set(TAU_PRESET_PLATFORM_MAP "${_entries}" PARENT_SCOPE)
endfunction()
