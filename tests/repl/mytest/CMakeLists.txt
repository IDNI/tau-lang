cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# In these tests cases we check the results obtained in the substitution command.
# The output of the tests cases must be validated against expected results.

# BF Substitution

add_repl_test(substitution_cmd_bf1 "m x. m 1. s x %-0 %-1" ": 1")

add_repl_test(substitution_cmd_bf2 "m ax + bx'. m y&z. s x %-0 %-1" ": \\(a & y & z\\) \\+ \\(b & \\(y & z\\)'\\)")

add_repl_test(substitution_cmd_bf3 "m fex k h & k. m v & w. s k %-0 %-1" ": fex k h & k")

# TODO: Variable is placed into scope of quantifier.
add_repl_test(substitution_cmd_bf4 "m fex x x & y. m x. s y %-0 %-1" ": fex x x & x")

add_repl_test(substitution_cmd_bf5 "m (x | (w & z)) & z. m b. s z %-0 %-1" ": \\(x | w & b\\) & b")

add_repl_test(substitution_cmd_bf6 "m x & y. m T. s x %-0 %-1" "error: invalid argument")

# WFF Substitution

add_repl_test(substitution_cmd_wff1 "m ?x && ?y. m T. s ?x %-0 %-1" ": T && \\?y")

add_repl_test(substitution_cmd_wff2 "m ?a && ?x ^ ?b && !?x. m ?y && ?z. s ?x %-0 %-1" ": \\(\\?a && \\?y && \\?z\\) \\^ \\(\\?b && !\\(\\?y && \\?z\\)\\)")

add_repl_test(substitution_cmd_wff3 "m bool_ex ?k ?h && ?k. m ?v && ?w. s ?k %-0 %-1" ": bool_ex \\?k \\?h && \\?k")

add_repl_test(substitution_cmd_wff4 "m ex x x=0 && y=0. m ?x. s ?y %-0 %-1" ": ex x x = 0 && y = 0")

# TODO: Variable is placed into scope of quantifier.
add_repl_test(substitution_cmd_wff5 "m bool_ex ?x ?x && ?y. m ?x. s ?y %-0 %-1" ": bool_ex \\?x \\?x && \\?x")

add_repl_test(substitution_cmd_wff6 "m (?x || (?w && ?z)) && ?z. m ?b. s ?z %-0 %-1" ": \\(\\?x || \\?w && \\?b\\) && \\?b")

add_repl_test(substitution_cmd_wff7 "m ?x && ?y. m 0. s ?x %-0 %-1" "error: argument has wrong type")
