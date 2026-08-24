# `tau compile` verb: emit + build + run the echo spec end-to-end.
include(add_compile_test)

add_compile_test(compile_verb-echo
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau")

# Period-terminated spec: proves compile_spec parses via get_spec, not just
# the bare-formula get_formula fallback (echo.tau above has no period).
add_compile_test(compile_verb-echo_dot
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo_dot.tau")
