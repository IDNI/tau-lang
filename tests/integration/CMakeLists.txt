cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	# sbf1
	# sbf2
	# tau_ba1
	# tau_ba3
	# normal_forms
	# nso_rr_execution
	# nso_rr_partial_eval
	# nso_rr_fixed_point
	# ref_types
	# bf_fixed_point
	# bf_normalization
	# wff_normalization
	# satisfiability
	# splitter
	# splitter2
	# solver
	# quantifiers
	# wff_hooks
	# bf_hooks
	# interpreter
)

set(DEPRECIATING_TESTS
	sbf1_depreciating
	sbf2_depreciating
	tau_ba1_depreciating
	tau_ba3_depreciating
	normal_forms_depreciating
	nso_rr_execution_depreciating
	nso_rr_partial_eval_depreciating
	nso_rr_fixed_point_depreciating
	ref_types_depreciating
	bf_fixed_point_depreciating
	bf_normalization_depreciating
	wff_normalization_depreciating
	satisfiability_depreciating
	splitter_depreciating
	splitter2_depreciating
	solver_depreciating
	quantifiers_depreciating
	wff_hooks_depreciating
	bf_hooks_depreciating
	interpreter_depreciating
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

foreach(TEST IN LISTS DEPRECIATING_TESTS)
	set(N "test_integration-${TEST}")
	add_executable(${N} "depreciating/${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
	set_tests_properties(${N} PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
endforeach()