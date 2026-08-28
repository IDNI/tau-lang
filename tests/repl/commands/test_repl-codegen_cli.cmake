#
# `tau compile <spec.tau>` exit-code contract (CG-N6 / CG-R1 successor).
#
# The standalone tau_codegen CLI this file used to drive is gone -- codegen
# is now the `compile` verb on the `tau` binary itself (src/main.cpp), which
# takes a spec FILE argument (not stdin) and drives compile_spec (see
# src/tau_compile.tmpl.h). Its exit-code contract collapsed from the old
# CLI's 0/3/4/5 spread to just two codes: main.cpp's compile branch returns
# `error(...)` (== 1) on every failure --
#   files.empty(), !ifs, src.empty(), or !res.ok() (codegen_result::ok() is
#   `!error.empty() && !exe_path.empty()`) --
# and 0 after `TAU_LOG_INFO << "compiled: " << res.exe_path;`. The *reason*
# for a failure (parse error, UNREALIZABLE, backend failure, cmake
# configure/build failure, ...) is now distinguished only by the res.error
# text `TAU_LOG_ERROR << "compile failed: " << res.error;` prints, not by a
# dedicated exit code -- there is no more UNKNOWN(4)/not-executable(5).
#

include(tau_repl_pack)

# CG-R7 successor: `tau compile` reads its spec from a FILE, not stdin, so
# each case below first writes the formula to a scratch file.

add_test(NAME "test_codegen_cli-always_one_emits"
	COMMAND bash -c "set -u; d=$(mktemp -d) || exit 1; trap 'rm -rf \"$d\"' EXIT; printf '%s' 'always o1[t] = 1' > \"$d/spec.tau\"; $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> compile \"$d/spec.tau\" -o \"$d/exe\"; echo EXIT=$?")
set_tests_properties("test_codegen_cli-always_one_emits" PROPERTIES
	PASS_REGULAR_EXPRESSION "compiled:(.*\n)*.*EXIT=0"
	FAIL_REGULAR_EXPRESSION "compile failed")

add_test(NAME "test_codegen_cli-unrealizable_exit_3"
	COMMAND bash -c "set -u; d=$(mktemp -d) || exit 1; trap 'rm -rf \"$d\"' EXIT; printf '%s' 'always (o1[t] = 1 && o1[t] = 0)' > \"$d/spec.tau\"; $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> compile \"$d/spec.tau\" -o \"$d/exe\"; echo EXIT=$?")
set_tests_properties("test_codegen_cli-unrealizable_exit_3" PROPERTIES
	PASS_REGULAR_EXPRESSION "compile failed: compile: spec is UNREALIZABLE(.*\n)*.*EXIT=1")

# CG-N6: ltlsynt stubbed to fail like an internal/usage error (exit 2, no
# verdict line -- see tests/repl/stubs/ltlsynt). The synthesis layer must
# surface this as a failure without ever claiming UNREALIZABLE (that verdict
# means something specific and different: the spec was actually decided).
add_test(NAME "test_codegen_cli-backend_failure_exit_4"
	COMMAND bash -c "set -u; d=$(mktemp -d) || exit 1; trap 'rm -rf \"$d\"' EXIT; printf '%s' 'F (o1[t] = 1)' > \"$d/spec.tau\"; PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs:$PATH $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> compile \"$d/spec.tau\" -o \"$d/exe\"; echo EXIT=$?")
set_tests_properties("test_codegen_cli-backend_failure_exit_4" PROPERTIES
	PASS_REGULAR_EXPRESSION "compile failed:(.*\n)*.*EXIT=1"
	FAIL_REGULAR_EXPRESSION "UNREALIZABLE|terminate called")
