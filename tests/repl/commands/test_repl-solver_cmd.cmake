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
