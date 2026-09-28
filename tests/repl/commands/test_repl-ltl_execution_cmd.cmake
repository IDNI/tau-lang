#
# Executing synthesized strategies: the run follows the spec from step 0,
# with the strategy the realizability check accepted. Needs ltlsynt on PATH
# like the other LTL run tests.
#

include(add_repl_test)

function(add_ltl_run_test name input pass fail)
	# The input carries printf escapes, so the helper pipes the exact bytes.
	# The helper gates on PACK for a bv[N] annotation in a pack without that
	# algebra, and on LTLSYNT because `run` needs the synthesized strategy.
	add_multiline_repl_test(ltl_execution-${name} "${pass}"
		STDIN "${input}" FAIL_REGEX "${fail}" REQUIRES ltlsynt)
endfunction()

# the strategy takes its initial transition once: o2 alternates from step 0
# instead of repeating the first output
add_ltl_run_test(mealy_initial_transition_once
	"run G((o2[t-1] = 0 || o2[t] = 0) && F(o2[t] = 1)).\\na\\na\\na\\nq\\nq\\n"
	"o2\\[0\\] := T\n.*o2\\[1\\] := F\n.*o2\\[2\\] := T|o2\\[0\\] := F\n.*o2\\[1\\] := T\n.*o2\\[2\\] := F"
	"o2\\[0\\] := T\n.*o2\\[1\\] := T|o2\\[1\\] := T\n.*o2\\[2\\] := T|unsat|Error")

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
	"o1\\[4\\] := T\n.*o1\\[5\\] := T"
	"o1\\[4\\] := F|o1\\[5\\] := F|unsat")

# The abstraction's strategy loses against the data here: the run plays the
# strategy of the data game, which decides the spec.
add_ltl_run_test(data_game_strategy_input_goals
	"run (sometimes (o2[t]:bv[1] = i2[t-1]:bv[1])) && (sometimes ((i1[t-1]:bv[1] = i1[t]:bv[1] || i1[t-1]:bv[1] = 1))).\\n1\\n0\\n0\\n1\\n1\\n0\\n0\\n1\\n1\\n1\\nq\\nq\\n"
	"o2\\[1\\] := 0\n.*o2\\[3\\] := "
	"no strategy|not executable|unsat")

# over the default type the goal needs a value no stream holds: o2 takes it
# at step 0, as i1 is F there
add_ltl_run_test(data_game_strategy_new_value
	"run (always o2[t] = o1[t-1]) && (sometimes o2[t] != 0 && o2[t] != 1 && o2[t] != i1[t]).\\nF.\\nT.\\nF.\\nq\\nq\\n"
	"o2\\[0\\] := <[^\n]*\n.*o1\\[2\\] := "
	"no strategy|not executable|unsat")

# the strategy of a game on codes of complement pairs: o1 is the complement
# of i1 two steps before (\x27 is the complement mark for printf)
add_ltl_run_test(data_game_strategy_complement_codes
	"run (always (o2[0] = 0 && (o1[t-1] = 1 -> o2[t] = 1) && i1[t-2] = o1[t]\\x27)) && (sometimes (o1[t] = 1 && o2[t] = o2[t-1]\\x27)) && (sometimes (o1[t-1] = i2[t-1])).\\nF.\\nT.\\nT.\\nF.\\nF.\\nF.\\nT.\\nT.\\nq\\nq\\n"
	"o1\\[2\\] := T\n.*o1\\[3\\] := F"
	"no strategy|not executable|unsat")

# the strategy of a game on the values of bv[2]: against odd inputs o2 takes
# the value 2
add_ltl_run_test(data_game_strategy_bv_values
	"run (always o2[t]:bv[2] = o1[t-1]:bv[2] + {1}:bv[2]) && (sometimes o2[t]:bv[2] * i1[t]:bv[2] = {2}:bv[2] || i1[t]:bv[2] = {0}:bv[2] || i1[t]:bv[2] = {2}:bv[2]).\\n3\\n1\\n3\\n3\\n1\\n1\\nq\\nq\\n"
	"o2\\[[0-3]\\] := 2"
	"no strategy|not executable|unsat")

# an output-only spec whose always part merges two warm-ups: the run goes
# on past step 0, o2 alternating (\x27 is the complement mark for printf)
add_ltl_run_test(data_game_strategy_merged_warm_up
	"run 4 steps (always ((o2[t]:bv[1] = 1 -> o2[t]:bv[1] = 1))) && (always (o2[t]:bv[1] = o2[t-1]:bv[1]\\x27)) && (sometimes (o2[t]:bv[1] = o2[t-1]:bv[1]\\x27)) && (sometimes (o2[t-2]:bv[1] = 1)).\\nq\\n"
	"o2\\[0\\] := 1\n.*o2\\[1\\] := 0\n.*o2\\[2\\] := 1\n.*o2\\[3\\] := 0|o2\\[0\\] := 0\n.*o2\\[1\\] := 1\n.*o2\\[2\\] := 0\n.*o2\\[3\\] := 1"
	"no solution|no strategy|unsat")

# the strategy of a game on the bits of bv[16]: 3 is invertible, so o2
# meets its goal at step 0, 3 * 335 = 1000 + 5
add_ltl_run_test(data_game_strategy_bv_bits
	"run (always o2[t]:bv[16] = o1[t-1]:bv[16] + i1[t]:bv[16]) && (sometimes o2[t]:bv[16] * {3}:bv[16] = {1000}:bv[16] + i1[t]:bv[16]).\\n5\\n7\\n9\\nq\\nq\\n"
	"o2\\[0\\] := 335"
	"no strategy|not executable|unsat|cannot")

# the strategy of a game on the order types of qlt values: every output is
# a rational, never the 0 or 1 of the type, and o1 meets 1/2 two steps
# after a value at most 0
add_ltl_run_test(data_game_strategy_order_types
	"run (((o1[t]:qlt != i1[t-1]:qlt || {1}:qlt <= o2[t-1]:qlt)) U ((o1[t-2]:qlt <= {0}:qlt && o1[t]:qlt = {1/2}:qlt))).\\n{1}\\n{2}\\n{3}\\n{4}\\nq\\nq\\n"
	"o1\\[[2-4]\\] := 1/2"
	":= bot|:= top|no strategy|not executable|unsat|no values|no outputs")

# once o1 has met its goal the strategy no longer depends on i1, and the
# run stops asking for it
add_ltl_run_test(data_game_strategy_reads_needed_inputs
	"run (sometimes o1[t]:bv[1] = i1[t]:bv[1]) && (always o2[t]:bv[1] = i2[t]:bv[1]).\\n1\\n0\\n1\\n0\\n1\\n0\\nq\\nq\\n"
	"i2\\[1\\][^\n]*\n.*o2\\[1\\] := 1\n.*i2\\[2\\]"
	"i1\\[1\\]|unsat|no strategy")

# fixed steps inside the always part that the safety pipeline cannot
# execute: the run plays a strategy that reads the step counter
add_ltl_run_test(fixed_steps_through_the_counter
	"run 4 steps (always o2[1] = 1 && o2[0] = 0 && o2[t] = o1[t-1] && o1[t-1] = 1).\\nq\\n"
	"o2\\[0\\] := F\n.*o2\\[1\\] := T\n.*o2\\[2\\] := T\n.*o2\\[3\\] := T"
	"unsat|no strategy")

# the same with a sometimes part the normalizer finds implied
add_ltl_run_test(fixed_steps_through_the_counter_sometimes
	"run 4 steps (always o2[0] = 0 && o2[t] = o1[t-1] && !(o1[t-1] = 0) && o1[t-1] = 1) && (sometimes o1[t-1] = 1).\\nq\\n"
	"o2\\[0\\] := F\n.*o2\\[1\\] := T\n.*o2\\[2\\] := T\n.*o2\\[3\\] := T"
	"unsat|no strategy")

# a fixed step no strategy can meet still leaves the spec unsat
add_ltl_run_test(fixed_steps_unrealizable_stays_unsat
	"run 4 steps (always o2[0] = 0 && o2[0] = 1 && o2[t] = o1[t-1]).\\nq\\n"
	"unsat"
	":= ")

# a revision of a run of the data game's strategy: the game is solved again
# for the revised spec from the values already played, and o1 stops
# alternating
add_ltl_run_test(data_game_strategy_revision
	"run (always u[t] = i1[t] && o1[t]:bv[1] != o1[t-1]:bv[1]) && (sometimes o2[t]:bv[1] = 1).\\nF.\\nF.\\nalways o1[t]:bv[1] = 1.\\nF.\\nF.\\nF.\\nq\\nq\\n"
	"Updated specification[^\n]*\n.*o1\\[4\\] := 1\n.*o1\\[5\\] := 1\n.*o1\\[6\\] := 1"
	"cannot follow|unsat|no strategy")

# A qlt stream holds one point of the order at each step: every run below
# prints rationals, never the type's 0 or 1 (bot, top) or an interval.
set(QLT_NO_POINT ":= bot|:= top|:= [[(]|not executable|unsat")

# the safety pipeline: a disequality with the previous step
add_ltl_run_test(qlt_points_safety_disequality
	"run 3 steps always o1[t]:qlt != o1[t-1]:qlt.\\nq\\n"
	"o1\\[1\\] := -?[0-9/]+\n.*o1\\[2\\] := -?[0-9/]+"
	"${QLT_NO_POINT}")

# the safety pipeline: two outputs equal to each other
add_ltl_run_test(qlt_points_safety_equal_outputs
	"run 3 steps always o1[t]:qlt = o2[t]:qlt && o2[t]:qlt != o2[t-1]:qlt.\\nq\\n"
	"o1\\[1\\] := -?[0-9/]+\n.*o2\\[1\\] := -?[0-9/]+"
	"${QLT_NO_POINT}")

# the safety pipeline: two outputs distinct from each other
add_ltl_run_test(qlt_points_safety_distinct_outputs
	"run 2 steps always o1[t]:qlt != o2[t]:qlt.\\nq\\n"
	"o1\\[0\\] := -?[0-9/]+\n.*o2\\[0\\] := -?[0-9/]+"
	"${QLT_NO_POINT}")

# the data game on streams only equalities read: order-type codes, not the
# codes of 0 and 1
add_ltl_run_test(qlt_points_data_game_equalities
	"run 3 steps ((o2[t]:qlt = o2[t]:qlt) U (o1[t]:qlt = o2[t]:qlt)).\\nq\\n"
	"o1\\[0\\] := -?[0-9/]+\n.*o2\\[0\\] := -?[0-9/]+"
	"${QLT_NO_POINT}")

# a goal between two outputs beside a disequality over time
add_ltl_run_test(qlt_points_goal_between_outputs
	"run 3 steps G(F(o1[t]:qlt = o2[t]:qlt) && (o1[t]:qlt != o1[t-1]:qlt)).\\nq\\n"
	"o1\\[2\\] := -?[0-9/]+\n.*o2\\[2\\] := -?[0-9/]+"
	"${QLT_NO_POINT}")

# realizability decided by a strategy over bookkeeping bits, which cannot be
# played: the run plays one over the data instead
add_ltl_run_test(qlt_points_bookkeeping_strategy_runs
	"run (G (i1[t]:qlt = i1[t]:qlt)) && (F (i1[t]:qlt > o1[t]:qlt)).\\n{1}\\n{2}\\n{0}\\nq\\nq\\n"
	"o1\\[0\\] := -?[0-9/]+\n.*o1\\[1\\] := -?[0-9/]+\n.*o1\\[2\\] := -?[0-9/]+"
	"${QLT_NO_POINT}")

# the data game proves that no strategy exists: the run says so
add_ltl_run_test(qlt_unrealizable_data_game
	"run (G (o2[t-1]:qlt = i1[t-1]:qlt)) && (G (F (i1[t]:qlt = o2[t-1]:qlt))).\\nq\\n"
	"not executable: it is unrealizable"
	":= |no strategy was synthesised")

# the algebra's own synthesis proves that no strategy exists
add_ltl_run_test(qlt_unrealizable_propositional
	"run (G ({0}:qlt < i1[t]:qlt)) && (G (F (({0}:qlt != i1[t]:qlt || {1}:qlt != o1[t]:qlt)))).\\nq\\n"
	"not executable: it is unrealizable"
	":= |no strategy was synthesised")
