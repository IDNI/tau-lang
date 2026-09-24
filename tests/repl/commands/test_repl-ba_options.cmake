include(add_repl_test)

# family/option resolution through pack_find_ba_option: each outcome has its
# own message (repl_evaluator.tmpl.h resolve_ba_option). A case naming an
# algebra outside the configured pack is skipped by add_repl_test itself.
add_repl_test_fail(ba_options-no_such_family "set nope-blasting on"
	"No BA named 'nope' in this pack")
add_repl_test_fail(ba_options-no_such_option "set bv-nosuch on"
	"BA 'bv' has no option 'nosuch'")
add_repl_test(ba_options-found "set bv-blasting off. get bv-blasting"
	"bv-blasting: off")

# GitHub #124: the definitional elimination and its caps are bv-declared.
add_repl_test(ba_options-defelim_flag "get bv-definitional-elimination"
	"bv-definitional-elimination: on")
add_repl_test(ba_options-defelim_flag_off "set bv-definitional-elimination off. get bv-definitional-elimination"
	"bv-definitional-elimination: off")
add_repl_test(ba_options-defelim_count "set bv-defelim-max-atoms 5. get bv-defelim-max-atoms"
	"bv-defelim-max-atoms: *5")
add_repl_test(ba_options-defelim_rounds_unlimited "set bv-defelim-max-rounds 0. get bv-defelim-max-rounds"
	"bv-defelim-max-rounds: *unlimited")

# The verdict memo is keyed on the formula, so changing a BA-declared option
# between two queries of the same spec must drop it: the second `sat` is
# decided again (its temporal normalization reports its fixpoint again)
# instead of being answered from the first.
set(_defelim_spec "always (!(((o2[t-1]:bv[2] = o2[t]) -> (o2[t-2] = o1[t-2]:bv[2]))) && ((o1[t] = o2[t-2]) <-> (o1[t-2] != o2[t])))")
add_repl_test(ba_options-change_drops_verdict_memo
	"sat ${_defelim_spec}. set bv-definitional-elimination off. sat ${_defelim_spec}"
	"reached fixpoint.*: F.*reached fixpoint.*: F")
