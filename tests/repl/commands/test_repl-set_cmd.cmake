#
# set command
#

include(add_repl_test)

add_repl_test(set_cmd-status "set status off"  "status: *false")
add_repl_test(set_cmd-color "set color off"  "color: *false")

# Regression test for AP-N1: set_cmd() used to memoize its setters map in a
# static local, whose lambdas captured the *first* call's `this`/value by
# reference. Every later "set" command in the same session then read those
# stale references instead of its own value. Issuing three "set" commands
# in one session and checking that the last one actually took effect
# catches that regression.
add_repl_test(set_cmd-multiple_in_one_session
	"set status off. set color off. set status on. get status"
	"status:[ ]+true")

# --- numeric limit options ---------------------------------------------------
#
# Unified limit options (2026-08-17): every limit has a CLI and a REPL option;
# 0 means unlimited (specsizewarn: off), the gc knobs keep raw semantics.
# Each test sets a value and reads it back through the library global the
# algorithm actually consults, so it proves the whole set->api->get chain.

add_repl_test(set_cmd-max_fixpoint_steps
	"set max-fixpoint-steps 7. get max-fixpoint-steps" "max-fixpoint-steps: *7")
add_repl_test(set_cmd-max_fixpoint_steps_zero_unlimited
	"set max-fixpoint-steps 7. set max-fixpoint-steps 0. get max-fixpoint-steps"
	"max-fixpoint-steps: *0")
# A count past the CLI's bound (LONG_MAX) is refused, as the CLI refuses it,
# never wrapped (2^64 would wrap to 0, which reads as unlimited).
add_repl_test(set_cmd-count_past_size_max_refused
	"set max-fixpoint-steps 7. set max-fixpoint-steps 18446744073709551616. get max-fixpoint-steps"
	"does not take the value.*name=max-fixpoint-steps.*max-fixpoint-steps: *7" NO_FAIL_REGEX)
add_repl_test(set_cmd-max_flag_search_steps
	"set max-flag-search-steps 12. get max-flag-search-steps" "max-flag-search-steps: *12")
add_repl_test(set_cmd-bv_blastdepth
	"set bv-blastdepth 8. get bv-blastdepth" "bv-blastdepth: *8")
add_repl_test(set_cmd-block_squeeze_cap
	"set block-squeeze-cap 64. get block-squeeze-cap" "block-squeeze-cap: *64")
add_repl_test(set_cmd-max_simplify_rounds
	"set max-simplify-rounds 1000. get max-simplify-rounds" "max-simplify-rounds: *1000")
add_repl_test(set_cmd-max_def_passes
	"set max-def-passes 40. get max-def-passes" "max-def-passes: *40")
add_repl_test(set_cmd-max_enum_steps
	"set max-enum-steps 33. get max-enum-steps" "max-enum-steps: *33")
add_repl_test(set_cmd-max_probe_steps
	"set max-probe-steps 44. get max-probe-steps" "max-probe-steps: *44")
add_repl_test(set_cmd-max_probe_steps_unlimited
	"set max-probe-steps 0. get max-probe-steps" "max-probe-steps: *0")
add_repl_test(set_cmd-max_rewrite_rounds
	"set max-rewrite-rounds 21. get max-rewrite-rounds" "max-rewrite-rounds: *21")
add_repl_test(set_cmd-gc_min_size
	"set gc-min-size 512. get gc-min-size" "gc-min-size: *512")
# gcgrowth is the one decimal-valued option; its value exercises the
# option_value => (alnum | '.')+ grammar extension.
add_repl_test(set_cmd-gc_growth_factor_decimal
	"set gc-growth-factor 2.5. get gc-growth-factor" "gc-growth-factor: *2.5")
add_repl_test(set_cmd-spec_size_warn
	"set spec-size-warn 4096. get spec-size-warn" "spec-size-warn: *4096")
add_repl_test(set_cmd-max_revision_alts
	"set max-revision-alts 3. get max-revision-alts" "max-revision-alts: *3")
add_repl_test(set_cmd-max_consistency_subsets
	"set max-consistency-subsets 7. get max-consistency-subsets" "max-consistency-subsets: *7")
add_repl_test(set_cmd-cache_bound
	"set cache-bound 99. get cache-bound" "cache-bound: *99")
add_repl_test(set_cmd-max_constant_size
	"set max-constant-size 300. get max-constant-size" "max-constant-size: *300")
add_repl_test(set_cmd-max_cover_products
	"set max-cover-products 17. get max-cover-products" "max-cover-products: *17")
# The two pre-existing numeric options now accept 0 as "unlimited" (they
# rejected 0 before this change, so no meaning was lost).
add_repl_test(set_cmd-block_max_splits_zero_unlimited
	"set block-max-splits 512. set block-max-splits 0. get block-max-splits"
	"block-max-splits: *0")
add_repl_test(set_cmd-block_max_rounds_roundtrip
	"set block-max-rounds 1000. get block-max-rounds" "block-max-rounds: *1000")
add_repl_test(set_cmd-ba_decision_pins_roundtrip
	"set ba-decision-pins 12. get ba-decision-pins" "ba-decision-pins: *12")
add_repl_test(set_cmd-ba_decision_pins_zero_is_none
	"set ba-decision-pins 0. get ba-decision-pins" "ba-decision-pins: *none")

# bv declares case-split-max-tests as its own option, addressed
# bv-case-split-max-tests, present when bv is in the configured pack. The
# command names the option but not the BA, so REQUIRES bv carries the pack
# gate, as bv-blastdepth's family does.
add_repl_test(set_cmd-bv_case_split_max_tests_roundtrip
	"set bv-case-split-max-tests 7. get bv-case-split-max-tests"
	"bv-case-split-max-tests: *7" REQUIRES bv)
add_repl_test(set_cmd-bv_case_split_max_tests_zero_unlimited
	"set bv-case-split-max-tests 7. set bv-case-split-max-tests 0. get bv-case-split-max-tests"
	"bv-case-split-max-tests: *0" REQUIRES bv)
# Numeric options reject flag values and non-numbers.
add_repl_test(set_cmd-max_fixpoint_steps_flag_value_rejected
	"set max-fixpoint-steps on" "does not take the value.*name=max-fixpoint-steps" NO_FAIL_REGEX)
add_repl_test(set_cmd-gc_growth_factor_bad_value_rejected
	"set gc-growth-factor 1..5" "does not take the value.*name=gc-growth-factor" NO_FAIL_REGEX)

# --- bv widening (exact bitvector arithmetic) -------------------------------
# bv-widening is a flag, bv-max-width a count; each round-trips through the
# api setter and back out of `get`. bv-max-width 0 does not mean unlimited:
# it leaves the current cap unchanged (the api setter ignores 0), so setting
# it after a real value reads that value back, not "unlimited".
add_repl_test(set_cmd-bvwidening_on
	"set bv-widening on. get bv-widening" "bv-widening: *true")
add_repl_test(set_cmd-bvwidening_off_again
	"set bv-widening on. set bv-widening off. get bv-widening"
	"bv-widening: *false")
add_repl_test(set_cmd-bvmaxwidth
	"set bv-max-width 64. get bv-max-width" "bv-max-width: *64")
add_repl_test(set_cmd-bvmaxwidth_zero_keeps_current
	"set bv-max-width 64. set bv-max-width 0. get bv-max-width" "bv-max-width: *64")
add_repl_test(set_cmd-bvmaxwidth_flag_value_rejected
	"set bv-max-width on" "does not take the value.*name=bv-max-width" NO_FAIL_REGEX)
add_repl_test(set_cmd-bvmaxwidth_past_size_max_refused
	"set bv-max-width 64. set bv-max-width 18446744073709551680. get bv-max-width"
	"does not take the value.*name=bv-max-width.*bv-max-width: *64" NO_FAIL_REGEX)
add_repl_test(set_cmd-bvmaxwidth_enable_rejected
	"enable bv-max-width" "takes a count, not a flag" NO_FAIL_REGEX)
# The mode actually changes what the decision procedures answer: 16 * 16
# wraps to 0 at 8 bits in the default mode and is 256 -- never 0 -- once the
# exact mode is on, in the same session.
add_repl_test(set_cmd-bvwidening_changes_semantics
	"sat {16}:bv[8] * {16}:bv[8] = {0}:bv[8]. set bv-widening on. sat {16}:bv[8] * {16}:bv[8] = {0}:bv[8]"
	"%1.*: T.*%2.*: F")
# The normalizer caches are keyed on the formula only, so a set that changes a
# semantic option empties them: the second normalize computes again (its scope
# shows the inner steps) instead of answering from the first one's cache.
add_repl_test(set_cmd-option_change_drops_normalizer_cache
	"normalize (x & y) = 0 && x = 0. set max-simplify-rounds 3. normalize (x & y) = 0 && x = 0"
	"max-simplify-rounds: *3[^%]*eliminate_arithmetic_and_quantifiers")
add_repl_test(set_cmd-ba_option_change_drops_normalizer_cache
	"normalize (x & y) = 0 && x = 0. set bv-definitional-elimination off. normalize (x & y) = 0 && x = 0"
	"bv-definitional-elimination: *false[^%]*eliminate_arithmetic_and_quantifiers")

# set prints the option it changed; these setters had no case.
add_repl_test(set_cmd-preprocessing_off "set preprocessing off" "preprocessing: *false")
add_repl_test(set_cmd-ba_component_factoring_off "set ba-component-factoring off" "ba-component-factoring: *false")
add_repl_test(set_cmd-cqe_max_clauses "set cqe-max-clauses 7" "cqe-max-clauses: *7")
add_repl_test(set_cmd-tref_budget "set tref-budget 5000000"
	"tref-budget: *5000000")
add_repl_test(set_cmd-tref_budget_soft "set tref-budget-soft 50" "tref-budget-soft: *50")
# a flag value that is no on/off spelling leaves the option as it was
add_repl_test(set_cmd-bool_invalid_value "set status maybe"
	"does not take the value.*name=status")
# std::stod throws on "..", which the grammar admits as a decimal
add_repl_test(set_cmd-gc_growth_factor_not_a_number "set gc-growth-factor .."
	"does not take the value.*name=gc-growth-factor")
# a BA flag option refuses a value that is no on/off spelling
add_repl_test(set_cmd-ba_flag_invalid_value "set bv-blasting maybe"
	"does not take the value.*name=bv-blasting" REQUIRES bv)
