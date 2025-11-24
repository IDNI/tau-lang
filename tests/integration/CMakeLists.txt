cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	bf_fixed_point
	bf_hooks
	bf_normalization
	quantifiers
	nso_rr_execution
	nso_rr_fixed_point
	nso_rr_partial_eval
	interpreter
	satisfiability1
	satisfiability2
	satisfiability3
	satisfiability4
	satisfiability5
	sbf1
	sbf2
	solver
	splitter
	splitter2
	tau_ba1
	tau_ba3
	wff_hooks
	wff_normalization
	ba_types_inference
)

foreach(TEST IN LISTS TESTS)
	set(N "test_integration-${TEST}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_include_directories(${N} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest cvc5::cvc5)
	add_dependencies(${N} tau_header)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	target_compile_definitions_if(${N} PUBLIC TAU_LOG_TRACE_TESTS)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
	set_tests_properties(${N}
		PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
	    ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/external/cvc5/build/src:$LD_LIBRARY_PATH"
	)
endforeach()
