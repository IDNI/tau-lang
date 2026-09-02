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

# Multiple substitution (issue #99)
#
# Several match/replace pairs, comma separated, in one bracket. All pairs are
# applied SIMULTANEOUSLY in a single pass over the input: every match is found
# against the original expression and no pair's replacement is ever re-matched
# by another pair.

# The issue's motivating example: three replacements at once
add_repl_test(substitution_cmd_multi_bf0
	"s a | b | c | d [a / x, b / y, c / z]" ": x\\|y\\|z\\|d")

# Two pairs, no whitespace around the comma or slashes
add_repl_test(substitution_cmd_multi_bf1 "s x & y [x/a,y/b]" ": ab")

# Simultaneity: a swap exchanges the variables instead of collapsing them
add_repl_test(substitution_cmd_multi_swap "s x & y [x / y, y / x]" ": yx")

# No chaining: the b introduced by the first pair is not rewritten to d
add_repl_test(substitution_cmd_multi_nochain
	"s a | c [a / b, b / d]" ": b\\|c")

# Overlapping patterns: the outermost match wins, the x inside the matched
# x | y is never visited
add_repl_test(substitution_cmd_multi_outermost
	"s (x | y) & z [x | y / w, x / v]" ": wz")

# History references work as pair operands
add_repl_test(substitution_cmd_multi_history
	"a. b. s x | y [x / %1, y / %2]" ": a\\|b")

# WFF input with two wff pairs
add_repl_test(substitution_cmd_multi_wff0
	"s x = 0 && y = 0 [x = 0 / a = 0, y = 0 / b = 0]" ": a = 0 && b = 0")

# WFF input mixing a wff pair and a bf pair in the same command
add_repl_test(substitution_cmd_multi_mixed
	"s x = 0 && a != 0 [x = 0 / y = 0, a / b]" ": y = 0 && b != 0")

# A repeated match pattern is rejected instead of silently picking one
add_repl_test_fail(substitution_cmd_multi_duplicate
	"s x | y [x / a, x / b]" "[Dd]uplicate")

# A type error in any pair rejects the whole command
add_repl_test_fail(substitution_cmd_multi_badtype
	"s x = 0 && y = 0 [x = 0 / a = 0, y = 0 / b]" "Invalid argument")

# A bf input cannot take a wff pair, even as a later pair
add_repl_test_fail(substitution_cmd_multi_bf_wffpair
	"s x | y [x / a, y = 0 / b = 0]" "wrong type")

# Type safety
#
# A substitution whose result no longer type-checks is rejected at subst time
# instead of storing an ill-typed expression in the history that every later
# inference-running command (n, sat, ...) would choke on. Untyped expressions
# carry the default type (tau) and unannotated replace operands are resolved
# by inference against their context, so only genuinely conflicting
# annotations are rejected.

# Replacing an sbf subterm by a bv one is rejected
add_repl_test_fail(substitution_cmd_type_mismatch_sbf_bv
	"s x:sbf & y:sbf = 0 [x:sbf / z:bv[16]]" "not well-typed")

# Mismatched bitvector widths are rejected
add_repl_test_fail(substitution_cmd_type_mismatch_bv_widths
	"s x:bv[8] + y:bv[8] = 0 [x:bv[8] / z:bv[16]]" "not well-typed")

# An untyped input carries the default type, so a bv replacement conflicts
add_repl_test_fail(substitution_cmd_type_mismatch_untyped_bv
	"s x & y [x / z:bv[16]]" "not well-typed")

# In a multi-pair command a single mismatching pair rejects the whole command
add_repl_test_fail(substitution_cmd_type_mismatch_multi
	"s x:bv[8] + y:bv[8] = 0 [x:bv[8] / z:bv[8], y:bv[8] / w:bv[16]]"
	"not well-typed")

# inst delegates to subst and is protected identically
add_repl_test_fail(substitution_cmd_type_mismatch_inst
	"i x:bv[8] + y:bv[8] = 0 [x:bv[8] / z:bv[16]]" "not well-typed")

# Controls: well-typed substitutions keep working
add_repl_test(substitution_cmd_type_ok_sbf
	"s x:sbf & y:sbf = 0 [x:sbf / z:sbf]" ": zy = 0")
add_repl_test(substitution_cmd_type_ok_bv
	"s x:bv[8] + y:bv[8] = 0 [x:bv[8] / z:bv[8]]" ": z\\+y = 0")
# an unannotated replacement adopts the matched context's type
add_repl_test(substitution_cmd_type_ok_untyped_replace
	"s x:sbf & y:sbf = 0 [x:sbf / z]" ": zy = 0")

# No silent no-op
#
# A pair whose pattern does not occur in the input is reported with a warning
# instead of silently leaving the input unchanged. The command still returns
# the (partially) substituted input, so history-driven flows keep working
# (see substitution_cmd_bf0/bf1 above, which substitute into an expression
# their pattern does not occur in).

add_repl_test(substitution_cmd_no_match_warns
	"s x & y [w / z]" "did not match")
add_repl_test(substitution_cmd_no_match_warns_multi
	"s x & y [x / a, w / z]" "did not match")
# ...and the pairs that do match are still applied
add_repl_test(substitution_cmd_no_match_multi_applies
	"s x & y [x / a, w / z]" ": ay")
# a pattern annotated with a type conflicting with the input's use of the
# same variable never reaches substitution: the command line is inferred as
# one tree first and rejected loudly there (not a silent no-op)
add_repl_test_fail(substitution_cmd_no_match_wrong_type
	"s x:sbf & y:sbf = 0 [x:bv[8] / z:bv[8]]" "Incompatible type")

# An input that cannot be type-inferred at all (bitvector arithmetic without
# width annotations) is matched as parsed, so unannotated patterns substitute
# in it instead of silently missing on inferred-vs-raw type ids
add_repl_test(substitution_cmd_untyped_arith
	"s a + b = c [a / d]" ": d\\+b = c")
add_repl_test(substitution_cmd_untyped_arith_multi
	"s a + b = c [a / d, b / e]" ": d\\+e = c")

# Multiple bracket groups (sequential composition)
#
# Several '[...]' groups may follow the input; each group is applied to the
# result of the previous one, while the pairs INSIDE a group stay
# simultaneous. So '[a / b] [b / d]' chains (the b introduced by the first
# group IS rewritten by the second) whereas '[a / b, b / d]' does not.

add_repl_test(substitution_cmd_groups_chain
	"s a | c [a / b] [b / d]" ": d\\|c")

# a group with several (simultaneous) pairs composes with a later group
add_repl_test(substitution_cmd_groups_mixed
	"s a | b [a / x, b / y] [x / z]" ": z\\|y")

# wff groups chain too
add_repl_test(substitution_cmd_groups_wff
	"s x = 0 && y = 0 [x = 0 / a = 0] [a = 0 && y = 0 / b = 0]" ": b = 0")

# the same pattern may appear in different groups (that is what chaining is
# for), but a duplicate inside one group is still an error
add_repl_test(substitution_cmd_groups_repeat_across
	"s a | c [a / b] [a / d]" ": b\\|c")
add_repl_test_fail(substitution_cmd_groups_dup_within
	"s x | y [x / a, x / b] [y / c]" "[Dd]uplicate")

# a later group matches against the intermediate result, so a pattern the
# earlier group consumed is reported as unmatched
add_repl_test(substitution_cmd_groups_stage_warn
	"s a | c [a / b] [a / d]" "did not match")

# type safety applies at every stage
add_repl_test_fail(substitution_cmd_groups_type
	"s x:sbf & y:sbf = 0 [x:sbf / z] [z / w:bv[8]]" "not well-typed")

# a fully unannotated pattern matches modulo types, so a later group can
# rewrite the z the first group introduced into an sbf context even though a
# command line pattern never carries the inferred sbf type id
add_repl_test(substitution_cmd_groups_untyped_pattern_chain
	"s x:sbf & y:sbf = 0 [x:sbf / z] [z / w]" ": wy = 0")
