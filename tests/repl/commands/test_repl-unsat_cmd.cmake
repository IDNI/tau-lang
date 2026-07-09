#
# unsat command
#

include(add_repl_test)

# the unsat command checks if a Tau formula is unsatisfiable and prints T or F
add_repl_test(unsat_cmd-f "unsat F" ": T")
add_repl_test(unsat_cmd-t "unsat T" ": F")
add_repl_test(unsat_cmd-contradiction "unsat x=0 && !x=0" ": T")
