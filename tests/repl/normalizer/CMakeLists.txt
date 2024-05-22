cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# In these tests cases we check the results obtained in the normalized command.
# The output of the tests cases must be validated against expected results.

#
# bf
#

add_repl_test(normalize_cmd_bf "n X & 1" ": X")
add_repl_test(normalize_cmd_nso "n X & X' = 0" ": T")

#
# wff
#

add_repl_test(normalize_cmd_wff "n X & X' = 0" ": T")
add_repl_test(normalize_cmd_memory "n X & X' = 0" ": T")
