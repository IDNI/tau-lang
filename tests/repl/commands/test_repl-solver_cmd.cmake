#
# solve command
#

include(add_repl_test)

# AP-35: this file previously existed but was never include()'d from
# CMakeLists.txt, so it was never actually run. Once wired in, its
# PASS_REGULAR_EXPRESSION turned out to be broken too: CMake's
# PASS_REGULAR_EXPRESSION is a regular expression, and `{`/`}` and `[`/`]`
# are regex metacharacters (interval and bracket-expression delimiters
# respectively) -- `[16]` in particular is a bracket expression matching a
# single '1' or '6' character, not the literal substring "[16]" -- so the
# unescaped pattern could never match the literal solver output. Escaping
# them fixes the match.

# solve cvc5 sample with mod
add_repl_test(solver_cvc5_cmd "solve x : bv[16] % { 2 } = { 1 }"
	"x := \\{ 1 \\}:bv\\[16\\]")

# issue-60: a solution whose value is the constant 0 or 1 is printed by
# print_solver_cmd_solution's zero/one branch, which needs the BA type of the
# solved variable. solve_cmd used to hand it the grammar nonterminal returned
# by get_type_and_arg instead, so every such solve aborted with
# `ba_types::type_tree: invalid ba_type_id <nt>`. bv equations were unaffected
# because their solutions take the general branch, which ignores the type.

# the constant-solution cases that used to abort
add_repl_test(solver_cmd-sbf_one "solve x:sbf = 1" "x := \\{ 1 \\}:sbf")
add_repl_test(solver_cmd-sbf_zero "solve x:sbf = 0" "x := \\{ 0 \\}:sbf")

# same, with the variable left untyped: the BA type falls back to
# node::ba::default_type(), which is `tau` for the REPL's BA pack
add_repl_test(solver_cmd-untyped_one "solve x = 1" "x := \\{ T \\}:tau")

# issue-60 as reported: through a user-defined function
add_repl_test(solver_cmd-sbf_through_def "f(x:sbf) := x:sbf. solve f(x:sbf) = 1"
	"x := \\{ 1 \\}:sbf")

# issue-60, bv half: solving through a user-defined function, both directions
add_repl_test(solver_cmd-bv_through_def
	"g(y:bv[16]) := y:bv[16]. solve g(y:bv[16]) = { 2 }:bv[16]"
	"y := \\{ 2 \\}:bv\\[16\\]")
add_repl_test(solver_cmd-bv_through_def_rev
	"g(y:bv[16]) := y:bv[16]. solve g({ 2 }:bv[16]) = y:bv[16]"
	"y := \\{ 2 \\}:bv\\[16\\]")
