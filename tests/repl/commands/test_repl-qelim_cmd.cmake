#
# qelim command
#

include(add_repl_test)

add_repl_test(qelim_cmd-form "qelim T" ": T")
add_repl_test(qelim_cmd-abs_mem "T. qelim %1" "T")
add_repl_test(qelim_cmd-rel_mem "T. qelim %-0" "T")

add_repl_test(qelim_cmd-form_1 "qelim all x ex y x=y" ": T")