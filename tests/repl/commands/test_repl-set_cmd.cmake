#
# set command
#

include(add_repl_test)

add_repl_test(set_cmd-status "set status off" "off")
add_repl_test(set_cmd-colors "set colors off" "off")

# Regression test for AP-N1: set_cmd() used to memoize its setters map in a
# static local, whose lambdas captured the *first* call's `this`/value by
# reference. Every later "set" command in the same session then read those
# stale references instead of its own value. Issuing three "set" commands
# in one session and checking that the last one actually took effect
# catches that regression.
add_repl_test(set_cmd-multiple_in_one_session
	"set status off. set colors off. set status on. get status"
	"status:[ ]+on")
