#
# normal forms commands
#

# dnf command
add_repl_test(dnf_bf_cmd "dnf X & (Y | Z)" ". & . | . & .")
add_repl_test(dnf_wff_cmd "dnf (X = 0) && ((Y=0) || (Z=0)). q" "X = 0 && Y = 0 || X = 0 && Z = 0")

# cnf command
add_repl_test(cnf_bf_cmd "cnf X | (Y & Z)" "\\(. | .\\) & \\(. | .\\)")
add_repl_test(cnf_wff_cmd "cnf (X = 0) || ((Y=0) && (Z=0))" "\\(X = 0 || Y = 0\\) && \\(X = 0 || Z = 0\\)")

# nnf command
#add_repl_test(nnf_bf_cmd "nnf (Y | Z)" "Y | Z")
add_repl_test(nnf_wff_cmd "nnf !((Y=0) || (Z=0))" "Y != 0 && Z != 0")

# mnf command
# TODO (HIGH) fixme it's not working properly, returns Y & X |Y & X | Z & X
add_repl_test(mnf_bf_cmd "mnf X & (Y | Z)" ". & . | . & .")
add_repl_test(mnf_wff_cmd "mnf (X = 0) && ((Y=0) || (Z=0))" "X = 0 && Y = 0 || X = 0 && Z = 0")

# onf command(only wff)
# TODO (HIGH) fixme it's not working properly, returns (X = 0) && (Y = 0 || Z = 0) again...
#add_test(NAME test_repl-onf_wff_cmd
#	COMMAND bash -c "echo 'onf X (X = 0) && ((Y=0) || (Z=0)). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-onf_wff_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "onf command converts")
