#
# history commands
#

include(add_repl_test_depreciated)

# history list command
add_repl_test(history_list_cmd-empty "history" "history is empty")
add_repl_test(history_list_cmd-non_empty "1. history" ": 1")

# history store command
add_repl_test(history_store_cmd-bf "1" ": 1")
add_repl_test(history_store_cmd-wff "T" ": T")
add_repl_test(history_store_cmd-tau "{T.}" "1")

add_repl_test(history_store_cmd-tau_fail "always o1[t]' = {x } i1[t]" "Syntax Error")
add_repl_test(history_store_cmd-sbf_fail "always o1[t]' = {x = 0}:sbf i1[t]" "Syntax Error")

# history print command
add_repl_test(history_print_cmd-empty_absolute "history %1" "history is empty")
add_repl_test(history_print_cmd-empty_relative "history %-1" "history is empty")
add_repl_test(history_print_cmd-non_empty_absolute "1. history %1" ": 1")
add_repl_test(history_print_cmd-non_empty_relative "1. history %" ": 1")
add_repl_test(history_print_cmd-non_empty_last "1. T. history %-1" ": 1")
