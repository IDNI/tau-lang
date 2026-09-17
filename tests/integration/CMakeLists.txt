cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	adt
	antiprenexing
	bf_fixed_point
	bf_hooks
	bf_normalization
	eliminability
	interpreter
	interpreter_ltl_multiio
	pointwise_revision
	ltl_aba_execution
	nso_rr_execution
	nso_rr_fixed_point
	nso_rr_partial_eval
	placement_matrix
	quantifiers
	satisfiability6
	normalizer_helpers
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
	bv_stress_check
	ba_component_factoring
	simp_tau_unsat_valid_unitwise
	order_atoms_reduce
	bv_quantifier_free_decision
	bv_case_split
)

foreach(X IN LISTS TESTS)
	add("test_integration-${X}")
	if(NOT TARGET "test_integration-${X}")
		continue()
	endif()
	target_include_directories("test_integration-${X}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()

foreach(X IN ITEMS ctl_star satisfiability6 ba_component_factoring
		simp_tau_unsat_valid_unitwise)
	if(NOT TARGET "test_integration-${X}")
		continue()
	endif()
	target_include_directories("test_integration-${X}" PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/satisfiability)
endforeach()

add_subdirectory(satisfiability)
add_subdirectory(solver)

# Bounds the whole binary, not just one TEST_CASE: nso_rr_fixed_point's
# "cross-family type-blocked mutual recursion" case exercises
# calculate_fixed_point's residual-ref probe cap (src/normalizer.tmpl.h --
# the guard that turns a hang into a fast error). A regression to the
# pre-cap unbounded probe would hang this binary instead of failing the
# assertion, mirroring how the REPL tests bound their own pre-fix-hang
# reproducers with a ctest TIMEOUT (see
# tests/repl/commands/test_repl-definition_cmd.cmake).
set_tests_properties(test_integration-nso_rr_fixed_point PROPERTIES TIMEOUT 60)
