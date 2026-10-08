#
# get command
#

include(add_repl_test)

add_repl_test(get_cmd-all "get" "status:")
add_repl_test(get_cmd-one "get color" "color:")

# Bare `get` also lists every numeric limit option with its default
# (unlimited caps except the finite temporal-search caps, tuned gc
# values, spec-size warning off).
add_repl_test(get_cmd-lists_limits "get" "block-max-splits: *0")
# The two temporal searches have no termination guarantee, so both caps
# ship FINITE (500) through the CLI as well; 0 still means unlimited.
add_repl_test(get_cmd-fixpointsteps_default_finite "get max-fixpoint-steps" "max-fixpoint-steps: *500")
add_repl_test(get_cmd-flagsteps_default_finite "get max-flag-search-steps" "max-flag-search-steps: *500")
add_repl_test(get_cmd-fixpointsteps_zero_is_unlimited "set max-fixpoint-steps 0. get max-fixpoint-steps" "max-fixpoint-steps: *0")
add_repl_test(get_cmd-lists_gc_defaults "get" "gc-growth-factor: *1.5")
add_repl_test(get_cmd-lists_specsizewarn_off "get" "spec-size-warn: *0")

# --- bare `get` prints every limit option (covers the limit_printers map) ----
foreach(opt block-max-splits block-max-rounds ba-decision-pins
		max-fixpoint-steps max-flag-search-steps block-squeeze-cap
		max-simplify-rounds max-def-passes max-enum-steps max-probe-steps
		max-rewrite-rounds gc-min-size gc-growth-factor spec-size-warn
		max-revision-alts max-consistency-subsets cache-bound
		max-cover-products max-constant-size)
	add_repl_test(get_cmd-all_lists_${opt} "get" "${opt}: ")
endforeach()

# bv declares blastdepth as its own option, so bare `get` lists it after the
# core options as bv-blastdepth. The command is plain "get", so the pack gate
# names bv through REQUIRES.
add_repl_test(get_cmd-all_lists_bv-blastdepth "get" "bv-blastdepth: " REQUIRES bv)

# bv declares case-split-max-tests as its own option, so bare `get` lists it
# after the core options as bv-case-split-max-tests. Same REQUIRES gate as
# bv-blastdepth above.
add_repl_test(get_cmd-all_lists_bv-case-split-max-tests "get"
	"bv-case-split-max-tests: " REQUIRES bv)

# LT-17 / LG-27: the two Batch-O3 caps ship FINITE (4096); 0 opts back into
# unlimited/unbounded, same shape as the SO-1 temporal caps above.
add_repl_test(get_cmd-maxsubsets_default_finite "get max-consistency-subsets"
	"max-consistency-subsets: *4096")
add_repl_test(get_cmd-maxsubsets_zero_is_unlimited
	"set max-consistency-subsets 0. get max-consistency-subsets" "max-consistency-subsets: *0")
add_repl_test(get_cmd-cachebound_default_finite "get cache-bound"
	"cache-bound: *4096")
add_repl_test(get_cmd-cachebound_zero_is_unlimited
	"set cache-bound 0. get cache-bound" "cache-bound: *0")
# Batch O8: the oracle's mixed-type coverage expansion cap ships
# FINITE (256); 0 opts into unlimited.
add_repl_test(get_cmd-maxconstantsize_default_finite "get max-constant-size"
	"max-constant-size: *2000")
add_repl_test(get_cmd-maxconstantsize_zero_is_unlimited
	"set max-constant-size 0. get max-constant-size"
	"max-constant-size: *0")
add_repl_test(get_cmd-maxcoverproducts_default_finite "get max-cover-products"
	"max-cover-products: *256")
add_repl_test(get_cmd-maxcoverproducts_zero_is_unlimited
	"set max-cover-products 0. get max-cover-products"
	"max-cover-products: *0")

# gcminsize round trip through `get` (only a `set` test existed).
add_repl_test(get_cmd-gcminsize "set gc-min-size 512. get gc-min-size"
	"gc-min-size: *512")
# specsizewarn set-then-disable round trip (0 prints as `off`).
add_repl_test(get_cmd-specsizewarn_off_roundtrip
	"set spec-size-warn 4096. set spec-size-warn 0. get spec-size-warn"
	"spec-size-warn: *0")

# --- bv widening (exact bitvector arithmetic) -------------------------------
# Both knobs are listed by bare `get` and readable on their own; the mode is
# off by default and the width cap defaults to 1024 (a hard ceiling, printed
# as a plain number: unlike the limits above, 0 is never a valid value).
# The bare-`get` cases below name no BA in their own command text, so the pack
# gate names bv through REQUIRES.
add_repl_test(get_cmd-all_lists_bv-widening "get" "bv-widening: *false" REQUIRES bv)
add_repl_test(get_cmd-all_lists_bv-max-width "get" "bv-max-width: *1024" REQUIRES bv)
add_repl_test(get_cmd-bv-widening "get bv-widening" "bv-widening: *false")
add_repl_test(get_cmd-bv-max-width "get bv-max-width" "bv-max-width: *1024")
