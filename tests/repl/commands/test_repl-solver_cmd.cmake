#
# definition command
#

# solve cvc5 sample with mod
add_repl_test(solver_cvc5_cmd "solve x : bv % { 2 } = { 1 }" "x := { 32767 }:bv")
