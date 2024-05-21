#
# normalize command
#

#test_repl-nnf_bf_cmd
#add_repl_test(normalize_cmd "normalize X  & (Y | Z). q" "X & Y | X & Z")

# 'normalize' command for Boolean functions
add_repl_test(normalize_cmd_bf1 "normalize x + x'" ": 1")

# 'n' command for normalization for Boolean functions
add_repl_test(normalize_cmd_bf2 "n x + x'" ": 1")
