#
# normalize command
#

#test_repl-nnf_bf_cmd
#add_test(NAME test_repl-normalize_cmd
#	COMMAND bash -c "echo 'normalize X  & (Y | Z). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-normalize_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "X & Y | X & Z")

# 'normalize' command for Boolean functions
add_test(NAME test_repl-normalize_cmd_bf1
        COMMAND bash -c "echo \"normalize x + x'. q\" | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normalize_cmd_bf1 PROPERTIES
        PASS_REGULAR_EXPRESSION "Boolean function normalizer")

# 'n' command for normalization for Boolean functions
add_test(NAME test_repl-normalize_cmd_bf2
        COMMAND bash -c "echo \"n x + x'. q\" | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normalize_cmd_bf2 PROPERTIES
        PASS_REGULAR_EXPRESSION "Boolean function normalizer")