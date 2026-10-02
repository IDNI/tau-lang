#
# normalize command
#

include(add_repl_test)

# term
add_repl_test(normalize_cmd_bf "n X & 1" ": X")
add_repl_test(normalize_cmd_nso "n X & X' = 0" ": T")

# formula
add_repl_test(normalize_cmd_wff "n X & X' = 0" ": T")
add_repl_test(normalize_cmd_memory "X & X' = 0. n %" ": T")

# function applications
add_repl_test(normalize_cmd_func_app_1 "f(x) := x'. n f(t)" "t'")

# predicate applications
add_repl_test(normalize_cmd_pred_app_1 "p(x) := x' = 0. n p(t)" "t' = 0")

# 'normalize' command for terms with history reference
add_repl_test(normalize_cmd_bf_nonmem "normalize 1" ": 1")
add_repl_test(normalize_cmd_bf_mem_rel "1. normalize %-0" "1")
add_repl_test(normalize_cmd_bf_mem_abs "1. normalize %1" "1")

# 'normalize' command for normalization for formulas with history reference
add_repl_test(normalize_cmd_wff_nonmem "normalize T" ": T")
add_repl_test(normalize_cmd_wff_mem_rel "T. normalize %-0" "T")
add_repl_test(normalize_cmd_wff_mem_abs "T. normalize %1" "T")

# bv[1] has two values, so pinning a variable to both is a tautology
add_repl_test(normalize_cmd-bv1_excluded_middle "normalize x:bv[1] = 1 || x:bv[1] = 0" ": T")
add_repl_test(normalize_cmd-bv1_excluded_middle_stream "normalize o1[t]:bv[1] = 0 || o1[t]:bv[1] = 1" ": T")
add_repl_test(normalize_cmd-bv1_excluded_middle_dual "normalize x:bv[1] != 1 && x:bv[1] != 0" ": F")

# AP-N3 regression: get_type_and_arg() used to deref a null child.
add_repl_test(normalize_cmd-multiindex_fixed_point_call
	"g[0, 0](Y) := Y = 0. g[n, 0](Y) := g[n - 1, 0](Y). normalize g(Y)"
	"multiindex offset relations is not supported" NO_FAIL_REGEX)

# by_grammar parse-only cases (SHAPE-I/K/O): assert parse success only.
add_repl_test(normalize_cmd-by_grammar-shape_i_01_f_f_f_fall_x_o1_and_x_eq_0 "normalize F (F (F ((fall x (o1[t]:bv[8] & x:bv[8])) = {0}:bv[8])))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_02_g_fall_x_o1_and_x_eq_0 "normalize G ((fall x (o1[t]:bv[8] & x:bv[8])) = {0}:bv[8])." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_03_f_f_f_fex_x_o1_and_x_eq_o1 "normalize F (F (F ((fex x (o1[t]:bv[8] & x:bv[8])) = o1[t]:bv[8])))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_04_g_fex_x_o1_and_x_eq_o1 "normalize G ((fex x (o1[t]:bv[8] & x:bv[8])) = o1[t]:bv[8])." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_05_fall_x_eq_0_until_fex_y_eq_o2 "normalize ((fall x (o1[t]:bv[8] & x:bv[8])) = {0}:bv[8]) until ((fex y (o2[t]:bv[8] & y:bv[8])) = o2[t]:bv[8])." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_06_g_f_fall_x_o1_and_x_eq_0 "normalize G (F ((fall x (o1[t]:bv[8] & x:bv[8])) = {0}:bv[8]))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_k_08_f_f_f "normalize F (F (F))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_01_f_f_f_i1_qlt_gt_0 "normalize F (F (F (i1[t]:qlt > {0}:qlt)))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_02_g_f_f_i1_qlt_gt_0 "normalize G (F (F (i1[t]:qlt > {0}:qlt)))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_03_f_f_g_i1_qlt_gt_0 "normalize F (F (G (i1[t]:qlt > {0}:qlt)))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_04_f_i1_gt_0_until_f_i2_gt_0 "normalize F ((i1[t]:qlt > {0}:qlt) until (F (i2[t]:qlt > {0}:qlt)))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_05_g_f_f_i1_bv_eq_b10110101 "normalize G (F (F (i1[t]:bv[8] = {#b10110101}:bv[8])))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_06_i1_gt_0_until_f_f_i2_gt_0 "normalize (i1[t]:qlt > {0}:qlt) until (F (F (i2[t]:qlt > {0}:qlt)))." "%[0-9]+")

# hooks: wff_imply / wff_rimply / wff_equiv / wff_lt / wff_lteq (tests/unit/test_hooks.cpp)
add_repl_test(normalize_cmd-hooks_wff_imply-01_f_imply_x_eq_t    "normalize F -> x = 0"           ": T")
add_repl_test(normalize_cmd-hooks_wff_imply-04_x_imply_t_eq_t    "normalize (x = 0) -> T"         ": T")
add_repl_test(normalize_cmd-hooks_wff_imply-05_x_imply_x_eq_t    "normalize (x = 0) -> (x = 0)"   ": T")
add_repl_test(normalize_cmd-hooks_wff_rimply-04_t_rimply_x_eq_t  "normalize T <- (x = 0)"         ": T")
add_repl_test(normalize_cmd-hooks_wff_rimply-05_x_rimply_f_eq_t  "normalize (x = 0) <- F"         ": T")
add_repl_test(normalize_cmd-hooks_wff_equiv-05_x_equiv_x_eq_t    "normalize (x = 0) <-> (x = 0)"  ": T")
add_repl_test(normalize_cmd-hooks_wff_equiv-06_x_equiv_negx_eq_f "normalize (x = 0) <-> !(x = 0)" ": F")
add_repl_test(normalize_cmd-hooks_wff_equiv-07_negx_equiv_x_eq_f "normalize !(x = 0) <-> (x = 0)" ": F")
add_repl_test(normalize_cmd-hooks_wff_lt_lteq-03_lt_self_eq_f    "normalize x < x"                ": F")
add_repl_test(normalize_cmd-hooks_wff_lt_lteq-04_lteq_self_eq_t  "normalize x <= x"               ": T")

# quantifiers -- normalized verdict (assertion 2; raw form is in history_cmd)
add_repl_test(normalize_cmd-quantifiers-ex_01_ex_x_x_eq_0        "normalize ex x x=0."             ": T")
add_repl_test(normalize_cmd-quantifiers-ex_02_ex_x_y_xy_eq_0     "normalize ex x,y xy=0."          ": T")
add_repl_test(normalize_cmd-quantifiers-ex_03_ex_x_ex_y_xy_eq_0  "normalize ex x ex y xy=0."       ": T")
# "all" TEST_CASE, all_cases (test_integration-quantifiers.cpp:18-22, 67)
add_repl_test(normalize_cmd-quantifiers-all_01_all_x_x_ne_0        "normalize all x x!=0."           ": F")
add_repl_test(normalize_cmd-quantifiers-all_02_all_x_y_xy_ne_0     "normalize all x,y xy!=0."        ": F")
add_repl_test(normalize_cmd-quantifiers-all_03_all_x_all_y_xy_ne_0 "normalize all x all y xy!=0."    ": F")
# "ex all" TEST_CASE, ex_all_cases (test_integration-quantifiers.cpp:24-28, 68)
add_repl_test(normalize_cmd-quantifiers-ex_all_01_ex_x_all_y_x_eq_y "normalize ex x all y x=y."                ": F")
add_repl_test(normalize_cmd-quantifiers-ex_all_02_ex_xy_all_wz_x_eq_w_and_y_eq_z "normalize ex x,y all w,z x=w&&y=z." ": F")
# "all ex" TEST_CASE, all_ex_cases (test_integration-quantifiers.cpp:30-34, 69)
add_repl_test(normalize_cmd-quantifiers-all_ex_01_all_x_ex_y_x_eq_y "normalize all x ex y x=y."                ": T")
add_repl_test(normalize_cmd-quantifiers-all_ex_02_all_xy_ex_wz_x_eq_w_and_y_eq_z "normalize all x,y ex w,z x=w && y=z." ": T")

# Regression tests for issue 36 (REPL hangs when normalizing undefined terms).

# A definition set that does not terminate for the given argument used to run
# forever with no output, no error and no interruption point. Here the
# unfolding oscillates: each pass adds a negation that the simplifier folds
# straight back, so no state is ever a fixpoint and none is ever new either.
add_repl_test(normalize_cmd-oscillating_definition
	"f(x) := f(x)'. normalize f(1)"
	"oscillates without reaching a normal form" NO_FAIL_REGEX)

# Issue 28's own script, and the wff spelling @pt7k gave in its thread. Both
# are the same oscillation: `'` only works on sbf, hence the `!` variant.
add_repl_test(normalize_cmd-self_negating_recurrence_bf
	"g[0](y) := 0. g[n](y) := g[n](y)'. normalize g[5](1)"
	"oscillates without reaching a normal form" NO_FAIL_REGEX)
add_repl_test(normalize_cmd-self_negating_recurrence_wff
	"g[0](y) := F. g[n](y) := !g[n](y). normalize g[5](1)"
	"oscillates without reaching a normal form" NO_FAIL_REGEX)

# The corrected form from the same thread, referring to the previous step,
# normalizes to 1. It used to print "Failed to translate the formula to cvc5: 1"
# first: a formula with no bitvector content at all passed
# is_bv_solvable_formula vacuously and was handed to a solver that cannot
# translate it, making a working normalization look like it had failed.
# add_repl_test fails the test on any "Error" in the output, which is the point.
add_repl_test(normalize_cmd-prior_step_recurrence_no_cvc5_error
	"g[0](y) := 0. g[n](y) := g[n-1](y)'. normalize g[5](1)"
	"1")

# A definition set whose unfolding grows instead of oscillating is not covered
# here: no revisit check can catch it, only repeat_all's round cap, which is
# unbounded pending the runtime parameter it is marked TODO (HIGH) for. Once a
# finite cap exists, `g(x) := h(g(x)). normalize g(1)` reports
# "did not reach a fixpoint" and belongs here (see the matching skipped case in
# tests/unit/test_execution.cpp).

# A definition whose head and body are both untyped is classified as a
# predicate relation, so its head used to be recorded as a wff reference. Every
# call from an argument position is a bf reference, so the rule never matched
# there and the call was silently left unexpanded.
add_repl_test(normalize_cmd-pred_shaped_def_used_as_argument
	"foo(a) := bar(a). baz(x) := x. normalize baz(foo(1))"
	"bar")

# End-to-end check of the same defect on the script from the issue thread: the
# `add(int[0](1), x) := x` base case only terminates if `pred` is expanded
# inside `add`'s argument, which is exactly what did not happen.
add_repl_test(normalize_cmd-recursive_defs_in_argument_position
	"succ(int[0](1)) := int[1](1). succ(int[1](1)) := int[2](1). pred(int[t](1)) := int[t-1](1). add(int[0](1), x) := x. add(x, int[0](1)) := x. add(x, y) := add(pred(x), succ(y)). normalize add(int[3](1), int[10](1))"
	"succ.*succ.*succ.*int")

# Fixpoint calls to FUNCTION recurrences. An offset-free call (`g(y)`) parses
# as a wff reference, and is_functional_ref only matched exact signatures, so
# the call was never reclassified into the rules' own bf world: no rule ever
# applied to the enumerated steps and calculate_fixed_point enumerated bare
# g[i](y) refs forever -- a silent REPL hang with no cap set. Indexed calls
# (`n g[5](y)`, above) always worked, since their signature matches exactly.
add_repl_test(normalize_cmd-fp_call_function_loop_default_fallback
	"g[0](x) := 0. g[n](x) := g[n-1](x)'. normalize g(y)"
	": 0")
add_repl_test(normalize_cmd-fp_call_function_loop_fallback_last
	"g[0](x) := 0. g[n](x) := g[n-1](x)'. normalize g(y) fallback last"
	": 1")
add_repl_test(normalize_cmd-fp_call_function_converging
	"g[0](x):sbf := 0. g[n](x):sbf := g[n-1](x) | x. normalize g(y:sbf)"
	": y")

# The same hang, one step removed: a FUNCTIONAL definition's argument types
# used to leak into the surrounding scope (open_same_type assigned in the
# current scope instead of opening one), so after defining g over :sbf y, an
# unrelated later call `k(y)` had its y typed :sbf while k's stored rule
# captures were :tau -- the rules silently never matched and the (predicate)
# fixpoint enumeration ran forever.
add_repl_test(normalize_cmd-fp_call_after_unrelated_function_def
	"g[0](y):sbf := 0. g[n](y):sbf := g[n-1](y)'. k[0](x) := x = 0. k[n](x) := k[n-1](x) && x != 1. normalize k(y)"
	": y = 0")

# A cast of a non-bitvector operand, or a cast result meeting a
# non-bitvector sibling, is rejected by type inference. Both used to reach
# the solver's cast translation and abort the process (Debug: "bv type must
# have explicit bitwidth"; Release: a core dump). The error IS the expected
# output, so NO_FAIL_REGEX.
add_repl_test(normalize_cmd-cast_of_sbf_operand_rejected
	"n (bv[8]) x:sbf = y:bv[8]" "Incompatible type information" NO_FAIL_REGEX)
add_repl_test(normalize_cmd-cast_of_tau_stream_rejected
	"n (bv[8]) i1[t]:tau = o1[t]:bv[8]" "Incompatible type information" NO_FAIL_REGEX)
add_repl_test(normalize_cmd-cast_result_meets_sbf_rejected
	"n ((bv[8]) x:bv[4]) & y:sbf = 0" "Incompatible type information" NO_FAIL_REGEX)
add_repl_test(normalize_cmd-cast_result_types_untyped_sibling
	"n (bv[8]) x:bv[4] = y" "y")

# GitHub #120: a chain of bitvector definitions read by two guards folds to
# constants in either conjunct order (the equality propagation orders the
# assignments by dependency).
add_repl_test(normalize_cmd_definition_chain_forward  "n (s:bv[8] = { 215 }:bv[8] ^ { 24 }:bv[8] ^ { 53 }:bv[8] ^ { 55 }:bv[8]) && (l:bv[8] = { 0 }:bv[8] + s:bv[8]) && (n:bv[8] = (l:bv[8] ^ (l:bv[8] << { 3 }:bv[8])) ^ ((l:bv[8] ^ (l:bv[8] << { 3 }:bv[8])) >> { 5 }:bv[8])) && (d:bv[8] = (n:bv[8] % { 6 }:bv[8]) + { 1 }:bv[8]) && (({ 53 }:bv[8] + d:bv[8] > { 42 }:bv[8]) || (w:bv[8] = { 42 }:bv[8])) && (({ 53 }:bv[8] + d:bv[8] !> { 42 }:bv[8]) || (w:bv[8] = { 53 }:bv[8] + d:bv[8]))" "w = { 58 }:bv")
add_repl_test(normalize_cmd_definition_chain_reversed "n (d:bv[8] = (n:bv[8] % { 6 }:bv[8]) + { 1 }:bv[8]) && (n:bv[8] = (l:bv[8] ^ (l:bv[8] << { 3 }:bv[8])) ^ ((l:bv[8] ^ (l:bv[8] << { 3 }:bv[8])) >> { 5 }:bv[8])) && (l:bv[8] = { 0 }:bv[8] + s:bv[8]) && (s:bv[8] = { 215 }:bv[8] ^ { 24 }:bv[8] ^ { 53 }:bv[8] ^ { 55 }:bv[8]) && (({ 53 }:bv[8] + d:bv[8] > { 42 }:bv[8]) || (w:bv[8] = { 42 }:bv[8])) && (({ 53 }:bv[8] + d:bv[8] !> { 42 }:bv[8]) || (w:bv[8] = { 53 }:bv[8] + d:bv[8]))" "w = { 58 }:bv")

# GitHub #188: the typed 0 and 1 of qlt are the empty set and all of Q, values
# a qlt variable may take like any other.
add_repl_test(normalize_cmd-qlt_end_is_no_point_ex
	"normalize ex x:qlt (x = 1 && all y:qlt (y < x))" ": F")
add_repl_test(normalize_cmd-qlt_end_is_no_point_all
	"normalize all x:qlt (x < 1) && ex y:qlt (y = 1)" ": F")
add_repl_test(normalize_cmd-qlt_var_eq_end "normalize x:qlt = 1" ": x' = 0")
add_repl_test(normalize_cmd-qlt_var_neq_end "normalize x:qlt != 0" ": x != 0")
add_repl_test(normalize_cmd-qlt_all_below_end "normalize all x:qlt (x < 1)" ": F")
add_repl_test(normalize_cmd-qlt_all_at_most_end "normalize all x:qlt (x <= 1)" ": T")
add_repl_test(normalize_cmd-qlt_ex_eq_ends "normalize ex x:qlt (x = 0) && ex y:qlt (y = 1)" ": T")

# GitHub #189: a closed conjunct that does not mention the variable under
# elimination is not dropped.
add_repl_test(normalize_cmd-qlt_closed_conjunct_not_dropped
	"normalize ex x:qlt ex z:qlt all w:qlt ((x != w && (x > w || z > x) && z >= x) || (x = w && z < x))" ": F")
add_repl_test(normalize_cmd-qlt_nested_equiv_valid
	"normalize all x:qlt all z:qlt ex w:qlt (((x != w) && ((x > w) || (z > x))) <-> (z < x))" ": T")
add_repl_test(normalize_cmd-qlt_nested_xor_unsat
	"normalize ex x:qlt ex z:qlt all w:qlt (((x != w) && ((x > w) || (z > x))) ^^ (z < x))" ": F")

# GitHub #187: a compound term holding a qlt variable is no bound on it:
# x & {3} = 0 lies strictly below {1} while x contains {5} and more.
add_repl_test(normalize_cmd-qlt_compound_term_is_no_bound
	"normalize ex x:qlt ((x & {3}:qlt) < {1}:qlt && x > {5}:qlt)" ": T")

# GitHub #183: Boole's elimination law does not hold for arithmetic, so a
# bitvector variable under `-` keeps its binder for the solver paths
add_repl_test(normalize_cmd-issue183_sub_true
	"normalize all x:bv[3] ((x:bv[3] != {6}:bv[3]) || (ex y:bv[3] ((x:bv[3] & {1}:bv[3]) = (y:bv[3] - {2}:bv[3]))))." "%1[^%]*: T")
add_repl_test(normalize_cmd-issue183_sub_false
	"normalize all x:bv[2] ((x:bv[2] = {0}:bv[2]) -> (ex y:bv[2] ((x:bv[2] | (y:bv[2] & {2}:bv[2])) = (y:bv[2] - {3}:bv[2]))))." "%1[^%]*: F")
set(_issue183_cmd "normalize all x:bv[3] ((x:bv[3] != {6}:bv[3]) || (ex y:bv[3] ((x:bv[3] & {1}:bv[3]) = (y:bv[3] - {2}:bv[3]))))")
foreach(_s 0 2)
	add_repl_test(normalize_cmd-issue183_sub_true_splits${_s} "${_issue183_cmd}"
		"%1[^%]*: T" NO_TRACE TIMEOUT 60
		FLAGS --preprocessing=false --bv-widening=false
			--bv-quantifier-free-decision=false --block-max-splits=${_s})
endforeach()

# GitHub #185: min/max are built-ins under a cast, a complement and a
# juxtaposed conjunction, and their printed form reads back the same
add_repl_test(normalize_cmd-issue185_cast_max_sat
	"sat ((bv[4]) max(y:bv[3], {1}:bv[3])) = {2}:bv[4]." "%1[^%]*: T")
add_repl_test(normalize_cmd-issue185_cast_max_valid
	"valid ((bv[4]) max(y:bv[3], {1}:bv[3])) != {2}:bv[4]." "%1[^%]*: F")
add_repl_test(normalize_cmd-issue185_cast_max_solve
	"solve ((bv[4]) max(y:bv[3], {1}:bv[3])) = {2}:bv[4]." "y := \\{ 2 \\}:bv\\[3\\]")
add_repl_test(normalize_cmd-issue185_complement_min_sat
	"sat min(x:bv[2], {2}:bv[2])' = {2}:bv[2]." "%1[^%]*: T")
add_repl_test(normalize_cmd-issue185_printed_complement_min_sat
	"sat min(x, { 2 }:bv[2])' = { 2 }:bv[2]." "%1[^%]*: T")
add_repl_test(normalize_cmd-issue185_juxtaposed_min_printed
	"normalize x:bv[2] min(x:bv[2], {2}:bv[2]) = {2}:bv[2]." "x min\\(x")
add_repl_test(normalize_cmd-issue185_juxtaposed_min_sat
	"sat x:bv[2] min(x:bv[2], {2}:bv[2]) = {2}:bv[2]." "%1[^%]*: T")

# GitHub #148: a disequality next to a qlt variable pinned to one value is kept:
# the pin is substituted for the variable.
add_repl_test(normalize_cmd-qlt_pinned_neq_sat
	"sat a:qlt = {1}:qlt && (ex x ({1}:qlt <= x:qlt && x:qlt <= {1}:qlt && x:qlt != a:qlt))" ": F")
add_repl_test(normalize_cmd-qlt_pinned_neq_valid
	"valid ex x (a:qlt <= x:qlt && x:qlt <= a:qlt && x:qlt != b:qlt)" ": F")
add_repl_test(normalize_cmd-qlt_pinned_neq_solve
	"solve a:qlt = {1}:qlt && (ex x ({1}:qlt <= x:qlt && x:qlt <= {1}:qlt && x:qlt != a:qlt))" "no solution")
add_repl_test(normalize_cmd-qlt_pinned_neq_closed
	"normalize ex a:qlt ex b:qlt ex c:qlt ((((a <= c) && (c <= a) && (b <= c)) || ((a <= c) && (c <= a) && (b < c))) && (c != a))" ": F")
add_repl_test(normalize_cmd-qlt_pinned_neq_residual
	"normalize ex x:qlt (a:qlt <= x && x <= a && x != b:qlt)" ": a != b")

# GitHub #149: fex / fall are evaluated before any substitution can cross their
# binder, on every command, so a binder never captures a variable of the same
# name outside it.
add_repl_test(normalize_cmd-fall_binder_not_captured
	"normalize ((fall y (x | y)) & y) = 0" ": (xy|yx) = 0")
add_repl_test(normalize_cmd-fex_binder_solve
	"solve b = 0 && (fex b (a & b)) != 0" "a := \\{ T \\}")
add_repl_test(normalize_cmd-fex_equals_body
	"normalize (fex y (x & y)) = x" ": T")

# GitHub #148 follow-up: disequalities beside a lower and an upper bound. In
# the order of sets ex x (L <= x <= U && x != c) is L < U || (L = U && L != c);
# with free bounds the binder stays, a closed formula is decided.
add_repl_test(normalize_cmd-qlt_density_residual
	"normalize ex x:qlt (a:qlt <= x && x <= b:qlt && x != c:qlt)" ": ex b1 ")
add_repl_test(normalize_cmd-qlt_density_strict
	"normalize ex x:qlt (a:qlt < x && x <= b:qlt && x != c:qlt && x != d:qlt)" ": ex b1 ")
add_repl_test(normalize_cmd-qlt_density_closed_some
	"normalize ex a:qlt ex b:qlt ex x:qlt (a <= x && x <= b && x != a && x != b)" ": T")
add_repl_test(normalize_cmd-qlt_density_closed_all
	"normalize all a:qlt all b:qlt ex x:qlt (a <= x && x <= b && x != a && x != b)" ": F")
add_repl_test(normalize_cmd-qlt_density_point_excluded
	"sat a:qlt = b:qlt && c:qlt = a:qlt && (ex x:qlt (a <= x && x <= b && x != c))" ": F")
add_repl_test(normalize_cmd-qlt_density_point_kept
	"sat a:qlt = b:qlt && (ex x:qlt (a <= x && x <= b && x != c:qlt))" ": T")
add_repl_test(normalize_cmd-qlt_density_equivalence
	"valid all a:qlt all b:qlt all c:qlt ((ex x:qlt (a <= x && x <= b && x != c)) <-> (a < b || (a = b && a != c)))" ": T")
# GitHub #185: a bare min/max is the built-in, not a function call, as the
# whole right-hand side of a definition and as a fixed-point fallback
add_repl_test(normalize_cmd-issue185_definition_max
	"g(x:bv[2]) := max(x:bv[2], {2}:bv[2]). normalize g({0}:bv[2]) = {2}:bv[2]." "%1[^%]*: T")
add_repl_test(normalize_cmd-issue185_definition_min
	"g(x:bv[2]) := min(x:bv[2], {2}:bv[2]). normalize g({3}:bv[2]) = {2}:bv[2]." "%1[^%]*: T")
add_repl_test(normalize_cmd-issue185_definition_ref_kept
	"f(x, y) := x | y. g(x) := f(x, x). normalize g(a) = 0." "%1[^%]*: a = 0")
add_repl_test(normalize_cmd-issue185_fallback_max
	"g[0](x:bv[2]) := {0}:bv[2]. g[n](x:bv[2]) := g[n-1](x:bv[2])'. normalize (g({1}:bv[2]) fallback max({0}:bv[2], {2}:bv[2])) = {2}:bv[2]." "%1[^%]*: T")

# The zero test and the test for one of a constant whose formula refers to
# absolute time are decided on the formula as a whole.
add_repl_test(normalize_cmd-tau_absolute_time_zero "set charvar off. normalize { (always o2[t] = 1) && (always o1[t-2] = 0) && (sometimes o2[t-1] = 0) }:tau = 0" ": F")
add_repl_test(normalize_cmd-tau_absolute_time_one "set charvar off. normalize { sometimes ((o1[t-2] != 0 && [t < 2]) || (o3[t] = o3[1] && i1[t] = o3[t-1]) || (o4[1] = 0 && [t >= 3])) }:tau = 1" ": F")
add_repl_test(normalize_cmd-tau_absolute_time_fixed_point_zero "set charvar off. normalize { (always (o2[0] = 0 && o2[t] = 1)) && (always o1[t-2] = 0) }:tau = 0" ": F")
add_repl_test(normalize_cmd-tau_absolute_time_constraint_zero "set charvar off. normalize { (always ([t < 2] -> i1[t] = 0)) && (always ((o4[t-2] = 1 && o1[t] = o4[t-2]) -> o1[t] = 1)) }:tau = 0" ": F")

# GitHub #197: a point is an atom, so no set y meets both {3} and its
# complement inside {3}; an interval, or two points, splits.
add_repl_test(normalize_cmd-qlt_point_meet_ex
	"normalize ex y:qlt ((({3}:qlt & y) != 0) && (({3}:qlt & y') != 0))" ": F")
add_repl_test(normalize_cmd-qlt_point_meet_valid
	"valid all y:qlt ((({3}:qlt & y) = 0) || (({3}:qlt & y') = 0))" ": T")
add_repl_test(normalize_cmd-qlt_point_meet_sat
	"sat ex y:qlt ((({3}:qlt & y) != 0) && (({3}:qlt & y') != 0))" ": F")
add_repl_test(normalize_cmd-qlt_point_meet_one_side
	"normalize ex y:qlt ((({3}:qlt & y) != 0) && (({3}:qlt & y') = 0))" ": T")
add_repl_test(normalize_cmd-qlt_point_meet_all_either
	"normalize all y:qlt ((({3}:qlt & y) != 0) || (({3}:qlt & y') != 0))" ": T")
add_repl_test(normalize_cmd-qlt_point_meet_subst_same
	"normalize (({3}:qlt & {3}:qlt) != 0) && (({3}:qlt & {3}:qlt') != 0)" ": F")
add_repl_test(normalize_cmd-qlt_point_meet_subst_other
	"normalize (({3}:qlt & {5}:qlt) != 0) && (({3}:qlt & {5}:qlt') != 0)" ": F")
add_repl_test(normalize_cmd-qlt_interval_meet_ex
	"normalize ex y:qlt ((({[0,1]}:qlt & y) != 0) && (({[0,1]}:qlt & y') != 0))" ": T")
add_repl_test(normalize_cmd-qlt_interval_meet_all
	"normalize all y:qlt ((({[0,1]}:qlt & y) = 0) || (({[0,1]}:qlt & y') = 0))" ": F")
add_repl_test(normalize_cmd-qlt_two_points_meet_ex
	"normalize ex y:qlt ((({3}:qlt & y) != 0) && (({5}:qlt & y') != 0))" ": T")
add_repl_test(normalize_cmd-qlt_two_points_meet_all
	"normalize all y:qlt ((({3}:qlt & y) = 0) || (({5}:qlt & y') = 0))" ": F")
add_repl_test(normalize_cmd-qlt_point_interval_meet_ex
	"normalize ex y:qlt ((({3}:qlt & y) != 0) && (({[0,1]}:qlt & y') != 0))" ": T")
add_repl_test(normalize_cmd-qlt_point_interval_meet_all
	"normalize all y:qlt ((({3}:qlt & y) = 0) || (({[0,1]}:qlt & y') = 0))" ": F")
add_repl_test(normalize_cmd-qlt_two_points_meet_both
	"normalize ex y:qlt ((({3}:qlt & y) != 0) && (({5}:qlt & y) != 0))" ": T")
add_repl_test(normalize_cmd-qlt_two_points_split_three
	"normalize ex x:qlt ex y:qlt ex z:qlt (((({3}:qlt|{5}:qlt) & x) != 0) && ((({3}:qlt|{5}:qlt) & y) != 0) && ((({3}:qlt|{5}:qlt) & z) != 0) && (x & y) = 0 && (x & z) = 0 && (y & z) = 0)" ": F")
add_repl_test(normalize_cmd-qlt_interval_split_three
	"normalize ex x:qlt ex y:qlt ex z:qlt ((({(0,1)}:qlt & x) != 0) && (({(0,1)}:qlt & y) != 0) && (({(0,1)}:qlt & z) != 0) && (x & y) = 0 && (x & z) = 0 && (y & z) = 0)" ": T")
# Conjuncts on y alone that no y satisfies refute the scope whatever x is.
add_repl_test(normalize_cmd-qlt_point_meet_open
	"normalize ex y:qlt ((({3}:qlt & y) != 0) && (({3}:qlt & y') != 0) && ((x:qlt & y) != 0))" ": F")
add_repl_test(normalize_cmd-qlt_point_meet_order
	"normalize ex y:qlt (y > {5}:qlt && ({3}:qlt & y) = 0)" ": T")
# The order of sets: {1} < x < {[0,3]} holds for x = {0, 1}, nothing lies
# strictly between {1} and {3}, and 1 is above every other set.
add_repl_test(normalize_cmd-qlt_order_is_inclusion_const
	"normalize {1}:qlt < {3}:qlt" ": F")
add_repl_test(normalize_cmd-qlt_order_is_inclusion_interval
	"normalize {1}:qlt < {[0,3]}:qlt" ": T")
add_repl_test(normalize_cmd-qlt_order_between_sets
	"normalize ex x:qlt ({1}:qlt < x && x < {[0,3]}:qlt)" ": T")
add_repl_test(normalize_cmd-qlt_order_between_points
	"normalize ex x:qlt ({1}:qlt < x && x < {3}:qlt)" ": F")
add_repl_test(normalize_cmd-qlt_order_vars
	"normalize ex x:qlt ex y:qlt (x & y' = 0 && x != y)" ": T")
add_repl_test(normalize_cmd-qlt_order_vars_expand
	"normalize x:qlt < y:qlt" ": xy' = 0 && (y != x|x != y)")
add_repl_test(normalize_cmd-qlt_ex_interval_value
	"normalize ex x:qlt (x = {[0,1]}:qlt)" ": T")
add_repl_test(normalize_cmd-qlt_point_meet_order_empty
	"normalize ex y:qlt (y < {1}:qlt && y > {5}:qlt && ({3}:qlt & y) != 0)" ": F")
# Excluding finitely many values leaves one, and Boole's necessary condition's
# F stands.
add_repl_test(normalize_cmd-qlt_excluded_values_ex
	"normalize ex o:qlt (o != {[0,1]}:qlt && o != i:qlt)" ": T")
add_repl_test(normalize_cmd-qlt_excluded_vars_ex
	"normalize ex x:qlt (x != a:qlt && x != b:qlt)" ": T")
add_repl_test(normalize_cmd-qlt_pinned_and_excluded_ex
	"normalize ex b:qlt (b != {[0,1]}:qlt && (b & {[0,1]}:qlt' | b' & {[0,1]}:qlt) = 0)" ": F")
