cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	### ok
	ba_types_inference
	nso_rr_partial_eval
	sbf2
	tau_ba1
	wff_hooks
	
	### only several tests fail
	bf_fixed_point        # remaining 1 test (no initial condition)
	bf_hooks              # remaining 1 check_type (infloop) 
	normal_forms          # remaining 2 to_snf tests
	ref_types             # remaining 3 infloops
	tau_ba3               # remaining 1 test + 2 infloops
	
	### inf loop - probably equality, hooks, unresolved/wrong types or bad traversal
	# nso_rr_fixed_point
	# wff_normalization
	
	### other fails
	bf_normalization
	interpreter
	nso_rr_execution  # (unresolved wff_ref)
	quantifiers
	satisfiability
	sbf1
	solver
	splitter
	splitter2
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
