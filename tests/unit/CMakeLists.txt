cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	apply_rule
	base_ba_dispatcher
	bv_ba_helpers
	ba_constants
	ba_types
	ba_types_inference
	bounded_cache
	ba_providers
	composite_ba_provider
	temporal_connectives
	bv_ba-simplification
	bv_ba-solver1
	bv_ba-solver2
	bv_ba-cast
	cvc5
	execution
	gc
	hooks
	interpreter
	interpreter_api_extensions
	pointwise_revision
	nlang_grammar
	#pwr_debug
	#pwr_deepseek_1
	#pwr_deepseek_2
	#pwr_deepseek_3
	#pwr_deepseek_4
	ltl_aba
	#ltl_adversarial
	#ltl_adversarial2
	ltl_benchmarks
	ltl_multi_type_oracle
	ltl_correctness
	#ltl_deepseek_generated
	#ltl_deepseek2
	#ltl_nlang_deepseek_1
	#ltl_nlang_deepseek_2
	#ltl_nlang_deepseek_3
	#ltl_nlang_deepseek_4
	#ltl_nlang_deepseek_5
	ltl_fuzz
	#ltl_fuzz100
	ltl_negative
	ltl_soundness_regression
	ltl_perf
	ltl_many_sorted
	ltl_qlt_bv
	ltl_strategy_exec
	qlt_oracle
	sbf_oracle

	algorithm_a
	algorithm_b
	algorithm_c
	algorithm_d
	algorithm_picker
	semantic_pwr
	cpp_codegen
	cpp_codegen_pwr
	cpp_codegen_roundtrip
	cpp_codegen_bench
	cpp_codegen_data_atoms
	gr1_detect
	liveness_decomp
	normal_forms
	normalizer_uf_arithmetic
	nso_ba
	nso_rr
	omcat_types
	spec_decompose
	product_ba
	sbf_ba_splitter
	tau_bdd
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

# Non-doctest evaluator for scripted fuzz testing
add_executable(tau_eval tau_eval.cpp)
target_setup(tau_eval)
target_include_directories(tau_eval PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(tau_eval ${TAU_STATIC_LIB_NAME} cvc5::cvc5)
add_dependencies(tau_eval tau_header)
target_compile_options(tau_eval PUBLIC -Wno-unused-function)

# add unit tests for the parser
add_subdirectory(boolean_algebras)
add_subdirectory(parser)