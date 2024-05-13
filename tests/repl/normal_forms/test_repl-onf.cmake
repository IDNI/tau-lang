#
# order normal form tests
#

# T
add_test(NAME test_repl-normal_forms-onf_T
	COMMAND bash -c "echo 'onf X T. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-onf_T PROPERTIES
	PASS_REGULAR_EXPRESSION "T")

# F
add_test(NAME test_repl-normal_forms-onf_F
	COMMAND bash -c "echo 'onf X F. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-onf_F PROPERTIES
	PASS_REGULAR_EXPRESSION "F")

# X=0
add_test(NAME test_repl-normal_forms-onf_X_eq_0
	COMMAND bash -c "echo 'onf X (X = 0). quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-onf_X_eq_0 PROPERTIES
	PASS_REGULAR_EXPRESSION "F")

# Y=0
add_test(NAME test_repl-normal_forms-onf_Y_eq_0
	COMMAND bash -c "echo 'onf X (Y = 0). quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-onf_Y_eq_0 PROPERTIES
	PASS_REGULAR_EXPRESSION "Y")

# X!=0
add_test(NAME test_repl-normal_forms-onf_X_neq_0
	COMMAND bash -c "echo 'onf X (X != 0). quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-onf_X_neq_0 PROPERTIES
	PASS_REGULAR_EXPRESSION "F")

# Y!=0
add_test(NAME test_repl-normal_forms-onf_Y_neq_0
	COMMAND bash -c "echo 'onf X (Y != 0). quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-onf_Y_neq_0 PROPERTIES
	PASS_REGULAR_EXPRESSION "Y")

