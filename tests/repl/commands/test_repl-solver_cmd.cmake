#
# definition command
#

# solve z3 sample with mod
add_repl_test(solver_z3_cmd "solve x % 2 =_ 1" "#x0000000000000001")
