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

# Regression test for AP-N3: get_type_and_arg() used to dereference the
# result of get_applied() unconditionally, crashing (DBG assert / null
# deref in release) whenever get_applied() failed instead of reporting
# the error gracefully. get_applied() legitimately returns nullptr when
# nso_rr_apply()'s fixed-point calculation hits an unsupported multi-index
# offset relation (see calculate_all_fixed_points), so normalizing a fixed
# point call on such a relation must report the error, not crash.
add_repl_test_fail(normalize_cmd-multiindex_fixed_point_call
	"g[0, 0](Y) := Y = 0. g[n, 0](Y) := g[n - 1, 0](Y). normalize g(Y)"
	"multiindex offset relations is not supported")

# Regression tests for issue 36 (REPL hangs when normalizing undefined terms).

# A definition set that does not terminate for the given argument used to run
# forever with no output, no error and no interruption point. Here the
# unfolding oscillates: each pass adds a negation that the simplifier folds
# straight back, so no state is ever a fixpoint and none is ever new either.
add_repl_test_fail(normalize_cmd-oscillating_definition
	"f(x) := f(x)'. normalize f(1)"
	"oscillates without reaching a normal form")

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
