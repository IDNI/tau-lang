#
# version command
#

add_test(NAME test_repl-version_cmd-full
	COMMAND bash -c "echo 'version. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-version_cmd-full PROPERTIES
	PASS_REGULAR_EXPRESSION "Tau version")
add_test(NAME test_repl-version_cmd-shortened
	COMMAND bash -c "echo 'v. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-version_cmd-shortened PROPERTIES
	PASS_REGULAR_EXPRESSION "Tau version")
