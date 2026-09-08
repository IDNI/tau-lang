#
# get command
#

include(add_repl_test)

add_repl_test(get_cmd-all "get" "status:")
add_repl_test(get_cmd-one "get colors" "colors:")

# Bare `get` also lists every numeric limit option with its default
# (unlimited caps, tuned gc values, spec-size warning off).
add_repl_test(get_cmd-lists_limits "get" "fixpointsteps: *unlimited")
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

# --- bv widening (exact bitvector arithmetic) -------------------------------
# Both knobs are listed by bare `get` and readable on their own; the mode is
# off by default and the width cap defaults to 1024 (a hard ceiling, printed
# as a plain number: unlike the limits above, 0 is never a valid value).
add_repl_test(get_cmd-all_lists_bvwidening "get" "bvwidening: *off")
add_repl_test(get_cmd-all_lists_bvmaxwidth "get" "bvmaxwidth: *1024")
add_repl_test(get_cmd-bvwidening "get bvwidening" "bvwidening: *off")
add_repl_test(get_cmd-bvmaxwidth "get bvmaxwidth" "bvmaxwidth: *1024")
