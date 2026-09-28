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

# reset frees the tree nodes the cleared session held
add_repl_test(reset_cmd-frees_nodes "x = 0 && y = 0. reset" "[0-9]+ tree nodes freed")

# the engine answers the same after a reset
add_repl_test(reset_cmd-sat_after_reset
	"sat x = 0 && x != 0. reset. sat x = 0 && x != 0" "F.*cleared.*F")

# reset stops a run in progress
add_repl_test(reset_cmd-stops_run
	"o1:tau := out console. run 2 steps G (o1[t]:tau = 0). reset. memory"
	"run stopped.*no run in progress")
