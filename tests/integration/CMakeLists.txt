cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	bdd1
	bdd2
	tau_ba1
	tau_ba3
	normal_forms
	nso_rr_execution
	nso_rr_partial_eval
	nso_rr_fixed_point
	bf_normalization
	wff_normalization
	splitter
	splitter2
	solver
	quantifiers
	wff_hooks
	bf_hooks
	interpreter
)

foreach(TEST IN LISTS TESTS)
	set(N "test_integration-${TEST}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
	set_tests_properties(${N} PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
endforeach()