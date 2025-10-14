cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# In these tests cases we check the results obtained in the substitution command.
# The output of the tests cases must be validated against expected results.

# BF Substitution

add_repl_test(substitution_cmd_bf0 "x. 1. s %[x / %1]" ": 1")

add_repl_test(substitution_cmd_bf1 "x. 1. s %2[x / %1]" ": 1")

add_repl_test(substitution_cmd_bf2 "ax + bx'. y&z. s %1[x / %2]" ": \\(ayz\\)'b\\(yz\\)'|ayz\\(b\\(yz\\)'\\)'")

add_repl_test(substitution_cmd_bf5 "(x | (w & z)) & z. b. s %1[ z / %2]" ": \\(x|wb\\)b")

add_repl_test_fail(substitution_cmd_bf6 "x & y. T. s %1[x / %2]" "Invalid argument")

# WFF Substitution

add_repl_test(substitution_cmd_wff1 "x=0. T. s %-1[x=0/ %]" ": T")

add_repl_test(substitution_cmd_wff2 "a=0 && x=0 ^^ b=0 && !x=0. y=0 && z=0. s %1[x=0 / %2]" ": !\\(a = 0 && y = 0 && z = 0\\) && b = 0 && !\\(y = 0 && z = 0\\) || !\\(b = 0 && !\\(y = 0 && z = 0\\)\\) && a = 0 && y = 0 && z = 0")

add_repl_test(substitution_cmd_wff3 "ex k h=0 && k=0. v=0 && w=0. s %1[k=0 / %2]" ": ex k h = 0 && k = 0")

add_repl_test(substitution_cmd_wff4 "ex x x=0 && y=0. x=0. s %1[y=0 / %2]" ": ex x2 x2 = 0 && x = 0")

add_repl_test(substitution_cmd_wff6 "(x=0 || (w=0 && z=0)) && z=0. b=0. s %1[z=0 / %2]" ": \\(x=0 || w=0 && b=0\\) && b=0")

add_repl_test_fail(substitution_cmd_wff7 "x=0 && y=0. 0. s %1[x=0 / %2]" "Argument has a wrong type")

add_repl_test(substitution_cmd_wff8 "s (ex x x=0 && y=0) && x=0 [y=0/x=0]" ": \\(ex x2 x2 = 0 && x = 0\\) && x = 0")

add_repl_test(substitution_cmd_wff9 "s (ex x x=0 && y=0) && x=0 [y=0/x=0 || a=0]" ": \\(ex x2 x2 = 0 && \\(x=0 || a = 0\\)\\) && x=0")

add_repl_test(substitution_cmd_wff10 "s (ex x x=0 && y=0 && all y y=0) && x=0 && y=0 [x=0 && y=0 / x=0 || a=0]"
        ": \\(ex x x=0 && y=0 && \\(all y y=0\\)\\) && \\(x=0 || a = 0\\)")

add_repl_test(substitution_cmd_wff11 "s ex x ex x x=0 && y=0 [y=0/x=0]" ": ex x2, x3 x3 = 0 && x = 0")

add_repl_test(substitution_cmd_wff12 "s (ex x x=0 && y=0) && x=0 [y/x]" ": \\(ex x2 x2 = 0 && x = 0\\) && x = 0")

add_repl_test_fail(substitution_cmd_wff13 "s (ex x x=0 && y=0) && x=0 [y/x=0]" "Invalid argument")

add_repl_test_fail(substitution_cmd_wff14 "s (ex x x=0 && y=0) && x=0 [y=0/x]" "Invalid argument")
