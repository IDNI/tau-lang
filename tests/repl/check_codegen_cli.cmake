# Drives one `tau compile` exit-code case: writes the spec to a scratch file,
# runs the verb, and prints the captured output plus `EXIT=<code>` so the
# caller's PASS/FAIL regexes see the same shape the shell case used to print.
# A CMake script so the case needs no bash, no mktemp and no trap.
#   -DNO_LTLSYNT=<dir>   makes the synthesis backend unfindable: PATH and
#                        TAU_SPOT_BIN point at this empty folder alone.
#   -DSTUB_PATH=<dir>    prepends <dir> to PATH so a stub tool is found first.
#   -DENV_EXTRA=<list>   NAME=VALUE items set in the child environment.
#   -DTAU_FLAGS=<text>   extra `tau` arguments, before the verb.
#   -DPROGRAM_STDIN=<s>  text, with `\n` for a newline, piped to the built
#                        program after the build.
#   -DEXE_SUFFIX=<.exe|> the executable suffix of the host.
# Usage: cmake -DTAU=<tau> -DSPEC_TEXT=<spec> -DTMP=<scratch> -P this.
if(NOT TAU OR NOT SPEC_TEXT OR NOT TMP)
	message(FATAL_ERROR
		"usage: -DTAU=<tau> -DSPEC_TEXT=<spec> -DTMP=<scratch>")
endif()
if(NOT DEFINED EXE_SUFFIX)
	set(EXE_SUFFIX "")
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
if(STUB_PATH)
	if(CMAKE_HOST_WIN32)
		set(_sep ";")
	else()
		set(_sep ":")
	endif()
	set(ENV{PATH} "${STUB_PATH}${_sep}$ENV{PATH}")
endif()
foreach(_env_item IN LISTS ENV_EXTRA)
	string(FIND "${_env_item}" "=" _eq)
	if(_eq LESS 0)
		message(FATAL_ERROR "ENV_EXTRA item is not NAME=VALUE: ${_env_item}")
	endif()
	string(SUBSTRING "${_env_item}" 0 ${_eq} _env_name)
	math(EXPR _env_start "${_eq} + 1")
	string(SUBSTRING "${_env_item}" ${_env_start} -1 _env_value)
	set(ENV{${_env_name}} "${_env_value}")
endforeach()

set(_tau_flags "")
if(TAU_FLAGS)
	separate_arguments(_tau_flags NATIVE_COMMAND "${TAU_FLAGS}")
endif()
execute_process(COMMAND "${TAU}" ${_tau_flags} compile "${TMP}/spec.tau"
		-o "${TMP}/exe${EXE_SUFFIX}"
	RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 600)
message("${_out}${_err}")
message("EXIT=${_rc}")

if(PROGRAM_STDIN)
	string(REPLACE "\\n" "\n" _stdin "${PROGRAM_STDIN}")
	file(WRITE "${TMP}/program_in" "${_stdin}")
	execute_process(COMMAND "${TMP}/exe${EXE_SUFFIX}"
		INPUT_FILE "${TMP}/program_in"
		RESULT_VARIABLE _prc OUTPUT_VARIABLE _pout ERROR_VARIABLE _perr
		TIMEOUT 600)
	message("${_pout}${_perr}")
endif()
file(REMOVE_RECURSE "${TMP}")
