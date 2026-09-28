# `tau compile` end-to-end smoke, written as a CMake script so the case needs
# no shell: copies the spec to a scratch dir (so the emitted spec.build/ lands
# outside the source tree), compiles it, then runs the produced executable.
# Usage: cmake -DTAU=<tau> -DSPEC=<spec> -DTMP=<scratch> -DEXE_SUFFIX=<.exe|>
#               -P check_compile_verb.cmake
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

execute_process(COMMAND "${TAU}" compile "${_spec}" -o "${_exe}"
	RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 600)
if(NOT _rc STREQUAL "0")
	message(FATAL_ERROR "tau compile exited ${_rc}\n${_out}${_err}")
endif()
if(NOT EXISTS "${_exe}")
	message(FATAL_ERROR "no executable produced at ${_exe}\n${_out}${_err}")
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
