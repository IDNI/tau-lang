#
# option-name aliases and severity values
#
# Coverage-driven addition (2026-08-02), branch pass. get_opt() and
# str2severity() in repl_evaluator.tmpl.h are two flat tables of string
# comparisons, and gcovr showed almost every alias arm cold: the pre-existing
# repl tests only ever spell options out in full ("status", "colors"), so the
# short forms and the second/third synonyms of each option had never been
# exercised, nor had either table's error arm.
#
# Each alias below is asserted to select the SAME option its long name does,
# which is the whole contract of an alias.
#

include(add_repl_test)

# --- get_opt: one test per alias arm ----------------------------------------

add_repl_test(option_alias-S        "get severity"            "severity:")
add_repl_test(option_alias-sev      "get severity"          "severity:")
add_repl_test(option_alias-s        "get status"            "status:")
add_repl_test(option_alias-c        "get color"            "color:")
add_repl_test(option_alias-color    "get color"        "color:")
add_repl_test(option_alias-V        "get charvar"            "charvar:")
add_repl_test(option_alias-preprocessing "get preprocessing" "preprocessing:")
add_repl_test(option_alias-H        "get highlighting"            "highlighting:")
add_repl_test(option_alias-highlight "get highlighting"   "highlighting:")
add_repl_test(option_alias-I        "get indenting"            "indenting:")
add_repl_test(option_alias-indent   "get indenting"       "indenting:")
add_repl_test(option_alias-benchmarks   "get benchmarks"   "benchmarks:")
add_repl_test(option_alias-benchmarking "get benchmarks" "benchmarks:")
# The debug option itself only exists in a build that defines DEBUG; a release
# build answers "Debug option not available in release build" instead of
# printing a value. Either message proves what these three tests are about --
# that get_opt resolved the alias to debug_opt rather than falling through to
# its invalid_opt arm.
#
# RE-2 (FIXED): that release answer once went to ERROR level, so the three
# cases needed a no-fail-regex helper. A query about an option this build does
# not carry is not an error. It now reports at info level, so the plain helper
# -- which fails on "Error" -- applies.
add_repl_test(option_alias-d     "get debug"     "debug:|Unknown option" NO_FAIL_REGEX)
add_repl_test(option_alias-debug "get debug" "debug:|Unknown option" NO_FAIL_REGEX)
add_repl_test(option_alias-dbg   "get debug"   "debug:|Unknown option" NO_FAIL_REGEX)

# "B" is the master preprocessing switch and "b" is benchmarks; both are
# asserted so each keeps its own letter.
add_repl_test(option_alias-B_is_preprocessing "get preprocessing" "preprocessing:")
add_repl_test(option_alias-b_is_benchmarks    "get benchmarks" "benchmarks:")

# get_opt's error arm. This case needs NO_FAIL_REGEX. The plain helper sets
# FAIL_REGULAR_EXPRESSION "Error", and the error IS the expected output.
add_repl_test(option_alias-invalid "get zzz" "Unknown option.*name=zzz" NO_FAIL_REGEX)

# --- str2severity: one test per value arm -----------------------------------
#
# Each pair sets the level then reads it back, so the assertion is that the
# short form and the long form land on the same level rather than merely being
# accepted. Note add_repl_test appends "-S trace", so a test that only read the
# value back would see "trace" no matter what it set.

add_repl_test(severity_value-e     "set severity error. get severity"     "severity: *error")
add_repl_test(severity_value-error "set severity error. get severity" "severity: *error")
add_repl_test(severity_value-d     "set severity debug. get severity"     "severity: *debug")
add_repl_test(severity_value-debug "set severity debug. get severity" "severity: *debug")
add_repl_test(severity_value-t     "set severity trace. get severity"     "severity: *trace")
add_repl_test(severity_value-trace "set severity trace. get severity" "severity: *trace")
add_repl_test(severity_value-i     "set severity info. get severity"     "severity: *info")
add_repl_test(severity_value-info  "set severity info. get severity"  "severity: *info")

# str2severity's error arm: the value is rejected and the level left untouched.
add_repl_test(severity_value-invalid
	"set severity zz. get severity"
	"does not take the value.*name=severity" NO_FAIL_REGEX)

# --- numeric limit option aliases (2026-08-17 unified limit options) ---------
#
# Same contract as above: each alias must select the same option its primary
# name does. Verified via get's label, which always prints the primary name.

add_repl_test(option_alias-maxfixpointsteps
	"get max-fixpoint-steps"    "max-fixpoint-steps:")
add_repl_test(option_alias-maxflagsearchsteps
	"get max-flag-search-steps"  "max-flag-search-steps:")
# blastdepth moved out of core's numeric-limit table entirely: it is now
# bv's own option, addressed bv-blastdepth, with no core alias left pointing
# at it. add_repl_test's own tau_repl_unsupported check skips this in a
# pack without bv, since the command text names "bv".
add_repl_test(option_alias-bv_blastdepth
	"get bv-blastdepth" "bv-blastdepth:")
add_repl_test(option_alias-blocksqueezecap
	"get block-squeeze-cap"     "block-squeeze-cap:")
add_repl_test(option_alias-maxsimplifyrounds
	"get max-simplify-rounds"   "max-simplify-rounds:")
add_repl_test(option_alias-maxdefpasses
	"get max-def-passes"        "max-def-passes:")
add_repl_test(option_alias-maxenumsteps
	"get max-enum-steps"        "max-enum-steps:")
add_repl_test(option_alias-maxprobesteps
	"get max-probe-steps"       "max-probe-steps:")
# CLI-mirroring aliases (the CLI long name with the dashes stripped).
add_repl_test(option_alias-bacomponentfactoring
	"get ba-component-factoring" "ba-component-factoring:")
add_repl_test(option_alias-badecisionpins
	"get ba-decision-pins"      "ba-decision-pins:")
# bv's own options, addressed bv-case-split / bv-case-split-max-tests, with
# no dashless core alias, same shape as bv_blastdepth above.
add_repl_test(option_alias-bv_case_split
	"get bv-case-split" "bv-case-split:")
add_repl_test(option_alias-bv_case_split_max_tests
	"get bv-case-split-max-tests" "bv-case-split-max-tests:")
add_repl_test(option_alias-maxrewriterounds
	"get max-rewrite-rounds"    "max-rewrite-rounds:")
add_repl_test(option_alias-gcgrowthfactor
	"get gc-growth-factor"      "gc-growth-factor:")
add_repl_test(option_alias-maxrevisionalts
	"get max-revision-alts"     "max-revision-alts:")
add_repl_test(option_alias-blockmaxsplits
	"get block-max-splits"      "block-max-splits:")
add_repl_test(option_alias-blockmaxrounds
	"get block-max-rounds"      "block-max-rounds:")
add_repl_test(option_alias-maxconsistencysubsets
	"get max-consistency-subsets" "max-consistency-subsets:")

# Numeric options take a count, not a flag: enable/disable/toggle must refuse.
add_repl_test(option_numeric-enable_refused
	"enable max-fixpoint-steps" "takes a count" NO_FAIL_REGEX)
add_repl_test(option_numeric-toggle_refused
	"toggle gc-growth-factor" "takes a value, not a flag" NO_FAIL_REGEX)
# Every numeric option, the LTL and memory limits included, says so.
foreach(_opt ltl-timeout ltl-qe-max-vars ltl-hoa-max-states
		ltl-max-observations ltl-mealy-max-states compile-max-table-edges
		bf-dependence-max-nodes tref-budget tref-budget-soft)
	add_repl_test(option_numeric-enable_${_opt}_refused
		"enable ${_opt}" "takes a count, not a flag" NO_FAIL_REGEX)
endforeach()
# severity and ltlalg take a word, not a count and not a flag.
add_repl_test(option_word-enable_ltlalg_refused
	"enable ltl-alg" "takes a value, not a flag" NO_FAIL_REGEX)

# bv's own options, addressed bv-widening / bv-max-width, with no core alias
# and no short form, same shape as bv_blastdepth above.
add_repl_test(option_alias-bv_widening
	"get bv-widening" "bv-widening:")
add_repl_test(option_alias-bv_max_width
	"get bv-max-width" "bv-max-width:")
