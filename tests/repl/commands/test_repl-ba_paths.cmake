#
# Paths of the Boolean-algebra plugins that the other files leave unreached:
# the nlang oracle against a local stub endpoint and nlang's structural
# folds, and the bitvector, qlt, hsb, qint and tau algebras on inputs that
# take their less common branches.
#

include(add_repl_test)
include(tau_repl_pack)

# nlang with no API key: the structural folds decide what they can, and every
# oracle question gets its conservative default.
set(_ba_paths_no_key TAU_NLANG_PROVIDER= TAU_NLANG_ENDPOINT= TAU_NLANG_MODEL=
	TAU_NLANG_API_KEY= OPENAI_API_KEY= ANTHROPIC_API_KEY= TAU_NLANG_EFFORT=)
add_repl_test(ba_paths-nlang_structural_folds_without_a_key
	"n {(nothing) or (it rains)}:nlang. n {(everything) or (it rains)}:nlang. n {(it rains) or (not (it rains))}:nlang. n {(it rains) and (not (it rains))}:nlang. n {(everything) and (it rains)}:nlang. n {it (rains}:nlang. n {it rains}:nlang ^ {it pours}:nlang"
	"{ \\(nothing\\) or \\(it rains\\) }:nlang.*: 1[\r\n].*: 0[\r\n].*{ \\(everything\\) and \\(it rains\\) }:nlang.*{ it \\(rains }:nlang.*{ \\(\\(it rains\\) or \\(it pours\\)\\) and \\(not \\(\\(it rains\\) and \\(it pours\\)\\)\\) }:nlang"
	ENV ${_ba_paths_no_key})
# the splitter's stronger statement falls back to the text it was given
add_repl_test(ba_paths-nlang_stronger_statement_without_a_key
	"solve x:nlang < {it rains}:nlang && x != 0"
	"x := { \\(it rains\\) and \\(\\(it rains\\) and \\(not \\(it rains and specifically so\\)\\)\\) }:nlang"
	ENV ${_ba_paths_no_key})

# nlang against tests/repl/stubs/nlang_llm/server.py, which serves one mode
# on a free loopback port and points tau at it. Its header lists the modes
# and the fixed world the oracle questions are answered from.
find_program(TAU_PYTHON3_PROGRAM python3)
if(NOT CMAKE_HOST_WIN32 AND TAU_PYTHON3_PROGRAM)
	set(_nlang_stub "${CMAKE_CURRENT_SOURCE_DIR}/../stubs/nlang_llm/server.py")
	# add_nlang_stub_test(<name> <mode> <flags> <cmds> <regex> [options...])
	function(add_nlang_stub_test name mode flags cmds regex)
		add_raw_repl_test(ba_paths-nlang_stub-${name}
			"\"${TAU_PYTHON3_PROGRAM}\" -I \"${_nlang_stub}\" ${mode} -- ${TAU_RUN} ${flags} -e \"${cmds}\""
			"${regex}" ${ARGN})
	endfunction()

	# decomposition, contradiction, tautology, an answer with no yes or no,
	# a disjunction with a negation, and braces that hold no JSON
	add_nlang_stub_test(openai_world openai ""
		"n {it rains and it pours}:nlang. n {a square circle exists}:nlang. n {it is always so}:nlang. n {i am unsure}:nlang. n {it rains or it pours}:nlang. n {mind the braces}:nlang"
		"{ \\(it rains\\) and \\(it pours\\) }:nlang.*: 0[\r\n].*: 1[\r\n].*{ i am unsure }:nlang.*{ \\(it rains\\) or \\(not \\(it pours\\)\\) }:nlang.*{ mind the braces }:nlang")
	add_nlang_stub_test(anthropic_world anthropic ""
		"n {it rains and it pours}:nlang. n {a square circle exists}:nlang. n {it is always so}:nlang"
		"{ \\(it rains\\) and \\(it pours\\) }:nlang.*: 0[\r\n].*: 1[\r\n]")
	# the splitter asks for a stronger statement once and trims it; a second
	# solve asks nothing again
	add_nlang_stub_test(stronger_statement_is_asked_once openai ""
		"solve x:nlang < {it rains}:nlang && x != 0. solve x:nlang < {it rains}:nlang && x != 0"
		"x := { \\(it rains\\) and \\(\\(it rains\\) and \\(not \\(it rains heavily\\)\\)\\) }:nlang"
		FAIL_REGEX "Error|stub: stronger it rains[\r\n].*stub: stronger it rains[\r\n]|stub: decompose it rains[\r\n].*stub: decompose it rains[\r\n]")
	# a stronger statement that trims to nothing falls back to the text
	add_nlang_stub_test(stronger_statement_of_dots_falls_back openai ""
		"solve x:nlang < {count the dots}:nlang && x != 0"
		"x := { \\(count the dots\\) and \\(\\(count the dots\\) and \\(not \\(count the dots and specifically so\\)\\)\\) }:nlang")
	# every reply that holds no answer leaves the constant undecomposed
	set(_undecomposed "Error|{ \\(it rains\\) and \\(it pours\\) }")
	add_nlang_stub_test(http_status_is_unanswered status:503 ""
		"n {it rains and it pours}:nlang"
		"endpoint returned HTTP 503"
		FAIL_REGEX "${_undecomposed}")
	add_nlang_stub_test(reply_not_json_is_unanswered badjson ""
		"n {it rains and it pours}:nlang"
		"holds no answer \\(the reply is not JSON\\)"
		FAIL_REGEX "${_undecomposed}")
	add_nlang_stub_test(reply_without_choices_is_unanswered nochoices ""
		"n {it rains and it pours}:nlang"
		"holds no answer \\(the reply has no choices\\)"
		FAIL_REGEX "${_undecomposed}")
	add_nlang_stub_test(error_without_message_is_unanswered typeerror ""
		"n {it rains and it pours}:nlang"
		"holds no answer \\(the endpoint reported an error\\)"
		FAIL_REGEX "${_undecomposed}")
	add_nlang_stub_test(refusal_is_unanswered refusal ""
		"n {it rains and it pours}:nlang"
		"holds no answer \\(the reply stopped with refusal\\)"
		FAIL_REGEX "${_undecomposed}")
	add_nlang_stub_test(timeout_is_unanswered slow "--nlang-http-timeout 1"
		"n {it rains and it pours}:nlang"
		"request failed \\(Timeout was reached\\)"
		FAIL_REGEX "${_undecomposed}" TIMEOUT 120)
endif()

# bv constants written with a 0x, 0X, 0b or 0B prefix read as #x and #b
add_repl_test(ba_paths-bv_literal_prefixes
	"n x:bv[8] = {0x1F}:bv[8]. n y:bv[8] = {0b101}:bv[8]. n z:bv[8] = {0X1f}:bv[8]. n w:bv[8] = {0B11}:bv[8]"
	"x = { 31 }:bv\\[8\\].*y = { 5 }:bv\\[8\\].*z = { 31 }:bv\\[8\\].*w = { 3 }:bv\\[8\\]")
# predicate blasting declines a shift or a division by a variable, so the
# atom keeps its operator
add_repl_test(ba_paths-bv_blasting_declines_a_shift_by_a_variable
	"n ex y:bv[8] (x:bv[8] << y) = { 4 }:bv[8]"
	"ex b1 x<<b1 = { 4 }:bv\\[8\\]"
	FLAGS --preprocessing true --bv-bitblast-max-width 0)
add_repl_test(ba_paths-bv_blasting_declines_a_division_by_a_variable
	"n ex y:bv[8] (x:bv[8] / y) = { 4 }:bv[8]"
	"ex b1 x/b1 = { 4 }:bv\\[8\\]"
	FLAGS --preprocessing true --bv-bitblast-max-width 0)

# qlt quantifier elimination with 0 and 1 as the ends of the order: no
# point is an end, so `0 < x` adds no bound and `x < 0` has no solution
add_repl_test(ba_paths-qlt_qelim_bounds_at_the_ends
	"qelim ex x:qlt (0 < x && x < y:qlt). qelim ex x:qlt (0 <= x && x <= {1}:qlt). qelim ex x:qlt (x < 0 && x < y:qlt). qelim ex x:qlt (1 <= x && x < y:qlt)"
	"%1[^\r\n]*: T[\r\n].*%1[^\r\n]*: T[\r\n].*%2[^\r\n]*: F[\r\n].*%2[^\r\n]*: F[\r\n]")

# qint endpoints: an exponent scales the mantissa, a value past the range
# of the representation does not parse, and an interval closes with `)`
add_repl_test(ba_paths-qint_exponent_endpoints
	"n x:qint = {[1.5e2, 2e2)}:qint" "x = { \\[150, 200\\) }:qint")
add_repl_test(ba_paths-qint_endpoint_out_of_range
	"n x:qint = {[1e30, 2e30)}:qint"
	"Unable to parse { \\[1e30, 2e30\\) } as a constant of type qint" NO_FAIL_REGEX)
add_repl_test(ba_paths-qint_closed_interval_is_a_syntax_error
	"n x:qint = {[1, 2]}:qint" "Syntax Error: Unexpected '\\]'" NO_FAIL_REGEX)

# a tau constant with its own recurrence relation joins and meets other
# constants: {f(y) := y = 0. ex x f(x).} is T
add_repl_test(ba_paths-tau_constant_with_a_definition
	"n {f(y) := y = 0. ex x f(x).}:tau & {ex z z = 0.}:tau. n {f(y) := y = 0. ex x f(x).}:tau | {all z z = 0.}:tau"
	"%1[^\r\n]*: 1[\r\n].*%1[^\r\n]*: 1[\r\n]")

# An indexed recurrence inside a tau constant reaches its base case, as it
# does at top level, and one without a base case ends instead of hanging.
add_repl_test(ba_paths-tau_constant_indexed_recurrence
	"n {g[n](y) := g[n-1](y). g[0](y) := y = 0. ex x g[1](x).}:tau"
	"%1[^\r\n]*: 1" NO_TRACE TIMEOUT 60)
add_repl_test(ba_paths-tau_constant_indexed_recurrence_without_base_case
	"n {g[n](y) := g[n-1](y). g[2](x).}:tau"
	"semantic error" NO_TRACE NO_FAIL_REGEX TIMEOUT 60)

# Between several bounds with one side strict the interval is empty or
# infinite, so an excluded point does not keep the quantifier; with every
# bound closed the single-point case keeps it.
add_repl_test(ba_paths-qlt_qelim_several_lower_bounds
	"qelim ex x:qlt (y:qlt < x && w:qlt < x && x < z:qlt && x != {1}:qlt)"
	"%1[^\r\n]*: y < z && w < z" NO_TRACE FAIL_REGEX "ex b")
add_repl_test(ba_paths-qlt_qelim_closed_bounds_keep_the_point
	"qelim ex x:qlt (y:qlt <= x && w:qlt <= x && x <= z:qlt && x != {1}:qlt)"
	"%1[^\r\n]*: ex b1 " NO_TRACE)
# a width bv does not take is named as such, a missing one as missing
add_repl_test(ba_paths-bv_width_out_of_range
	"sat ex x:bv[0] x = 0"
	"Incomplete type :bv\\[0\\] .*The parameter is not one the type takes" NO_FAIL_REGEX)
add_repl_test(ba_paths-bv_width_missing
	"sat ex x:bv x = 0"
	"Incomplete type :bv .*The type needs a parameter" NO_FAIL_REGEX)
# an hsb constant prints with the digits of its exact value; the two
# conjuncts may print in either order
add_repl_test(ba_paths-hsb_constant_prints_every_digit
	"n o1[t]:hsb = {x[0] < 1.0000001}:hsb && o2[t]:hsb = {x[0] < 1.0000002}:hsb"
	"x\\[0\\] - 1\\.0000001 < 0 .*x\\[0\\] - 1\\.0000002 < 0|x\\[0\\] - 1\\.0000002 < 0 .*x\\[0\\] - 1\\.0000001 < 0" NO_TRACE)
