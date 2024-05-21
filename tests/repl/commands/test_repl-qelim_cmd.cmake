#
# qelim command
#

add_repl_test(qelim_cmd-form "qelim all x ex y x=y" "!\\(ex x F\\)")
add_repl_test(qelim_cmd-memory "mem all x ex y x=y. qelim %0" "!\\(ex x F\\)")
