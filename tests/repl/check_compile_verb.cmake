# `tau compile` end-to-end smoke, written as a CMake script so the case needs
# no shell: copies the spec to a scratch dir (so the emitted spec.build/ lands
# outside the source tree), compiles it, then runs the produced executable.
# Usage: cmake -DTAU=<tau> -DSPEC=<spec> -DTMP=<scratch> -DEXE_SUFFIX=<.exe|>
#               [-DPRESET=<platform>] [-DSDK=<sdk-dir>] [-DTAU_CXX_ENV=<cxx>]
#               [-DEXTRA_ARGS=<list>] [-DEXPECT_FAIL=ON] [-DEXPECT_LOG=<text>]
#               [-DNO_RUN=ON] -P check_compile_verb.cmake
if(NOT TAU OR NOT SPEC OR NOT TMP)
	message(FATAL_ERROR "usage: -DTAU=<tau> -DSPEC=<spec> -DTMP=<scratch> "
		"-DEXE_SUFFIX=<.exe|> -P check_compile_verb.cmake")
endif()

file(REMOVE_RECURSE "${TMP}")
file(MAKE_DIRECTORY "${TMP}")
get_filename_component(_spec_name "${SPEC}" NAME)
set(_spec "${TMP}/${_spec_name}")
file(COPY_FILE "${SPEC}" "${_spec}")
set(_exe "${TMP}/spec_exe${EXE_SUFFIX}")

set(_extra "")
if(PRESET)
	list(APPEND _extra --preset "${PRESET}")
endif()
list(APPEND _extra ${EXTRA_ARGS})

# TAU_SDK_DIR pins the SDK the compile script uses. It is set only when the
# host SDK is also the target platform; a cross case omits it and `tau compile`
# passes the target box it resolved itself. TAU_CXX_ENV names the compiler in
# the environment instead of on the command line.
set(_env "")
if(SDK)
	list(APPEND _env "TAU_SDK_DIR=${SDK}")
endif()
if(TAU_CXX_ENV)
	list(APPEND _env "TAU_CXX=${TAU_CXX_ENV}")
endif()
if(_env)
	set(_command "${CMAKE_COMMAND}" -E env ${_env} "${TAU}")
else()
	set(_command "${TAU}")
endif()

execute_process(COMMAND ${_command} compile "${_spec}" -o "${_exe}"
		${_extra}
	RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 600)
if(EXPECT_FAIL)
	if(_rc STREQUAL "0")
		message(FATAL_ERROR "tau compile was expected to fail\n${_out}${_err}")
	endif()
	if(EXPECT_LOG)
		set(_log "${_spec}.build/configure.log")
		if(NOT EXISTS "${_log}")
			message(FATAL_ERROR "no configure log at ${_log}\n${_out}${_err}")
		endif()
		file(READ "${_log}" _log_text)
		string(FIND "${_log_text}" "${EXPECT_LOG}" _found)
		if(_found EQUAL -1)
			message(FATAL_ERROR "configure.log does not name "
				"${EXPECT_LOG}\n${_log_text}")
		endif()
	endif()
	file(REMOVE_RECURSE "${TMP}")
	return()
endif()
if(NOT _rc STREQUAL "0")
	set(_script_log "${_spec}.build/compile.log")
	set(_script_text "")
	if(EXISTS "${_script_log}")
		file(READ "${_script_log}" _script_text)
	endif()
	message(FATAL_ERROR "tau compile exited ${_rc}\n${_out}${_err}\n"
		"${_script_log}:\n${_script_text}")
endif()
if(NOT EXISTS "${_exe}")
	message(FATAL_ERROR "no executable produced at ${_exe}\n${_out}${_err}")
endif()

# A -D must reach the emitted project's configure: an unused cache variable
# makes cmake name it in the configure log.
if(EXPECT_LOG)
	set(_log "${_spec}.build/configure.log")
	if(NOT EXISTS "${_log}")
		message(FATAL_ERROR "no configure log at ${_log}")
	endif()
	file(READ "${_log}" _log_text)
	string(FIND "${_log_text}" "${EXPECT_LOG}" _found)
	if(_found EQUAL -1)
		message(FATAL_ERROR "configure.log does not name "
			"${EXPECT_LOG}\n${_log_text}")
	endif()
endif()

if(NO_RUN)
	file(REMOVE_RECURSE "${TMP}")
	return()
endif()

# stdin closed: a spec with no inputs must terminate on its own, and an
# inherited stdin that never closes (an IDE, a tool socket) would hold the
# executable at its input prompt until the timeout.
file(WRITE "${TMP}/empty_stdin" "")
execute_process(COMMAND "${_exe}" INPUT_FILE "${TMP}/empty_stdin"
	RESULT_VARIABLE _rrc OUTPUT_VARIABLE _rout ERROR_VARIABLE _rerr TIMEOUT 30)
if(NOT _rrc STREQUAL "0")
	message(FATAL_ERROR "compiled executable exited ${_rrc}\n${_rout}${_rerr}")
endif()

file(REMOVE_RECURSE "${TMP}")
