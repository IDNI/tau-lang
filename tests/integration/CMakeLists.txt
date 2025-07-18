cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	### ok
	ba_types_inference
	bf_fixed_point
	bf_hooks
	normal_forms
	nso_rr_execution
	nso_rr_fixed_point
	nso_rr_partial_eval
	sbf1
	sbf2
	quantifiers
	tau_ba1
	wff_hooks
	
	### only several tests fail
	ref_types             # remaining 2 tests (fp call over inferred refs incl bf, and typo undetected)
	tau_ba3               # remaining 3 quantified unresolved rr's
	
	### other fails
	interpreter
	satisfiability
	solver
	splitter
	splitter2

	### inf loop
	# bf_normalization
	# wff_normalization	
)

foreach(TEST IN LISTS TESTS)
	set(N "test_integration-${TEST}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_include_directories(${N} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	target_compile_definitions_if(${N} PUBLIC TAU_LOG_TRACE_TESTS)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
	set_tests_properties(${N} PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)
endforeach()
