#
# Executing synthesized strategies: the run follows the spec from step 0,
# with the strategy the realizability check accepted. Needs ltlsynt on PATH
# like the other LTL run tests.
#

include(tau_repl_pack)

function(add_ltl_run_test name input pass fail)
	# Several inputs below annotate with bv[N]; gate them like add_repl_test
	# does, or a pack without that algebra runs a spec it cannot type.
	tau_repl_unsupported(_tau_skip "${input}")
	if(_tau_skip)
		tau_repl_record_skip("test_repl-ltl_execution-${name}")
		return()
	endif()
	add_test(NAME "test_repl-ltl_execution-${name}"
		COMMAND bash -c "printf '${input}' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
	set_tests_properties("test_repl-ltl_execution-${name}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${pass}"
		FAIL_REGULAR_EXPRESSION "${fail}")
endfunction()

# the strategy takes its initial transition once: o2 alternates from step 0
# instead of repeating the first output
add_ltl_run_test(mealy_initial_transition_once
	"run G((o2[t-1] = 0 || o2[t] = 0) && F(o2[t] = 1)).\\na\\na\\na\\nq\\nq\\n"
	"o2\\[0\\] := T\n(.*\n)*o2\\[1\\] := F\n(.*\n)*o2\\[2\\] := T|o2\\[0\\] := F\n(.*\n)*o2\\[1\\] := T\n(.*\n)*o2\\[2\\] := F"
	"o2\\[0\\] := T\n(.*\n)*o2\\[1\\] := T|o2\\[1\\] := T\n(.*\n)*o2\\[2\\] := T|unsat|Error")

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

# a pure-past spec without lookback of its own is enforced at step 0; the
# run asks i1 and then i2 at each step
add_ltl_run_test(pure_past_step_zero
	"run G((o1[t]:bv[1] = 1) <-> ((i1[t]:bv[1] = 1) since (i2[t]:bv[1] = 1))).\\n1\\n1\\n0\\n1\\nq\\nq\\n"
	"o1\\[0\\] := 1"
	"o1\\[0\\] := 0|unsat")

# a revision alternative that no input sequence lets hold over time is
# dropped: after G(o1 = 1) the run stays on the update instead of
# alternating between it and a dead alternative
add_ltl_run_test(revision_drops_dead_alternative
	"run G(u[t] = i1[t] && (o1[t] != o1[t-1])).\\nF.\\nF.\\nG(o1[t] = 1).\\nF.\\nF.\\nF.\\nq\\nq\\n"
	"o1\\[4\\] := T\n(.*\n)*o1\\[5\\] := T"
	"o1\\[4\\] := F|o1\\[5\\] := F|unsat")

# The abstraction's strategy loses against the data here: the run plays the
# strategy of the data game, which decides the spec.
add_ltl_run_test(data_game_strategy_input_goals
	"run (sometimes (o2[t]:bv[1] = i2[t-1]:bv[1])) && (sometimes ((i1[t-1]:bv[1] = i1[t]:bv[1] || i1[t-1]:bv[1] = 1))).\\n1\\n0\\n0\\n1\\n1\\n0\\n0\\n1\\n1\\n1\\nq\\nq\\n"
	"o2\\[1\\] := 0\n(.*\n)*o2\\[3\\] := "
	"no strategy|not executable|unsat")

# over the default type the goal needs a value no stream holds: o2 takes it
# at step 0, as i1 is F there
add_ltl_run_test(data_game_strategy_new_value
	"run (always o2[t] = o1[t-1]) && (sometimes o2[t] != 0 && o2[t] != 1 && o2[t] != i1[t]).\\nF.\\nT.\\nF.\\nq\\nq\\n"
	"o2\\[0\\] := <[^\n]*\n(.*\n)*o1\\[2\\] := "
	"no strategy|not executable|unsat")

# the strategy of a game on codes of complement pairs: o1 is the complement
# of i1 two steps before (\x27 is the complement mark for printf)
add_ltl_run_test(data_game_strategy_complement_codes
	"run (always (o2[0] = 0 && (o1[t-1] = 1 -> o2[t] = 1) && i1[t-2] = o1[t]\\x27)) && (sometimes (o1[t] = 1 && o2[t] = o2[t-1]\\x27)) && (sometimes (o1[t-1] = i2[t-1])).\\nF.\\nT.\\nT.\\nF.\\nF.\\nF.\\nT.\\nT.\\nq\\nq\\n"
	"o1\\[2\\] := T\n(.*\n)*o1\\[3\\] := F"
	"no strategy|not executable|unsat")

# the strategy of a game on the values of bv[2]: against odd inputs o2 takes
# the value 2
add_ltl_run_test(data_game_strategy_bv_values
	"run (always o2[t]:bv[2] = o1[t-1]:bv[2] + {1}:bv[2]) && (sometimes o2[t]:bv[2] * i1[t]:bv[2] = {2}:bv[2] || i1[t]:bv[2] = {0}:bv[2] || i1[t]:bv[2] = {2}:bv[2]).\\n3\\n1\\n3\\n3\\n1\\n1\\nq\\nq\\n"
	"o2\\[[0-3]\\] := 2"
	"no strategy|not executable|unsat")
