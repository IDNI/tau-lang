#
# input/output stream definition command
#

include(add_repl_test)

add_repl_test(io_definition_cmd-sbf_in_console "i1:sbf := in console. defs" "i1:sbf := in console")
add_repl_test(io_definition_cmd-tau_in_console "i1:tau := in console. defs" "i1:tau := in console")
add_repl_test(io_definition_cmd-in_console "i1 := in console. defs" "i1:untyped := in console")
add_repl_test(io_definition_cmd-sbf_out_console "o1:sbf := out console. defs" "o1:sbf := out console")
add_repl_test(io_definition_cmd-tau_out_console "o1:tau := out console. defs" "o1:tau := out console")
add_repl_test(io_definition_cmd-out_console "o1 := out console. defs" "o1:untyped := out console")
add_repl_test(io_definition_cmd-sbf_in_file "i1:sbf := in file(\\\"data\\\"). defs" "i1:sbf := in file\\(\"data\\\"\\)")
add_repl_test(io_definition_cmd-tau_in_file "i1:tau := in file(\\\"data\\\"). defs" "i1:tau := in file\\(\"data\\\"\\)")
add_repl_test(io_definition_cmd-in_file "i1 := in file(\\\"data\\\"). defs" "i1:untyped := in file\\(\"data\"\\)")
add_repl_test(io_definition_cmd-sbf_out_file "o1:sbf := out file(\\\"data\\\"). defs" "o1:sbf := out file\\(\"data\"\\)")
add_repl_test(io_definition_cmd-tau_out_file "o1:tau := out file(\\\"data\\\"). defs" "o1:tau := out file\\(\"data\"\\)")
add_repl_test(io_definition_cmd-out_file "o1 := out file(\\\"data\\\"). defs" "o1:untyped := out file\\(\"data\"\\)")

# IN-R6: `w_` is the reserved CTL* witness prefix -- a user stream definition
# with that name would collide with the internal witness outputs an executed
# E reduction registers.
add_repl_test_fail(io_definition_cmd-w_prefix_out_rejected
	"w_0 := out console." "reserved witness prefix")
add_repl_test_fail(io_definition_cmd-w_prefix_in_rejected
	"w_1:sbf := in console." "reserved witness prefix")
