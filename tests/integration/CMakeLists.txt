cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	# ### ok
	nso_rr_partial_eval
	sbf2
	ba_types_inference

	# ### equality, hooks or bad traversal
	# normal_forms          # last 2 to_snf tests

	### inf loop - probably equality, hooks or bad traversal
	# bf_fixed_point
	# bf_normalization
	# quantifiers
	# splitter
	# wff_normalization
	# nso_rr_fixed_point
	
	### assertions and aborts
	solver
	splitter2
	satisfiability
	interpreter

	# ### requires BA type inference and fixed 1 and 0 with types
	tau_ba1
	tau_ba3
	bf_hooks
	wff_hooks
	
	### does not crash or assert, just don't pass
	sbf1
	ref_types
	nso_rr_execution
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
