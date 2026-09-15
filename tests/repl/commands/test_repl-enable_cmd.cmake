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
add_repl_test(enable_cmd-factoring "disable factoring. enable factoring. get factoring" "factoring: *on")

# bv declares case-split as its own option, addressed bv-case-split, present
# when bv is in the configured pack -- hence gated by hand, as bv-blasting is.
tau_repl_unsupported(_tau_skip "enable bv-case-split")
if(_tau_skip)
	tau_repl_record_skip("enable_cmd-bv_case_split")
else()
	add_repl_test(enable_cmd-bv_case_split "enable bv-case-split. get bv-case-split" "bv-case-split: *on")
endif()

add_repl_test(enable_cmd-bvwidening "enable bv-widening. get bv-widening"
	"bv-widening: *on")
