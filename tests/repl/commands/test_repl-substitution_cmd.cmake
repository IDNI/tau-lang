cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# In these tests cases we check the results obtained in the substitution command.
# The output of the tests cases must be validated against expected results.

# BF Substitution

add_repl_test(substitution_cmd_bf0 "x. 1. s %[x / %1]" ": 1")

add_repl_test(substitution_cmd_bf1 "x. 1. s %2[x / %1]" ": 1")

add_repl_test(substitution_cmd_bf2 "ax ^ bx'. y&z. s %1[x / %2]" ": ayz\\^b&\\(yz\\)'")

add_repl_test(substitution_cmd_bf5 "(x | (w & z)) & z. b. s %1[ z / %2]" ": \\(x|wb\\)b")

add_repl_test_fail(substitution_cmd_bf6 "x & y. T. s %1[x / %2]" "Invalid argument")

# WFF Substitution

add_repl_test(substitution_cmd_wff1 "x=0. T. s %-1[x=0/ %]" ": T")

add_repl_test(substitution_cmd_wff2 "a=0 && x=0 ^^ b=0 && !x=0. y=0 && z=0. s %1[x=0 / %2]" ": !\\(a = 0 && y = 0 && z = 0\\) && b = 0 && !\\(y = 0 && z = 0\\) || !\\(b = 0 && !\\(y = 0 && z = 0\\)\\) && a = 0 && y = 0 && z = 0")

add_repl_test(substitution_cmd_wff3 "ex k h=0 && k=0. v=0 && w=0. s %1[k=0 / %2]" ": ex b1 h = 0 && b1 = 0")

add_repl_test(substitution_cmd_wff4 "ex x x=0 && y=0. x=0. s %1[y=0 / %2]" ": ex b1 b1 = 0 && x = 0")

add_repl_test(substitution_cmd_wff6 "(x=0 || (w=0 && z=0)) && z=0. b=0. s %1[z=0 / %2]" ": \\(x=0 || w=0 && b=0\\) && b=0")

add_repl_test_fail(substitution_cmd_wff7 "x=0 && y=0. 0. s %1[x=0 / %2]" "Argument has a wrong type")

add_repl_test(substitution_cmd_wff8 "s (ex x x=0 && y=0) && x=0 [y=0/x=0]" ": \\(ex b1 b1 = 0 && x = 0\\) && x = 0")

add_repl_test(substitution_cmd_wff9 "s (ex x x=0 && y=0) && x=0 [y=0/x=0 || a=0]" ": \\(ex x2 x2 = 0 && \\(x=0 || a = 0\\)\\) && x=0")

add_repl_test(substitution_cmd_wff10 "s (ex x x=0 && y=0 && all y y=0) && x=0 && y=0 [x=0 && y=0 / x=0 || a=0]"
        ": \\(ex x x=0 && y=0 && \\(all y y=0\\)\\) && \\(x=0 || a = 0\\)")

add_repl_test(substitution_cmd_wff11 "s ex x ex x x=0 && y=0 [y=0/x=0]" ": ex b2, b1 b1 = 0 && x = 0")

add_repl_test(substitution_cmd_wff12 "s (ex x x=0 && y=0) && x=0 [y/x]" ": \\(ex b1 b1 = 0 && x = 0\\) && x = 0")

add_repl_test_fail(substitution_cmd_wff13 "s (ex x x=0 && y=0) && x=0 [y/x=0]" "Invalid argument")

add_repl_test_fail(substitution_cmd_wff14 "s (ex x x=0 && y=0) && x=0 [y=0/x]" "Invalid argument")

# Substituting into the result of another command
#
# subst_cmd resolves its arguments but never inferred their BA types, while a
# history entry produced by dnf/cnf/normalize has already been through
# inference and so carries resolved type ids on its nodes. Matching is
# type-id sensitive, so a pattern parsed from the command line -- which had no
# type id at all -- could not match anything in such an entry: the command
# returned its input unchanged, at the same history position, with no error.
# Writing the type out by hand (`X:tau`) was the only way to make it match.
#
# Every case below substitutes into a command's result rather than into a
# directly stored expression.
#
# The third argument is a REGEX, so every `|` in an expected value has to be
# escaped. Unescaped it reads as alternation, and since the buggy output
# differs from the expected one only in the operand left of the `|`, a bare
# ": WZ|ZY" matches "XZ|ZY" through its "ZY" branch -- the case then passes
# with the fix backed out and pins nothing.

add_repl_test(substitution_cmd-into_dnf_result
	"(X | Y) & Z. dnf %. s % [X / W]" ": WZ\\|ZY")

add_repl_test(substitution_cmd-into_cnf_result
	"(X Y) | Z. cnf %. s % [X / W]" ": \\(W\\|Z\\)&\\(Z\\|Y\\)")

add_repl_test(substitution_cmd-into_normalize_result
	"X Z | Z Y. n %. s % [X / W]" ": WZ\\|YZ")

# The same applies to a wff result, and to a wff pattern
add_repl_test(substitution_cmd-into_nnf_result
	"nnf !(X = 0 && Y = 0). s % [X != 0 / W != 0]" ": W != 0 \\|\\| Y != 0")

# inst_cmd delegates to subst_cmd, so it was affected identically. Note that
# substitution does not simplify, hence Z|ZY rather than Z.
add_repl_test(substitution_cmd-inst_into_dnf_result
	"(X | Y) & Z. dnf %. i % [X / 1]" ": Z\\|ZY")

# The three below are controls: each already passed before the fix and has to
# keep passing, so unlike the five above they do not fail when it is backed out.
#
# Naming the type explicitly was the documented workaround, and must keep working
add_repl_test(substitution_cmd-into_dnf_result_typed_pattern
	"(X | Y) & Z. dnf %. s % [X:tau / W]" ": WZ\\|ZY")

# A result whose type is not the default one, with and without the type named in
# the pattern. Both forms already matched -- inference resolves an untyped
# pattern against the scope the earlier command left behind -- which is what
# makes the default-typed cases above the ones that actually exercised the bug.
add_repl_test(substitution_cmd-into_result_non_default_type
	"n x:sbf | y:sbf. s % [x / z]" ": z\\|y")
add_repl_test(substitution_cmd-into_result_non_default_type_typed_pattern
	"n x:sbf | y:sbf. s % [x:sbf / z]" ": z\\|y")
