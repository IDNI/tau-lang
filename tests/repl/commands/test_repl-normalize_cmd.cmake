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

# AP-N3 regression: get_type_and_arg() used to deref a null child.
add_repl_test_fail(normalize_cmd-multiindex_fixed_point_call
	"g[0, 0](Y) := Y = 0. g[n, 0](Y) := g[n - 1, 0](Y). normalize g(Y)"
	"multiindex offset relations is not supported")

# by_grammar parse-only cases (SHAPE-I/K/O): assert parse success only.
add_repl_test(normalize_cmd-by_grammar-shape_i_01_f_f_f_fall_x_o1_and_x_eq_0 "normalize F (F (F ((fall x (o1[t]:bv[8] & x:bv[8])) = {0}:bv[8])))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_02_g_fall_x_o1_and_x_eq_0 "normalize G ((fall x (o1[t]:bv[8] & x:bv[8])) = {0}:bv[8])." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_03_f_f_f_fex_x_o1_and_x_eq_o1 "normalize F (F (F ((fex x (o1[t]:bv[8] & x:bv[8])) = o1[t]:bv[8])))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_04_g_fex_x_o1_and_x_eq_o1 "normalize G ((fex x (o1[t]:bv[8] & x:bv[8])) = o1[t]:bv[8])." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_05_fall_x_eq_0_until_fex_y_eq_o2 "normalize ((fall x (o1[t]:bv[8] & x:bv[8])) = {0}:bv[8]) U ((fex y (o2[t]:bv[8] & y:bv[8])) = o2[t]:bv[8])." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_i_06_g_f_fall_x_o1_and_x_eq_0 "normalize G (F ((fall x (o1[t]:bv[8] & x:bv[8])) = {0}:bv[8]))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_k_08_f_f_f "normalize F (F (F))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_01_f_f_f_i1_qlt_gt_0 "normalize F (F (F (i1[t]:qlt > {0}:qlt)))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_02_g_f_f_i1_qlt_gt_0 "normalize G (F (F (i1[t]:qlt > {0}:qlt)))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_03_f_f_g_i1_qlt_gt_0 "normalize F (F (G (i1[t]:qlt > {0}:qlt)))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_04_f_i1_gt_0_until_f_i2_gt_0 "normalize F ((i1[t]:qlt > {0}:qlt) U (F (i2[t]:qlt > {0}:qlt)))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_05_g_f_f_i1_bv_eq_b10110101 "normalize G (F (F (i1[t]:bv[8] = {#b10110101}:bv[8])))." "%[0-9]+")
add_repl_test(normalize_cmd-by_grammar-shape_o_06_i1_gt_0_until_f_f_i2_gt_0 "normalize (i1[t]:qlt > {0}:qlt) U (F (F (i2[t]:qlt > {0}:qlt)))." "%[0-9]+")

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
add_repl_test_fail(normalize_cmd-oscillating_definition
	"f(x) := f(x)'. normalize f(1)"
	"oscillates without reaching a normal form")

# Issue 28's own script, and the wff spelling @pt7k gave in its thread. Both
# are the same oscillation: `'` only works on sbf, hence the `!` variant.
add_repl_test_fail(normalize_cmd-self_negating_recurrence_bf
	"g[0](y) := 0. g[n](y) := g[n](y)'. normalize g[5](1)"
	"oscillates without reaching a normal form")
add_repl_test_fail(normalize_cmd-self_negating_recurrence_wff
	"g[0](y) := F. g[n](y) := !g[n](y). normalize g[5](1)"
	"oscillates without reaching a normal form")

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
