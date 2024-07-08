cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	test_integration-bdd1
	test_integration-bdd2
	test_integration-tau_ba1
	test_integration-tau_ba2
	test_integration-tau_ba3
	test_integration-nso_rr_execution
	test_integration-gssotc_rr_execution
	test_integration-nso_rr_partial_eval
	test_integration-gssotc_rr_partial_eval
	test_integration-bf_normalization
	test_integration-tau1
	test_integration-tau2
	test_integration-executor
	test_integration-satisfiability
	test_integration-wff_normalization
	test_integration-normal_forms
)

foreach(N IN LISTS TESTS)
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()