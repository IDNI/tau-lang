#
# input/output stream definition command
#

include(add_repl_test)

add_repl_test(def_io_cmd-sbf_in_console "i1:sbf = in console. defs" "i1:sbf = in console")
add_repl_test(def_io_cmd-tau_in_console "i1:tau = in console. defs" "i1:tau = in console")
add_repl_test(def_io_cmd-in_console "i1 = in console. defs" "i1:tau = in console") # fails
add_repl_test(def_io_cmd-sbf_out_console "o1:sbf = out console. defs" "o1:sbf = out console")
add_repl_test(def_io_cmd-tau_out_console "o1:tau = out console. defs" "o1:tau = out console")
add_repl_test(def_io_cmd-out_console "o1 = out console. defs" "o1:tau = out console") # fails
add_repl_test(def_io_cmd-sbf_in_file "i1:sbf = in file(\"data\"). defs" "i1:sbf = in file(\"data\")") # fails
add_repl_test(def_io_cmd-tau_in_file "i1:tau = in file(\"data\"). defs" "i1:tau = in file(\"data\")") # fails
add_repl_test(def_io_cmd-in_file "i1 = in file(\"data\"). defs" "i1:tau = in file(\"data\")") # fails
add_repl_test(def_io_cmd-sbf_out_file "o1:sbf = out file(\"data\"). defs" "o1:sbf = out file(\"data\")") # fails
add_repl_test(def_io_cmd-tau_out_file "o1:tau = out file(\"data\"). defs" "o1:tau = out file(\"data\")") # fails
add_repl_test(def_io_cmd-out_file "o1 = out file(\"data\"). defs" "o1:tau = out file(\"data\")") # fails
