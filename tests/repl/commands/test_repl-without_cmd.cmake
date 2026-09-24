# without: the normal form of an always-conjunction without one of its
# conjuncts. The remainders are chosen so that their printed form has one
# order (a single unit, or a unit chain whose order the comparator fixes);
# a unit against 0 and a unit against "not 0" are both removed, a clause
# is removed from beside a unit, the only conjunct leaves T, a clause that
# is no conjunct leaves the formula, a history reference is accepted, and
# a formula that is no always-conjunction comes back normalized.
add_repl_test(without_cmd-unit_neq_removed    "set charvar off. without (always (o1[t] = 0 && o2[t] != 0)) (always o2[t] != 0)" ": always o1\\[t\\]:tau = 0\n")
add_repl_test(without_cmd-unit_eq_removed     "set charvar off. without (always (o1[t] = 0 && o2[t] != 0)) (always o1[t] = 0)" ": always o2\\[t\\]:tau != 0\n")
add_repl_test(without_cmd-clause_removed      "set charvar off. without (always (o1[t] = 0 && (o2[t] = 0 || o3[t] != 0))) (always (o2[t] = 0 || o3[t] != 0))" ": always o1\\[t\\]:tau = 0\n")
add_repl_test(without_cmd-only_conjunct       "set charvar off. without (always o1[t] = 0) (always o1[t] = 0)" ": T\n")
# The printed order of the two remaining conjuncts is not pinned across
# platforms, so both orders are accepted.
add_repl_test(without_cmd-absent_clause       "set charvar off. without (always (o1[t] = 0 && o2[t] != 0)) (always o3[t] = 0)" ": always (o1\\[t\\]:tau = 0 && o2\\[t\\]:tau != 0|o2\\[t\\]:tau != 0 && o1\\[t\\]:tau = 0)\n")
add_repl_test(without_cmd-from_history        "set charvar off. normalize (always (o1[t] = 0 && o2[t] != 0 && (o3[t] = 0 || o4[t] = 0))). without %1 (always (o3[t] = 0 || o4[t] = 0))" ": always (o1\\[t\\]:tau = 0 && o2\\[t\\]:tau != 0|o2\\[t\\]:tau != 0 && o1\\[t\\]:tau = 0)\n")
add_repl_test(without_cmd-not_a_conjunction   "set charvar off. without (sometimes o1[t] = 0) (always o1[t] = 0)" ": F o1\\[t\\]:tau = 0\n")
# The shadow mode runs the pipeline over the remainder as well and reports
# at exit; the removal must have been assembled and must agree with it.
add_test(NAME "test_repl-without_cmd-shadow_agrees"
	COMMAND bash -c "TAU_BA_NORMALIZED_WITHOUT=2 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"set charvar off. without (always (o1[t] = 0 && o2[t] != 0 && (o3[t] = 0 || o4[t] = 0))) (always o2[t] != 0)\" -S trace 2>&1")
set_tests_properties("test_repl-without_cmd-shadow_agrees" PROPERTIES
	PASS_REGULAR_EXPRESSION "tau_ba normalized without shadow: hits 1 \\(shaped 1\\), mismatches 0"
	FAIL_REGULAR_EXPRESSION "Error"
)
