#
# memory commands
#

# memory list command
add_repl_test(memory_list_cmd-empty "memory" "memory is empty")
add_repl_test(memory_list_cmd-non_empty "m 1. memory" ": 1")

# memory clear command
add_repl_test(memory_clear_cmd-empty "memory clear" "memory is empty")
add_repl_test(memory_clear_cmd-non_empty "m 0. memory clear" "memory cleared")

# memory store command
add_repl_test(memory_store_cmd-bf "m 1" ": 1")
add_repl_test(memory_store_cmd-wff "m T" ": T")
add_repl_test(memory_store_cmd-tau "m {T}" "{ T }")

# memory delete command
add_repl_test(memory_delete_cmd-empty "memory delete %-0" "memory is empty")
add_repl_test(memory_delete_cmd-non_empty_absolute "m 0. memory delete %-0" "deleted index")
add_repl_test(memory_delete_cmd-non_empty_relative "m 0. memory delete %0" "deleted index")
add_repl_test(memory_delete_cmd-non_empty_last "m 0. memory delete %" "deleted index")

# memory print command
add_repl_test(memory_print_cmd-empty_absolute "memory %-0" "memory is empty")
add_repl_test(memory_print_cmd-empty_relative "memory %0" "memory is empty")
add_repl_test(memory_print_cmd-non_empty_absolute "m 1. memory %-0" ": 1")
add_repl_test(memory_print_cmd-non_empty_relative "m 1. memory %0" ": 1")
add_repl_test(memory_print_cmd-non_empty_last "m 1. memory %" ": 1")
