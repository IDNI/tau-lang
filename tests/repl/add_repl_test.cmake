# include(add_repl_test) to use new tree

function(add_repl_test test_name test_cmd test_regex)
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash -c "echo \"${test_cmd}. q\" | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${test_regex}"
		FAIL_REGULAR_EXPRESSION "Error"
	)
endfunction()

function(add_repl_test_fail test_name test_cmd test_regex)
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash -c "echo \"${test_cmd}. q\" | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${test_regex}"
	)
endfunction()
