cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

add_test(NAME test_repl-quit_cmd-full
	COMMAND bash -c "echo 'quit.' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-quit_cmd-full PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit."
	TIMEOUT 10)

add_test(NAME test_repl-quit_cmd-shortened
	COMMAND bash -c "echo 'q.' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-quit_cmd-shortened PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit."
	TIMEOUT 10)

#add_test(NAME  test_repl-version_cmd COMMAND bash -c "$<TARGET_FILE:MyTest> | diff - expected_output.txt")