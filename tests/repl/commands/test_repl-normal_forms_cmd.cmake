#
# normal forms commands
#

# dnf command
add_test(NAME test_repl-dnf_bf_cmd
	COMMAND bash -c "echo 'dnf X & (Y | Z). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-dnf_bf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "X & Y | X & Z")
add_test(NAME test_repl-dnf_wff_cmd
	COMMAND bash -c "echo 'dnf (X = 0) && ((Y=0) || (Z=0)). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-dnf_wff_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "X = 0 && Y = 0 || X = 0 && Z = 0")

# cnf command
add_test(NAME test_repl-cnf_bf_cmd
	COMMAND bash -c "echo 'cnf X | (Y & Z). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-cnf_bf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")
add_test(NAME test_repl-cnf_wff_cmd
	COMMAND bash -c "echo 'cnf (X = 0) || ((Y=0) && (Z=0)). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-cnf_wff_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")

# nnf command
#add_test(NAME test_repl-nnf_bf_cmd
#	COMMAND bash -c "echo 'nnf (Y | Z)'. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-nnf_bf_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "Quit.")
add_test(NAME test_repl-nnf_wff_cmd
	COMMAND bash -c "echo 'nnf !((Y=0) || (Z=0)). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-nnf_wff_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")

# mnf command
# TODO (HIGH) fixme it's not working properly, returns Y & X |Y & X | Z & X
add_test(NAME test_repl-mnf_bf_cmd
	COMMAND bash -c "echo 'mnf X & (Y | Z). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-mnf_bf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")
add_test(NAME test_repl-mnf_wff_cmd
	COMMAND bash -c "echo 'mnf (X = 0) && ((Y=0) || (Z=0)). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-mnf_wff_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")

# onf command(only wff)
# TODO (HIGH) fixme it's not working properly, returns (X = 0) && (Y = 0 || Z = 0) again...
#add_test(NAME test_repl-onf_wff_cmd
#	COMMAND bash -c "echo 'onf X (X = 0) && ((Y=0) || (Z=0)). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-onf_wff_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "onf command converts")
