#
# definition command
#

# definition command tau_rule
#add_test(NAME test_repl-def_tau_cmd
#	COMMAND bash -c "echo 'def g($Y) :::= {T}. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-def_tau_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "command")

# definition command wff_tule
#add_test(NAME test_repl-def_wff_cmd
#	COMMAND bash -c "echo 'def g($Y) ::= T. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-def_wff_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "command")

# definition command bf_rule
#add_test(NAME test_repl-def_bf_cmd
#	COMMAND bash -c "echo 'def g($Y) := 1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-def_bf_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "command")

# definition command list
## no defs
#add_test(NAME test_repl-def_list_cmd
#	COMMAND bash -c "echo 'def list. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-def_list_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "command")
## with defs
#add_test(NAME test_repl-def_list_cmd-with_defs
#	COMMAND bash -c "echo 'def g($Y) := 1. def list. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-def_list_cmd-with_defs PROPERTIES
#	PASS_REGULAR_EXPRESSION "[0]")

# definition command del
## no defs
# TODO (HIGH) fixme it's failing with a optional access error
#add_test(NAME test_repl-def_del_cmd
#	COMMAND bash -c "echo 'def del 0. def list. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-def_del_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "command")
## with defs
#add_test(NAME test_repl-def_del_cmd-with_defs
#	COMMAND bash -c "echo 'def g($Y) := 1. def del 0. def list. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-def_list_cmd-with_defs PROPERTIES
#	FAIL_REGULAR_EXPRESSION "[0]")
# TODO (HIGH) add test case deleting non existing definition

# definition command clear
#add_test(NAME test_repl-def_clear_cmd
#	COMMAND bash -c "echo 'def g($Y) := 1. def clear. def list. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-def_clear_cmd PROPERTIES
#	FAIL_REGULAR_EXPRESSION "[0]")
#add_test(NAME test_repl-def_clear_empty_cmd
#	COMMAND bash -c "echo 'def clear. def list. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-def_clear_empty_cmd PROPERTIES
#	FAIL_REGULAR_EXPRESSION "[0]")

