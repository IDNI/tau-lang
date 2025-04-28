#
# normal forms commands
#

include(add_repl_test)

# dnf command
add_repl_test(normal_forms-dnf_wff "dnf T" ": T")
add_repl_test(normal_forms-dnf_wff_mem_abs "T. dnf %0" "T")
add_repl_test(normal_forms-dnf_wff_mem_rel "T. dnf %-0" "T")

add_repl_test(normal_forms-dnf_bf "dnf 1" ": 1")
add_repl_test(normal_forms-dnf_bf_mem_abs "1. dnf %0" "1")
add_repl_test(normal_forms-dnf_bf_mem_rel "1. dnf %-0" "1")

# cnf command
add_repl_test(normal_forms-cnf_wff "cnf T" ": T")
add_repl_test(normal_forms-cnf_wff_mem_abs "T. cnf %0" "T")
add_repl_test(normal_forms-cnf_wff_mem_rel "T. cnf %-0" "T")

add_repl_test(normal_forms-cnf_bf "cnf 1" ": 1")
add_repl_test(normal_forms-cnf_bf_mem_abs "1. cnf %0" "1")
add_repl_test(normal_forms-cnf_bf_mem_rel "1. cnf %-0" "1")

# nnf command
add_repl_test(normal_forms-nnf_wff "nnf T" ": T")
add_repl_test(normal_forms-nnf_wff_mem_abs "T. nnf %0" "T")
add_repl_test(normal_forms-nnf_wff_mem_rel "T. nnf %-0" "T")

add_repl_test(normal_forms-nnf_bf "nnf 1" ": 1")
add_repl_test(normal_forms-nnf_bf_mem_abs "1. nnf %0" "1")
add_repl_test(normal_forms-nnf_bf_mem_rel "1. nnf %-0" "1")

# mnf command
# TODO (HIGH) fixme it's not working properly, returns Y & X |Y & X | Z & X
add_repl_test(normal_forms-mnf_wff "mnf T" ": T")
add_repl_test(normal_forms-mnf_wff_mem_abs "T. mnf %0" "T")
add_repl_test(normal_forms-mnf_wff_mem_rel "T. mnf %-0" "T")

add_repl_test(normal_forms-mnf_bf "mnf T" ": T")
add_repl_test(normal_forms-mnf_bf_mem_abs "T. mnf %0" "T")
add_repl_test(normal_forms-mnf_bf_mem_rel "T. mnf %-0" "T")

# onf command(only wff)
# TODO (HIGH) fixme it's not working properly, returns (X = 0) && (Y = 0 || Z = 0) again...
#add_test(NAME test_repl-onf_wff_cmd
#	COMMAND bash -c "echo 'onf X (X = 0) && ((Y=0) || (Z=0)). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-onf_wff_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "onf command converts")

# snf command
add_repl_test(normal_forms-snf-wff "snf T" "T")
add_repl_test(normal_forms-snf-wff_mem_abs "T. snf %0" "T")
add_repl_test(normal_forms-snf-wff_mem_rel "T. snf %-0" "T")

add_repl_test(normal_forms-snf-bf "snf 1" "1")
add_repl_test(normal_forms-snf-bf_mem_abs "1. snf %0" "1")
add_repl_test(normal_forms-snf-bf_mem_rel "1. snf %-0" "1")