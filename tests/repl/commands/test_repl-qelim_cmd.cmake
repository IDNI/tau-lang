#
# qelim command
#

include(add_repl_test)

add_repl_test(qelim_cmd-form "qelim T" ": T")
add_repl_test(qelim_cmd-abs_mem "T. qelim %1" "T")
add_repl_test(qelim_cmd-rel_mem "T. qelim %-0" "T")

add_repl_test(qelim_cmd-form_1 "qelim all x ex y x=y" ": T")

# qelim takes a formula: a term argument is rejected instead of the whole line
# being stored as a term (`qelim x` used to print `%1: qelimx`)
add_repl_test_fail(qelim_cmd-term_rejected "qelim x" "Invalid formula")
