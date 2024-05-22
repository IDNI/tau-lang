#
# help command
#

# basic help command
add_repl_test(help_cmd-full "help" "Commands")
add_repl_test(help_cmd-shortened "h" "Commands")

# help normalize command
add_repl_test(help_normalize_command_cmd-full "help normalize" "normalizes the given NSO RR")
add_repl_test(help_shortened_normalize_cmd "h normalize" "normalizes the given NSO RR")

# help qelim command
add_test(NAME test_repl-help_qelim_command_cmd-full
	COMMAND bash -c "echo 'help qelim. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_qelim_command_cmd-full PROPERTIES
	PASS_REGULAR_EXPRESSION "qelim command eliminates inner most quantifier")
add_test(NAME test_repl-help_shortened_qelim_cmd
	COMMAND bash -c "echo 'h qelim. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-help_shortened_qelim_cmd PROPERTIES
	PASS_REGULAR_EXPRESSION "qelim command eliminates inner most quantifier")

# help normalize command with shortened command
add_repl_test(help_normalize_cmd_shortened "help n" "normalizes the given NSO RR")
add_repl_test(help_shortened_normalize_cmd-shortened "h n" "normalizes the given NSO RR")

# help dnf
add_repl_test(help_shortened-dnf_cmd "h dnf" "dnf command converts")
add_repl_test(help_dnf_cmd "help dnf" "dnf command converts")

# help cnf command
add_repl_test(help_shortened-cnf_cmd "h cnf" "cnf command converts")
add_repl_test(help_cnf_cmd "help cnf" "cnf command converts")

# help nnf command
add_repl_test(help_shortened-nnf_cmd "h nnf" "nnf command converts")
add_repl_test(help_nnf_cmd "help nnf" "nnf command converts")

# help mnf command
add_repl_test(help_shortened-mnf_cmd "h mnf" "mnf command converts")
add_repl_test(help_mnf_cmd "help mnf" "mnf command converts")

# help onf command
add_repl_test(help_shortened-onf_cmd "h onf" "onf command convert")
add_repl_test(help_onf_cmd "help onf" "onf command converts")

# help memory command
add_repl_test(help_shortened-memory_cmd "h memory" "memory command")
add_repl_test(help_memory_cmd "help memory" "memory command")
add_repl_test(help_shortened-memory_shortened_m_cmd "h m" "memory command")
add_repl_test(help-memory_shortened_m_cmd "help m" "memory command")
add_repl_test(help_shortened-memory_shortened_mem_cmd "h mem" "memory command")
add_repl_test(help-memory_shortened_mem_cmd "help mem" "memory command")
