# Tests `tau compile <spec.tau>`. A successful compile exits 0. Every compile
# failure exits 1. The report names the reason (UNREALIZABLE, a missing
# backend, and so on).

include(add_repl_test)

# The case logic is a CMake script, so the case needs no bash, no mktemp and
# no trap.
set(TAU_CODEGEN_CLI_CHECKER "${CMAKE_CURRENT_LIST_DIR}/../check_codegen_cli.cmake")

# `tau compile` reads its spec from a file. Each case hands its formula to
# the script, which writes a scratch file and passes that path. The
# backend-failure case hides ltlsynt with an empty PATH and TAU_SPOT_BIN.

# add_codegen_cli_test(<name> <spec_text> <scratch_stem> <pass_regex>
#     [FAIL_REGEX <re>] [NO_FAIL_REGEX] [NO_LTLSYNT <dir>])
#
# `tau compile` writes its spec to a host path and spawns a host compiler
# (src/tau_compile.tmpl.h), so a wasm node host cannot run these even with the
# NODEFS filesystem. NO_LTLSYNT points PATH and TAU_SPOT_BIN at an empty folder.
function(add_codegen_cli_test test_name spec_text scratch_stem pass_regex)
	cmake_parse_arguments(PARSE_ARGV 4 _tau "NO_FAIL_REGEX" "NO_LTLSYNT"
		"FAIL_REGEX;REQUIRES")
	list(APPEND _tau_REQUIRES subprocess)
	tau_repl_gate_case("${test_name}" spec_text)
	set(_args "-DTAU=${TAU_LAUNCHER}" "-DSPEC_TEXT=${spec_text}")
	if(_tau_NO_LTLSYNT)
		list(APPEND _args "-DNO_LTLSYNT=${_tau_NO_LTLSYNT}")
	endif()
	list(APPEND _args "-DTMP=${CMAKE_BINARY_DIR}/${scratch_stem}")
	add_test(NAME "${test_name}"
		COMMAND ${CMAKE_COMMAND} ${_args} -P "${TAU_CODEGEN_CLI_CHECKER}")
	tau_repl_check_case("${test_name}" "${pass_regex}")
endfunction()

add_codegen_cli_test(test_codegen_cli-always_one_emits "always o1[t] = 1"
	"test_codegen_cli-always_one.scratch"
	"compiled:(.*\n)*.*EXIT=0" FAIL_REGEX "EXIT=1")

add_codegen_cli_test(test_codegen_cli-unrealizable_exit_3
	"always (o1[t] = 1 && o1[t] = 0)"
	"test_codegen_cli-unrealizable.scratch"
	"compile: spec is UNREALIZABLE(.*\n)*.*EXIT=1" NO_FAIL_REGEX)

# The message follows the three-valued verdict. A spec with no strategy that
# `realizable` decides F keeps UNREALIZABLE...
add_codegen_cli_test(test_codegen_cli-unrealizable_full_ltl
	"G (o1[t] = 0) && F (o1[t] = 1)"
	"test_codegen_cli-unrealizable_full_ltl.scratch"
	"compile: spec is UNREALIZABLE(.*\n)*.*EXIT=1" FAIL_REGEX "UNKNOWN")

# ...and one it leaves undecided is UNKNOWN, with the budget that stopped it.
# The stub answers UNREALIZABLE for the abstraction and outlasts the 1 s
# ltl-timeout on the data game (tests/repl/stubs/slow_game/ltlsynt); the input
# atom reads two steps, so no other check decides the spec.
set(_unknown_compile "set -u; d=$(mktemp -d) || exit 1; trap 'rm -rf \"$d\"' EXIT; printf '%s' 'F (o1[t] = 1 && i1[t] != i1[t-1])' > \"$d/spec.tau\"; TAU_LTL_TIMEOUT_SEC=1 PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/slow_game:$PATH $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> compile \"$d/spec.tau\" -o \"$d/exe\" 2>&1; echo EXIT=$?")
add_test(NAME "test_codegen_cli-unknown_is_not_unrealizable"
	COMMAND bash -c "${_unknown_compile}")
set_tests_properties("test_codegen_cli-unknown_is_not_unrealizable" PROPERTIES
	PASS_REGULAR_EXPRESSION "compile: the realizability of the spec is UNKNOWN.*EXIT=1"
	FAIL_REGULAR_EXPRESSION "UNREALIZABLE")
add_test(NAME "test_codegen_cli-unknown_names_the_budget"
	COMMAND bash -c "${_unknown_compile}")
set_tests_properties("test_codegen_cli-unknown_names_the_budget" PROPERTIES
	PASS_REGULAR_EXPRESSION "killed by the ltl-timeout watchdog")

# The same budget given as the option: the verb reads the global options
# (without them each stubbed game call waits out the 60 s default).
add_test(NAME "test_codegen_cli-compile_reads_the_ltl_timeout_option"
	COMMAND bash -c "set -u; d=$(mktemp -d) || exit 1; trap 'rm -rf \"$d\"' EXIT; printf '%s' 'F (o1[t] = 1 && i1[t] != i1[t-1])' > \"$d/spec.tau\"; PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/slow_game:$PATH $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --ltl-timeout 1 compile \"$d/spec.tau\" -o \"$d/exe\" 2>&1; echo EXIT=$?")
set_tests_properties("test_codegen_cli-compile_reads_the_ltl_timeout_option" PROPERTIES
	PASS_REGULAR_EXPRESSION "compile: the realizability of the spec is UNKNOWN.*EXIT=1"
	TIMEOUT 30)

# With no synthesis backend the verb must fail. It must not claim
# UNREALIZABLE, which the solver returns only for a decided spec. An empty
# PATH and TAU_SPOT_BIN hide ltlsynt on POSIX and on Windows without a stub.
add_codegen_cli_test(test_codegen_cli-backend_failure_exit_4 "F (o1[t] = 1)"
	"test_codegen_cli-backend_failure.scratch"
	"ltlsynt not found(.*\n)*.*EXIT=1" FAIL_REGEX "UNREALIZABLE|terminate called"
	NO_LTLSYNT "${CMAKE_BINARY_DIR}/test_codegen_cli-backend_failure.no-ltlsynt")

# only the data game decides this spec: the program plays the Mealy view of
# that game's strategy, as `run` does
set(_dg_spec "(sometimes (o2[t]:bv[1] = i2[t-1]:bv[1])) && (sometimes ((i1[t-1]:bv[1] = i1[t]:bv[1] || i1[t-1]:bv[1] = 1)))")
tau_repl_unsupported(_tau_skip "${_dg_spec}")
if(NOT _tau_skip)
	add_test(NAME "test_codegen_cli-data_game_only_emits"
		COMMAND bash -c "set -u; d=$(mktemp -d) || exit 1; trap 'rm -rf \"$d\"' EXIT; printf '%s' '${_dg_spec}' > \"$d/spec.tau\"; $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> compile \"$d/spec.tau\" -o \"$d/exe\"; echo EXIT=$?; printf '1\\n0\\n0\\n1\\n1\\n0\\n0\\n1\\n' | \"$d/exe\"")
	set_tests_properties("test_codegen_cli-data_game_only_emits" PROPERTIES
		PASS_REGULAR_EXPRESSION "EXIT=0.*o2\\[1\\] := 0"
		FAIL_REGULAR_EXPRESSION "only through|UNREALIZABLE|EXIT=1"
		TIMEOUT 900 RUN_SERIAL TRUE)
endif()

# an edge solves a witness atom jointly with the templates of its
# variable: the program needs that atom too, and used to stop at start
# (map::at)
set(_tw_spec "(always o2[0]:bv[1] = 0 && o2[t]:bv[1] = o1[t-1]:bv[1]) && (sometimes !(!(o1[t]:bv[1] = 1)))")
tau_repl_unsupported(_tau_skip "${_tw_spec}")
if(NOT _tau_skip)
	add_test(NAME "test_codegen_cli-template_witness_atom_emitted"
		COMMAND bash -c "set -u; d=$(mktemp -d) || exit 1; trap 'rm -rf \"$d\"' EXIT; printf '%s' '${_tw_spec}' > \"$d/spec.tau\"; $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> compile \"$d/spec.tau\" -o \"$d/exe\"; echo EXIT=$?; printf '\\n\\n\\n' | \"$d/exe\"")
	set_tests_properties("test_codegen_cli-template_witness_atom_emitted" PROPERTIES
		PASS_REGULAR_EXPRESSION "EXIT=0.*o2\\[0\\] := 0"
		FAIL_REGULAR_EXPRESSION "map::at|terminate called|EXIT=1"
		TIMEOUT 900 RUN_SERIAL TRUE)
endif()
