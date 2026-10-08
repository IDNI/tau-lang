# Tests `tau compile <spec.tau>`. A successful compile exits 0. Every compile
# failure exits 1. The report names the reason (UNREALIZABLE, a missing
# backend, and so on).

include(add_repl_test)

# The case logic is a CMake script, so the case needs no bash, no mktemp and
# no trap.
set(TAU_CODEGEN_CLI_CHECKER "${CMAKE_CURRENT_LIST_DIR}/../check_codegen_cli.cmake")

# `tau compile` takes a spec from a file or from stdin. Each case hands its
# formula to the script, which writes a scratch file and passes that path. The
# backend-failure case hides ltlsynt with an empty PATH and TAU_SPOT_BIN.

# add_codegen_cli_test(<name> <spec_text> <scratch_stem> <pass_regex>
#     [FAIL_REGEX <re>] [NO_FAIL_REGEX] [NO_LTLSYNT <dir>] [STUB_PATH <dir>]
#     [ENV <name=value>...] [TAU_FLAGS <flags>] [PROGRAM_STDIN <text>]
#     [TIMEOUT <sec>] [RUN_SERIAL])
#
# `tau compile` writes its spec to a host path and spawns a host compiler
# (src/tau_compile.tmpl.h), so a wasm node host cannot run these even with the
# NODEFS filesystem. NO_LTLSYNT points PATH and TAU_SPOT_BIN at an empty folder.
# STUB_PATH puts a stub in front of PATH. TAU_FLAGS goes before the verb, so a
# budget option reaches the synthesis. PROGRAM_STDIN feeds the built program.
function(add_codegen_cli_test test_name spec_text scratch_stem pass_regex)
	cmake_parse_arguments(PARSE_ARGV 4 _tau "NO_FAIL_REGEX;RUN_SERIAL"
		"NO_LTLSYNT;STUB_PATH;TAU_FLAGS;PROGRAM_STDIN;TIMEOUT"
		"FAIL_REGEX;REQUIRES;ENV")
	list(APPEND _tau_REQUIRES subprocess)
	tau_repl_gate_case("${test_name}" spec_text)
	set(_args "-DTAU=${TAU_LAUNCHER}" "-DSPEC_TEXT=${spec_text}"
		"-DEXE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}")
	if(_tau_NO_LTLSYNT)
		list(APPEND _args "-DNO_LTLSYNT=${_tau_NO_LTLSYNT}")
	endif()
	if(_tau_STUB_PATH)
		list(APPEND _args "-DSTUB_PATH=${_tau_STUB_PATH}")
	endif()
	if(_tau_ENV)
		list(APPEND _args "-DENV_EXTRA=${_tau_ENV}")
	endif()
	if(_tau_TAU_FLAGS)
		list(APPEND _args "-DTAU_FLAGS=${_tau_TAU_FLAGS}")
	endif()
	if(_tau_PROGRAM_STDIN)
		list(APPEND _args "-DPROGRAM_STDIN=${_tau_PROGRAM_STDIN}")
	endif()
	list(APPEND _args "-DTMP=${CMAKE_BINARY_DIR}/${scratch_stem}")
	add_test(NAME "${test_name}"
		COMMAND ${CMAKE_COMMAND} ${_args} -P "${TAU_CODEGEN_CLI_CHECKER}")
	tau_repl_check_case("${test_name}" "${pass_regex}")
	if(_tau_RUN_SERIAL)
		set_tests_properties("${test_name}" PROPERTIES RUN_SERIAL TRUE)
	endif()
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
add_codegen_cli_test(test_codegen_cli-unknown_is_not_unrealizable
	"F (o1[t] = 1 && i1[t] != i1[t-1])"
	"test_codegen_cli-unknown_is_not_unrealizable.scratch"
	"compile: the realizability of the spec is UNKNOWN.*EXIT=1"
	FAIL_REGEX "UNREALIZABLE"
	ENV "TAU_LTL_TIMEOUT=1"
	STUB_PATH "${CMAKE_CURRENT_LIST_DIR}/../stubs/slow_game")

add_codegen_cli_test(test_codegen_cli-unknown_names_the_budget
	"F (o1[t] = 1 && i1[t] != i1[t-1])"
	"test_codegen_cli-unknown_names_the_budget.scratch"
	"killed by the ltl-timeout watchdog"
	ENV "TAU_LTL_TIMEOUT=1"
	STUB_PATH "${CMAKE_CURRENT_LIST_DIR}/../stubs/slow_game")

# The same budget given as the option: the verb reads the global options
# (without them each stubbed game call waits out the 60 s default).
add_codegen_cli_test(test_codegen_cli-compile_reads_the_ltl_timeout_option
	"F (o1[t] = 1 && i1[t] != i1[t-1])"
	"test_codegen_cli-compile_reads_the_ltl_timeout_option.scratch"
	"compile: the realizability of the spec is UNKNOWN.*EXIT=1"
	TAU_FLAGS "--ltl-timeout 1"
	STUB_PATH "${CMAKE_CURRENT_LIST_DIR}/../stubs/slow_game"
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
add_codegen_cli_test(test_codegen_cli-data_game_only_emits
	"(sometimes (o2[t]:bv[1] = i2[t-1]:bv[1])) && (sometimes ((i1[t-1]:bv[1] = i1[t]:bv[1] || i1[t-1]:bv[1] = 1)))"
	"test_codegen_cli-data_game_only_emits.scratch"
	"EXIT=0.*o2\\[1\\] := 0"
	FAIL_REGEX "only through|UNREALIZABLE|EXIT=1"
	PROGRAM_STDIN "1\\n0\\n0\\n1\\n1\\n0\\n0\\n1\\n"
	TIMEOUT 900 RUN_SERIAL)

# an edge solves a witness atom jointly with the templates of its
# variable: the program needs that atom too, and used to stop at start
# (map::at)
add_codegen_cli_test(test_codegen_cli-template_witness_atom_emitted
	"(always o2[0]:bv[1] = 0 && o2[t]:bv[1] = o1[t-1]:bv[1]) && (sometimes !(!(o1[t]:bv[1] = 1)))"
	"test_codegen_cli-template_witness_atom_emitted.scratch"
	"EXIT=0.*o2\\[0\\] := 0"
	FAIL_REGEX "map::at|terminate called|EXIT=1"
	PROGRAM_STDIN "\\n\\n\\n"
	TIMEOUT 900 RUN_SERIAL)
