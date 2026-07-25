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
