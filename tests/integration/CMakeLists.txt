cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	adt
	antiprenexing
	bf_fixed_point
	bf_hooks
	bv_hooks
	bf_normalization
	eliminability
	interpreter
	nso_rr_execution
	nso_rr_fixed_point
	nso_rr_partial_eval
	placement_matrix
	quantifiers
	satisfiability1
	satisfiability2
	satisfiability3
	satisfiability4
	satisfiability5
	satisfiability6
	normalizer_helpers
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
	semantic
	type_scoped_resolver
	heuristics-simplify_using_equality
	heuristics-syntactic_path_simplification
	heuristics-ex_subs_based_elimination
	heuristics-bv_ba_custom_simplification
	heuristics-bv_ba_custom_simplification_equivalence
	heuristics-bv_ba_cvc5_simplification
	heuristics-bv_predicate_blasting
	heuristics-bv_predicate_blasting_predicates
	heuristics-trivial_skolem
	blasting_correctness_check1
	blasting_correctness_check2
	blasting_correctness_check3
	blasting_correctness_check4
	blasting_correctness_check5
	bv_stress_check
	ba_component_factoring
	simp_tau_unsat_valid_unitwise
	bv_quantifier_free_decision
)

foreach(X IN LISTS TESTS)
	add("test_integration-${X}")
endforeach()

# Bounds the whole binary, not just one TEST_CASE: nso_rr_fixed_point's
# "cross-family type-blocked mutual recursion" case exercises
# calculate_fixed_point's residual-ref probe cap (src/normalizer.tmpl.h --
# the guard that turns a hang into a fast error). A regression to the
# pre-cap unbounded probe would hang this binary instead of failing the
# assertion, mirroring how the REPL tests bound their own pre-fix-hang
# reproducers with a ctest TIMEOUT (see
# tests/repl/commands/test_repl-definition_cmd.cmake).
set_tests_properties(test_integration-nso_rr_fixed_point PROPERTIES TIMEOUT 60)
