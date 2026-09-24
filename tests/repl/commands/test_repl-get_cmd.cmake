#
# get command
#

include(add_repl_test)

add_repl_test(get_cmd-all "get" "status:")
add_repl_test(get_cmd-one "get colors" "colors:")

# Bare `get` also lists every numeric limit option with its default
# (unlimited caps except the finite temporal-search caps, tuned gc
# values, spec-size warning off).
add_repl_test(get_cmd-lists_limits "get" "maxsplits: *unlimited")
# The two temporal searches have no termination guarantee, so both caps
# ship FINITE (500) through the CLI as well; 0 still means unlimited.
add_repl_test(get_cmd-fixpointsteps_default_finite "get fixpointsteps" "fixpointsteps: *500")
add_repl_test(get_cmd-flagsteps_default_finite "get flagsteps" "flagsteps: *500")
add_repl_test(get_cmd-fixpointsteps_zero_is_unlimited "set fixpointsteps 0. get fixpointsteps" "fixpointsteps: *unlimited")
add_repl_test(get_cmd-lists_gc_defaults "get" "gcgrowth: *1.5")
add_repl_test(get_cmd-lists_specsizewarn_off "get" "specsizewarn: *off")

# --- bare `get` prints every limit option (covers the limit_printers map) ----
foreach(opt maxsplits maxrounds decisionpins fixpointsteps flagsteps squeezecap
		simplifyrounds defpasses enumsteps probesteps rewriterounds gcminsize
		gcgrowth specsizewarn revisionalts maxsubsets cachebound
		maxcoverproducts)
	add_repl_test(get_cmd-all_lists_${opt} "get" "${opt}: ")
endforeach()

# bv declares blastdepth as its own option, so bare `get` lists it after the
# core options as bv-blastdepth. The command below is plain "get", so the gate
# is applied by hand against a probe string naming bv.
tau_repl_unsupported(_tau_skip "get bv-blastdepth")
if(_tau_skip)
	tau_repl_record_skip("get_cmd-all_lists_bv-blastdepth")
else()
	add_repl_test(get_cmd-all_lists_bv-blastdepth "get" "bv-blastdepth: ")
endif()

# bv declares case-split-max-tests as its own option, so bare `get` lists it
# after the core options as bv-case-split-max-tests. Gated by hand the same
# way as bv-blastdepth above.
tau_repl_unsupported(_tau_skip "get bv-case-split-max-tests")
if(_tau_skip)
	tau_repl_record_skip("get_cmd-all_lists_bv-case-split-max-tests")
else()
	add_repl_test(get_cmd-all_lists_bv-case-split-max-tests "get" "bv-case-split-max-tests: ")
endif()

# LT-17 / LG-27: the two Batch-O3 caps ship FINITE (4096); 0 opts back into
# unlimited/unbounded, same shape as the SO-1 temporal caps above.
add_repl_test(get_cmd-maxsubsets_default_finite "get maxsubsets"
	"maxsubsets: *4096")
add_repl_test(get_cmd-maxsubsets_zero_is_unlimited
	"set maxsubsets 0. get maxsubsets" "maxsubsets: *unlimited")
add_repl_test(get_cmd-cachebound_default_finite "get cachebound"
	"cachebound: *4096")
add_repl_test(get_cmd-cachebound_zero_is_unlimited
	"set cachebound 0. get cachebound" "cachebound: *unlimited")
# Batch O8: the oracle's mixed-type coverage expansion cap ships
# FINITE (256); 0 opts into unlimited.
add_repl_test(get_cmd-maxcoverproducts_default_finite "get maxcoverproducts"
	"maxcoverproducts: *256")
add_repl_test(get_cmd-maxcoverproducts_zero_is_unlimited
	"set maxcoverproducts 0. get maxcoverproducts"
	"maxcoverproducts: *unlimited")

# gcminsize round trip through `get` (only a `set` test existed).
add_repl_test(get_cmd-gcminsize "set gcminsize 512. get gcminsize"
	"gcminsize: *512")
# specsizewarn set-then-disable round trip (0 prints as `off`).
add_repl_test(get_cmd-specsizewarn_off_roundtrip
	"set specsizewarn 4096. set specsizewarn 0. get specsizewarn"
	"specsizewarn: *off")

# --- bv widening (exact bitvector arithmetic) -------------------------------
# Both knobs are listed by bare `get` and readable on their own; the mode is
# off by default and the width cap defaults to 1024 (a hard ceiling, printed
# as a plain number: unlike the limits above, 0 is never a valid value).
# The bare-`get` cases below name no BA in their own command text, so they are
# gated by hand the same way as bv-blastdepth above.
tau_repl_unsupported(_tau_skip "get bv-widening")
if(_tau_skip)
	tau_repl_record_skip("get_cmd-all_lists_bv-widening")
	tau_repl_record_skip("get_cmd-all_lists_bv-max-width")
else()
	add_repl_test(get_cmd-all_lists_bv-widening "get" "bv-widening: *off")
	add_repl_test(get_cmd-all_lists_bv-max-width "get" "bv-max-width: *1024")
endif()
add_repl_test(get_cmd-bv-widening "get bv-widening" "bv-widening: *off")
add_repl_test(get_cmd-bv-max-width "get bv-max-width" "bv-max-width: *1024")
