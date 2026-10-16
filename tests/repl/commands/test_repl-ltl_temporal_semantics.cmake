#
# Where the LTL pipeline places a formula in time: fixed-position atoms under
# the step counter, time constraints, top-level clauses and past operators.
# Each case has one right verdict over bv[1]; the conjuncts carry outer
# parentheses because a prefix temporal operator takes the whole conjunction
# to its right.
#

# A fixed-position atom read by two hoisted conjuncts of different depths
# stands for a different step's data in each: `i1[1]` is `i1[t]` at step 1
# and `i1[t-2]` at step 3. The extra conjunct is satisfiable with
# `o2[1] = 0`, so it changes no verdict, in either position.
add_repl_test(ltl_temporal-shared_positional_atom_first_unrealizable
	"realizable (G(i1[1]:bv[1] = 0 || o2[1]:bv[1] = 0)) && (G(i1[1]:bv[1] = 0 || i1[3]:bv[1] != 0)) && (F(o3[t]:bv[1] = 1))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-shared_positional_atom_second_unrealizable
	"realizable (G(i1[1]:bv[1] = 0 || i1[3]:bv[1] != 0)) && (G(i1[1]:bv[1] = 0 || o2[1]:bv[1] = 0)) && (F(o3[t]:bv[1] = 1))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-shared_positional_atom_first_realizable
	"realizable (G(i1[1]:bv[1] = 0 || o2[1]:bv[1] = 0)) && (G(i1[1]:bv[1] = 0 || o3[2]:bv[1] = 0)) && (G(o3[2]:bv[1] != 0 || i1[1]:bv[1] != 0)) && (F(o4[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)

# A time constraint in a hoisted conjunct varies with the step while its
# fixed-position atoms do not: `G([t = 0] || i1[1] = 0)` asks `i1[1] = 0`
# at every step but the first, and the environment can refuse it.
add_repl_test(ltl_temporal-hoisted_time_constraint_input_unrealizable
	"realizable (G([t = 0] || i1[1]:bv[1] = 0)) && (F(o3[t]:bv[1] = 1))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-hoisted_time_constraint_output_realizable
	"realizable (G([t = 0] || o2[1]:bv[1] = 0)) && (F(o3[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-hoisted_time_constraint_later_step_unrealizable
	"realizable (G([t > 5] || i1[1]:bv[1] = 0)) && (F(o3[t]:bv[1] = 1))"
	": F" REQUIRES ltlsynt)

# Time constraints under full-LTL operators read the absolute step, through
# the same step guards as the lookback warm-ups.
add_repl_test(ltl_temporal-time_constraint_beside_eventually_always
	"sat (G([t = 0] -> o1[t]:bv[1] = 0)) && (F(G(o1[t]:bv[1] = 1)))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-time_constraint_until_exact_step
	"realizable ((o1[t]:bv[1] = 0) U (o1[t]:bv[1] = 1 && [t = 3])) && (G([t != 3] -> o1[t]:bv[1] = 0))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-time_constraint_until_too_early
	"sat ((o1[t]:bv[1] = 0) U (o1[t]:bv[1] = 1 && [t < 2])) && (G([t < 2] -> o1[t]:bv[1] = 0))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-time_constraint_numeral_left_until_too_early
	"sat ((o1[t]:bv[1] = 0) U (o1[t]:bv[1] = 1 && [2 > t])) && (G([t < 2] -> o1[t]:bv[1] = 0))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-time_constraint_until_run
	"run 5 steps ((o1[t]:bv[1] = 0) U (o1[t]:bv[1] = 1 && [t = 3])) && (G([t != 3] -> o1[t]:bv[1] = 0))"
	"o1\\[3\\] := 1" REQUIRES ltlsynt)

# The operands of a past operator are scoped like those of until: the ψ of
# `φ S ψ` is a witness, which a lookback keeps from firing before the past it
# reads exists, so the environment can keep `i1` away from 1 forever.
add_repl_test(ltl_temporal-since_lookback_witness_unrealizable
	"realizable G((o1[t]:bv[2] = 1) S (i1[t-1]:bv[2] = 1))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-since_lookback_witness_bv1_unrealizable
	"realizable G((o1[t]:bv[1] = 1) S (i1[t-1]:bv[1] = 1))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-since_lookback_output_witness_realizable
	"realizable G((o1[t]:bv[1] = 1) S (o2[t-1]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-trigger_lookback_witness_realizable
	"realizable G((i1[t-1]:bv[1] = 1) T (o1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)

# A top-level past operator whose ψ reads a lookback holds at the first step
# its clause enforces, not at an absolute step 0 that would name o2[-1].
add_repl_test(ltl_temporal-run_top_level_since_lookback_in_psi
	"run 4 steps (o1[t]:bv[1] = 1) S (o2[t-1]:bv[1] = 1)"
	"o2\\[0\\] := 1" FAIL_REGEX "unsat|smaller than 0")
add_repl_test(ltl_temporal-run_top_level_trigger_lookback_in_psi
	"run 4 steps (o2[t]:bv[1] = 1) T (o1[t-1]:bv[1] = 1)"
	"o1\\[0\\] := 1" FAIL_REGEX "unsat|smaller than 0")
add_repl_test(ltl_temporal-run_since_beside_always_lookback_in_psi
	"run 4 steps (always (o3[t]:bv[1] = 1)) && ((o1[t]:bv[1] = 1) S (o2[t-1]:bv[1] = 1))"
	"o3\\[3\\] := 1" FAIL_REGEX "unsat|smaller than 0")

# A top-level conjunct without a future operator holds at every step, beside
# temporal conjuncts as alone. For a past formula `realizable`, `sat` and
# `run` therefore agree: `(o2 = 1) T (o1 = 1 && o1[t-1] = 0)` asks
# `o1 = 1 && o1[t-1] = 0` at every step, which no trace gives.
add_repl_test(ltl_temporal-top_level_trigger_invariant_realizable
	"realizable (o2[t]:bv[1] = 1) T (o1[t]:bv[1] = 1 && o1[t-1]:bv[1] = 0)"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-top_level_trigger_invariant_sat
	"sat (o2[t]:bv[1] = 1) T (o1[t]:bv[1] = 1 && o1[t-1]:bv[1] = 0)"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-top_level_trigger_invariant_run
	"run 3 steps (o2[t]:bv[1] = 1) T (o1[t]:bv[1] = 1 && o1[t-1]:bv[1] = 0)"
	"unsat" NO_FAIL_REGEX REQUIRES ltlsynt)
add_repl_test(ltl_temporal-top_level_since_invariant_realizable
	"realizable (o1[t]:bv[1] = 1) S (o2[t-1]:bv[1] = 1)"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-bare_atom_beside_weak_until
	"realizable (o2[t]:bv[1] = 1) && (G((o2[t]:bv[1] = 1) W (o1[t]:bv[1] = 1)))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-bare_fixed_position_atom_beside_eventually
	"realizable (o1[1]:bv[1] = 1) && (F(o2[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-bare_atom_contradicts_eventually
	"realizable (o2[t]:bv[1] = 0) && (F(o2[t]:bv[1] = 1))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-bare_input_atom_beside_eventually
	"realizable (i1[t]:bv[1] = 0) && (F(o2[t]:bv[1] = 1))"
	": F" REQUIRES ltlsynt)

# A constant left operand leaves ψ read at the first step only: `T R ψ`,
# `F U ψ` and `F W ψ` are weaker than the always that a bare ψ means, so the
# growth that no bv[2] stream keeps up forever is satisfiable once. A past
# operator folds: `F S ψ` holds at every step, as ψ does.
add_repl_test(ltl_temporal-true_release_reads_first_step
	"sat T R (o3[t]:bv[2] > o3[t-1]:bv[2])" ": T" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-false_until_reads_first_step
	"sat F U (o3[t]:bv[2] > o3[t-1]:bv[2])" ": T" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-false_weak_until_reads_first_step
	"sat F W (o3[t]:bv[2] > o3[t-1]:bv[2])" ": T" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-false_since_holds_every_step
	"sat F S (o3[t]:bv[2] > o3[t-1]:bv[2])" ": F")
add_repl_test(ltl_temporal-true_release_beside_eventually
	"sat (T R (o1[t]:bv[1] = 0)) && (F (o1[t]:bv[1] != 0))" ": T" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-false_until_does_not_imply_always
	"valid (F U (o1[t]:bv[1] = 0)) -> (G (o1[t]:bv[1] = 0))" ": F" REQUIRES ltlsynt)

# `E χ` is asked from the deepest lookback χ reads, like any clause, so its
# root witness and its constraint meet: with no input E is A, and `o1 = 0`
# cannot happen beside `always o1 = 1`, at any step.
add_repl_test(ltl_temporal-ctl_star_root_E_with_lookback_unrealizable
	"fragment ctl_star. realizable (always o1[t]:bv[1] = 1) && E (sometimes (o1[t]:bv[1] = 0 && o2[t-1]:bv[1] = 0))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-ctl_star_root_E_with_lookback_sat
	"fragment ctl_star. sat (always o1[t]:bv[1] = 1) && E (sometimes (o1[t]:bv[1] = 0 && o2[t-1]:bv[1] = 0))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-ctl_star_root_E_input_lookback_unrealizable
	"fragment ctl_star. realizable (always o1[t]:bv[1] = 1) && E (sometimes (o1[t]:bv[1] = 0 && i1[t-1]:bv[1] = 0))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-ctl_star_root_E_input_lookback_realizable
	"fragment ctl_star. realizable (always (o1[t]:bv[1] = 1 || i1[t]:bv[1] = 1)) && E (sometimes (o1[t]:bv[1] = 0 && i1[t-1]:bv[1] = 0))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-ctl_star_E_lookback_witness_realizable
	"fragment ctl_star. realizable E (sometimes (o1[t]:bv[1] = 1 && o2[t-1]:bv[1] = 0))"
	": T" REQUIRES ltlsynt)
