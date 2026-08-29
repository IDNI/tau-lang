#
# Limit options changing algorithm behavior (not just set/get round trips).
# Each give-up branch is driven to fire with a cap of 1 on a workload that
# needs more. Workloads verified by hand 2026-08-17.
#
# NOT covered here, deliberately: to_unbounded_continuation's flag-search
# give-up (satisfiability.tmpl.h SO-1). It only fires on a satisfiable spec
# whose eventual flag stays unraisable past the boundary -- GitHub-#70-class
# mixed tau/bv specs measured at 12-271 s. Every small `sometimes` spec
# raises its flag immediately, and unsat specs are caught by the pre-loop
# is_run_satisfiable check before the capped loop begins.
#

# find_fixpoint_phi: a lookback-2 always-spec needs at least 2 phi steps, so
# a cap of 1 must give up (loudly) and still terminate.
add_test(NAME "test_repl-limit_effect-fixpointsteps_giveup"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --max-fixpoint-steps 1 -e \"sat always o1[t] = o1[t-2]\"")
set_tests_properties("test_repl-limit_effect-fixpointsteps_giveup" PROPERTIES
	PASS_REGULAR_EXPRESSION "find_fixpoint_phi: exceeded 1 steps")

# fixpointsteps ships unlimited, so this workload completes without
# ever reaching a cap.
add_test(NAME "test_repl-limit_effect-fixpointsteps_default_completes"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"sat always o1[t] = o1[t-2]\"")
set_tests_properties("test_repl-limit_effect-fixpointsteps_default_completes" PROPERTIES
	FAIL_REGULAR_EXPRESSION "exceeded"
	PASS_REGULAR_EXPRESSION ": T")
add_test(NAME "test_repl-limit_effect-fixpointsteps_cli_zero_unlimited"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --max-fixpoint-steps 0 -e \"get fixpointsteps\"")
set_tests_properties("test_repl-limit_effect-fixpointsteps_cli_zero_unlimited" PROPERTIES
	PASS_REGULAR_EXPRESSION "fixpointsteps: *unlimited")

# The same cap reached through the REPL `set` instead of the CLI flag.
add_test(NAME "test_repl-limit_effect-fixpointsteps_via_set"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"set fixpointsteps 1. sat always o1[t] = o1[t-2]\"")
set_tests_properties("test_repl-limit_effect-fixpointsteps_via_set" PROPERTIES
	PASS_REGULAR_EXPRESSION "find_fixpoint_phi: exceeded 1 steps")

# Definition expansion: g needs one pass per nesting level forever.
add_test(NAME "test_repl-limit_effect-defpasses_giveup"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --max-def-passes 1 -e \"g(x) := h(g(x)). h(x) := x'. normalize g(0)\"")
set_tests_properties("test_repl-limit_effect-defpasses_giveup" PROPERTIES
	PASS_REGULAR_EXPRESSION "Definition expansion did not settle after 1 passes")

# Fixed-point enumeration: the recurrence converges, but not within 1 step.
add_test(NAME "test_repl-limit_effect-enumsteps_giveup"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --max-enum-steps 1 -e \"g[n](x) := g[n-1](x) || x = 0. g[0](x) := F. normalize g(y)\"")
set_tests_properties("test_repl-limit_effect-enumsteps_giveup" PROPERTIES
	PASS_REGULAR_EXPRESSION "no fixed point and no loop after 1 enumeration steps")

# spec-size-warn: any accepted update trips a 1-char threshold. The update
# stream u takes its value from i1; the interactive run is driven on stdin
# (-X legacy REPL, the tty-free branch), same pattern as test_repl-run_cmd.
add_test(NAME "test_repl-limit_effect-specsizewarn_fires"
	COMMAND bash -c "printf 'run u[t] = i1[t] && o1[t] = 0.\\no1[t] = 0.\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X --spec-size-warn 1")
set_tests_properties("test_repl-limit_effect-specsizewarn_fires" PROPERTIES
	PASS_REGULAR_EXPRESSION "exceeds the spec-size-warn threshold 1")

# And the negative: without the flag (threshold 0 = off) no warning appears.
add_test(NAME "test_repl-limit_effect-specsizewarn_off_by_default"
	COMMAND bash -c "printf 'run u[t] = i1[t] && o1[t] = 0.\\no1[t] = 0.\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-limit_effect-specsizewarn_off_by_default" PROPERTIES
	FAIL_REGULAR_EXPRESSION "spec-size-warn threshold"
	PASS_REGULAR_EXPRESSION "o1\\[0\\] := ")

# LT-17: the k-ary consistency-subset walk. Four same-type output atoms with
# a pairwise-feasible but jointly-infeasible triple {o1|o2=1, o1&o2=0, o1=o2}
# put ">= 2" subset checks on the walk's plate; a cap of 1 must give up
# loudly and still return the correct verdict (T -- the common commitment
# o3=1 is dischargeable at t=0, and D3 = skip+log means a fired cap is at
# worst a false UNREALIZABLE, never an error). Needs a live ltlsynt on PATH
# (same as the other `sat`-on-full-LTL tests).
add_test(NAME "test_repl-limit_effect-maxsubsets_giveup"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --max-consistency-subsets 1 -e \"sat ((o1[t] | o2[t] = 1) U (o3[t] = 1)) && ((o1[t] & o2[t] = 0) U (o3[t] = 1)) && ((o1[t] = o2[t]) U (o3[t] = 1))\" 2>&1")
set_tests_properties("test_repl-limit_effect-maxsubsets_giveup" PROPERTIES
	PASS_REGULAR_EXPRESSION "k-ary consistency walk capped after 1 subset checks")

# D3 pin, degradation direction: on THAT spec the capped run really does
# return the documented worst case -- a false UNREALIZABLE (ltlsynt's
# strategy uses the unforbidden triple on an edge, the per-edge oracle
# refuses it) -- never a false REALIZABLE, and never an error. The uncapped
# twin (maxsubsets_default_completes below) proves the true verdict is T.
add_test(NAME "test_repl-limit_effect-maxsubsets_capped_false_unreal_at_worst"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --max-consistency-subsets 1 -e \"sat ((o1[t] | o2[t] = 1) U (o3[t] = 1)) && ((o1[t] & o2[t] = 0) U (o3[t] = 1)) && ((o1[t] = o2[t]) U (o3[t] = 1))\" 2>&1")
set_tests_properties("test_repl-limit_effect-maxsubsets_capped_false_unreal_at_worst" PROPERTIES
	PASS_REGULAR_EXPRESSION ": F")

# Correctness pin: when the skipped subsets are all feasible there was no
# forbid to miss, so the capped verdict is provably unchanged -- the warning
# fires and the answer is still T. Two tests on the same command line: a
# single regex bridging both markers is the pinned ctest-backtracking trap.
add_test(NAME "test_repl-limit_effect-maxsubsets_capped_verdict_correct"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --max-consistency-subsets 1 -e \"sat ((o1[t] = o2[t]) U (o4[t] = 1)) && ((o2[t] = o3[t]) U (o4[t] = 1)) && ((o3[t] = o1[t]) U (o4[t] = 1))\" 2>&1")
set_tests_properties("test_repl-limit_effect-maxsubsets_capped_verdict_correct" PROPERTIES
	PASS_REGULAR_EXPRESSION ": T")
add_test(NAME "test_repl-limit_effect-maxsubsets_capped_verdict_correct_warns"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --max-consistency-subsets 1 -e \"sat ((o1[t] = o2[t]) U (o4[t] = 1)) && ((o2[t] = o3[t]) U (o4[t] = 1)) && ((o3[t] = o1[t]) U (o4[t] = 1))\" 2>&1")
set_tests_properties("test_repl-limit_effect-maxsubsets_capped_verdict_correct_warns" PROPERTIES
	PASS_REGULAR_EXPRESSION "k-ary consistency walk capped after 1 subset checks")

# Under the shipped default (4096) the same workload completes silently
# with the same verdict.
add_test(NAME "test_repl-limit_effect-maxsubsets_default_completes"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"sat ((o1[t] | o2[t] = 1) U (o3[t] = 1)) && ((o1[t] & o2[t] = 0) U (o3[t] = 1)) && ((o1[t] = o2[t]) U (o3[t] = 1))\" 2>&1")
set_tests_properties("test_repl-limit_effect-maxsubsets_default_completes" PROPERTIES
	FAIL_REGULAR_EXPRESSION "consistency walk capped"
	PASS_REGULAR_EXPRESSION ": T")

# The same cap reached through the REPL `set` instead of the CLI flag.
add_test(NAME "test_repl-limit_effect-maxsubsets_via_set"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"set maxsubsets 1. sat ((o1[t] | o2[t] = 1) U (o3[t] = 1)) && ((o1[t] & o2[t] = 0) U (o3[t] = 1)) && ((o1[t] = o2[t]) U (o3[t] = 1))\" 2>&1")
set_tests_properties("test_repl-limit_effect-maxsubsets_via_set" PROPERTIES
	PASS_REGULAR_EXPRESSION "k-ary consistency walk capped after 1 subset checks")
