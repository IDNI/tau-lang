#
# valid command
#

include(add_repl_test)

# the valid command checks if a Tau formula is logically equivalent to T
# and prints T or F
add_repl_test(valid_cmd-t "valid T" ": T")
add_repl_test(valid_cmd-formula "valid x = 0" ": F")
add_repl_test(valid_cmd-tautology "valid x=0 || !x=0" ": T")

# issue #144: a top-level sometimes is decided
add_repl_test(valid_cmd-issue144_sometimes_tautology "valid sometimes (o1[t] = 0 || o1[t] != 0)" ": T")
add_repl_test(valid_cmd-issue144_sometimes_input_tautology "valid sometimes (i1[t] = 0 || i1[t] != 0)" ": T")
add_repl_test(valid_cmd-issue144_sometimes_atom "valid sometimes o1[t] = 1" ": F")
# validity is over traces: a trace whose input is never 1 violates these, so
# the input must not be read universally when `always ¬ψ` is decided
add_repl_test(valid_cmd-issue144_sometimes_input_atom "valid sometimes i1[t] = 1" ": F")
add_repl_test(valid_cmd-issue144_sometimes_input_or_output "valid sometimes (o1[t] = 1 || i1[t] = 1)" ": F")
add_repl_test(valid_cmd-issue144_sometimes_or_always_input "valid (sometimes i1[t] = 1) || (always i1[t] = 1)" ": F")
add_repl_test(valid_cmd-issue144_input_excluded_middle "valid (sometimes i1[t] = 1) || (always i1[t] != 1)" ": T")
