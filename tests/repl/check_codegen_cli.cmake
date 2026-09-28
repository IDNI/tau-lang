# Drives one `tau compile` exit-code case: writes the spec to a scratch file,
# runs the verb, and prints the captured output plus `EXIT=<code>` so the
# caller's PASS/FAIL regexes see the same shape the shell case used to print.
# A CMake script so the case needs no bash, no mktemp and no trap.
#   -DNO_LTLSYNT=<dir>  makes the synthesis backend unfindable: PATH and
#                      TAU_SPOT_BIN point at this empty folder alone.
# Usage: cmake -DTAU=<tau> -DSPEC_TEXT=<spec> -DTMP=<scratch> -P this.
if(NOT TAU OR NOT SPEC_TEXT OR NOT TMP)
	message(FATAL_ERROR
		"usage: -DTAU=<tau> -DSPEC_TEXT=<spec> -DTMP=<scratch>")
endif()

file(REMOVE_RECURSE "${TMP}")
file(MAKE_DIRECTORY "${TMP}")
file(WRITE "${TMP}/spec.tau" "${SPEC_TEXT}")

# The children inherit this script's environment. At most one folder is set,
# so no list separator is involved and CMake keeps the value intact.
if(NO_LTLSYNT)
	file(MAKE_DIRECTORY "${NO_LTLSYNT}")
	set(ENV{PATH} "${NO_LTLSYNT}")
	set(ENV{TAU_SPOT_BIN} "${NO_LTLSYNT}")
endif()

execute_process(COMMAND "${TAU}" compile "${TMP}/spec.tau" -o "${TMP}/exe"
	RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 600)
message("${_out}${_err}")
message("EXIT=${_rc}")
file(REMOVE_RECURSE "${TMP}")
