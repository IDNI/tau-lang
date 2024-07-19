#
# definition command
#

# definition command wff_tule
add_repl_test(def_wff_cmd "g($Y) := T" "\\[1\\] g")

# definition command bf_rule
add_repl_test(def_bf_cmd "g($Y) := 1" "\\[1\\] g")

# definition command list
#
# no defs
add_repl_test(def_list_cmd "defs" "definitions are empty")

# with defs
add_repl_test(def_list_cmd-with_defs "g($Y) := 1. defs" "\\[1\\] g")

# definition command print
#
# no defs
add_repl_test(def_print_cmd-empty "defs 1" "definitions are empty")

# printing defs
add_repl_test(def_print_cmd-1 "g($Y) := 1. defs 1" "\\[1\\] g")
add_repl_test(def_print_cmd-2 "g($Y) := 1. f($Y) := T. defs 2" "\\[2\\] f")

# not existing defs
add_repl_test(def_print_cmd-0 "g($Y) := 1. defs 0" "error: definition \\[0\\] does not exist")
add_repl_test(def_print_cmd-3 "g($Y) := 1. defs 3" "error: definition \\[3\\] does not exist")
