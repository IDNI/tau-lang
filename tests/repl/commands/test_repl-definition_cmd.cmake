#
# definition command
#

# definition command tau_rule
add_repl_test(def_tau_cmd "g($Y) :::= {T}" "[1]")

# definition command wff_tule
add_repl_test(def_wff_cmd "g($Y) ::= T" "[1]")

# definition command bf_rule
add_repl_test(def_bf_cmd "g($Y) := 1" "[1]")

# definition command list
## no defs
add_repl_test(def_list_cmd "defs" "definitions are empty")
# with defs
add_repl_test(def_list_cmd-with_defs "g($Y) := 1. defs" "[1]")
