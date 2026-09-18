#
# Executing synthesized strategies: the run follows the spec from step 0,
# with the strategy the realizability check accepted. Needs ltlsynt on PATH
# like the other LTL run tests.
#

function(add_ltl_run_test name input pass fail)
	add_test(NAME "test_repl-ltl_execution-${name}"
		COMMAND bash -c "printf '${input}' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
	set_tests_properties("test_repl-ltl_execution-${name}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${pass}"
		FAIL_REGULAR_EXPRESSION "${fail}")
endfunction()

# a multi-state strategy takes its initial transition once: o2 alternates
# from step 0 instead of repeating the first output
add_ltl_run_test(mealy_initial_transition_once
	"run G((o2[t-1] = 0 || o2[t] = 0) && F(o2[t] = 1)).\\na\\na\\na\\nq\\nq\\n"
	"o2\\[0\\] := T\n(.*\n)*o2\\[1\\] := F\n(.*\n)*o2\\[2\\] := T"
	"o2\\[1\\] := T|unsat|Error")

# a lookback guard at step 0 reads no negative time
add_ltl_run_test(no_negative_time_guard
	"run G(F(o1[t]:bv[1] = i1[t-1]:bv[1])).\\n1\\n0\\n1\\nq\\nq\\n"
	"o1\\[1\\] := 1"
	"\\[-1\\]|18446744073709551615|Unsolved|unsat")

# the refined strategy runs, not ltlsynt's first one
add_ltl_run_test(refined_strategy_runs
	"run G(F(o1[t]:bv[4] = { 3 }:bv[4]) && (o1[t]:bv[4] != o1[t-1]:bv[4])).\\na\\na\\na\\nq\\nq\\n"
	"o1\\[1\\] := 3"
	"unsat|not executable")

# a pure-past spec without lookback of its own is enforced at step 0
add_ltl_run_test(pure_past_step_zero
	"run G((o1[t]:bv[1] = 1) <-> ((i1[t]:bv[1] = 1) S (i2[t]:bv[1] = 1))).\\n1\\n0\\n1\\nq\\nq\\n"
	"o1\\[0\\] := 1"
	"o1\\[0\\] := 0|unsat")
