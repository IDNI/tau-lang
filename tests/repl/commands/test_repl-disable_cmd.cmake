#
# disable command
#
# Coverage-driven addition (2026-08-01). As with enable, the include existed
# but was commented out, leaving update_bool_opt_cmd's disable arm untested.
#

include(add_repl_test)

add_repl_test(disable_cmd-status "disable status" "status: *off")
add_repl_test(disable_cmd-colors "disable colors" "colors: *off")
add_repl_test(disable_cmd-factoring "disable factoring. get factoring" "factoring: *off")

# bv declares case-split as its own option, addressed bv-case-split, present
# when bv is in the configured pack -- hence gated by hand, as bv-blasting is.
tau_repl_unsupported(_tau_skip "disable bv-case-split")
if(_tau_skip)
	tau_repl_record_skip("disable_cmd-bv_case_split")
else()
	add_repl_test(disable_cmd-bv_case_split "enable bv-case-split. disable bv-case-split. get bv-case-split" "bv-case-split: *off")
endif()

add_repl_test(disable_cmd-bvwidening
	"enable bv-widening. disable bv-widening. get bv-widening"
	"bv-widening: *off")
