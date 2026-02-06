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
