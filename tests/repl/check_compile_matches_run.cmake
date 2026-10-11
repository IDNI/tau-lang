# `tau compile` against `run`: compiles a spec with no inputs, runs the
# program and `run N steps` of the same spec, and requires both to print the
# same value for every output of the first N steps. A CMake script so the case
# needs no shell and no mktemp.
# Usage: cmake -DTAU=<tau-binary> -DSTEPS=<n> -DSPEC=<formula>
#               -DTMP=<scratch> -DEXE_SUFFIX=<.exe|> -P this.
if(NOT TAU OR NOT STEPS OR NOT SPEC OR NOT TMP)
	message(FATAL_ERROR "usage: -DTAU=<tau> -DSTEPS=<n> -DSPEC=<formula> "
		"-DTMP=<scratch> -DEXE_SUFFIX=<.exe|> -P check_compile_matches_run.cmake")
endif()

file(REMOVE_RECURSE "${TMP}")
file(MAKE_DIRECTORY "${TMP}")
file(WRITE "${TMP}/spec.tau" "${SPEC}")
set(_exe "${TMP}/spec_exe${EXE_SUFFIX}")

execute_process(COMMAND "${TAU}" compile "${TMP}/spec.tau" -o "${_exe}"
	RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 600)
if(NOT _rc STREQUAL "0" OR NOT EXISTS "${_exe}")
	message("FAIL: tau compile exited ${_rc}")
	message("${_out}${_err}")
	message(FATAL_ERROR "compile failed")
endif()

# The outputs of the first STEPS steps, one "name[t] := value" per line, as
# `run` prints them. The legacy REPL colorizes its output, so the escapes go
# before splitting the lines.
function(tau_step_outputs raw out_var)
	string(ASCII 27 _esc)
	string(REGEX REPLACE "${_esc}\\[[0-9;]*m" "" _clean "${raw}")
	string(REPLACE "\r\n" "\n" _clean "${_clean}")
	string(REPLACE "\n" ";" _lines "${_clean}")
	set(_kept "")
	foreach(_line IN LISTS _lines)
		string(STRIP "${_line}" _line)
		if(NOT _line MATCHES "^o[0-9]+\\[[0-9]+\\] := ")
			continue()
		endif()
		string(REGEX MATCH "\\[([0-9]+)\\]" _unused "${_line}")
		if(CMAKE_MATCH_1 LESS STEPS)
			list(APPEND _kept "${_line}")
		endif()
	endforeach()
	list(SORT _kept)
	string(REPLACE ";" "\n" _kept "${_kept}")
	set(${out_var} "${_kept}" PARENT_SCOPE)
endfunction()

# The program asks for ENTER at each step that reads no input.
file(WRITE "${TMP}/program_in" "")
math(EXPR _last_step "${STEPS} - 1")
foreach(_i RANGE 0 ${_last_step})
	file(APPEND "${TMP}/program_in" "\n")
endforeach()
execute_process(COMMAND "${_exe}" INPUT_FILE "${TMP}/program_in"
	RESULT_VARIABLE _prc OUTPUT_VARIABLE _pout ERROR_VARIABLE _perr
	TIMEOUT 60)
tau_step_outputs("${_pout}${_perr}" _program)

file(WRITE "${TMP}/run_in" "run ${STEPS} steps ${SPEC}.\nq\n")
execute_process(COMMAND "${TAU}" -X INPUT_FILE "${TMP}/run_in"
	WORKING_DIRECTORY "${TMP}"
	RESULT_VARIABLE _rrc OUTPUT_VARIABLE _rout ERROR_VARIABLE _rerr
	TIMEOUT 300)
tau_step_outputs("${_rout}${_rerr}" _run)

if(_program STREQUAL "")
	message(FATAL_ERROR "FAIL: the program printed no output")
endif()
if(NOT _program STREQUAL _run)
	message("FAIL: the program and run differ")
	message("program:\n${_program}")
	message("run:\n${_run}")
	message(FATAL_ERROR "mismatch")
endif()

string(REPLACE "\n" ";" _program_lines "${_program}")
list(LENGTH _program_lines _count)
message("SAME: ${_count} outputs")
file(REMOVE_RECURSE "${TMP}")
