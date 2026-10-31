#
# BA constant syntaxes through the REPL surface (BA1-30)
#

include(add_repl_test)

# qlt: satisfiability verdicts and the interval printed form
add_repl_test(ba_qlt-sat_neq "sat x : qlt != {0}:qlt" ": T")
add_repl_test(ba_qlt-unsat "sat x : qlt = {0}:qlt && x : qlt != {0}:qlt" ": F")
add_repl_test(ba_qlt-interval_print "n {(0, 1)}:qlt x = 0" "\\(0, 1\\)")

# qint
add_repl_test(ba_qint-sat "sat x : qint != {0}:qint" ": T")
# GitHub #186: endpoints are exact rationals, so 1/3 and its 16-digit
# decimal rounding are distinct.
add_repl_test(ba_qint-exact_endpoints_differ
	"normalize {[0,1/3)}:qint = {[0,0.3333333333333333)}:qint" ": F")
add_repl_test(ba_qint-exact_gap_is_nonempty
	"normalize ({[0,1/3)}:qint & {[0.3333333333333333,1)}:qint) = 0" ": F")
add_repl_test(ba_qint-exact_witness
	"normalize ex x:qint ((x = {[0,1/3)}:qint) && (x != {[0,0.3333333333333333)}:qint))" ": T")
add_repl_test(ba_qint-exact_print "n {[0,1/3)}:qint x = 0" "\\[0, 1/3\\)")

# An endpoint prints as text that reads back, and a literal is refused only
# when its value does not fit.
add_repl_test(ba_qint-tiny_endpoint_prints_its_fraction
	"n { [0, 3/4611686018427387904) }:qint" "\\[0, 3/4611686018427387904\\)")
add_repl_test(ba_qint-long_decimal_reads_back
	"n { [0, 0.00000000000000088817841970012523233890533447265625) }:qint"
	"\\[0, 0.00000000000000088817841970012523233890533447265625\\)")
add_repl_test(ba_qint-trailing_zeros_fit
	"n { [0, 1.0000000000000000000000000000000000000000) }:qint" "\\[0, 1\\)")
# The witness needs an endpoint past 64 bits: sat answers T, so solve must not
# claim there is none.
add_repl_test(ba_qint-solve_past_the_endpoints_is_unknown
	"solve x:qint != 0 && (x & {[0, 1/9223372036854775807)}:qint') = 0 && x != {[0, 1/9223372036854775807)}:qint"
	"UNKNOWN: no splitter" FAIL_REGEX "no solution")

# A printed union reads back whichever piece is a singleton.
add_repl_test(ba_qlt-singleton_leads_a_union
	"n { 0 | (1, 2) }:qlt" "\\{ 0 \\| \\(1, 2\\) \\}:qlt")
add_repl_test(ba_qlt-name_leads_a_union
	"n { c | (1, 2) }:qlt" "\\{ \\(1, 2\\) \\| c \\}:qlt")
add_repl_test(ba_qlt-leading_top_is_the_keyword "n { top | (1, 2) }:qlt" ": 1")
add_repl_test(ba_qlt-leading_bot_is_the_keyword
	"n { bot | (1, 2) }:qlt" "\\{ \\(1, 2\\) \\}:qlt")

# `all` over a bound by another variable: the interval of the constants is
# full and says nothing.
add_repl_test(ba_qlt-all_above_a_variable "n all y:qlt x:qlt < y" ": F")
add_repl_test(ba_qlt-all_equal_to_a_variable "n all y:qlt (y:qlt = x:qlt)" ": F")
add_repl_test(ba_qlt-all_other_than_a_variable "n all y:qlt (y:qlt != x:qlt)" ": F")
add_repl_test(ba_qlt-all_sat_over_a_stream "sat all y:qlt (y:qlt = o1[t]:qlt)" ": F")
add_repl_test(ba_qlt-all_of_the_order_law "n all y:qlt (x:qlt < y || y <= x:qlt)" ": T")
add_repl_test(ba_qlt-all_keeps_a_free_conjunct
	"n all y:qlt (x:qlt < {0}:qlt)" "x < \\{ 0 \\}:qlt")
add_repl_test(ba_qlt-data_game_capped_keeps_its_verdict
	"realizable F(i1[t-1]:qlt < i1[t]:qlt)" ": F" FLAGS --ltl-data-game-max-nodes 8)

# A comparison over names stays open unless it has one truth wherever the
# names lie, and an operation that depends on them stays a term.
add_repl_test(ba_qlt-names_may_coincide "n {c}:qlt = {d}:qlt" "= 0"
	FAIL_REGEX "%[0-9]+: [TF]" NO_TRACE)
add_repl_test(ba_qlt-names_may_differ "n {c}:qlt != {d}:qlt" "!= 0"
	FAIL_REGEX "%[0-9]+: [TF]" NO_TRACE)
add_repl_test(ba_qlt-xor_of_names_stays_a_term
	"n {c}:qlt ^ {d}:qlt" "\\(-inf, d\\) \\| \\(d, \\+inf\\)" NO_TRACE)
add_repl_test(ba_qlt-complement_of_names_stays_a_term
	"n ({[c, d]}:qlt' & {[0, 1]}:qlt) = {[0, 1]}:qlt" "\\[c, d\\] \\}:qlt'"
	FAIL_REGEX "%[0-9]+: [TF]" NO_TRACE)
add_repl_test(ba_qlt-all_over_a_complement_of_names_stays
	"n all x (({[c, d]}:qlt' & x:qlt) != 0)" "\\[c, d\\] \\}:qlt'"
	FAIL_REGEX "%[0-9]+: [TF]" NO_TRACE)
add_repl_test(ba_qlt-names_keep_a_law "n {c}:qlt < {d}:qlt || {d}:qlt <= {c}:qlt" ": T")

# A constant covering all of Q is the typed 1, the upper end of the order.
add_repl_test(ba_qlt-full_constant_is_the_upper_end
	"n {[0,1]}:qlt < {(-inf,+inf)}:qlt" ": T")
add_repl_test(ba_qlt-full_constant_of_two_pieces_is_the_upper_end
	"n all x (x:qlt <= {(-inf, 5) | [3, +inf)}:qlt)" ": T")

# hsb
add_repl_test(ba_hsb-sat "sat x : hsb != 0" ": T")
# The negation of a normalized constant is a product of its cells.
add_repl_test(ba_hsb-fold_of_two_constants
	"n { ((2*x[0] + 2*x[1] - 1/2 < 0 & -2*x[0] + 2*x[1] <= 0) | (-x[1] + 3/2 <= 0 | x[0] - x[1] + 1 < 0)) }:hsb & { ((-2*x[0] - x[1] - 1 <= 0 & x[0] + 1/2 < 0) & (-x[1] + 3 <= 0 | x[1] - 2 < 0)) }:hsb"
	"& x\\[0\\] \\+ 0.5 < 0" FAIL_REGEX "UNKNOWN|bad_alloc" NO_TRACE)
add_repl_test(ba_hsb-dnf_cap_answers_unknown
	"set hsb-max-dnf-clauses 1. n { (x[0] < 0 | x[1] < 0) }:hsb & { (x[0] - 1 < 0 | x[1] - 1 < 0) }:hsb"
	"UNKNOWN: an hsb constant needs more DNF clauses")
add_repl_test(ba_hsb-index_past_64_bits_is_refused
	"n { x[99999999999999999999999] < 0 }:hsb" "parsing a constant failed")
add_repl_test(ba_hsb-index_past_the_dimension_is_refused
	"n { x[4096] < 0 }:hsb" "parsing a constant failed")
add_repl_test(ba_hsb-index_below_the_dimension_reads
	"n { x[4095] < 0 }:hsb" "x\\[4095\\] < 0")
add_repl_test(ba_hsb-max_dimension_is_an_option
	"n { x[3] < 0 }:hsb" "parsing a constant failed" FLAGS --hsb-max-dimension 3)

# sbf: `xy` is `x & y` in charvar mode and one variable in var mode, so the
# same source read again after the mode changes is another constant.
# A line is parsed whole before it runs, so the switch has a line of its own.
add_multiline_repl_test(ba_sbf-charvar_toggle_rereads ": T.*set charvar off.*: F"
	"normalize {xy}:sbf = {x}:sbf & {y}:sbf"
	"set charvar off"
	"normalize {xy}:sbf = {x}:sbf & {y}:sbf")
