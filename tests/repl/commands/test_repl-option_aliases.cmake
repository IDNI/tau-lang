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

add_repl_test(option_alias-S        "get S"            "severity:")
add_repl_test(option_alias-sev      "get sev"          "severity:")
add_repl_test(option_alias-s        "get s"            "status:")
add_repl_test(option_alias-c        "get c"            "colors:")
add_repl_test(option_alias-color    "get color"        "colors:")
add_repl_test(option_alias-V        "get V"            "charvar:")
add_repl_test(option_alias-blasting "get blasting"     "blasting:")
add_repl_test(option_alias-H        "get H"            "highlighting:")
add_repl_test(option_alias-highlight "get highlight"   "highlighting:")
add_repl_test(option_alias-I        "get I"            "indenting:")
add_repl_test(option_alias-indent   "get indent"       "indenting:")
add_repl_test(option_alias-benchmarks   "get benchmarks"   "benchmarks:")
add_repl_test(option_alias-benchmarking "get benchmarking" "benchmarks:")
# The debug option itself only exists in a build that defines DEBUG; a release
# build answers "Debug option not available in release build" instead of
# printing a value. Either message proves what these three tests are about --
# that get_opt resolved the alias to debug_opt rather than falling through to
# its invalid_opt arm.
#
# RE-2 (FIXED): that release answer used to be logged at ERROR level, which is
# why these three needed add_repl_test_fail. Answering a query about an option
# this build does not carry is not an error condition, so it is reported at
# info level now and the plain helper -- which fails on "Error" -- applies.
add_repl_test(option_alias-d     "get d"     "debug-repl:|Debug option not available")
add_repl_test(option_alias-debug "get debug" "debug-repl:|Debug option not available")
add_repl_test(option_alias-dbg   "get dbg"   "debug-repl:|Debug option not available")

# RE-1 (FIXED): "B" used to be declared twice in get_opt -- first for blasting,
# then again for benchmarks -- so the benchmarks arm was unreachable and
# `get B` silently meant blasting. Benchmarks now owns the previously free
# lowercase "b" and blasting keeps "B". Both are asserted so the two can never
# collide again unnoticed.
add_repl_test(option_alias-B_is_blasting   "get B" "blasting:")
add_repl_test(option_alias-b_is_benchmarks "get b" "benchmarks:")

# get_opt's error arm. add_repl_test_fail is required here: the plain helper
# sets FAIL_REGULAR_EXPRESSION "Error", and the error IS the expected output.
add_repl_test_fail(option_alias-invalid "get zzz" "Invalid option: zzz")

# --- str2severity: one test per value arm -----------------------------------
#
# Each pair sets the level then reads it back, so the assertion is that the
# short form and the long form land on the same level rather than merely being
# accepted. Note add_repl_test appends "-S trace", so a test that only read the
# value back would see "trace" no matter what it set.

add_repl_test(severity_value-e     "set severity e. get severity"     "severity: *error")
add_repl_test(severity_value-error "set severity error. get severity" "severity: *error")
add_repl_test(severity_value-d     "set severity d. get severity"     "severity: *debug")
add_repl_test(severity_value-debug "set severity debug. get severity" "severity: *debug")
add_repl_test(severity_value-t     "set severity t. get severity"     "severity: *trace")
add_repl_test(severity_value-trace "set severity trace. get severity" "severity: *trace")
add_repl_test(severity_value-i     "set severity i. get severity"     "severity: *info")
add_repl_test(severity_value-info  "set severity info. get severity"  "severity: *info")

# str2severity's error arm: the value is rejected and the level left untouched.
add_repl_test_fail(severity_value-invalid
	"set severity zz. get severity"
	"Invalid severity value: zz")

# --- numeric limit option aliases (2026-08-17 unified limit options) ---------
#
# Same contract as above: each alias must select the same option its primary
# name does. Verified via get's label, which always prints the primary name.

add_repl_test(option_alias-maxfixpointsteps
	"get maxfixpointsteps"    "fixpointsteps:")
add_repl_test(option_alias-maxflagsearchsteps
	"get maxflagsearchsteps"  "flagsteps:")
add_repl_test(option_alias-maxblastreentrydepth
	"get maxblastreentrydepth" "blastdepth:")
add_repl_test(option_alias-blocksqueezecap
	"get blocksqueezecap"     "squeezecap:")
add_repl_test(option_alias-maxsimplifyrounds
	"get maxsimplifyrounds"   "simplifyrounds:")
add_repl_test(option_alias-maxdefpasses
	"get maxdefpasses"        "defpasses:")
add_repl_test(option_alias-maxenumsteps
	"get maxenumsteps"        "enumsteps:")
add_repl_test(option_alias-maxrewriterounds
	"get maxrewriterounds"    "rewriterounds:")
add_repl_test(option_alias-gcgrowthfactor
	"get gcgrowthfactor"      "gcgrowth:")
add_repl_test(option_alias-maxrevisionalts
	"get maxrevisionalts"     "revisionalts:")
add_repl_test(option_alias-blockmaxsplits
	"get blockmaxsplits"      "maxsplits:")
add_repl_test(option_alias-blockmaxrounds
	"get blockmaxrounds"      "maxrounds:")

# Numeric options take a count, not a flag: enable/disable/toggle must refuse.
add_repl_test_fail(option_numeric-enable_refused
	"enable fixpointsteps" "takes a count")
add_repl_test_fail(option_numeric-toggle_refused
	"toggle gcgrowth" "takes a count")
