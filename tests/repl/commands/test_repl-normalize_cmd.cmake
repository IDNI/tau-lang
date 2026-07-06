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
