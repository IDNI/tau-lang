# `tau compile --cxx <name>` must hand the name through to the emitted
# project's configure instead of replacing it with whatever compiler is on
# PATH. A CMake script so the case needs no shell, no mktemp and no grep.
# Usage: cmake -DTAU=<tau> -DSPEC=<spec> -DTMP=<scratch> -P this.
if(NOT TAU OR NOT SPEC OR NOT TMP)
	message(FATAL_ERROR "usage: -DTAU=<tau> -DSPEC=<spec> -DTMP=<scratch>")
endif()

file(REMOVE_RECURSE "${TMP}")
file(MAKE_DIRECTORY "${TMP}")
# The build tree is placed beside the spec file, so it gets a stable name.
set(_spec "${TMP}/spec.tau")
file(COPY_FILE "${SPEC}" "${_spec}")

execute_process(COMMAND "${TAU}" compile --cxx /nonexistent/c++ "${_spec}"
	-o "${TMP}/out"
	RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 600)
if(_rc STREQUAL "0")
	message(FATAL_ERROR "tau compile succeeded with a nonexistent --cxx")
endif()

# The configure log names the compiler the configure was told to use; the
# caller's PASS regex matches that name, so it must reach the output.
set(_log "${_spec}.build/configure.log")
if(EXISTS "${_log}")
	file(READ "${_log}" _log_text)
	message("${_log_text}")
else()
	message(FATAL_ERROR "no configure log at ${_log}\n${_out}${_err}")
endif()

file(REMOVE_RECURSE "${TMP}")
