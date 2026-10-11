# `tau compile` path cases: a build never writes over its own spec, an output
# equal to the spec is refused, and a relative -o resolves against the caller's
# working directory instead of the artifact directory.
# Usage: cmake -DTAU=<tau> -DSPEC=<spec> -DTMP=<scratch> -DCASE=<case>
#               [-DPRESET=<platform>] [-DSDK=<sdk-dir>]
#               -P check_compile_paths.cmake
if(NOT TAU OR NOT SPEC OR NOT TMP OR NOT CASE)
	message(FATAL_ERROR "usage: -DTAU=<tau> -DSPEC=<spec> -DTMP=<scratch> "
		"-DCASE=<case> -P check_compile_paths.cmake")
endif()

file(REMOVE_RECURSE "${TMP}")
file(MAKE_DIRECTORY "${TMP}")
get_filename_component(_spec_name "${SPEC}" NAME)
set(_spec "${TMP}/${_spec_name}")
file(COPY_FILE "${SPEC}" "${_spec}")
file(SHA256 "${_spec}" _before)

set(_extra "")
if(PRESET)
	list(APPEND _extra --preset "${PRESET}")
endif()

# TAU_SDK_DIR pins the SDK the compile script uses; a source-tree build has its
# own box, so the case omits it otherwise.
set(_command "${TAU}")
if(SDK)
	set(_command "${CMAKE_COMMAND}" -E env "TAU_SDK_DIR=${SDK}" "${TAU}")
endif()

if(CASE STREQUAL "same_spec")
	# -o naming the spec is refused before any build and leaves it untouched.
	execute_process(COMMAND ${_command} compile "${_spec}" -o "${_spec}"
			${_extra}
		WORKING_DIRECTORY "${TMP}"
		RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err
		TIMEOUT 300)
	if(_rc STREQUAL "0")
		message(FATAL_ERROR "tau compile -o <spec> succeeded\n${_out}${_err}")
	endif()
	file(SHA256 "${_spec}" _after)
	if(NOT _before STREQUAL _after)
		message(FATAL_ERROR "tau compile -o <spec> changed the spec")
	endif()
elseif(CASE STREQUAL "default_out")
	# No -o: the default output drops the extension. The build must not touch
	# the spec, and a spec with no extension must be refused as its own output.
	execute_process(COMMAND ${_command} compile "${_spec}" ${_extra}
		WORKING_DIRECTORY "${TMP}"
		RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err
		TIMEOUT 900)
	if(NOT _rc STREQUAL "0")
		message(FATAL_ERROR "tau compile (default output) exited "
			"${_rc}\n${_out}${_err}")
	endif()
	get_filename_component(_stem "${_spec}" NAME_WE)
	if(NOT EXISTS "${TMP}/${_stem}")
		message(FATAL_ERROR "no default output at ${TMP}/${_stem}\n"
			"${_out}${_err}")
	endif()
	file(SHA256 "${_spec}" _after)
	if(NOT _before STREQUAL _after)
		message(FATAL_ERROR "tau compile changed the spec")
	endif()
elseif(CASE STREQUAL "relative_out")
	# A relative -o resolves against the caller's directory, never against
	# the artifact directory the nested configure runs in.
	execute_process(COMMAND ${_command} compile "${_spec}" -o sim ${_extra}
		WORKING_DIRECTORY "${TMP}"
		RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err
		TIMEOUT 900)
	if(NOT _rc STREQUAL "0")
		message(FATAL_ERROR "tau compile -o sim exited ${_rc}\n${_out}${_err}")
	endif()
	if(NOT EXISTS "${TMP}/sim" AND NOT EXISTS "${TMP}/sim.exe")
		message(FATAL_ERROR "no relative output at ${TMP}/sim\n"
			"${_out}${_err}")
	endif()
else()
	message(FATAL_ERROR "unknown CASE ${CASE}")
endif()

file(REMOVE_RECURSE "${TMP}")
