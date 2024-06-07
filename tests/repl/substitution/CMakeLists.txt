cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# In these tests cases we check the results obtained in the substitution command.
# The output of the tests cases must be validated against expected results.

# BF Substitution

add_repl_test(substitution_cmd_bf1 "m x. m 1. s %-0[x / %-1]" ": 1")

add_repl_test(substitution_cmd_bf2 "m ax + bx'. m y&z. s %-0[x / %-1]" ": \\(a & y & z\\) \\+ \\(b & \\(y & z\\)'\\)")

add_repl_test(substitution_cmd_bf5 "m (x | (w & z)) & z. m b. s %-0[ z / %-1]" ": \\(x | w & b\\) & b")

add_repl_test(substitution_cmd_bf6 "m x & y. m T. s %-0[x / %-1]" "error: invalid argument")

# WFF Substitution

add_repl_test(substitution_cmd_wff1 "m ?x && ?y. m T. s %-0[?x/ %-1]" ": T && \\?y")

add_repl_test(substitution_cmd_wff2 "m ?a && ?x ^ ?b && !?x. m ?y && ?z. s %-0[?x / %-1]" ": \\(\\?a && \\?y && \\?z\\) \\^ \\(\\?b && !\\(\\?y && \\?z\\)\\)")

add_repl_test(substitution_cmd_wff3 "m bool_ex ?k ?h && ?k. m ?v && ?w. s %-0[?k / %-1]" ": bool_ex \\?k \\?h && \\?k")

add_repl_test(substitution_cmd_wff4 "m ex x x=0 && y=0. m ?x. s %-0[?y / %-1]" ": ex x x = 0 && y = 0")

add_repl_test(substitution_cmd_wff5 "m bool_ex ?x ?x && ?y. m ?x. s %-0[?y / %-1]" ": bool_ex \\?x2 \\?x2 && \\?x")

add_repl_test(substitution_cmd_wff6 "m (?x || (?w && ?z)) && ?z. m ?b. s %-0[?z / %-1]" ": \\(\\?x || \\?w && \\?b\\) && \\?b")

add_repl_test(substitution_cmd_wff7 "m ?x && ?y. m 0. s %-0[?x / %-1]" "error: argument has wrong type")

add_repl_test(substitution_cmd_wff8 "s (ex x x=0 && y=0) && x=0 [y=0/x=0]" ": \\(ex x2 x2 = 0 && x = 0\\) && x = 0")

add_repl_test(substitution_cmd_wff9 "s (bool_ex ?x ?x && ?y) && ?x [?y/?x || a=0]" ": \\(bool_ex \\?x2 \\?x2 && \\(\\?x || a = 0\\)\\) && \\?x")

add_repl_test(substitution_cmd_wff10 "s (bool_ex ?x ?x && ?y && bool_all ?y ?y) && ?x && ?y [?x && ?y /?x || a=0]"
        ": \\(bool_ex \\?x \\?x && \\?y && \\(bool_all \\?y \\?y\\)\\) && \\(\\?x || a = 0\\)")

add_repl_test(substitution_cmd_wff11 "s bool_ex ?x bool_ex ?x ?x && ?y [?y/?x]" ": bool_ex \\?x2 bool_ex \\?x3 \\?x3 && \\?x")

add_repl_test(substitution_cmd_wff12 "s (ex x x=0 && y=0) && x=0 [y/x]" ": \\(ex x2 x2 = 0 && x = 0\\) && x = 0")

add_repl_test(substitution_cmd_wff13 "s (ex x x=0 && y=0) && x=0 [y/?x]" "error: invalid argument")

add_repl_test(substitution_cmd_wff14 "s (ex x x=0 && y=0) && x=0 [?y/x]" "error: invalid argument")
