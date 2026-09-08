#
# toggle command
#

include(add_repl_test)

add_repl_test(toggle_cmd-status "toggle status" "off")
add_repl_test(toggle_cmd-colors "toggle colors" "off")

# bvwidening defaults to off, so one toggle proves the command acted.
add_repl_test(toggle_cmd-bvwidening "toggle bvwidening. get bvwidening"
	"bvwidening: *on")
