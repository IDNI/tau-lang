#
# get command
#

include(add_repl_test)

add_repl_test(get_cmd-all "get" "status:")
add_repl_test(get_cmd-one "get colors" "colors:")

# Bare `get` also lists every numeric limit option with its default
# (unlimited caps except the two finite SO-1 temporal caps, tuned gc values,
# spec-size warning off).
add_repl_test(get_cmd-lists_limits "get" "maxsplits: *unlimited")
# IN-M3: the temporal-normalization caps ship FINITE (500) because their
# searches have no termination guarantee; 0 still means unlimited.
add_repl_test(get_cmd-fixpointsteps_default_finite "get fixpointsteps" "fixpointsteps: *500")
add_repl_test(get_cmd-flagsteps_default_finite "get flagsteps" "flagsteps: *500")
add_repl_test(get_cmd-fixpointsteps_zero_is_unlimited "set fixpointsteps 0. get fixpointsteps" "fixpointsteps: *unlimited")
add_repl_test(get_cmd-lists_gc_defaults "get" "gcgrowth: *1.5")
add_repl_test(get_cmd-lists_specsizewarn_off "get" "specsizewarn: *off")

# --- bare `get` prints every limit option (covers the limit_printers map) ----
foreach(opt maxsplits maxrounds fixpointsteps flagsteps blastdepth squeezecap
		simplifyrounds defpasses enumsteps rewriterounds gcminsize
		gcgrowth specsizewarn revisionalts)
	add_repl_test(get_cmd-all_lists_${opt} "get" "${opt}: ")
endforeach()

# gcminsize round trip through `get` (only a `set` test existed).
add_repl_test(get_cmd-gcminsize "set gcminsize 512. get gcminsize"
	"gcminsize: *512")
# specsizewarn set-then-disable round trip (0 prints as `off`).
add_repl_test(get_cmd-specsizewarn_off_roundtrip
	"set specsizewarn 4096. set specsizewarn 0. get specsizewarn"
	"specsizewarn: *off")
