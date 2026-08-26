#
# enable command
#
# Coverage-driven addition (2026-08-01). The include for this file already
# existed in CMakeLists.txt but was commented out, so update_bool_opt_cmd's
# enable arm had no test. `indenting` is used rather than `status`/`colors`
# because it defaults to off, so "on" proves the command acted rather than
# matching a pre-existing value.
#

include(add_repl_test)

add_repl_test(enable_cmd-indenting "enable indenting" "indenting: *on")
add_repl_test(enable_cmd-status "enable status" "status: *on")
