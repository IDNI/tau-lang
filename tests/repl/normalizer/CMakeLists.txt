cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# In these tests cases we check the results obtained in the normalized command.
# The output of the tests cases must be validated against expected results.

#
# sample
#

#add_test(NAME test_repl-quit_cmd-full
#	COMMAND bash -c "echo 'quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-quit_cmd-full PROPERTIES
#	PASS_REGULAR_EXPRESSION "Quit.")
#add_test(NAME test_repl-quit_cmd-shortened
#	COMMAND bash -c "echo 'q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-quit_cmd-shortened PROPERTIES
#	PASS_REGULAR_EXPRESSION "Quit.")
