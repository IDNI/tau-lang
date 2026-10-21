#
# Paths of the solver and of the satisfiability and type checks that the
# other command suites do not take: the value a minimum or maximum solve gives
# a variable that only an assignment mentions, a qlt assignment, the bitvector
# route that reads `var = constant` off before handing the rest to the theory,
# a reverse implication read by the warm-up pinning of `sat` and `valid`, a
# CTL* `sat` whose reduction stays undecided, and a cast that lends its width
# to a constant which does not fit it.
#

include(add_repl_test)
include(tau_repl_pack)

# `x = y` becomes the assignment x := y; y occurs nowhere else, so a minimum
# solve sets it to 0 and a maximum solve to 1, and x follows.
add_repl_test(solver_paths-min_assignment_sets_free_var_to_zero
	"solve --min (x = y)" "x := \\{ F \\}:tau"
	NO_TRACE FAIL_REGEX "\\{ T \\}|Error|no solution")
add_repl_test(solver_paths-max_assignment_sets_free_var_to_one
	"solve --max (x = y)" "x := \\{ T \\}:tau"
	NO_TRACE FAIL_REGEX "\\{ F \\}|Error|no solution")

# A qlt variable is a point: the free side of a qlt assignment takes the
# order's zero.
add_repl_test(solver_paths-qlt_assignment_takes_the_zero_point
	"solve (x:qlt = y:qlt)" "y := \\{ 0 \\}:qlt"
	NO_TRACE FAIL_REGEX "\\{ 1 \\}:qlt|Error|no solution" REQUIRES qlt)

# x = 3 is read off first; y + 3 = 5 goes to the theory solver, which keeps
# the read-off value beside its own. 2y = 1 has no solution modulo 16.
add_repl_test(solver_paths-bv_read_off_kept_beside_theory_solution
	"solve (x:bv[4] = {3}:bv[4] && y:bv[4] + x:bv[4] = {5}:bv[4])"
	"y := \\{ 2 \\}:bv\\[4\\]" NO_TRACE REQUIRES bv)
add_repl_test(solver_paths-bv_read_off_kept_beside_theory_solution_x
	"solve (x:bv[4] = {3}:bv[4] && y:bv[4] + x:bv[4] = {5}:bv[4])"
	"x := \\{ 3 \\}:bv\\[4\\]" NO_TRACE REQUIRES bv)
add_repl_test(solver_paths-bv_read_off_with_unsolvable_theory_part
	"solve (x:bv[4] = {3}:bv[4] && y:bv[4] + y:bv[4] = {1}:bv[4])"
	"no solution" NO_TRACE FAIL_REGEX "Error|:= " REQUIRES bv)

# `A <- B` reads A positively and B negatively, as `B -> A` does.
add_repl_test(solver_paths-sat_reverse_implication
	"sat (always o1[t] = o1[t-1]) <- (always o1[t] = 0)" ": T" NO_TRACE)
add_repl_test(solver_paths-valid_reverse_implication
	"valid (always o1[t] = 0) <- (always o1[t] = 0 && o1[t-1] = 0)" ": T" NO_TRACE)
add_repl_test(solver_paths-valid_reverse_implication_fails
	"valid (always o1[t] = 1) <- (always o1[t] = 0)" ": F" NO_TRACE)
add_repl_test(solver_paths-sat_negated_reverse_implication
	"sat !((always o1[t] = 0) <- (always o1[t-1] = 1))" ": T" NO_TRACE)

# Past the tableau cap the E witnesses of `sat` share one set of directions;
# the reduction that loses then leaves satisfiability unknown, no verdict.
add_repl_test(solver_paths-ctl_star_sat_tableau_cap_unknown
	"fragment ctl_star. set ltl-ctl-star-tableau-max-states 1. sat (always oc[t]:bv[2] = oc[t-1]:bv[2] + {1}:bv[2]) && (always E ((oc[t]:bv[2] = {0}:bv[2] -> always (oc[t]:bv[2] = {2}:bv[2] -> i1[t]:bv[1] = 1)) && (oc[t]:bv[2] = {1}:bv[2] -> always (oc[t]:bv[2] = {2}:bv[2] -> i1[t]:bv[1] = 0))))"
	"UNKNOWN: the CTL\\* encoding is unrealizable" NO_TRACE
	FAIL_REGEX "%[0-9]+: [TF]" REQUIRES ltlsynt bv)

# A cast to bv[8] lends its width to the `:bv` constant under it; 999 does
# not fit in 8 bits.
add_repl_test(solver_paths-cast_lent_width_rejects_constant
	"n ((bv[8]) { 999 }:bv) = y:bv[8]"
	"the value does not fit in the width \\(width=8\\)" NO_TRACE
	FAIL_REGEX "%[0-9]+: " REQUIRES bv)

# `--min` before an unparenthesized equation is read as two semantic
# negations of `m & i & n & x`, not as the option
add_repl_test(solver_paths-min_option_without_parentheses
	"solve --min x = y" "solution: \\{" NO_TRACE
	FAIL_REGEX "temporal quantifier")
set_tests_properties(test_repl-solver_paths-min_option_without_parentheses
	PROPERTIES WILL_FAIL TRUE)
