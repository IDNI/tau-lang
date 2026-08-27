#
# set command
#

include(add_repl_test)

add_repl_test(set_cmd-status "set status off" "off")
add_repl_test(set_cmd-colors "set colors off" "off")

# Regression test for AP-N1: set_cmd() used to memoize its setters map in a
# static local, whose lambdas captured the *first* call's `this`/value by
# reference. Every later "set" command in the same session then read those
# stale references instead of its own value. Issuing three "set" commands
# in one session and checking that the last one actually took effect
# catches that regression.
add_repl_test(set_cmd-multiple_in_one_session
	"set status off. set colors off. set status on. get status"
	"status:[ ]+on")

# --- numeric limit options ---------------------------------------------------
#
# Unified limit options (2026-08-17): every limit has a CLI and a REPL option;
# 0 means unlimited (specsizewarn: off), the gc knobs keep raw semantics.
# Each test sets a value and reads it back through the library global the
# algorithm actually consults, so it proves the whole set->api->get chain.

add_repl_test(set_cmd-fixpointsteps
	"set fixpointsteps 7. get fixpointsteps" "fixpointsteps: *7")
add_repl_test(set_cmd-fixpointsteps_zero_unlimited
	"set fixpointsteps 7. set fixpointsteps 0. get fixpointsteps"
	"fixpointsteps: *unlimited")
add_repl_test(set_cmd-flagsteps
	"set flagsteps 12. get flagsteps" "flagsteps: *12")
add_repl_test(set_cmd-bv_blastdepth
	"set bv-blastdepth 8. get bv-blastdepth" "bv-blastdepth: *8")
add_repl_test(set_cmd-squeezecap
	"set squeezecap 64. get squeezecap" "squeezecap: *64")
add_repl_test(set_cmd-simplifyrounds
	"set simplifyrounds 1000. get simplifyrounds" "simplifyrounds: *1000")
add_repl_test(set_cmd-defpasses
	"set defpasses 40. get defpasses" "defpasses: *40")
add_repl_test(set_cmd-enumsteps
	"set enumsteps 33. get enumsteps" "enumsteps: *33")
add_repl_test(set_cmd-rewriterounds
	"set rewriterounds 21. get rewriterounds" "rewriterounds: *21")
add_repl_test(set_cmd-gcminsize
	"set gcminsize 512. get gcminsize" "gcminsize: *512")
# gcgrowth is the one decimal-valued option; its value exercises the
# option_value => (alnum | '.')+ grammar extension.
add_repl_test(set_cmd-gcgrowth_decimal
	"set gcgrowth 2.5. get gcgrowth" "gcgrowth: *2.5")
add_repl_test(set_cmd-specsizewarn
	"set specsizewarn 4096. get specsizewarn" "specsizewarn: *4096")
add_repl_test(set_cmd-revisionalts
	"set revisionalts 3. get revisionalts" "revisionalts: *3")
add_repl_test(set_cmd-maxsubsets
	"set maxsubsets 7. get maxsubsets" "maxsubsets: *7")
add_repl_test(set_cmd-cachebound
	"set cachebound 99. get cachebound" "cachebound: *99")
add_repl_test(set_cmd-maxcoverproducts
	"set maxcoverproducts 17. get maxcoverproducts" "maxcoverproducts: *17")
# The two pre-existing numeric options now accept 0 as "unlimited" (they
# rejected 0 before this change, so no meaning was lost).
add_repl_test(set_cmd-maxsplits_zero_unlimited
	"set maxsplits 512. set maxsplits 0. get maxsplits"
	"maxsplits: *unlimited")
add_repl_test(set_cmd-maxrounds_roundtrip
	"set maxrounds 1000. get maxrounds" "maxrounds: *1000")
# Numeric options reject flag values and non-numbers.
add_repl_test_fail(set_cmd-fixpointsteps_flag_value_rejected
	"set fixpointsteps on" "Invalid value")
add_repl_test_fail(set_cmd-gcgrowth_bad_value_rejected
	"set gcgrowth 1..5" "Invalid value")
