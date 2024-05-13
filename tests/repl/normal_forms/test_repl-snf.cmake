#
# order normal form tests
#

# T
add_test(NAME test_repl-normal_forms-snf_T
	COMMAND bash -c "echo 'snf T. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-snf_T PROPERTIES
	PASS_REGULAR_EXPRESSION "T")