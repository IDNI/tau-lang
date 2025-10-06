#
# definition command
#

# solve cvc5 sample with mod
add_repl_test(solver_cvc5_cmd "solve x % 2 =_ 1" "x := #b0111111111111111")
