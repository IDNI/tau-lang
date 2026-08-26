#
# tau_codegen CLI exit codes (CG-N6 / CG-R1; re-creates the orphaned July
# harness in miniature): 0 emitted, 3 UNREALIZABLE, 4 UNKNOWN (backend
# failure), 5 REALIZABLE but not executable.
#

include(tau_repl_pack)

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

tau_repl_unsupported(_tau_skip
	"F (o1[t]:qlt = {1/2}:qlt) && G (i1[t]:qlt = {1/4}:qlt -> o1[t]:qlt != {3/4}:qlt)")
if(_tau_skip)
	tau_repl_record_skip("test_codegen_cli-non_executable_exit_5")
else()
	add_test(NAME "test_codegen_cli-non_executable_exit_5"
		COMMAND bash -c "echo 'F (o1[t]:qlt = {1/2}:qlt) && G (i1[t]:qlt = {1/4}:qlt -> o1[t]:qlt != {3/4}:qlt)' | TAU_LTL_ALG=B $<TARGET_FILE:tau_codegen> -; echo EXIT=$?")
	set_tests_properties("test_codegen_cli-non_executable_exit_5" PROPERTIES
		PASS_REGULAR_EXPRESSION "not executable(.*\n)*.*EXIT=5"
		FAIL_REGULAR_EXPRESSION "class TauProgram")
endif()

# LA-10: a spec decided by the constant-output fast path now emits a program
# with the synthesised constant embedded (it used to be exit 5, REALIZABLE
# but not executable).
tau_repl_unsupported(_tau_skip "(i1[t]:qlt = {0}:qlt) U (o1[t]:qlt = {1}:qlt)")
if(_tau_skip)
	tau_repl_record_skip("test_codegen_cli-constant_output_emits")
else()
	add_test(NAME "test_codegen_cli-constant_output_emits"
		COMMAND bash -c "echo '(i1[t]:qlt = {0}:qlt) U (o1[t]:qlt = {1}:qlt)' | $<TARGET_FILE:tau_codegen> -; echo EXIT=$?")
	set_tests_properties("test_codegen_cli-constant_output_emits" PROPERTIES
		PASS_REGULAR_EXPRESSION "synthesised constant"
		FAIL_REGULAR_EXPRESSION "not executable|UNREALIZABLE|UNKNOWN")
endif()
