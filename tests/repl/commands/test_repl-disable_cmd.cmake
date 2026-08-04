#
# disable command
#
# Coverage-driven addition (2026-08-01). As with enable, the include existed
# but was commented out, leaving update_bool_opt_cmd's disable arm untested.
#

include(add_repl_test)

add_repl_test(disable_cmd-status "disable status" "status: *off")
add_repl_test(disable_cmd-colors "disable colors" "colors: *off")
