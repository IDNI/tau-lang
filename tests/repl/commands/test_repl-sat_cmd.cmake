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

# A spec without a temporal wrapper is implicitly `always`: a 2-bit stream
# cannot increase forever and the environment owns i1, so these are unsat
# like their explicit `always` spelling; `=` and `>=` are satisfiable
# controls.
add_repl_test(sat_cmd-issue137_bare_increase "sat o1[t]:bv[2] > o1[t-1]:bv[2]." ": F")
add_repl_test(sat_cmd-issue137_always_increase "sat always o1[t]:bv[2] > o1[t-1]:bv[2]." ": F")
add_repl_test(sat_cmd-issue137_bare_decrease "sat o1[t]:bv[3] < o1[t-1]:bv[3]." ": F")
add_repl_test(sat_cmd-issue137_bare_input "sat o1[t]:bv[2] > i1[t]:bv[2]." ": F")
add_repl_test(sat_cmd-issue137_bare_input_lookback "sat o1[t]:bv[2] > i1[t-1]:bv[2]." ": F")
add_repl_test(sat_cmd-issue137_bare_const_position "sat o1[t]:bv[2] = {1}:bv[2] && o1[1]:bv[2] = {2}:bv[2]." ": F")
add_repl_test(sat_cmd-issue137_bare_equal "sat o1[t]:bv[2] = o1[t-1]:bv[2]." ": T")
add_repl_test(sat_cmd-issue137_bare_input_geq "sat o1[t]:bv[2] >= i1[t]:bv[2]." ": T")
add_repl_test(sat_cmd-issue137_bare_unsat_fast "sat o1[t]:bv[2] > {3}:bv[2]." ": F")
foreach(_t bare_increase always_increase bare_decrease bare_input
		bare_input_lookback bare_const_position bare_equal bare_input_geq
		bare_unsat_fast)
	if(TEST "test_repl-sat_cmd-issue137_${_t}")
		set_tests_properties("test_repl-sat_cmd-issue137_${_t}"
			PROPERTIES TIMEOUT 60)
	endif()
endforeach()

# issue #130: equality substitution looped forever on a representative that
# contained its own class member and ended in std::bad_alloc
add_repl_test(sat_cmd-issue130_self_absorbing_x "sat x = x & y' && x' != 0" ": T")
add_repl_test(sat_cmd-issue130_self_absorbing_a "sat a = a & b && a' != 0" ": T")
add_repl_test(sat_cmd-issue130_negated_alias "sat x = x & y' && z = x'" ": T")
add_repl_test(sat_cmd-issue130_sbf "sat x:sbf = x:sbf & y:sbf' && x:sbf' != 0" ": T")
add_repl_test(sat_cmd-issue130_normalize "n x = x & y' && x' != 0" ": .*x' != 0")
foreach(_t self_absorbing_x self_absorbing_a negated_alias sbf normalize)
	if(TEST "test_repl-sat_cmd-issue130_${_t}")
		set_tests_properties("test_repl-sat_cmd-issue130_${_t}"
			PROPERTIES TIMEOUT 30)
	endif()
endforeach()

# The Boole decomposition of x1 ^ ... ^ xn is a DAG with two distinct
# cofactors per level but 2^n paths. The decomposition, the BDD build of its
# result, the equality substitution over it and the quantifier renaming must
# each process a shared subterm once, or n=32 never finishes.
add_test(NAME "test_repl-sat_cmd-xor_ladder_sat"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"sat x1 ^ x2 ^ x3 ^ x4 ^ x5 ^ x6 ^ x7 ^ x8 ^ x9 ^ x10 ^ x11 ^ x12 ^ x13 ^ x14 ^ x15 ^ x16 ^ x17 ^ x18 ^ x19 ^ x20 ^ x21 ^ x22 ^ x23 ^ x24 ^ x25 ^ x26 ^ x27 ^ x28 ^ x29 ^ x30 ^ x31 ^ x32 = 0\"")
set_tests_properties("test_repl-sat_cmd-xor_ladder_sat" PROPERTIES
	PASS_REGULAR_EXPRESSION ": T"
	FAIL_REGULAR_EXPRESSION "Error"
	TIMEOUT 60)
add_test(NAME "test_repl-sat_cmd-xor_ladder_valid"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"valid x1 ^ x2 ^ x3 ^ x4 ^ x5 ^ x6 ^ x7 ^ x8 ^ x9 ^ x10 ^ x11 ^ x12 ^ x13 ^ x14 ^ x15 ^ x16 ^ x17 ^ x18 ^ x19 ^ x20 ^ x21 ^ x22 ^ x23 ^ x24 ^ x25 ^ x26 ^ x27 ^ x28 ^ x29 ^ x30 ^ x31 ^ x32 = 0\"")
set_tests_properties("test_repl-sat_cmd-xor_ladder_valid" PROPERTIES
	PASS_REGULAR_EXPRESSION ": F"
	FAIL_REGULAR_EXPRESSION "Error"
	TIMEOUT 60)
add_test(NAME "test_repl-sat_cmd-xor_ladder_quantified"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"valid all x1 ex x2 (x1 ^ x2 ^ x3 ^ x4 ^ x5 ^ x6 ^ x7 ^ x8 ^ x9 ^ x10 ^ x11 ^ x12 ^ x13 ^ x14 ^ x15 ^ x16 ^ x17 ^ x18 ^ x19 ^ x20 ^ x21 ^ x22 ^ x23 ^ x24 ^ x25 ^ x26 ^ x27 ^ x28 ^ x29 ^ x30 ^ x31 ^ x32 = 0)\"")
set_tests_properties("test_repl-sat_cmd-xor_ladder_quantified" PROPERTIES
	PASS_REGULAR_EXPRESSION ": T"
	FAIL_REGULAR_EXPRESSION "Error"
	TIMEOUT 60)

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
add_repl_test(sat_cmd-issue143_o1_o2 "sat always (!(((o2[t-1]:bv[2] = o2[t]) -> (o2[t-2] = o1[t-2]:bv[2]))) && ((o1[t] = o2[t-2]) <-> (o1[t-2] != o2[t])))" ": F")
add_repl_test(sat_cmd-issue143_o2_o1 "sat always (!(((o1[t-1]:bv[2] = o1[t]) -> (o1[t-2] = o2[t-2]:bv[2]))) && ((o2[t] = o1[t-2]) <-> (o2[t-2] != o1[t])))" ": F")
foreach(_t o1_o2 o2_o1)
	if(TEST "test_repl-sat_cmd-issue143_${_t}")
		set_tests_properties("test_repl-sat_cmd-issue143_${_t}"
			PROPERTIES TIMEOUT 60)
	endif()
endforeach()

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
# An always-conjunction over streams is decided by its stream-disjoint
# components (api.tmpl.h, sat_prepared): each component on its own, the
# verdicts conjoined. Pin sat and unsat over two and three components, an
# unsatisfiable component next to satisfiable ones, and two controls whose
# conjuncts share a stream (one component, the whole-formula decision).
add_repl_test(sat_cmd-components_two_sat        "set charvar off. sat (always o1[t] = 0) && (always (o2[t] != 0 || o3[t] = 0))" ": T")
add_repl_test(sat_cmd-components_three_sat      "set charvar off. sat (always o1[t] = 0) && (always o2[t] != 0) && (always (o3[t] = 0 || o4[t] != 0))" ": T")
add_repl_test(sat_cmd-components_one_unsat      "set charvar off. sat (always o1[t] = 0) && (always o2[t] != 0) && (always (o2[t] = 0 && o3[t] = 0))" ": F")
add_repl_test(sat_cmd-components_shared_stream  "set charvar off. sat (always (o1[t] = 0 || o2[t] = 0)) && (always (o1[t] != 0 || o2[t] != 0))" ": T")
add_repl_test(sat_cmd-components_shared_unsat   "set charvar off. sat (always o1[t] = 0) && (always (o1[t] != 0 || o2[t] = 0)) && (always o2[t] != 0)" ": F")
# The shadow mode runs both decisions and reports at exit; the factored one
# must have been taken and must agree with the whole-formula verdict.
add_test(NAME "test_repl-sat_cmd-components_shadow_agrees"
	COMMAND bash -c "TAU_API_SAT_FACTORED=2 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"set charvar off. sat (always o1[t] = 0) && (always (o2[t] != 0 || o3[t] = 0))\" -S trace 2>&1")
set_tests_properties("test_repl-sat_cmd-components_shadow_agrees" PROPERTIES
	PASS_REGULAR_EXPRESSION "api sat factored shadow: hits [1-9][0-9]*, mismatches 0"
	FAIL_REGULAR_EXPRESSION "Error"
)
