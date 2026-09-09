cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# In these tests cases we check the results obtained in the instantiation command.
# The output of the tests cases must be validated against expected results.

# BF Instantiation

add_repl_test(instantiation_cmd_bf1 "x. 1. i %[x / %1]" ": 1")

add_repl_test(instantiation_cmd_bf2 "ax ^ bx'. y&z. i %1[x / %2]" ": ayz\\^b&\\(yz\\)'")

add_repl_test(instantiation_cmd_bf3 "(x | (w & z)) & z. b. i %1[ z / %2]" ": \\(x|wb\\)b")

add_repl_test_fail(instantiation_cmd_bf4 "x & y. T. i %1[x / %2]" "Invalid argument")

# WFF Instantiation

add_repl_test(instantiation_cmd_wff1 "x=0. 1. i %-1[x/ %]" ": F")

add_repl_test(instantiation_cmd_wff2 "a=0 && x=0^^ b=0 && !x=0. y & z. i %1[x/ %2]" ": !\\(a = 0 && y & z = 0\\) && b = 0 && !y & z = 0 || !\\(b = 0 && !y & z = 0\\) && a = 0 && y & z = 0")

add_repl_test(instantiation_cmd_wff3 "ex k h=0 && k=0. v & w. i %1[k / %2]" ": ex b1 h = 0 && b1 = 0")

add_repl_test(instantiation_cmd_wff4 "ex x x=0 && y=0. x. i %1[y / %2]" ": ex b1 b1 = 0 && x = 0")

add_repl_test(instantiation_cmd_wff5 "ex x x=0 && y=0. x. i %1[y / %2]" ": ex b1 b1 = 0 && x = 0")

add_repl_test(instantiation_cmd_wff6 "(x=0|| (w=0 && z=0)) && z=0. b. i %1[z / %2]" ": \\(x = 0 || w = 0 && b = 0\\) && b = 0")

add_repl_test_fail(instantiation_cmd_wff7 "x=0&& y=0. x=0. i %1[x/ %2]" "Argument has a wrong type")

add_repl_test(instantiation_cmd_wff8 "i (ex x x=0 && y=0) && x=0[y/x|a]" ": \\(ex b1 b1 = 0 && x|a = 0\\) && x = 0")

add_repl_test(instantiation_cmd_wff9 "i ex x ex x x=0 && y=0 [y/x]" ": ex b2, b1 b1 = 0 && x = 0")

add_repl_test(instantiation_cmd_wff10 "i (ex x x=0 && y=0) && x=0 [y/x]" ": \\(ex b1 b1 = 0 && x = 0\\) && x = 0")

# substituting for something that is not a variable is rejected
add_repl_test_fail(instantiation_cmd-non_variable "x. i %1[1 / x]" "Invalid argument")

# Multiple instantiation (issue #99, mirroring subst)
#
# Several comma separated variable/value pairs in one bracket are applied
# SIMULTANEOUSLY; several bracket groups compose sequentially, each applied
# to the previous group's result.

# two pairs at once
add_repl_test(instantiation_cmd_multi "i x & y [x / a, y / b]" ": ab")

# simultaneity: a swap exchanges the variables instead of collapsing them
add_repl_test(instantiation_cmd_multi_swap "i x & y [x / y, y / x]" ": yx")

# wff input with two pairs
add_repl_test(instantiation_cmd_multi_wff
	"i x = 0 && y = 0 [x / 1, y / 0]" ": F")

# groups chain: the b introduced by the first group is rewritten to d
add_repl_test(instantiation_cmd_groups_chain "i a | c [a / b] [b / d]" ": d\\|c")

# every pair's match side must be a variable, in any pair of any group
add_repl_test_fail(instantiation_cmd_multi_non_variable
	"i x | y [x / 1, x & y / 0]" "Invalid argument")
add_repl_test_fail(instantiation_cmd_groups_non_variable
	"i x | y [x / 1] [x & y / 0]" "Invalid argument")

# a duplicate variable inside one group is an error
add_repl_test_fail(instantiation_cmd_multi_dup
	"i x | y [x / 1, x / 0]" "[Dd]uplicate")
