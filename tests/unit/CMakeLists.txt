cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	apply_rule
	base_ba_dispatcher
	ba_constants
	ba_conformance
	ba_descriptor_pack
	ba_types
	ba_types_inference
	bounded_cache
	canonicalize_tref
	temporal_connectives
	execution
	gc
	hooks
	interpreter
	interpreter_api_extensions
	table_step_provider
	io_context
	pointwise_revision
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
	ltl_strategy_exec

	algorithm_a
	algorithm_b
	algorithm_c
	algorithm_d
	algorithm_picker
	cli_options
	codegen_parity
	cpp_codegen
	cpp_codegen_pwr
	cpp_codegen_roundtrip
	cpp_codegen_bench
	cpp_codegen_data_atoms
	cpp_codegen_program_desc
	gr1_detect
	liveness_decomp
	mealy_extract
	normal_forms
	normalizer_uf_arithmetic
	nso_ba
	nso_rr
	ocltl_phi_delta
	ocltl_runtime_witness
	ocltl_types
	rr
	spec_decompose
	product_ba
	tau_bdd
	tau_tree
	tau_tree_from_parser
	tau_tree_printers
	traversal
	union_find
	union_find_with_sets
	bitblasting_predicates
	var_dict
	variant_ba
)

foreach(X IN LISTS TESTS)
	add("test_${X}")
endforeach()

# codegen_parity's compile+run comparison needs the tau CLI, which unit-only
# presets don't build; bake its path in when the target exists (TAU_CODEGEN_TAU_EXE
# overrides at runtime for a binary built elsewhere), else the test skips.
if(TARGET test_codegen_parity AND TARGET ${TAU_EXECUTABLE_NAME})
	target_compile_definitions(test_codegen_parity PRIVATE
		TAU_CLI_EXE_PATH="$<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
	add_dependencies(test_codegen_parity ${TAU_EXECUTABLE_NAME})
endif()

# Non-doctest evaluator for scripted fuzz testing
add_executable(tau_eval tau_eval.cpp)
target_setup(tau_eval)
target_include_directories(tau_eval PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(tau_eval ${TAU_STATIC_LIB_NAME} ${TAU_BA_LINK_LIBS})
add_dependencies(tau_eval tau_header)
target_compile_options(tau_eval PUBLIC -Wno-unused-function)

# Kept out of the default build.  Nothing references it: test_ltl_fuzz cross-
# checks against Spot's ltlsynt via popen and calls tau in-process, so this is a
# hand-driven helper only.  Build it on demand with
#   cmake --build <dir> --target tau_eval
exclude(tau_eval)

# add unit tests for the parser
add_subdirectory(boolean_algebras)
add_subdirectory(parser)