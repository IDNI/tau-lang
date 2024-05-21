#
# qelim command
#

add_test(NAME test_repl-qelim_cmd-form
	COMMAND bash -c "echo 'qelim all x ex y x=y. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-qelim_cmd-form PROPERTIES
	PASS_REGULAR_EXPRESSION "!\\(ex x F\\)")
add_test(NAME test_repl-qelim_cmd-memory
	COMMAND bash -c "echo 'mem all x ex y x=y. qelim %0. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-qelim_cmd-memory PROPERTIES
	PASS_REGULAR_EXPRESSION "!\\(ex x F\\)")
