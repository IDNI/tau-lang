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
