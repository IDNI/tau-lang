cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	apply_rule
	base_ba_dispatcher
	bv_ba_helpers
	ba_constants
	ba_types
	ba_types_inference
	bool
	bv_ba-simplification
	bv_ba-solver1
	bv_ba-solver2
	bv_ba-cast
	cvc5
	execution
	gc
	hbdd
	interpreter
	normal_forms
	nso_ba
	nso_rr
	product_ba
	sbf_ba_parsing
	sbf_ba_splitter
	tau_ba
	tau_bdd
	tau_spec
	tau_tree
	tau_tree_from_parser
	tau_tree_printers
	traversal
	union_find
	bitblasting_predicates
	var_dict
	variant_ba
)

foreach(X IN LISTS TESTS)
	add("test_${X}")
endforeach()
