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

# help memory command
add_test(NAME test_repl-help_shortened-memory_cmd
	COMMAND bash -c "echo 'h memory. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_shortened-memory_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "memory command")
add_test(NAME test_repl-help_memory_cmd
	COMMAND bash -c "echo 'help memory. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_memory_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "memory command")
add_test(NAME test_repl-help_shortened-memory_shortened_m_cmd
	COMMAND bash -c "echo 'h m. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_shortened-memory_shortened_m_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "memory command")
add_test(NAME test_repl-help-memory_shortened_m_cmd
	COMMAND bash -c "echo 'help m. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help-memory_shortened_m_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "memory command")
add_test(NAME test_repl-help_shortened-memory_shortened_mem_cmd
	COMMAND bash -c "echo 'h mem. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_shortened-memory_shortened_mem_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "memory command")
add_test(NAME test_repl-help-memory_shortened_mem_cmd
	COMMAND bash -c "echo 'help mem. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help-memory_shortened_mem_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "memory command")
