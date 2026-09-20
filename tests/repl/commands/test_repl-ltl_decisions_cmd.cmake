#
# Temporal formulas reaching the decision and normal-form commands: an
# undecidable shape is UNKNOWN, never a verdict, and no temporal operator is
# dropped or re-bracketed.
#

include(add_repl_test)

# a data quantifier under a temporal operator has no proposition in the
# skeleton (it used to become the constant 1: REALIZABLE)
add_repl_test_fail(ltl_decisions-quantifier_under_temporal_undecided
	"realizable G (all x (F (i1[t] = x)))" "could not be decided")
# a positional atom under F is refused, not an abort
add_repl_test_fail(ltl_decisions-positional_under_F_no_abort
	"realizable F (o1[0] = 1)" "could not be decided")

# input facts the environment cannot violate are assumed: tautologies over
# three input atoms, and a valid single input atom
add_repl_test(ltl_decisions-input_tautology_bv_three_atoms
	"realizable F ((i1[t]:bv[2] = {0}) || (i1[t]:bv[2] = {1}) || (i1[t]:bv[2] > {1}))" ": T")
add_repl_test(ltl_decisions-input_tautology_tau_three_atoms
	"realizable F ((i1[t] = i2[t]) || (i2[t] != i3[t]) || (i1[t] != i3[t]))" ": T")
add_repl_test(ltl_decisions-input_valid_single_atom
	"realizable F (i1[t]:bv[2] + {1} != i1[t]:bv[2])" ": T")

# ltl takes its verdict from the realizability procedure
add_repl_test_fail(ltl_decisions-ltl_agrees_with_realizable
	"ltl G ((o1[1] = 1) && (o1[t] = o1[t-1]) && F (o1[t]=0))" "\nUNREALIZABLE")

# validity of full LTL: F when the negation is satisfiable, never T for both
add_repl_test(ltl_decisions-valid_until_F
	"valid (o1[t] = 1) U (o2[t] = 1)" ": F")
add_repl_test(ltl_decisions-valid_negated_until_F
	"valid !((o1[t] = 1) U (o2[t] = 1))" ": F")
add_repl_test(ltl_decisions-valid_G_F_no_abort "valid G (F o1[t] = 1)" ": F")

# normal forms keep U / R / W / S / T and CTL* scopes as opaque literals
add_repl_test(ltl_decisions-dnf_keeps_until
	"dnf (o1[t] = 1 || o2[t] = 1) U (o3[t] = 1)"
	"\\(o1\\[t\\]:tau = 1 \\|\\| o2\\[t\\]:tau = 1\\) U o3")
add_test(NAME "test_repl-ltl_decisions-dnf_keeps_semantic_negation"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"fragment ctl_star. dnf (-(G o1[t] = 1)) && (G o2[t] = 1)\"")
set_tests_properties("test_repl-ltl_decisions-dnf_keeps_semantic_negation"
	PROPERTIES PASS_REGULAR_EXPRESSION "-\\(always o1")

# U binds tighter than || in the grammar: the printer keeps the operand
add_repl_test(ltl_decisions-print_until_operand
	"nnf (o1[t] = 1 || o3[t] = 1) U (o2[t] = 1)"
	"\\(o1\\[t\\]:tau = 1 \\|\\| o3\\[t\\]:tau = 1\\) U o2")

# a decided operand folds: φ U F = F
add_repl_test(ltl_decisions-normalize_until_false_target
	"normalize (o1[t] = 1) U (o2[t] = 1 && o2[t] = 0)" ": F")

# a lookback guard binds each literal, not the whole operand: the
# lookback-free conjunct of the G body holds from step 0, so the F cannot
# witness there, and sat and realizable agree
add_repl_test(ltl_decisions-per_literal_guard_sat
	"sat (F (o1[t] = 0)) && (G (o2[t] = o2[t-1] && o1[t] = 1))" ": F")
add_repl_test(ltl_decisions-per_literal_guard_realizable
	"realizable (F (o1[t] = 0)) && (G (o2[t] = o2[t-1] && o1[t] = 1))" ": F")

# a consistency cap that gave up cannot decide UNREALIZABLE
add_repl_test_fail(ltl_decisions-consistency_cap_is_unknown
	"set maxsubsets 1. realizable F ((i1[t] = i2[t]) || (i2[t] != i3[t]) || (i1[t] != i3[t]))"
	"could not be decided")

# a binary temporal or negated operand of U is wrapped, so the print
# re-parses as the same tree
add_repl_test(ltl_decisions-print_wraps_negated_until_operand
	"qelim (!((o1[t] = 1) && (o3[t] = 1))) U (o2[t] = 1)"
	"\\(!\\(o1\\[t\\]:tau = 1 && o3\\[t\\]:tau = 1\\)\\) U o2")
add_repl_test(ltl_decisions-print_wraps_nested_until
	"qelim ((o1[t] = 1) U (o2[t] = 1)) U (o3[t] = 1)"
	"\\(o1\\[t\\]:tau = 1 U o2\\[t\\]:tau = 1\\) U o3")
