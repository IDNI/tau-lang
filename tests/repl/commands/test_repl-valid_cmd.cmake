#
# valid command
#

include(add_repl_test)

# the valid command checks if a Tau formula is logically equivalent to T
# and prints T or F
add_repl_test(valid_cmd-t "valid T" ": T")
add_repl_test(valid_cmd-formula "valid x = 0" ": F")
add_repl_test(valid_cmd-tautology "valid x=0 || !x=0" ": T")
