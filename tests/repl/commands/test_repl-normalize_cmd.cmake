#
# normalize command
#

include(add_repl_test)

#test_repl-nnf_bf_cmd
#add_repl_test(normalize_cmd "normalize X  & (Y | Z). q" "X & Y | X & Z")

# 'normalize' command for Boolean functions
add_repl_test(normalize_cmd_bf "normalize 1" ": 1")
add_repl_test(normalize_cmd_bf_mem_rel "1. normalize %-0" "1")
add_repl_test(normalize_cmd_bf_mem_abs "1. normalize %0" "1")

# 'n' command for normalization for Boolean functions
add_repl_test(normalize_cmd_wff "normalize T" ": T")
add_repl_test(normalize_cmd_wff_mem_rel "T. normalize %-0" "T")
add_repl_test(normalize_cmd_wff_mem_abs "T. normalize %0" "T")
