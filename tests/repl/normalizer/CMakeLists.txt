cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# In these tests cases we check the results obtained in the normalized command.
# The output of the tests cases must be validated against expected results.

add_test(NAME test_repl-normalize_cmd_bf
	COMMAND bash -c "echo 'n X & 1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normalize_cmd_bf PROPERTIES
	PASS_REGULAR_EXPRESSION ": X")

add_test(NAME test_repl-normalize_cmd_nso
		COMMAND bash -c "echo \"n X & X' = 0. q\" | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normalize_cmd_nso PROPERTIES
		PASS_REGULAR_EXPRESSION ": T")

#TODO
add_test(NAME test_repl-normalize_cmd_wff
		COMMAND bash -c "echo \"n X & X' = 0. q\" | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normalize_cmd_wff PROPERTIES
		PASS_REGULAR_EXPRESSION ": T")

#TODO
add_test(NAME test_repl-normalize_cmd_memory
		COMMAND bash -c "echo \"n X & X' = 0. q\" | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normalize_cmd_memory PROPERTIES
		PASS_REGULAR_EXPRESSION ": T")

