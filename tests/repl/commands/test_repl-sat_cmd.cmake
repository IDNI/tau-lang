#
# definition command
#

# solve z3 sample with mod
add_repl_test(sat_z3_cmd "solve x % 2 =_ 1" "x := 1")
