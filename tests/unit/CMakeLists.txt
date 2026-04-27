cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	bv_ba_helpers
	ba_constants
	ba_types
	ba_types_inference
	bool
	bv_ba-simplification
	bv_ba-solver1
	bv_ba-solver2
	cvc5
	gc
	hbdd
	interpreter
	normal_forms
	product_ba
	sbf_ba_parsing
    tau_bdd
	tau_spec
	tau_tree_from_parser
	tau_tree_printers
	traversal
	union_find
	bitblasting_predicates
)

foreach(X IN LISTS TESTS)
	add("test_${X}")
endforeach()
