cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	ba_types_inference
	bf_fixed_point
	bf_hooks
	bf_normalization
	heuristics-bv_ba_custom_simplification
	heuristics-bv_ba_cvc5_simplification
	heuristics-ex_subs_based_elimination
	interpreter
	nso_rr_execution
	nso_rr_fixed_point
	nso_rr_partial_eval
	quantifiers
	semantic
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
	type_scoped_resolver
	wff_hooks
	wff_normalization
	ba_types_inference
	semantic
	type_scoped_resolver
	heuristics-ex_subs_based_elimination
	heuristics-bv_ba_custom_simplification
	heuristics-bv_ba_cvc5_simplification
	heuristics-bv_predicate_blasting
	heuristics-bv_predicate_blasting_predicates
)

foreach(X IN LISTS TESTS)
	add("test_integration-${X}")
endforeach()
