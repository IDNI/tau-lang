cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# In these tests cases we check the results obtained in the substitution command.
# The output of the tests cases must be validated against expected results.

# BF Substitution

add_repl_test(substitution_cmd_bf1 "x. 1. s %[x / %1]" ": 1")

add_repl_test(substitution_cmd_bf2 "ax + bx'. y&z. s %1[x / %2]" ": \\(a & y & z\\) \\+ \\(b & \\(y & z\\)'\\)")

add_repl_test(substitution_cmd_bf5 "(x | (w & z)) & z. b. s %1[ z / %2]" ": \\(x | w & b\\) & b")

add_repl_test(substitution_cmd_bf6 "x & y. T. s %1[x / %2]" "error: invalid argument")

# WFF Substitution

add_repl_test(substitution_cmd_wff1 "?x. T. s %-1[?x/ %]" ": T")

add_repl_test(substitution_cmd_wff2 "?a && ?x ^ ?b && !?x. ?y && ?z. s %1[?x / %2]" ": \\(\\?a && \\?y && \\?z\\) \\^ \\(\\?b && !\\(\\?y && \\?z\\)\\)")

add_repl_test(substitution_cmd_wff3 "b_ex ?k ?h && ?k. ?v && ?w. s %1[?k / %2]" ": b_ex \\?k \\?h && \\?k")

add_repl_test(substitution_cmd_wff4 "ex x x=0 && y=0. ?x. s %1[?y / %2]" ": ex x x = 0 && y = 0")

add_repl_test(substitution_cmd_wff5 "b_ex ?x ?x && ?y. ?x. s %1[?y / %2]" ": b_ex \\?x2 \\?x2 && \\?x")

add_repl_test(substitution_cmd_wff6 "(?x || (?w && ?z)) && ?z. ?b. s %1[?z / %2]" ": \\(\\?x || \\?w && \\?b\\) && \\?b")

add_repl_test(substitution_cmd_wff7 "?x && ?y. 0. s %1[?x / %2]" "error: argument has wrong type")

add_repl_test(substitution_cmd_wff8 "s (ex x x=0 && y=0) && x=0 [y=0/x=0]" ": \\(ex x2 x2 = 0 && x = 0\\) && x = 0")

add_repl_test(substitution_cmd_wff9 "s (b_ex ?x ?x && ?y) && ?x [?y/?x || a=0]" ": \\(b_ex \\?x2 \\?x2 && \\(\\?x || a = 0\\)\\) && \\?x")

add_repl_test(substitution_cmd_wff10 "s (b_ex ?x ?x && ?y && b_all ?y ?y) && ?x && ?y [?x && ?y /?x || a=0]"
        ": \\(b_ex \\?x \\?x && \\?y && \\(b_all \\?y \\?y\\)\\) && \\(\\?x || a = 0\\)")

add_repl_test(substitution_cmd_wff11 "s b_ex ?x b_ex ?x ?x && ?y [?y/?x]" ": b_ex \\?x2 b_ex \\?x3 \\?x3 && \\?x")

add_repl_test(substitution_cmd_wff12 "s (ex x x=0 && y=0) && x=0 [y/x]" ": \\(ex x2 x2 = 0 && x = 0\\) && x = 0")

add_repl_test(substitution_cmd_wff13 "s (ex x x=0 && y=0) && x=0 [y/?x]" "error: invalid argument")

add_repl_test(substitution_cmd_wff14 "s (ex x x=0 && y=0) && x=0 [?y/x]" "error: invalid argument")
