# `tau compile --machine`: compiles a spec, requires the table main, runs the
# program with no ltlsynt reachable (PATH empty, TAU_SPOT_BIN unset) on the
# given input lines (or on STEPS steps of seeded random bits), requires each
# EXPECT line among its outputs and the outputs to match EXPECT_REGEX, and
# checks the trace against the spec: `sat` of the spec, its inputs renamed
# to outputs, and the trace's values at their fixed positions must be T. A
# trace that breaks a clause no continuation repairs gives F. The spec names
# its streams i<N> and o<N>.
# Usage: cmake -DTAU=<tau-binary> -DSPEC=<spec text> -DTMP=<scratch>
#               -DEXE_SUFFIX=<.exe|> [-DINPUT=<line|line|...>]
#               [-DSTEPS=<n> [-DSEED=<n>]]
#               [-DEXPECT=<line|line|...>] [-DEXPECT_REGEX=<re>] -P this.
if(NOT TAU OR NOT SPEC OR NOT TMP)
	message(FATAL_ERROR "usage: -DTAU=<tau> -DSPEC=<spec> -DTMP=<scratch> "
		"-DEXE_SUFFIX=<.exe|> [-DINPUT=...] [-DEXPECT=...] "
		"[-DEXPECT_REGEX=...] -P check_compile_machine.cmake")
endif()

string(REPLACE "|" ";" INPUT "${INPUT}")
string(REPLACE "|" ";" EXPECT "${EXPECT}")
if(STEPS)
	string(REGEX MATCHALL "i[0-9]+\\[" _ins "${SPEC}")
	list(REMOVE_DUPLICATES _ins)
	list(LENGTH _ins _nin)
	math(EXPR _bits "${STEPS} * ${_nin}")
	if(NOT DEFINED SEED)
		set(SEED 1)
	endif()
	set(INPUT "")
	if(_bits GREATER 0)
		string(RANDOM LENGTH ${_bits} ALPHABET 01 RANDOM_SEED ${SEED} _rand)
		string(REGEX MATCHALL "." INPUT "${_rand}")
	endif()
endif()
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
# the trace: each input prompt took the next input line
string(REGEX MATCHALL "i[0-9]+\\[[0-9]+\\] := " _prompts "${_clean}")
set(_trace "")
set(_k 0)
list(LENGTH INPUT _nlines)
foreach(_p IN LISTS _prompts)
	if(_k GREATER_EQUAL _nlines)
		break()
	endif()
	list(GET INPUT ${_k} _v)
	string(REGEX REPLACE "^i([0-9]+\\[[0-9]+\\]) := $" "oi\\1" _pos "${_p}")
	string(APPEND _trace " && ${_pos}:bv[1] = ${_v}")
	math(EXPR _k "${_k} + 1")
endforeach()
foreach(_o IN LISTS _outputs)
	string(REGEX REPLACE "^(o[0-9]+\\[[0-9]+\\]) := (.*)$" "\\1:bv[1] = \\2"
		_atom "${_o}")
	string(APPEND _trace " && ${_atom}")
endforeach()
string(REGEX REPLACE "(^|[^A-Za-z0-9_])i([0-9]+\\[)" "\\1oi\\2" _renamed
	"${SPEC}")
execute_process(COMMAND "${TAU}" --color=false -S error
		-e "sat (${_renamed})${_trace}"
	RESULT_VARIABLE _src OUTPUT_VARIABLE _sout ERROR_VARIABLE _serr
	TIMEOUT 600)
if(NOT _sout MATCHES ": T\n")
	message("program:\n${_shown}")
	message("sat (${_renamed})${_trace}\n${_sout}${_serr}")
	message(FATAL_ERROR "FAIL: the trace does not satisfy the spec")
endif()
list(LENGTH _outputs _count)
message("MACHINE: ${_count} outputs, trace checked")
file(REMOVE_RECURSE "${TMP}")
