#
# toggle command
#

include(add_repl_test)

add_repl_test(toggle_cmd-status "toggle status"  "status: *false")
add_repl_test(toggle_cmd-colors "toggle color" "color: *false")

# bv-widening defaults to off, so one toggle proves the command acted.
add_repl_test(toggle_cmd-bvwidening "toggle bv-widening. get bv-widening"
	"bv-widening: *true")
