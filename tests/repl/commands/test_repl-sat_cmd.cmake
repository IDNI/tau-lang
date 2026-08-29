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
# add_repl_test_fail is used because the diagnostic contains "Error".
add_repl_test_fail(sat_cmd-oscillating_definition
	"f(x) := f(x)'. sat f(1) = 0" "Definition expansion oscillates")
# GitHub #72: a conjunction of N clauses with pairwise disjoint variable
# support used to be decided by a single Boole decomposition over the whole
# formula -- a 2^N Shannon expansion, since no branch ever simplified a
# sibling clause. N=15 took ~15 s and N=18 timed out before the per-component
# block split; each component is now decided on its own, so N=20 is instant.
add_test(NAME "test_repl-sat_cmd-issue72_disjoint_support_ladder"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"sat always ((o0[t]=1 -> o1[t]=1) && (o10[t]=1 -> o11[t]=1) && (o20[t]=1 -> o21[t]=1) && (o30[t]=1 -> o31[t]=1) && (o40[t]=1 -> o41[t]=1) && (o50[t]=1 -> o51[t]=1) && (o60[t]=1 -> o61[t]=1) && (o70[t]=1 -> o71[t]=1) && (o80[t]=1 -> o81[t]=1) && (o90[t]=1 -> o91[t]=1) && (o100[t]=1 -> o101[t]=1) && (o110[t]=1 -> o111[t]=1) && (o120[t]=1 -> o121[t]=1) && (o130[t]=1 -> o131[t]=1) && (o140[t]=1 -> o141[t]=1) && (o150[t]=1 -> o151[t]=1) && (o160[t]=1 -> o161[t]=1) && (o170[t]=1 -> o171[t]=1) && (o180[t]=1 -> o181[t]=1) && (o190[t]=1 -> o191[t]=1))\"")
set_tests_properties("test_repl-sat_cmd-issue72_disjoint_support_ladder" PROPERTIES
	PASS_REGULAR_EXPRESSION ": T"
	FAIL_REGULAR_EXPRESSION "Error"
	TIMEOUT 60)
