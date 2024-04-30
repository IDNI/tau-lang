cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

#
# quit command
#
add_test(NAME test_repl-quit_cmd-full
	COMMAND bash -c "echo 'quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-quit_cmd-full PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")
add_test(NAME test_repl-quit_cmd-shortened
	COMMAND bash -c "echo 'q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-quit_cmd-shortened PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")
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

#
# help command
#

# basic help command
add_test(NAME test_repl-help_cmd-full
	COMMAND bash -c "echo 'help. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_cmd-full PROPERTIES
	PASS_REGULAR_EXPRESSION "Commands")
add_test(NAME test_repl-help_cmd-shortened
	COMMAND bash -c "echo 'h. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_cmd-shortened PROPERTIES
	PASS_REGULAR_EXPRESSION "Commands")

# help normalize command
add_test(NAME test_repl-help_normalize_command_cmd-full
	COMMAND bash -c "echo 'help normalize. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_normalize_command_cmd-full PROPERTIES
	PASS_REGULAR_EXPRESSION "normalizes the given NSO RR")
add_test(NAME test_repl-help_shortened_normalize_cmd
	COMMAND bash -c "echo 'h normalize. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_shortened_normalize_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "normalizes the given NSO RR")

# help normalize command with shortened command
add_test(NAME test_repl-help_normalize_cmd_shortened
	COMMAND bash -c "echo 'help n. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_normalize_cmd_shortened PROPERTIES
	PASS_REGULAR_EXPRESSION "normalizes the given NSO RR")
add_test(NAME test_repl-help_shortened_normalize_cmd-shortened
	COMMAND bash -c "echo 'h n. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_shortened_normalize_cmd-shortened PROPERTIES
	PASS_REGULAR_EXPRESSION "normalizes the given NSO RR")

# help dnf
add_test(NAME test_repl-help_shortened-dnf_cmd
	COMMAND bash -c "echo 'h dnf. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_shortened-dnf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "dnf command converts")
add_test(NAME test_repl-help_dnf_cmd
	COMMAND bash -c "echo 'help dnf. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_dnf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "dnf command converts")

# help cnf command
add_test(NAME test_repl-help_shortened-cnf_cmd
	COMMAND bash -c "echo 'h cnf. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_shortened-cnf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "cnf command converts")
add_test(NAME test_repl-help_cnf_cmd
	COMMAND bash -c "echo 'help cnf. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_cnf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "cnf command converts")

# help nnf command
add_test(NAME test_repl-help_shortened-nnf_cmd
	COMMAND bash -c "echo 'h nnf. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_shortened-nnf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "nnf command converts")
add_test(NAME test_repl-help_nnf_cmd
	COMMAND bash -c "echo 'help nnf. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_nnf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "nnf command converts")

# help mnf command
add_test(NAME test_repl-help_shortened-mnf_cmd
	COMMAND bash -c "echo 'h mnf. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_shortened-mnf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "mnf command converts")
add_test(NAME test_repl-help_mnf_cmd
	COMMAND bash -c "echo 'help mnf. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_mnf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "mnf command converts")

# help onf command
add_test(NAME test_repl-help_shortened-onf_cmd
	COMMAND bash -c "echo 'h onf. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_shortened-onf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "onf command convert")
add_test(NAME test_repl-help_onf_cmd
	COMMAND bash -c "echo 'help onf. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_onf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "onf command converts")

#
# normal forms commands
#

# dnf command
add_test(NAME test_repl-dnf_bf_cmd
	COMMAND bash -c "echo 'dnf X & (Y | Z). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-dnf_bf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "X & Y | X & Z")
add_test(NAME test_repl-dnf_wff_cmd
	COMMAND bash -c "echo 'dnf (X = 0) && ((Y=0) || (Z=0)). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-dnf_wff_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "X = 0 && Y = 0 || X = 0 && Z = 0")

# cnf command
add_test(NAME test_repl-cnf_bf_cmd
	COMMAND bash -c "echo 'cnf X | (Y & Z). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-cnf_bf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")
add_test(NAME test_repl-cnf_wff_cmd
	COMMAND bash -c "echo 'cnf (X = 0) || ((Y=0) && (Z=0)). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-cnf_wff_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")

# nnf command
#add_test(NAME test_repl-nnf_bf_cmd
#	COMMAND bash -c "echo 'nnf (Y | Z)'. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-nnf_bf_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "Quit.")
add_test(NAME test_repl-nnf_wff_cmd
	COMMAND bash -c "echo 'nnf !((Y=0) || (Z=0)). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-nnf_wff_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")

# mnf command
# TODO (HIGH) fixme it's not working properly, returns Y & X |Y & X | Z & X
add_test(NAME test_repl-mnf_bf_cmd
	COMMAND bash -c "echo 'mnf X & (Y | Z). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-mnf_bf_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")
add_test(NAME test_repl-mnf_wff_cmd
	COMMAND bash -c "echo 'mnf (X = 0) && ((Y=0) || (Z=0)). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-mnf_wff_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")

# onf command(only wff)
# TODO (HIGH) fixme it's not working properly, returns (X = 0) && (Y = 0 || Z = 0) again...
#add_test(NAME test_repl-onf_wff_cmd
#	COMMAND bash -c "echo 'onf X (X = 0) && ((Y=0) || (Z=0)). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-onf_wff_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "onf command converts")

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

#
# normalize command
#test_repl-nnf_bf_cmd
#add_test(NAME test_repl-normalize_cmd
#	COMMAND bash -c "echo 'normalize X  & (Y | Z). q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-normalize_cmd PROPERTIES
#	PASS_REGULAR_EXPRESSION "X & Y | X & Z")



# TODO (VERY_HIGH) add tests for normalize command
# TODO (VERY_HIGH) add tests for definition command
# TODO (VERY_HIGH) add tests for sat/unsat/valid command
# TODO (VERY_HIGH) add tests for solving command
# TODO (VERY_HIGH) add tests for configuration commands
# TODO (VERY_HIGH) add tests for examples command
# TODO (VERY_HIGH) add tests for outputs command


# sample for comparing output with a file
#add_test(NAME test_name COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> | diff - test_name.dump")