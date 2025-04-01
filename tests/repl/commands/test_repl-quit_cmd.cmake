#
# quit command
#

include(add_repl_test)

add_repl_test(quit_cmd-full "quit" "Quit.")
add_repl_test(quit_cmd-shortened "q" "Quit.")
