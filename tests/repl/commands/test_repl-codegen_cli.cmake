#
# tau_codegen CLI exit codes (CG-N6 / CG-R1; re-creates the orphaned July
# harness in miniature): 0 emitted, 3 UNREALIZABLE, 4 UNKNOWN (backend
# failure), 5 REALIZABLE but not executable.
#

# CG-R7: a spec piped through stdin carries a trailing newline, which the
# formula parser used to reject (the same text from a file parsed fine).
add_test(NAME "test_codegen_cli-always_one_emits"
	COMMAND bash -c "echo 'always o1[t] = 1' | $<TARGET_FILE:tau_codegen> -; echo EXIT=$?")
set_tests_properties("test_codegen_cli-always_one_emits" PROPERTIES
	PASS_REGULAR_EXPRESSION "EXIT=0"
	FAIL_REGULAR_EXPRESSION "UNKNOWN|UNREALIZABLE")

add_test(NAME "test_codegen_cli-unrealizable_exit_3"
	COMMAND bash -c "echo 'always (o1[t] = 1 && o1[t] = 0)' | $<TARGET_FILE:tau_codegen> -; echo EXIT=$?")
set_tests_properties("test_codegen_cli-unrealizable_exit_3" PROPERTIES
	PASS_REGULAR_EXPRESSION "UNREALIZABLE(.*\n)*.*EXIT=3")

add_test(NAME "test_codegen_cli-backend_failure_exit_4"
	COMMAND bash -c "echo 'F (o1[t] = 1)' | PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs:$PATH $<TARGET_FILE:tau_codegen> -; echo EXIT=$?")
set_tests_properties("test_codegen_cli-backend_failure_exit_4" PROPERTIES
	PASS_REGULAR_EXPRESSION "UNKNOWN(.*\n)*.*EXIT=4"
	FAIL_REGULAR_EXPRESSION "class TauProgram|terminate called")

add_test(NAME "test_codegen_cli-non_executable_exit_5"
	COMMAND bash -c "echo 'F (o1[t]:qlt = {1/2}:qlt) && G (i1[t]:qlt = {1/4}:qlt -> o1[t]:qlt != {3/4}:qlt)' | TAU_LTL_ALG=B $<TARGET_FILE:tau_codegen> -; echo EXIT=$?")
set_tests_properties("test_codegen_cli-non_executable_exit_5" PROPERTIES
	PASS_REGULAR_EXPRESSION "not executable(.*\n)*.*EXIT=5"
	FAIL_REGULAR_EXPRESSION "class TauProgram")
