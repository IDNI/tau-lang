# `tau compile` verb: emit + build + run the echo spec end-to-end.
include(add_compile_test)

add_compile_test(compile_verb-echo
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau")

# Period-terminated spec: proves compile_spec parses via get_spec, not just
# the bare-formula get_formula fallback (echo.tau above has no period).
add_compile_test(compile_verb-echo_dot
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo_dot.tau")

# --cxx names the compiler the nested cmake configure uses: an unusable
# name must reach that configure (and fail it) instead of being replaced
# by whatever clang++ is on PATH. The spec names no BA so every pack gets
# as far as the configure; it is copied to a temp dir because the build
# tree is placed beside the spec file.
add_test(NAME "test_repl-compile_verb-cxx_override_is_used"
	COMMAND bash -c "d=$(mktemp -d /tmp/tau_cxx_override.XXXXXX); cp ${CMAKE_SOURCE_DIR}/tests/codegen_specs/declare_open_codegen.tau $d/spec.tau; $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> compile --cxx /nonexistent/c++ $d/spec.tau -o $d/out; rc=$?; test $rc -ne 0 && grep -q 'nonexistent/c++' $d/spec.tau.build/configure.log && echo CXX_OVERRIDE_REACHED_CONFIGURE; rm -rf $d")
set_tests_properties("test_repl-compile_verb-cxx_override_is_used" PROPERTIES
	PASS_REGULAR_EXPRESSION "CXX_OVERRIDE_REACHED_CONFIGURE")
