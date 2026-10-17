# `tau compile --machine`: compiles a spec, requires the table main, runs the
# program with no ltlsynt reachable (PATH empty, TAU_SPOT_BIN unset) on the
# given input lines, and requires each EXPECT line among its outputs and the
# outputs to match EXPECT_REGEX.
# Usage: cmake -DTAU=<tau-binary> -DSPEC=<spec text> -DTMP=<scratch>
#               -DEXE_SUFFIX=<.exe|> [-DINPUT=<line|line|...>]
#               [-DEXPECT=<line|line|...>] [-DEXPECT_REGEX=<re>] -P this.
if(NOT TAU OR NOT SPEC OR NOT TMP)
	message(FATAL_ERROR "usage: -DTAU=<tau> -DSPEC=<spec> -DTMP=<scratch> "
		"-DEXE_SUFFIX=<.exe|> [-DINPUT=...] [-DEXPECT=...] "
		"[-DEXPECT_REGEX=...] -P check_compile_machine.cmake")
endif()

string(REPLACE "|" ";" INPUT "${INPUT}")
string(REPLACE "|" ";" EXPECT "${EXPECT}")
file(REMOVE_RECURSE "${TMP}")
file(MAKE_DIRECTORY "${TMP}/empty_path")
file(WRITE "${TMP}/spec.tau" "${SPEC}\n")
set(_exe "${TMP}/spec_exe${EXE_SUFFIX}")

execute_process(COMMAND "${TAU}" compile --machine "${TMP}/spec.tau"
		-o "${_exe}"
	RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 600)
if(NOT _rc STREQUAL "0" OR NOT EXISTS "${_exe}")
	message("FAIL: tau compile --machine exited ${_rc}")
	message("${_out}${_err}")
	message(FATAL_ERROR "compile failed")
endif()
file(READ "${TMP}/spec.tau.build/main.cpp" _main)
if(NOT _main MATCHES "table_step_provider<node_t>>\\(" OR _main MATCHES
		"get_interpreter")
	message(FATAL_ERROR "FAIL: the program is not the table of the machine")
endif()

file(WRITE "${TMP}/program_in" "")
foreach(_line IN LISTS INPUT)
	file(APPEND "${TMP}/program_in" "${_line}\n")
endforeach()
execute_process(COMMAND ${CMAKE_COMMAND} -E env --unset=TAU_SPOT_BIN
		"PATH=${TMP}/empty_path" "${_exe}"
	INPUT_FILE "${TMP}/program_in"
	RESULT_VARIABLE _prc OUTPUT_VARIABLE _pout ERROR_VARIABLE _perr
	TIMEOUT 60)

# "name[t] := value" per output; the escapes of a colored run go first
string(ASCII 27 _esc)
string(REGEX REPLACE "${_esc}\\[[0-9;]*m" "" _clean "${_pout}${_perr}")
# an output follows the prompt of an input on its line
string(REGEX MATCHALL "o[0-9]+\\[[0-9]+\\] := [^ \r\n]+" _outputs
	"${_clean}")
string(REPLACE ";" "\n" _shown "${_outputs}")
if(_outputs STREQUAL "")
	message("${_pout}${_perr}")
	message(FATAL_ERROR "FAIL: the program printed no output")
endif()
foreach(_want IN LISTS EXPECT)
	if(NOT _want IN_LIST _outputs)
		message("program:\n${_shown}")
		message(FATAL_ERROR "FAIL: no output ${_want}")
	endif()
endforeach()
if(DEFINED EXPECT_REGEX AND NOT _shown MATCHES "${EXPECT_REGEX}")
	message("program:\n${_shown}")
	message(FATAL_ERROR "FAIL: the outputs do not match ${EXPECT_REGEX}")
endif()
list(LENGTH _outputs _count)
message("MACHINE: ${_count} outputs")
file(REMOVE_RECURSE "${TMP}")
