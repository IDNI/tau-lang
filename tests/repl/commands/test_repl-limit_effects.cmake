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
