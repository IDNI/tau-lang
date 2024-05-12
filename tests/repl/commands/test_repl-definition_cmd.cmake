#
# definition command
#

# definition command tau_rule
add_test(NAME test_repl-def_tau_cmd
	COMMAND bash -c "echo 'def g($Y) :::= {T}. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-def_tau_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "command")

# definition command wff_tule
add_test(NAME test_repl-def_wff_cmd
	COMMAND bash -c "echo 'def g($Y) ::= T. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-def_wff_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "command")

# definition command bf_rule
add_test(NAME test_repl-def_bf_cmd
	COMMAND bash -c "echo 'def g($Y) := 1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-def_bf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "command")

# definition command list
## no defs
add_test(NAME test_repl-def_list_cmd
	COMMAND bash -c "echo 'def. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-def_list_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "command")
# with defs
add_test(NAME test_repl-def_list_cmd-with_defs
	COMMAND bash -c "echo 'def g($Y) := 1. def. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-def_list_cmd-with_defs PROPERTIES
	PASS_REGULAR_EXPRESSION "[0]")

# definition command del
## no defs
add_test(NAME test_repl-def_del_cmd
	COMMAND bash -c "echo 'def del @0. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-def_del_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "definition does not exist")
## with defs
add_test(NAME test_repl-def_del_cmd-with_defs
	COMMAND bash -c "echo 'def g($Y) := 1. def del @0. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-def_del_cmd-with_defs PROPERTIES
	PASS_REGULAR_EXPRESSION "deleted index")

# definition command clear
add_test(NAME test_repl-def_clear_cmd
	COMMAND bash -c "echo 'def g($Y) := 1. def clear. def del @0. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-def_clear_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "definition does not exist")
add_test(NAME test_repl-def_clear_empty_cmd
	COMMAND bash -c "echo 'def clear. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-def_clear_empty_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "command")
