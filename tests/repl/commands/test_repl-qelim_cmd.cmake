#
# qelim command
#

add_repl_test(qelim_cmd-form "qelim T" ": T")
add_repl_test(qelim_cmd-abs_mem "mem T. qelim %0" "T")
add_repl_test(qelim_cmd-rel_mem "mem T. qelim %-0" "T")
