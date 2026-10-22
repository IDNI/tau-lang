#
# Paths of the solver and of the satisfiability and type checks that the
# other command suites do not take: the value a minimum or maximum solve gives
# a variable that only an assignment mentions, a qlt assignment, the bitvector
# route that reads `var = constant` off before handing the rest to the theory,
# a reverse implication read by the warm-up pinning of `sat` and `valid`, a
# CTL* `sat` whose reduction stays undecided, and a cast that lends its width
# to a constant which does not fit it. Also the flag search giving up past its
# step limit, a DNF whose paths merge twice, construction hooks of the
# negated order operators, the warning on a prefix operator over a
# connective, onf over disjunctions and arithmetic, and fixpoint calls met
# twice or defined with several offset counts.
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

# `--min` before an unparenthesized equation reads as two semantic negations
# of `m & i & n & x`: solve says so and names the parenthesized form
add_repl_test(solver_paths-min_option_without_parentheses
	"solve --min x = y" "put the formula in parentheses to pass an option: solve --min \\(x = y\\)" NO_TRACE
	FAIL_REGEX "temporal quantifier|solution: ")
add_repl_test(solver_paths-min_option_with_parentheses
	"solve --min (x = y)" "solution: \\{" NO_TRACE)

# a constant the cast's type cannot read is named as written, with that type
add_repl_test(solver_paths-cast_constant_parse_error_names_the_type
	"n ((bv[8]) { zz }:bv) = y:bv[8]"
	"Unable to parse \\{ zz \\} as a constant of type bv\\[8\\]" NO_TRACE
	FAIL_REGEX ":untyped|valid: ")

# Past --max-flag-search-steps the flag search gives up without a verdict,
# for a spec and for a temporal Tau constant.
add_repl_test(solver_paths-flag_search_limit_gives_up
	"set max-flag-search-steps 1. sat (always o1[t]:bv[4] = o1[t-1]:bv[4] + {1}:bv[4]) && o1[0]:bv[4] = {0}:bv[4] && (sometimes o1[t]:bv[4] = {10}:bv[4])"
	"could not raise the eventual variable flag" NO_TRACE
	FAIL_REGEX "%[0-9]+[^\r\n]*: [TF]" REQUIRES bv)
add_repl_test(solver_paths-tau_constant_flag_search_gives_up
	"set max-flag-search-steps 1. n {(always o1[t]:bv[4] = o1[t-1]:bv[4] + {1}:bv[4]) && o1[0]:bv[4] = {0}:bv[4] && (sometimes o1[t]:bv[4] = {10}:bv[4])}:tau = 0"
	"could not raise the eventual variable flag" NO_TRACE
	FAIL_REGEX "%[0-9]+[^\r\n]*: " REQUIRES bv)

# Three clauses whose paths merge twice in a row reduce to z = 0.
add_repl_test(solver_paths-dnf_paths_merge_twice
	"dnf (x = 0 && z = 0) || (x != 0 && y = 0 && z = 0) || (x != 0 && y != 0 && z = 0)"
	"%1[^\r\n]*: z = 0[\r\n]" NO_TRACE)

# The construction hooks: `x !> x` is T, `0 !<= x` is F, and over the tau
# algebra `!<` and `!>` unfold into an inequality or an equality.
add_repl_test(solver_paths-hook_not_greater_than_itself
	"n x !> x" "%1[^\r\n]*: T[\r\n]" NO_TRACE)
add_repl_test(solver_paths-hook_zero_not_below
	"n 0 !<= x" "%1[^\r\n]*: F[\r\n]" NO_TRACE)
add_repl_test(solver_paths-hook_not_less_unfolds
	"n x !< y" "%1[^\r\n]*: ([^\r\n]* != 0 \\|\\| x = y|x = y \\|\\| [^\r\n]* != 0)[\r\n]" NO_TRACE)
add_repl_test(solver_paths-hook_not_greater_unfolds
	"n x !> y" "%1[^\r\n]*: ([^\r\n]* != 0 \\|\\| x = y|x = y \\|\\| [^\r\n]* != 0)[\r\n]" NO_TRACE)
add_repl_test(solver_paths-hook_complement_inequality
	"n x' != x. n x != x'" "%1[^\r\n]*: T[\r\n].*%1[^\r\n]*: T[\r\n]" NO_TRACE)

# A prefix G with a parenthesized left side and a temporal right side warns
# that it takes the whole connective, for <-, <-> and ^^ too.
add_repl_test(solver_paths-prefix_warning_reverse_implication
	"sat always (o1[t] = 0) <- sometimes (o1[t] = 1)"
	"`G\\(A\\) <- B` is `G\\(A <- B\\)`" NO_TRACE)
add_repl_test(solver_paths-prefix_warning_equivalence
	"sat always (o1[t] = 0) <-> sometimes (o1[t] = 1)"
	"`G\\(A\\) <-> B` is `G\\(A <-> B\\)`" NO_TRACE)
add_repl_test(solver_paths-prefix_warning_xor
	"sat always (o1[t] = 0) ^^ sometimes (o1[t] = 1)"
	"`G\\(A\\) \\^\\^ B` is `G\\(A \\^\\^ B\\)`" NO_TRACE)

# A fixpoint call met twice is calculated once; of two definitions of one
# name the call takes the one with fewer offsets.
add_repl_test(solver_paths-fixpoint_call_met_twice
	"f[0](x) := x = 0. f[n](x) := f[n-1](x). n f(y) && (z = 0 || f(y))"
	"%1[^\r\n]*: y = 0[\r\n]" NO_TRACE)
add_repl_test(solver_paths-fixpoint_call_takes_fewer_offsets
	"g[n, m](x) := x = 0. g[n](x) := x = 1. n g(y)"
	"%1[^\r\n]*: y' = 0[\r\n]" NO_TRACE)

# onf takes each disjunct of a disjunction on its own, and leaves an
# arithmetic atom as it is.
add_repl_test(solver_paths-onf_each_disjunct
	"onf x (x y = 0 || x z != 0)"
	"%1[^\r\n]*: [^\r\n]* \\|\\| [^\r\n]*[\r\n]" NO_TRACE)
add_repl_test(solver_paths-onf_keeps_arithmetic_atom
	"onf x (x:bv[4] + y:bv[4] = {1}:bv[4])"
	"%1[^\r\n]*: x\\+y = \\{ 1 \\}:bv\\[4\\][\r\n]" NO_TRACE REQUIRES bv)
