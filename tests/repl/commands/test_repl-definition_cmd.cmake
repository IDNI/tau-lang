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

# Cases of one recurrence must agree on their argument types. Annotating
# only the base case used to leave an indexed call silently unexpanded and
# send an offset-free call into an enumeration that never terminates.
add_repl_test_fail(definitions-rr_case_type_mismatch_indexed
	"k6[0](x) := (x:sbf = 0). k6[n](x) := k6[n-1](x) && x != 1. type Point = {a: sbf, b: sbf}. n ex p:Point k6[3](p.a)"
	"disagree")
set_tests_properties("test_repl-definitions-rr_case_type_mismatch_indexed" PROPERTIES TIMEOUT 60)
add_repl_test_fail(definitions-rr_case_type_mismatch_fixpoint
	"k7[0](x) := (x:sbf = 0). k7[n](x) := k7[n-1](x) && x != 1. type Point = {a: sbf, b: sbf}. n ex p:Point k7(p.a)"
	"disagree")
set_tests_properties("test_repl-definitions-rr_case_type_mismatch_fixpoint" PROPERTIES TIMEOUT 60)

# A call whose argument types can never match its definition's parameter
# types used to be left silently unexpanded (the rules simply never fired);
# it is now rejected at assembly time with an error naming both sides. An
# untyped parameter or argument counts as tau -- that is how rule matching
# treats it -- so only a genuinely unmatchable pairing errors.
add_repl_test_fail(definitions-call_arg_type_mismatch
	"pr2(u) := (u:sbf = 0). n pr2(z:tau)"
	"disagrees with")
set_tests_properties("test_repl-definitions-call_arg_type_mismatch" PROPERTIES TIMEOUT 60)
# The whole-tuple form of the same defect (demo_4.4 territory): the Point
# flattens into sbf members, but the definition's second parameter is
# untyped (= tau), so the call could never match.
add_repl_test_fail(definitions-whole_tuple_call_arg_type_mismatch
	"origin(u,v) := (u:sbf = 0 && v = 0). type Point = {a: sbf, b: sbf}. n (p:Point = p) && origin(p)"
	"disagrees with")
set_tests_properties("test_repl-definitions-whole_tuple_call_arg_type_mismatch" PROPERTIES TIMEOUT 60)
# A reference with no definition at all reaching solve is not an internal
# error: the solver names the unresolved reference instead.
add_repl_test_fail(definitions-solve_unresolved_reference
	"type Point = {a: sbf, b: sbf}. solve x:Point = x && foo(x.a, x.b) && x.a != 0"
	"unresolved reference")
set_tests_properties("test_repl-definitions-solve_unresolved_reference" PROPERTIES TIMEOUT 60)
