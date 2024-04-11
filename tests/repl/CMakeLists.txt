cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# quit command
add_test(NAME test_repl-quit_cmd-full
	COMMAND bash -c "echo 'quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-quit_cmd-full PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")

add_test(NAME test_repl-quit_cmd-shortened
	COMMAND bash -c "echo 'q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-quit_cmd-shortened PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")

# version command
add_test(NAME test_repl-version_cmd-full
	COMMAND bash -c "echo 'version. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-version_cmd-full PROPERTIES
	PASS_REGULAR_EXPRESSION "Tau version")

add_test(NAME test_repl-version_cmd-shortened
	COMMAND bash -c "echo 'v. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-version_cmd-shortened PROPERTIES
	PASS_REGULAR_EXPRESSION "Tau version")

# help command
add_test(NAME test_repl-help_cmd-full
	COMMAND bash -c "echo 'help. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_cmd-full PROPERTIES
	PASS_REGULAR_EXPRESSION "Commands")

add_test(NAME test_repl-help_cmd-shortened
	COMMAND bash -c "echo 'h. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_cmd-shortened PROPERTIES
	PASS_REGULAR_EXPRESSION "Commands")

add_test(NAME test_repl-help_command_cmd-full
	COMMAND bash -c "echo 'help normalize. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_command_cmd-full PROPERTIES
	PASS_REGULAR_EXPRESSION "normalizes the given NSO RR")

add_test(NAME test_repl-help_command_cmd-shortened
	COMMAND bash -c "echo 'h normalize. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_command_cmd-shortened PROPERTIES
	PASS_REGULAR_EXPRESSION "normalizes the given NSO RR")

add_test(NAME test_repl-help_short_command_cmd-full
	COMMAND bash -c "echo 'help n. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_short_command_cmd-full PROPERTIES
	PASS_REGULAR_EXPRESSION "normalizes the given NSO RR")

add_test(NAME test_repl-help_short_command_cmd-shortened
	COMMAND bash -c "echo 'h n. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_short_command_cmd-shortened PROPERTIES
	PASS_REGULAR_EXPRESSION "normalizes the given NSO RR")

# sample for comparing output with a file
#add_test(NAME test_name COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> | diff - test_name.dump")