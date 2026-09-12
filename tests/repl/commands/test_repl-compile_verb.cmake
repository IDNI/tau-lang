# `tau compile` verb: emit + build + run the echo spec end-to-end.
include(add_compile_test)

add_compile_test(compile_verb-echo
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau")

# Period-terminated spec: proves compile_spec parses via get_spec, not just
# the bare-formula get_formula fallback (echo.tau above has no period).
add_compile_test(compile_verb-echo_dot
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo_dot.tau")

# A lookback atom under an eventuality drives a __step_ge<k> guard
# (ltl_aba_helpers.tmpl.h's step_guard_prop) into the synthesized strategy;
# this is the emit_main/table_step_provider path's own smoke test for it.
add_compile_test(compile_verb-ltl_lookback_under_eventuality
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/ltl_lookback_under_eventuality.tau")
