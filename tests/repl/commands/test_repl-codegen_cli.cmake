#
# `tau compile <spec.tau>` exit-code contract (CG-N6 / CG-R1 successor).
#
# The standalone tau_codegen CLI this file used to drive is gone -- codegen
# is now the `compile` verb on the `tau` binary itself (src/main.cpp), which
# takes a spec FILE argument (not stdin) and drives compile_spec (see
# src/tau_compile.tmpl.h). Its exit-code contract collapsed from the old
# CLI's 0/3/4/5 spread to just two codes: main.cpp's compile branch returns
# `error(...)` (== 1) on every failure --
#   files.empty(), !ifs, src.empty(), or !res.has_value() (compile_spec
#   returns a result<codegen_result>, whose codegen_result holds only
#   exe_path; codegen_result::ok() means `!exe_path.empty()`) --
# and 0 after `TAU_LOG_INFO << "compiled: " << res.value().exe_path;`. The
# *reason* for a failure (parse error, UNREALIZABLE, backend failure, cmake
# configure/build failure, ...) is now distinguished only by the report
# `res.print();` writes, not by a dedicated exit code -- there is no more
# UNKNOWN(4)/not-executable(5).
#

include(tau_repl_pack)

# CG-R7 successor: `tau compile` reads its spec from a FILE, not stdin, so
# each case below first writes the formula to a scratch file.

add_test(NAME "test_codegen_cli-always_one_emits"
	COMMAND bash -c "set -u; d=$(mktemp -d) || exit 1; trap 'rm -rf \"$d\"' EXIT; printf '%s' 'always o1[t] = 1' > \"$d/spec.tau\"; $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> compile \"$d/spec.tau\" -o \"$d/exe\"; echo EXIT=$?")
set_tests_properties("test_codegen_cli-always_one_emits" PROPERTIES
	PASS_REGULAR_EXPRESSION "compiled:.*EXIT=0"
	FAIL_REGULAR_EXPRESSION "EXIT=1")

add_test(NAME "test_codegen_cli-unrealizable_exit_3"
	COMMAND bash -c "set -u; d=$(mktemp -d) || exit 1; trap 'rm -rf \"$d\"' EXIT; printf '%s' 'always (o1[t] = 1 && o1[t] = 0)' > \"$d/spec.tau\"; $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> compile \"$d/spec.tau\" -o \"$d/exe\"; echo EXIT=$?")
set_tests_properties("test_codegen_cli-unrealizable_exit_3" PROPERTIES
	PASS_REGULAR_EXPRESSION "compile: spec is UNREALIZABLE.*EXIT=1")

# The message follows the three-valued verdict. A spec with no strategy that
# `realizable` decides F keeps UNREALIZABLE...
add_test(NAME "test_codegen_cli-unrealizable_full_ltl"
	COMMAND bash -c "set -u; d=$(mktemp -d) || exit 1; trap 'rm -rf \"$d\"' EXIT; printf '%s' 'G (o1[t] = 0) && F (o1[t] = 1)' > \"$d/spec.tau\"; $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> compile \"$d/spec.tau\" -o \"$d/exe\"; echo EXIT=$?")
set_tests_properties("test_codegen_cli-unrealizable_full_ltl" PROPERTIES
	PASS_REGULAR_EXPRESSION "compile: spec is UNREALIZABLE.*EXIT=1"
	FAIL_REGULAR_EXPRESSION "UNKNOWN")

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

# CG-N6: ltlsynt stubbed to fail like an internal/usage error (exit 2, no
# verdict line -- see tests/repl/stubs/ltlsynt). The synthesis layer must
# surface this as a failure without ever claiming UNREALIZABLE (that verdict
# means something specific and different: the spec was actually decided).
add_test(NAME "test_codegen_cli-backend_failure_exit_4"
	COMMAND bash -c "set -u; d=$(mktemp -d) || exit 1; trap 'rm -rf \"$d\"' EXIT; printf '%s' 'F (o1[t] = 1)' > \"$d/spec.tau\"; PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs:$PATH $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> compile \"$d/spec.tau\" -o \"$d/exe\"; echo EXIT=$?")
set_tests_properties("test_codegen_cli-backend_failure_exit_4" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlsynt produced no verdict.*EXIT=1"
	FAIL_REGULAR_EXPRESSION "UNREALIZABLE|terminate called")

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
