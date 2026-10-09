include(add_repl_test)
include(tau_repl_pack)

#
# Limit options changing algorithm behavior (not just set/get round trips).
# Each give-up branch is driven to fire with a cap of 1 on a workload that
# needs more. Workloads verified by hand 2026-08-17.
#
# NOT covered here, deliberately: to_unbounded_continuation's flag-search
# give-up (satisfiability.tmpl.h SO-1). It only fires on a satisfiable spec
# whose eventual flag stays unraisable past the boundary -- GitHub-#70-class
# mixed tau/bv specs measured at 12-271 s. Every small `sometimes` spec
# raises its flag immediately, and unsat specs are caught by the pre-loop
# is_run_satisfiable check before the capped loop begins.
#

# find_fixpoint_phi: a lookback-2 always-spec needs at least 2 phi steps, so
# a cap of 1 must give up (loudly) and still terminate. A give-up is no
# verdict: the query must end in an error, never print `%1: T` or `%1: F`
# (the partial phi used to be decided as if it were the continuation).
add_repl_test(limit_effect-max_fixpoint_steps_giveup
	"sat always o1[t] = o1[t-2]"
	"find_fixpoint_phi: exceeded 1 steps" NO_TRACE
	FLAGS --max-fixpoint-steps 1 FAIL_REGEX ": T|: F")
add_repl_test(limit_effect-max_fixpoint_steps_giveup_is_an_error
	"sat always o1[t] = o1[t-2]"
	"gave up before reaching a result" NO_FAIL_REGEX NO_TRACE
	FLAGS --max-fixpoint-steps 1)

# The verdict memo is keyed on the formula; a budget change between two
# queries must drop it. With the cap raised back to unlimited the same
# spec, asked again in the same session, must be decided (`: T`) instead
# of answered from the first query's give-up.
add_repl_test(limit_effect-max_fixpoint_steps_memo_dropped_on_change
	"set max-fixpoint-steps 1. sat always o1[t] = o1[t-2]. set max-fixpoint-steps 0. sat always o1[t] = o1[t-2]"
	": T" NO_FAIL_REGEX NO_TRACE)

# This workload needs a handful of steps, so it completes under the
# shipped cap of 500 without ever reaching it.
add_repl_test(limit_effect-max_fixpoint_steps_default_completes
	"sat always o1[t] = o1[t-2]"
	": T" NO_TRACE FAIL_REGEX "exceeded")
add_repl_test(limit_effect-max_fixpoint_steps_cli_zero_unlimited
	"get max-fixpoint-steps"
	"max-fixpoint-steps: *0" NO_FAIL_REGEX NO_TRACE
	FLAGS --max-fixpoint-steps 0)

# The same cap reached through the REPL `set` instead of the CLI flag.
add_repl_test(limit_effect-max_fixpoint_steps_via_set
	"set max-fixpoint-steps 1. sat always o1[t] = o1[t-2]"
	"find_fixpoint_phi: exceeded 1 steps" NO_FAIL_REGEX NO_TRACE)

# Definition expansion: g needs one pass per nesting level forever.
add_repl_test(limit_effect-max_def_passes_giveup
	"g(x) := h(g(x)). h(x) := x'. normalize g(0)"
	"definition expansion did not settle within the pass cap.*limit=1" NO_FAIL_REGEX NO_TRACE
	FLAGS --max-def-passes 1)

# Fixed-point enumeration: the recurrence converges, but not within 1 step.
add_repl_test(limit_effect-max_enum_steps_giveup
	"g[n](x) := g[n-1](x) || x = 0. g[0](x) := F. normalize g(y)"
	"no fixed point and no loop after 1 enumeration steps" NO_FAIL_REGEX NO_TRACE
	FLAGS --max-enum-steps 1)

# The untyped saturation probe cap (--max-probe-steps) has no REPL-level
# effect test: the REPL's call-type validation rejects every type-blocked
# recurrence before calculate_fixed_point runs, so its exhausted-probe
# verdict is exercised from test_integration-nso_rr_fixed_point instead.

# spec-size-warn: any accepted update trips a 1-char threshold. The update
# stream u takes its value from i1; the interactive run is driven on stdin
# (-X legacy REPL, the tty-free branch), same pattern as test_repl-run_cmd.
add_multiline_repl_test(limit_effect-specsizewarn_fires
	"exceeds the spec-size-warn threshold 1"
	NO_FAIL_REGEX STDIN "run u[t] = i1[t] && o1[t] = 0.\\no1[t] = 0.\\nq\\nq\\n"
	FLAGS --spec-size-warn 1 X_FIRST)

# And the negative: without the flag (threshold 0 = off) no warning appears.
add_multiline_repl_test(limit_effect-specsizewarn_off_by_default
	"o1\\[0\\] := "
	STDIN "run u[t] = i1[t] && o1[t] = 0.\\no1[t] = 0.\\nq\\nq\\n"
	FAIL_REGEX "spec-size-warn threshold")

# LT-17: the k-ary consistency-subset walk. Four same-type output atoms with
# a pairwise-feasible but jointly-infeasible triple {o1|o2=1, o1&o2=0, o1=o2}
# put ">= 2" subset checks on the walk's plate; a cap of 1 must give up
# loudly and still return the correct verdict (T -- the common commitment
# o3=1 is dischargeable at t=0, and a fired cap skips and logs: at worst it
# leaves an UNREALIZABLE verdict undecided, never an error). Needs a live
# ltlsynt on PATH
# (same as the other `sat`-on-full-LTL tests).
add_repl_test(limit_effect-max_consistency_subsets_giveup
	"sat ((o1[t] | o2[t] = 1) until (o3[t] = 1)) && ((o1[t] & o2[t] = 0) until (o3[t] = 1)) && ((o1[t] = o2[t]) until (o3[t] = 1))"
	"k-ary consistency walk capped after 1 subset checks" NO_FAIL_REGEX NO_TRACE
	FLAGS --max-consistency-subsets 1 REQUIRES ltlsynt)

# The cap only skips eager forbids; the per-edge oracle still refines the
# chosen strategy afterward, so the verdict stays the true T even on the
# spec that used to hit the capped-walk worst case.
add_repl_test(limit_effect-max_consistency_subsets_capped_verdict_recovered
	"sat ((o1[t] | o2[t] = 1) until (o3[t] = 1)) && ((o1[t] & o2[t] = 0) until (o3[t] = 1)) && ((o1[t] = o2[t]) until (o3[t] = 1))"
	": T" NO_FAIL_REGEX NO_TRACE
	FLAGS --max-consistency-subsets 1 REQUIRES ltlsynt)

# Correctness pin: when the skipped subsets are all feasible there was no
# forbid to miss, so the capped verdict is provably unchanged -- the warning
# fires and the answer is still T. Two tests on the same command line: a
# single regex bridging both markers is the pinned ctest-backtracking trap.
add_repl_test(limit_effect-max_consistency_subsets_capped_verdict_correct
	"sat ((o1[t] = o2[t]) until (o4[t] = 1)) && ((o2[t] = o3[t]) until (o4[t] = 1)) && ((o3[t] = o1[t]) until (o4[t] = 1))"
	": T" NO_FAIL_REGEX NO_TRACE
	FLAGS --max-consistency-subsets 1 REQUIRES ltlsynt)
add_repl_test(limit_effect-max_consistency_subsets_capped_verdict_correct_warns
	"sat ((o1[t] = o2[t]) until (o4[t] = 1)) && ((o2[t] = o3[t]) until (o4[t] = 1)) && ((o3[t] = o1[t]) until (o4[t] = 1))"
	"k-ary consistency walk capped after 1 subset checks" NO_FAIL_REGEX NO_TRACE
	FLAGS --max-consistency-subsets 1 REQUIRES ltlsynt)

# Under the shipped default (4096) the same workload completes silently
# with the same verdict.
add_repl_test(limit_effect-max_consistency_subsets_default_completes
	"sat ((o1[t] | o2[t] = 1) until (o3[t] = 1)) && ((o1[t] & o2[t] = 0) until (o3[t] = 1)) && ((o1[t] = o2[t]) until (o3[t] = 1))"
	": T" NO_TRACE FAIL_REGEX "consistency walk capped" REQUIRES ltlsynt)

# The same cap reached through the REPL `set` instead of the CLI flag.
add_repl_test(limit_effect-max_consistency_subsets_via_set
	"set max-consistency-subsets 1. sat ((o1[t] | o2[t] = 1) until (o3[t] = 1)) && ((o1[t] & o2[t] = 0) until (o3[t] = 1)) && ((o1[t] = o2[t]) until (o3[t] = 1))"
	"k-ary consistency walk capped after 1 subset checks" NO_FAIL_REGEX NO_TRACE
	REQUIRES ltlsynt)
