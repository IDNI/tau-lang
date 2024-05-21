#
# qelim command
#

add_repl_test(qelim_cmd-form "qelim all x ex y x=y. q" "!\\(ex x F\\)")
add_repl_test(qelim_cmd-memory "mem all x ex y x=y. qelim %0. q" "!\\(ex x F\\)")
