cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	bf_fixed_point
	bf_hooks
	bf_normalization
	normal_forms
	nso_rr_execution
	nso_rr_fixed_point
	nso_rr_partial_eval
	quantifiers
	ref_types
	satisfiability
	sbf1
	sbf2
	splitter
	splitter2
	tau_ba1
	tau_ba3
	wff_hooks
	wff_normalization
	# solver
	# interpreter
)

foreach(TEST IN LISTS TESTS)
	set(N "test_integration-${TEST}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_include_directories(${N} PUBLIC ${CMAKE_SOURCE_DIR}/tests)
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
	set_tests_properties(${N} PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
endforeach()
