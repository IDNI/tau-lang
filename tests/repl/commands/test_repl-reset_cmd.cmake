#
# reset command
#

include(add_repl_test)

# basic reset runs without error
add_repl_test(reset_cmd-basic "reset" "cleared")

# after reset, history is empty
add_repl_test(reset_cmd-clears_history "1. reset. history" "cleared")

# after reset, definitions are empty
add_repl_test(reset_cmd-clears_defs "g(Y) := T. reset. defs" "Definitions: empty")

# can define again after reset
add_repl_test(reset_cmd-reuse_after_reset "g(Y) := T. reset. g(Y) := F. defs" "g")
