#
# definition command
#

include(add_repl_test)

# definition command rec_relation
add_repl_test(def_wff_cmd "g(Y) := T" "\\[1\\] g")

# definition command rec_relation
add_repl_test(def_bf_cmd "g(Y):tau := 1" "\\[1\\] g")

# definition command list
#
# no defs
add_repl_test(def_list_cmd "defs" "Definitions: empty")

# with defs
add_repl_test(def_list_cmd-with_defs "g(Y):tau := 1. defs" "\\[1\\] g")

# definition command print
#
# no defs
add_repl_test(def_print_cmd-empty "defs" "Definitions: empty")

# printing defs
add_repl_test(def_print_cmd-1 "g(Y):tau := 1. defs 1" "\\[1\\] g")
add_repl_test(def_print_cmd-2 "g(Y):tau := 1. f(Y) := T. defs 2" "\\[2\\] f")

# not existing defs
add_repl_test_fail(def_print_cmd-0 "g(Y):tau := 1. defs 0" "Definition \\[0\\] does not exist")
add_repl_test_fail(def_print_cmd-3 "g(Y):tau := 1. defs 3" "Definition \\[3\\] does not exist")

# defining an input stream
add_repl_test(def_input_stream_cmd "data_in:tau := in console" "in console")

# defining an output stream
add_repl_test(def_output_stream_cmd "data_out:tau := out console" "out console")

# A typed head classifies a definition as functional, so a formula body
# cannot define it. Used to hard-crash (null tref from update_functional_rr
# inserted into the transformed map): the only acceptable outcome is a
# clean error.
add_repl_test_fail(def_typed_head_formula_body_cmd "p5(u):sbf := u = 0" "cannot define a function")