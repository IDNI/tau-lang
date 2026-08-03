cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	bf_fixed_point
	bf_hooks
	bf_normalization
	interpreter
	interpreter_ltl_multiio
	pointwise_revision
	ltl_aba_execution
	nso_rr_execution
	nso_rr_fixed_point
	nso_rr_partial_eval
	quantifiers
	splitter
	splitter2
	tau_ba1
	tau_ba3
	wff_hooks
	wff_normalization
	ba_types_inference
	semantic
	type_scoped_resolver
	heuristics-simplify_using_equality
	heuristics-syntactic_path_simplification
	heuristics-ex_subs_based_elimination
	heuristics-trivial_skolem
	ctl_star
	ocfuncs
)

foreach(X IN LISTS TESTS)
	add("test_integration-${X}")
	if(NOT TARGET "test_integration-${X}")
		continue()
	endif()
	target_include_directories("test_integration-${X}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()

foreach(X IN ITEMS ctl_star ocfuncs)
	if(NOT TARGET "test_integration-${X}")
		continue()
	endif()
	target_include_directories("test_integration-${X}" PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/satisfiability)
endforeach()

add_subdirectory(satisfiability)
add_subdirectory(solver)
