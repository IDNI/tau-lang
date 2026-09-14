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
add_repl_test(set_cmd-blastdepth
	"set blastdepth 8. get blastdepth" "blastdepth: *8")
add_repl_test(set_cmd-squeezecap
	"set squeezecap 64. get squeezecap" "squeezecap: *64")
add_repl_test(set_cmd-simplifyrounds
	"set simplifyrounds 1000. get simplifyrounds" "simplifyrounds: *1000")
add_repl_test(set_cmd-defpasses
	"set defpasses 40. get defpasses" "defpasses: *40")
add_repl_test(set_cmd-enumsteps
	"set enumsteps 33. get enumsteps" "enumsteps: *33")
add_repl_test(set_cmd-probesteps
	"set probesteps 44. get probesteps" "probesteps: *44")
add_repl_test(set_cmd-probesteps_unlimited
	"set probesteps 0. get probesteps" "probesteps: *unlimited")
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
# The two pre-existing numeric options now accept 0 as "unlimited" (they
# rejected 0 before this change, so no meaning was lost).
add_repl_test(set_cmd-maxsplits_zero_unlimited
	"set maxsplits 512. set maxsplits 0. get maxsplits"
	"maxsplits: *unlimited")
add_repl_test(set_cmd-maxrounds_roundtrip
	"set maxrounds 1000. get maxrounds" "maxrounds: *1000")
add_repl_test(set_cmd-decisionpins_roundtrip
	"set decisionpins 12. get decisionpins" "decisionpins: *12")
add_repl_test(set_cmd-decisionpins_zero_is_none
	"set decisionpins 0. get decisionpins" "decisionpins: *0")
add_repl_test(set_cmd-casesplitmaxtests_roundtrip
	"set casesplitmaxtests 7. get casesplitmaxtests" "casesplitmaxtests: *7")
add_repl_test(set_cmd-casesplitmaxtests_zero_unlimited
	"set casesplitmaxtests 7. set casesplitmaxtests 0. get casesplitmaxtests"
	"casesplitmaxtests: *unlimited")
# Numeric options reject flag values and non-numbers.
add_repl_test_fail(set_cmd-fixpointsteps_flag_value_rejected
	"set fixpointsteps on" "Invalid value")
add_repl_test_fail(set_cmd-gcgrowth_bad_value_rejected
	"set gcgrowth 1..5" "Invalid value")

# --- bv widening (exact bitvector arithmetic) -------------------------------
# bvwidening is a flag, bvmaxwidth a count; each round-trips through the api
# setter and back out of `get`. bvmaxwidth 0 does not mean unlimited: it
# leaves the current cap unchanged (the api setter ignores 0), so setting it
# after a real value reads that value back, not "unlimited".
add_repl_test(set_cmd-bvwidening_on
	"set bvwidening on. get bvwidening" "bvwidening: *on")
add_repl_test(set_cmd-bvwidening_off_again
	"set bvwidening on. set bvwidening off. get bvwidening"
	"bvwidening: *off")
add_repl_test(set_cmd-bvmaxwidth
	"set bvmaxwidth 64. get bvmaxwidth" "bvmaxwidth: *64")
add_repl_test(set_cmd-bvmaxwidth_zero_keeps_current
	"set bvmaxwidth 64. set bvmaxwidth 0. get bvmaxwidth" "bvmaxwidth: *64")
add_repl_test_fail(set_cmd-bvmaxwidth_flag_value_rejected
	"set bvmaxwidth on" "Invalid value")
add_repl_test_fail(set_cmd-bvmaxwidth_enable_rejected
	"enable bvmaxwidth" "takes a count, not a flag")
# The mode actually changes what the decision procedures answer: 16 * 16
# wraps to 0 at 8 bits in the default mode and is 256 -- never 0 -- once the
# exact mode is on, in the same session.
add_repl_test(set_cmd-bvwidening_changes_semantics
	"sat {16}:bv[8] * {16}:bv[8] = {0}:bv[8]. set bvwidening on. sat {16}:bv[8] * {16}:bv[8] = {0}:bv[8]"
	"%1.*: T(.*\n)*.*%2.*: F")
