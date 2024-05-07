#
# normalize command
#

#test_repl-nnf_bf_cmd
#add_test(NAME test_repl-normalize_cmd
#	COMMAND bash -c "echo 'normalize X  & (Y | Z). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-normalize_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "X & Y | X & Z")
