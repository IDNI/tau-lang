#
# option names and severity values
#
# The REPL reads an option by its option name only. These cases read the
# options that no other case reads by name, the severity words, and the
# refusal of enable and toggle for an option that is not a flag.
#

include(add_repl_test)

add_repl_test(option_alias-preprocessing "get preprocessing" "preprocessing:")
add_repl_test(option_alias-benchmarks   "get benchmarks"   "benchmarks:")
# A release build does not declare the debug option.
add_repl_test(option_alias-debug "get debug" "debug:|Unknown option" NO_FAIL_REGEX)

# The error is the expected output, so the case needs NO_FAIL_REGEX.
add_repl_test(option_alias-invalid "get zzz" "Unknown option.*name=zzz" NO_FAIL_REGEX)

# --- severity: one case per word -------------------------------------------
#
# Each case sets the level then reads it back. add_repl_test appends
# "-S trace", so a case that only read the value back would see "trace".

add_repl_test(severity_value-error "set severity error. get severity" "severity: *error")
add_repl_test(severity_value-debug "set severity debug. get severity" "severity: *debug")
add_repl_test(severity_value-trace "set severity trace. get severity" "severity: *trace")
add_repl_test(severity_value-info  "set severity info. get severity"  "severity: *info")

# A bad word is refused and the level stays.
add_repl_test(severity_value-invalid
	"set severity zz. get severity"
	"does not take the value.*name=severity" NO_FAIL_REGEX)

# --- BA options --------------------------------------------------------------
#
# add_repl_test's own tau_repl_unsupported check skips these in a pack without
# bv, since the command text names "bv".
add_repl_test(option_alias-bv_blastdepth
	"get bv-blastdepth" "bv-blastdepth:")
add_repl_test(option_alias-bv_case_split
	"get bv-case-split" "bv-case-split:")
add_repl_test(option_alias-bv_case_split_max_tests
	"get bv-case-split-max-tests" "bv-case-split-max-tests:")
add_repl_test(option_alias-bv_widening
	"get bv-widening" "bv-widening:")
add_repl_test(option_alias-bv_max_width
	"get bv-max-width" "bv-max-width:")

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
# ltl-alg takes a word, not a count and not a flag.
add_repl_test(option_word-enable_ltlalg_refused
	"enable ltl-alg" "takes a value, not a flag" NO_FAIL_REGEX)
