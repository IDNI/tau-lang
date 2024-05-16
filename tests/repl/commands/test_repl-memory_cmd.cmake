#
# memory commands
#

# memory list command
add_test(NAME test_repl-memory_list_cmd-empty
	COMMAND bash -c "echo 'memory. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_list_cmd-empty PROPERTIES
	PASS_REGULAR_EXPRESSION "memory is empty.")
add_test(NAME test_repl-memory_list_cmd-non_empty
	COMMAND bash -c "echo 'm 0. memory. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_list_cmd-non_empty PROPERTIES
	FAIL_REGULAR_EXPRESSION "memory empty")

# memory clear command
add_test(NAME test_repl-memory_clear_cmd-empty
	COMMAND bash -c "echo 'memory clear. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_clear_cmd-empty PROPERTIES
	PASS_REGULAR_EXPRESSION "memory is empty")
add_test(NAME test_repl-memory_clear_cmd-non_empty
	COMMAND bash -c "echo 'm 0. memory clear. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_clear_cmd-non_empty PROPERTIES
	PASS_REGULAR_EXPRESSION "memory cleared")

# memory store command
add_test(NAME test_repl-memory_store_cmd-bf
	COMMAND bash -c "echo 'm 0. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_store_cmd-bf PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")
	add_test(NAME test_repl-memory_store_cmd-wff
	COMMAND bash -c "echo 'm T. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_store_cmd-wff PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")
add_test(NAME test_repl-memory_store_cmd-tau
	COMMAND bash -c "echo 'm {T}. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_store_cmd-tau PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit.")

# memory delete command
add_test(NAME test_repl-memory_delete_cmd-empty
	COMMAND bash -c "echo 'memory delete %-0. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_delete_cmd-empty PROPERTIES
	PASS_REGULAR_EXPRESSION "memory is empty")
add_test(NAME test_repl-memory_delete_cmd-non_empty_absolute
	COMMAND bash -c "echo 'm 0. memory delete %-0. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_delete_cmd-non_empty_absolute PROPERTIES
	PASS_REGULAR_EXPRESSION "deleted index")
add_test(NAME test_repl-memory_delete_cmd-non_empty_relative
	COMMAND bash -c "echo 'm 0. memory delete %-0. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_delete_cmd-non_empty_relative PROPERTIES
	PASS_REGULAR_EXPRESSION "deleted index")

# memory print command
add_test(NAME test_repl-memory_print_cmd-empty_absolute
	COMMAND bash -c "echo 'memory %-0. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_print_cmd-empty_absolute PROPERTIES
	PASS_REGULAR_EXPRESSION "memory is empty")
add_test(NAME test_repl-memory_print_cmd-empty_relative
	COMMAND bash -c "echo 'memory %0. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_print_cmd-empty_relative PROPERTIES
	PASS_REGULAR_EXPRESSION "memory is empty")
add_test(NAME test_repl-memory_print_cmd-non_empty_absolute
	COMMAND bash -c "echo 'm 0. memory %-0. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_print_cmd-non_empty_absolute PROPERTIES
	FAIL_REGULAR_EXPRESSION "memory is empty")
add_test(NAME test_repl-memory_print_cmd-non_empty_relative
	COMMAND bash -c "echo 'm 0. memory %0. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-memory_print_cmd-non_empty_relative PROPERTIES
	FAIL_REGULAR_EXPRESSION "memory is empty")
