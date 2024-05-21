#
# definition command
#

# definition command tau_rule
add_repl_test(def_tau_cmd "def g($Y) :::= {T}" "[0]")

# definition command wff_tule
add_repl_test(def_wff_cmd "def g($Y) ::= T" "[0]")

# definition command bf_rule
add_repl_test(def_bf_cmd "def g($Y) := 1" "[0]")

# definition command list
## no defs
add_repl_test(def_list_cmd "def" "definitions are empty")
# with defs
add_repl_test(def_list_cmd-with_defs "def g($Y) := 1. def" "[0]")

# definition command del
## no defs
add_repl_test(def_del_cmd "def del %-0" "definition does not exist")
## with defs
add_repl_test(def_del_cmd-with_defs "def g($Y) := 1. def del %-0" "deleted index")

# definition command clear
add_repl_test(def_clear_cmd "def g($Y) := 1. def clear. def del %-0" "definition does not exist")
add_repl_test(def_clear_empty_cmd "def clear" "definitions cleared")
