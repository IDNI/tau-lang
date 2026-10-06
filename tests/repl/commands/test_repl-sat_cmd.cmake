#
# sat command
#

include(add_repl_test)

# the sat command checks if a Tau formula is satisfiable and prints T or F
add_repl_test(sat_cmd-t "sat T" ": T")
add_repl_test(sat_cmd-f "sat F" ": F")
add_repl_test(sat_cmd-formula "sat x = 0" ": T")

# satisfiable formulas
add_repl_test(sat_cmd-T         "sat T"            ": T")
add_repl_test(sat_cmd-wff_sat   "sat X = 0"        ": T")
add_repl_test(sat_cmd-wff_sat2  "sat X != 0"       ": T")
add_repl_test(sat_cmd-disjunct  "sat X = 0 || Y = 0" ": T")

# unsatisfiable formulas
add_repl_test(sat_cmd-F           "sat F"                  ": F")
add_repl_test(sat_cmd-contradiction "sat X = 0 && X != 0" ": F")

# with rec-relation defs
add_repl_test(sat_cmd-rr_pred "p(X) := X = 0. sat p(t)" ": T")

# history references
add_repl_test(sat_cmd-mem_rel  "T.   sat %-0"  "T")
add_repl_test(sat_cmd-mem_abs  "T.   sat %1"   "T")
add_repl_test(sat_cmd-mem_last "T.   sat %"    "T")
add_repl_test(sat_cmd-mem_F    "F.   sat %"    "F")

# temporal_connectives
add_repl_test(sat_cmd-tc-g_a_and_g_b_disjoint_outputs "sat (G (o1[t] != 0)) && (G (o2[t] != 0))." ": T")
add_repl_test(sat_cmd-tc-g_a_and_g_b_disjoint_outputs_equivalent_form "sat G ((o1[t] != 0) && (o2[t] != 0))." ": T")
add_repl_test(sat_cmd-tc-each_conjunct_in_isolation_is_sat_o1 "sat G (o1[t] != 0)." ": T")
add_repl_test(sat_cmd-tc-each_conjunct_in_isolation_is_sat_o2 "sat G (o2[t] != 0)." ": T")
add_repl_test(sat_cmd-tc-g_a_eq_0_and_g_b_eq_0_is_sat "sat (G (o1[t] = 0)) && (G (o2[t] = 0))." ": T")
add_repl_test(sat_cmd-tc-g_a_neq_0_and_g_a_eq_0_is_unsat_contradiction "sat (G (o1[t] != 0)) && (G (o1[t] = 0))." ": F")
add_repl_test(sat_cmd-tc-g_and_g_disjoint_variables_sat "sat (G (o1[t] = 0)) && (G (o2[t] = 1))." ": T")
add_repl_test(sat_cmd-tc-g_and_g_shared_variable_consistent_sat "sat (G (o1[t] != 0)) && (G (o1[t] = 1))." ": T")
add_repl_test(sat_cmd-tc-g_and_g_shared_variable_contradictory_unsat "sat (G (o1[t] = 0)) && (G (o1[t] = 1))." ": F")
add_repl_test(sat_cmd-tc-g_or_g_sat_either_witness "sat (G (o1[t] = 0)) || (G (o2[t] = 1))." ": T")
add_repl_test(sat_cmd-tc-g_or_g_same_variable_exclusive_choice_sat "sat (G (o1[t] = 0)) || (G (o1[t] = 1))." ": T")
add_repl_test(sat_cmd-tc-g_contradiction_or_g_satisfiable_sat_via_right_disjunct "sat ((G (o1[t] = 0)) && (G (o1[t] = 1))) || (G (o2[t] = 1))." ": T")
add_repl_test(sat_cmd-tc-not_g_contradiction_sat_negation_of_unsat "sat ! ((G (o1[t] = 0)) && (G (o1[t] = 1)))." ": T")
add_repl_test(sat_cmd-tc-not_g_or_g_sat_negation_of_sat_disjunction "sat ! ((G (o1[t] = 0)) || (G (o1[t] = 1)))." ": T")
add_repl_test(sat_cmd-tc-g_implies_g_implication_sat "sat (G (o1[t] = 0)) -> (G (o2[t] = 0))." ": T")
add_repl_test(sat_cmd-tc-g_a_iff_g_a_biconditional_same_arg_sat "sat (G (o1[t] = 1)) <-> (G (o1[t] = 1))." ": T")
add_repl_test(sat_cmd-tc-g_a_iff_f_a_same_a_hold_forever "sat (G (o1[t] = 1)) <-> (F (o1[t] = 1))." ": T")
add_repl_test(sat_cmd-tc-f_and_f_disjoint_variables_sat "sat (F (o1[t] = 1)) && (F (o2[t] = 1))." ": T")
add_repl_test(sat_cmd-tc-f_and_g_sat "sat (F (o1[t] = 1)) && (G (o2[t] = 1))." ": T")
add_repl_test(sat_cmd-tc-f_or_f_sat "sat (F (o1[t] = 1)) || (F (o2[t] = 1))." ": T")
add_repl_test(sat_cmd-tc-f_implies_g_sat "sat (F (o1[t] = 1)) -> (G (o2[t] = 0))." ": T")
add_repl_test(sat_cmd-tc-triple_g_and_g_and_g_disjoint_variables "sat (G (o1[t] = 0)) && (G (o2[t] = 1)) && (G (o3[t] = 0))." ": T")
add_repl_test(sat_cmd-tc-triple_g_and_g_and_g_one_contradictory_pair "sat (G (o1[t] = 0)) && (G (o1[t] = 1)) && (G (o3[t] = 0))." ": F")
add_repl_test(sat_cmd-tc-quadruple_g_with_non_g_conjunct "sat (G (o1[t] = 0)) && (G (o2[t] = 1)) && (G (o3[t] = 0)) && (G (o4[t] = 1))." ": T")
# XOR: the C++ case now carries a real CHECK; the "no XOR support" note was stale.
add_repl_test(sat_cmd-tc-g_xor_g_negated_arg_sat "sat (G (o1[t] = 0)) ^^ (G (o1[t] = 1))." ": T")

# AP1-1: a definition set whose expansion oscillates (`f(x) := f(x)'`, the
# same shape as the normalize_cmd regressions above) fails to normalize.
# `sat` reports the structured normalization error instead of deciding a
# verdict, so no ": T"/": F" line is printed.
# The diagnostic contains "Error", so the case needs NO_FAIL_REGEX.
add_repl_test(sat_cmd-oscillating_definition
	"f(x) := f(x)'. sat f(1) = 0" "definition expansion oscillates" NO_FAIL_REGEX)
# GitHub #72: a conjunction of N clauses with pairwise disjoint variable
# support used to be decided by a single Boole decomposition over the whole
# formula -- a 2^N Shannon expansion, since no branch ever simplified a
# sibling clause. N=15 took ~15 s and N=18 timed out before the per-component
# block split; each component is now decided on its own, so N=20 is instant.
add_repl_test(sat_cmd-issue72_disjoint_support_ladder
	"sat always ((o0[t]=1 -> o1[t]=1) && (o10[t]=1 -> o11[t]=1) && (o20[t]=1 -> o21[t]=1) && (o30[t]=1 -> o31[t]=1) && (o40[t]=1 -> o41[t]=1) && (o50[t]=1 -> o51[t]=1) && (o60[t]=1 -> o61[t]=1) && (o70[t]=1 -> o71[t]=1) && (o80[t]=1 -> o81[t]=1) && (o90[t]=1 -> o91[t]=1) && (o100[t]=1 -> o101[t]=1) && (o110[t]=1 -> o111[t]=1) && (o120[t]=1 -> o121[t]=1) && (o130[t]=1 -> o131[t]=1) && (o140[t]=1 -> o141[t]=1) && (o150[t]=1 -> o151[t]=1) && (o160[t]=1 -> o161[t]=1) && (o170[t]=1 -> o171[t]=1) && (o180[t]=1 -> o181[t]=1) && (o190[t]=1 -> o191[t]=1))"
	": T" NO_TRACE TIMEOUT 60)
# A spec without a temporal wrapper is implicitly `always`: a 2-bit stream
# cannot increase forever and the environment owns i1, so these are unsat
# like their explicit `always` spelling; `=` and `>=` are satisfiable
# controls.
add_repl_test(sat_cmd-issue137_bare_increase "sat o1[t]:bv[2] > o1[t-1]:bv[2]." ": F" TIMEOUT 60)
add_repl_test(sat_cmd-issue137_always_increase "sat always o1[t]:bv[2] > o1[t-1]:bv[2]." ": F" TIMEOUT 60)
add_repl_test(sat_cmd-issue137_bare_decrease "sat o1[t]:bv[3] < o1[t-1]:bv[3]." ": F" TIMEOUT 60)
add_repl_test(sat_cmd-issue137_bare_input "sat o1[t]:bv[2] > i1[t]:bv[2]." ": F" TIMEOUT 60)
add_repl_test(sat_cmd-issue137_bare_input_lookback "sat o1[t]:bv[2] > i1[t-1]:bv[2]." ": F" TIMEOUT 60)
add_repl_test(sat_cmd-issue137_bare_const_position "sat o1[t]:bv[2] = {1}:bv[2] && o1[1]:bv[2] = {2}:bv[2]." ": F" TIMEOUT 60)
add_repl_test(sat_cmd-issue137_bare_equal "sat o1[t]:bv[2] = o1[t-1]:bv[2]." ": T" TIMEOUT 60)
add_repl_test(sat_cmd-issue137_bare_input_geq "sat o1[t]:bv[2] >= i1[t]:bv[2]." ": T" TIMEOUT 60)
add_repl_test(sat_cmd-issue137_bare_unsat_fast "sat o1[t]:bv[2] > {3}:bv[2]." ": F" TIMEOUT 60)

# issue #130: equality substitution looped forever on a representative that
# contained its own class member and ended in std::bad_alloc
add_repl_test(sat_cmd-issue130_self_absorbing_x "sat x = x & y' && x' != 0" ": T" TIMEOUT 30)
add_repl_test(sat_cmd-issue130_self_absorbing_a "sat a = a & b && a' != 0" ": T" TIMEOUT 30)
add_repl_test(sat_cmd-issue130_negated_alias "sat x = x & y' && z = x'" ": T" TIMEOUT 30)
add_repl_test(sat_cmd-issue130_sbf "sat x:sbf = x:sbf & y:sbf' && x:sbf' != 0" ": T" TIMEOUT 30)
add_repl_test(sat_cmd-issue130_normalize "n x = x & y' && x' != 0" ": .*x' != 0" TIMEOUT 30)

# The Boole decomposition of x1 ^ ... ^ xn is a DAG with two distinct
# cofactors per level but 2^n paths. The decomposition, the BDD build of its
# result, the equality substitution over it and the quantifier renaming must
# each process a shared subterm once, or n=32 never finishes.
add_repl_test(sat_cmd-xor_ladder_sat
	"sat x1 ^ x2 ^ x3 ^ x4 ^ x5 ^ x6 ^ x7 ^ x8 ^ x9 ^ x10 ^ x11 ^ x12 ^ x13 ^ x14 ^ x15 ^ x16 ^ x17 ^ x18 ^ x19 ^ x20 ^ x21 ^ x22 ^ x23 ^ x24 ^ x25 ^ x26 ^ x27 ^ x28 ^ x29 ^ x30 ^ x31 ^ x32 = 0"
	": T" NO_TRACE TIMEOUT 60)
add_repl_test(sat_cmd-xor_ladder_valid
	"valid x1 ^ x2 ^ x3 ^ x4 ^ x5 ^ x6 ^ x7 ^ x8 ^ x9 ^ x10 ^ x11 ^ x12 ^ x13 ^ x14 ^ x15 ^ x16 ^ x17 ^ x18 ^ x19 ^ x20 ^ x21 ^ x22 ^ x23 ^ x24 ^ x25 ^ x26 ^ x27 ^ x28 ^ x29 ^ x30 ^ x31 ^ x32 = 0"
	": F" NO_TRACE TIMEOUT 60)
add_repl_test(sat_cmd-xor_ladder_quantified
	"valid all x1 ex x2 (x1 ^ x2 ^ x3 ^ x4 ^ x5 ^ x6 ^ x7 ^ x8 ^ x9 ^ x10 ^ x11 ^ x12 ^ x13 ^ x14 ^ x15 ^ x16 ^ x17 ^ x18 ^ x19 ^ x20 ^ x21 ^ x22 ^ x23 ^ x24 ^ x25 ^ x26 ^ x27 ^ x28 ^ x29 ^ x30 ^ x31 ^ x32 = 0)"
	": T" NO_TRACE TIMEOUT 60)

# a top-level sometimes, alone or next to an always, is decided, not
# rejected as a Boolean combination of models
add_repl_test(sat_cmd-issue144_sometimes_tautology "sat sometimes (o1[t] = 0 || o1[t] != 0)" ": T")
add_repl_test(sat_cmd-issue144_sometimes_atom "sat sometimes o1[t] = 1" ": T")
add_repl_test(sat_cmd-issue144_sometimes_contradiction "sat sometimes (o1[t] = 1 && o1[t] = 0)" ": F")
add_repl_test(sat_cmd-issue144_always_and_sometimes "sat (always o1[t] = 1) && (sometimes o1[t] = 1)" ": T")
add_repl_test(sat_cmd-issue144_always_and_sometimes_conflict "sat (always o1[t] = 1) && (sometimes o1[t] = 0)" ": F")
add_repl_test(sat_cmd-issue144_sometimes_and_sometimes "sat (sometimes o1[t] = 1) && (sometimes o1[t] = 0)" ": T")
add_repl_test(sat_cmd-issue144_not_sometimes "sat !(sometimes o1[t] = 1)" ": T")
add_repl_test(sat_cmd-issue144_sometimes_lookback "sat sometimes (o1[t] = 1 && o1[t-1] = 0)" ": T")
add_repl_test(sat_cmd-issue144_unsat_sometimes_atom "unsat sometimes o1[t] = 1" ": F")

# The verdict of an unsatisfiable temporal bv spec must not depend
# on its stream names (the bv definitional elimination keeps the readers of a
# definition it reuses from an earlier round)
add_repl_test(sat_cmd-issue143_o1_o2 "sat always (!(((o2[t-1]:bv[2] = o2[t]) -> (o2[t-2] = o1[t-2]:bv[2]))) && ((o1[t] = o2[t-2]) <-> (o1[t-2] != o2[t])))" ": F" TIMEOUT 60)
add_repl_test(sat_cmd-issue143_o2_o1 "sat always (!(((o1[t-1]:bv[2] = o1[t]) -> (o1[t-2] = o2[t-2]:bv[2]))) && ((o2[t] = o1[t-2]) <-> (o2[t-2] != o1[t])))" ": F" TIMEOUT 60)

# A sometimes target behind a delay chain of three or more stages is reached
# only after the initial segment the flag search covers directly, so the
# verdict rests on the reachability fixpoint of the flag
set(_chain3 "o1[0] = 0 && o2[0] = 0 && o3[0] = 0 && o4[0] = 0 && o2[t] = o1[t-1] && o3[t] = o2[t-1] && o4[t] = o3[t-1]")
set(_chain5 "o1[0] = 0 && o2[0] = 0 && o3[0] = 0 && o4[0] = 0 && o5[0] = 0 && o6[0] = 0 && o2[t] = o1[t-1] && o3[t] = o2[t-1] && o4[t] = o3[t-1] && o5[t] = o4[t-1] && o6[t] = o5[t-1]")
add_repl_test(sat_cmd-delay_chain3_sometimes_eq "sat (always (${_chain3})) && (sometimes o4[t] = 1)" ": T")
add_repl_test(sat_cmd-delay_chain3_sometimes_neq "sat (always (${_chain3})) && (sometimes o4[t] != 0)" ": T")
add_repl_test(sat_cmd-delay_chain3_unsat "unsat (always (${_chain3})) && (sometimes o4[t] = 1)" ": F")
add_repl_test(sat_cmd-delay_chain3_valid_negation "valid !((always (${_chain3})) && (sometimes o4[t] = 1))" ": F")
add_repl_test(sat_cmd-delay_chain3_blocked "sat (always (${_chain3} && o1[t] = 0)) && (sometimes o4[t] = 1)" ": F")
add_repl_test(sat_cmd-delay_chain5_sometimes_mid "sat (always (${_chain5})) && (sometimes o4[t] = 1)" ": T")
add_repl_test(sat_cmd-delay_chain5_sometimes_last "sat (always (${_chain5})) && (sometimes o6[t] = 1)" ": T")
add_repl_test(sat_cmd-delay_chain3_bv "sat (always (o1[0]:bv[8] = 0 && o2[0]:bv[8] = 0 && o3[0]:bv[8] = 0 && o4[0]:bv[8] = 0 && o2[t]:bv[8] = o1[t-1]:bv[8] && o3[t]:bv[8] = o2[t-1]:bv[8] && o4[t]:bv[8] = o3[t-1]:bv[8])) && (sometimes o4[t]:bv[8] = 1)" ": T")

# The inputs inside a sometimes are quantified universally, like every other
# input: the specification must reach the sometimes whatever the inputs do
add_repl_test(sat_cmd-sometimes_input_atom "sat sometimes i1[t] = 1" ": F")
add_repl_test(sat_cmd-sometimes_input_atom_unsat "unsat sometimes i1[t] = 1" ": T")
add_repl_test(sat_cmd-sometimes_input_lookback "sat sometimes i1[t-1] = 1" ": F")
add_repl_test(sat_cmd-always_output_sometimes_input "sat (always o1[t] = 1) && (sometimes i1[t] = 1)" ": F")
add_repl_test(sat_cmd-sometimes_output_and_input "sat (sometimes o1[t] = 1) && (sometimes i1[t] = 1)" ": F")
add_repl_test(sat_cmd-sometimes_output_copies_input "sat sometimes o1[t] = i1[t]" ": T")
add_repl_test(sat_cmd-sometimes_output_differs_from_input "sat sometimes o1[t] != i1[t]" ": T")
add_repl_test(sat_cmd-sometimes_output_reacts "sat (always o1[t] = i1[t]) && (sometimes (i1[t] = 1 -> o2[t] = 1))" ": T")
add_repl_test(sat_cmd-sometimes_constant_output_vs_input "sat (always o1[t] = 0) && (sometimes o1[t] = i1[t])" ": F")
# adding a conjunct never makes a specification satisfiable
add_repl_test(sat_cmd-sometimes_follower_output "sat (always o1[t] = i1[t]) && (sometimes o1[t] = 1)" ": F")
add_repl_test(sat_cmd-sometimes_follower_stronger "sat (always o1[t] = i1[t]) && (sometimes (i1[t] = 1 && o1[t] = 1))" ": F")
# sat agrees with realizable and with the full-LTL spelling
add_repl_test(sat_cmd-sometimes_input_realizable "realizable sometimes i1[t] = 1" ": F" REQUIRES ltlsynt)
add_repl_test(sat_cmd-sometimes_input_until "sat T U (i1[t] = 1)" ": F" REQUIRES ltlsynt)
# the step at which the sometimes holds may depend on the inputs: no single
# step forces it, but every input sequence reaches it
add_repl_test(sat_cmd-sometimes_input_dependent_step "sat sometimes (i1[t]:bv[1] = 1 || i1[t-1]:bv[1] = 0)" ": T" TIMEOUT 60)
add_repl_test(sat_cmd-sometimes_input_dependent_step2 "sat sometimes (i1[t-1]:bv[1] = 1 || i1[t-2]:bv[1] = 0)" ": T" TIMEOUT 60)
add_repl_test(sat_cmd-sometimes_input_constant_avoids "sat sometimes i1[t]:bv[1] != i1[t-1]:bv[1]" ": F" TIMEOUT 60)
add_repl_test(sat_cmd-sometimes_inputs_two_goals "sat (always ((o2[t-1]:bv[1] = 1 -> o1[t-1]:bv[1] = 1))) && (sometimes (o1[t]:bv[1] != i2[t]:bv[1])) && (sometimes (i2[t]:bv[1] = 0 && o1[t]:bv[1] = o1[t-1]:bv[1]'))" ": F" TIMEOUT 60)

# Each clause is enforced from its own deepest lookback: an always part that
# reads the past asks nothing during its warm-up, where a sometimes part that
# reads less far back may already hold
add_repl_test(sat_cmd-warm_up_sometimes_first_step "sat (always o1[t-1] = 1 && !(o2[t] = o1[t-1])) && (sometimes o2[t] = 1)" ": T")
add_repl_test(sat_cmd-warm_up_sometimes_output "sat (always o2[t] = 0 && o1[t-1] = 1) && (sometimes o2[t] = 1)" ": T")
add_repl_test(sat_cmd-warm_up_realizable "realizable (always o2[t] = 0 && o1[t-1] = 1) && (sometimes o2[t] = 1)" ": T" REQUIRES ltlsynt)
add_repl_test(sat_cmd-warm_up_continuation "sat (always (i2[t] = o1[t-2] || o1[t-1] = 0) && o1[t-1] = o1[t]) && (sometimes i1[t] != o1[t])" ": T")
add_repl_test(sat_cmd-warm_up_same_lookback "sat (always o2[t] = 0 && o1[t-1] = 1) && (sometimes (o2[t] = 1 && o1[t-1] = 1))" ": F")
add_repl_test(sat_cmd-warm_up_none "sat (always o2[t] = 0) && (sometimes o2[t] = 1)" ": F")
# The warm-up is the lookback a clause is written with, a literal that
# normalization drops included: the always parts below leave o2 free at steps
# 0 and 1
add_repl_test(sat_cmd-warm_up_tautology "sat (always o2[t] = 1 && o1[t-2] = o1[t-2]) && (sometimes o2[t-1] = 0)" ": T")
add_repl_test(sat_cmd-warm_up_tautology_unsat "unsat (always o2[t] = 1 && o1[t-2] = o1[t-2]) && (sometimes o2[t-1] = 0)" ": F")
add_repl_test(sat_cmd-warm_up_tautology_complement "sat (always o2[t] = 1 && (o1[t-2] | o1[t-2]') = 1) && (sometimes o2[t-1] = 0)" ": T")
add_repl_test(sat_cmd-warm_up_tautology_disjunction "sat (always o2[t] = 1 && (o1[t-2] = 0 || o1[t-2] != 0)) && (sometimes o2[t-1] = 0)" ": T")
add_repl_test(sat_cmd-warm_up_tautology_own_always "sat (always o2[t] = 1) && (always o1[t-2] = o1[t-2]) && (sometimes o2[t-1] = 0)" ": T")
add_repl_test(sat_cmd-warm_up_absorbed "sat (always o2[t] = 1) && (always (o2[t] = 1 || o1[t-2] = 0)) && (sometimes o2[t-1] = 0)" ": T")
add_repl_test(sat_cmd-warm_up_tautology_input "sat (always o2[t] = 1 && i1[t-1] = i1[t-1]) && (sometimes o2[t] = 0)" ": T")
add_repl_test(sat_cmd-warm_up_tautology_bv "sat (always o2[t]:bv[1] = 1 && o1[t-2]:bv[1] = o1[t-2]:bv[1]) && (sometimes o2[t-1]:bv[1] = 0)" ": T")
add_repl_test(sat_cmd-warm_up_tautology_realizable "realizable (always o2[t] = 1 && o1[t-2] = o1[t-2]) && (sometimes o2[t-1] = 0)" ": T" REQUIRES ltlsynt)
add_repl_test(sat_cmd-warm_up_tautology_until "sat (G (o2[t] = 1 && o1[t-2] = o1[t-2])) && (T U (o2[t-1] = 0))" ": T" REQUIRES ltlsynt)
# in a sometimes clause it delays the first step the clause may hold at
add_repl_test(sat_cmd-warm_up_tautology_sometimes "sat (always o2[t] = 1 && o3[t-1] = 0) && (sometimes (o2[t] = 0 && o1[t-1] = o1[t-1]))" ": F")
# a sometimes part implied by another one is still asked from its own warm-up
add_repl_test(sat_cmd-warm_up_implied_sometimes "sat (always o1[0] = 0 && o1[t] = 0 && o2[t] = 1 && o3[t-1] = 0) && (sometimes o2[t] = 0) && (sometimes (o2[t] = 0 || o1[t-1] = 1))" ": F")
# valid decides the negation with the same warm-ups: valid !φ agrees with
# sat φ, and an implication sees the later start of the always part
add_repl_test(sat_cmd-warm_up_tautology_valid_negation "valid !((always o2[t] = 1 && o1[t-2] = o1[t-2]) && (sometimes o2[t-1] = 0))" ": F")
add_repl_test(sat_cmd-warm_up_absorbed_valid_negation "valid !((always o2[t] = 1) && (always (o2[t] = 1 || o1[t-2] = 0)) && (sometimes o2[t-1] = 0))" ": F")
add_repl_test(sat_cmd-warm_up_tautology_valid_implied "valid (always o2[t] = 1) -> (always o2[t] = 1 && o1[t-2] = o1[t-2])" ": T")
add_repl_test(sat_cmd-warm_up_tautology_valid_not_implied "valid (always o2[t] = 1 && o1[t-2] = o1[t-2]) -> (always o2[t] = 1)" ": F")
# the always statements of a negated specification still form one always
# part: o2[0] is free, so the sometimes can hold at step 1
add_repl_test(sat_cmd-warm_up_merged_always_valid_negation "valid !((always o2[t] = 1) && (always o2[t-1] != o1[t-1]) && (sometimes (o2[t-1] = o2[t]')))" ": F")
# a negated sometimes is asked from its own warm-up on: o2 may be 0 at step 0
add_repl_test(sat_cmd-warm_up_tautology_sat_negation "sat !(sometimes (o2[t] = 0 && o1[t-1] = o1[t-1])) && (sometimes o2[t] = 0)" ": T")
add_repl_test(sat_cmd-warm_up_tautology_sometimes_realizable "realizable (always o2[t] = 1 && o3[t-1] = 0) && (sometimes (o2[t] = 0 && o1[t-1] = o1[t-1]))" ": F")

# An 8-bit output that falls every step and stays above four times its last
# value runs out of values only after a long unrolling, whose fixpoint checks
# nest one quantifier per step: the bits of the values decide them.
add_repl_test(sat_cmd-bv_falling_output "sat always o1[t]:bv[8] < o1[t-1]:bv[8] && {4}:bv[8] * o1[t-1]:bv[8] < o1[t]:bv[8]" ": F" TIMEOUT 120)
# A product under alternating quantifiers: whatever r is, s = -r leaves no u
# with s - u < s + r = 0.
add_repl_test(sat_cmd-bv_alternating_product "sat all p:bv[8] ex q:bv[8] ex r:bv[8] all s:bv[8] ex u:bv[8] (r * q != p && s - u < s + r)" ": F" TIMEOUT 120)
# Products of two 16-bit values under an until and of two 8-bit values of
# different steps: questions that stop on their time budget, not hang.
add_repl_budget_test(sat_cmd-bv_budget_wide_product
	"sat (((i1[t]:bv[16] = (o2[t-1]:bv[16] + o1[t]:bv[16])) && (o2[t]:bv[16] != i1[t]:bv[16])) U ((i1[t]:bv[16] = o2[t-1]:bv[16]) || ((o2[t]:bv[16] * o1[t]:bv[16]) < (o1[t]:bv[16] + o1[t]:bv[16]))))")
add_repl_budget_test(sat_cmd-bv_budget_product_of_steps
	"sat (always (((o1[t-1]:bv[8] - i1[t-1]:bv[8]) <= (o1[t]:bv[8] + i1[t-1]:bv[8])) || (i1[t-1]:bv[8] = o1[t]:bv[8]))) && (sometimes (((o1[t-1]:bv[8] - o1[t-1]:bv[8]) <= (o1[t-1]:bv[8] * {0}:bv[8])) && (o1[t-1]:bv[8] <= (i1[t]:bv[8] * i1[t-1]:bv[8])))) && (sometimes ((i1[t-1]:bv[8] - i1[t-1]:bv[8]) < o1[t-1]:bv[8]))")
# asked twice, the question stops on its budget twice: nothing the first
# command computed from the missing answer serves the second
add_repl_budget_test(sat_cmd-bv_budget_asked_twice
	"sat (G ((o2[t]:bv[16] * o2[t]:bv[16]) <= i1[t-1]:bv[16])) && (F ((i1[t]:bv[16] + o2[t]:bv[16]) < (i1[t-1]:bv[16] + o2[t-1]:bv[16]))). sat (G ((o2[t]:bv[16] * o2[t]:bv[16]) <= i1[t-1]:bv[16])) && (F ((i1[t]:bv[16] + o2[t]:bv[16]) < (i1[t-1]:bv[16] + o2[t-1]:bv[16])))")
if(TEST "test_repl-sat_cmd-bv_budget_asked_twice")
	set_tests_properties("test_repl-sat_cmd-bv_budget_asked_twice" PROPERTIES
		PASS_REGULAR_EXPRESSION "passed its time budget.*passed its time budget")
endif()

# A ground qlt formula over named endpoints that holds wherever they lie.
add_repl_test(sat_cmd-qlt_named_ground_tautology
	"sat {c}:qlt < {d}:qlt || {d}:qlt <= {c}:qlt" "%1[^%]*: T")

# A time constraint that never holds once its clause applies is reported: the
# clause reads o1[t-1], so it starts at step 1 and [t = 0] has no effect
# (GitHub #204). One that still holds there, or whose negation does, is not.
add_repl_test(sat_cmd-dead_time_constraint_warns
	"sat always ([t = 0] -> o1[t] = 1) && ([t >= 1] -> o1[t] = o1[t-1])"
	"never holds.*\\[t = 0\\].*%1[^%]*: T")
add_repl_test(sat_cmd-dead_time_constraint_lt
	"sat always ([t < 2] -> o1[t] = 1) && o1[t] = o1[t-2]"
	"never holds.*\\[t < 2\\]")
add_repl_test(sat_cmd-live_time_constraint_quiet
	"sat always ([t < 3] -> o1[t] = 1) && o1[t] = o1[t-2]"
	"%1[^%]*: T" FAIL_REGEX "never holds")
add_repl_test(sat_cmd-time_constraint_at_lookback_quiet
	"sat always ([t >= 2] -> o1[t] = o1[t-2])"
	"%1[^%]*: T" FAIL_REGEX "never holds")
add_repl_test(sat_cmd-time_constraint_without_lookback_quiet
	"sat always ([t = 0] -> o1[t] = 1) && ([t = 1] -> o1[t] = 1)"
	"%1[^%]*: T" FAIL_REGEX "never holds")
