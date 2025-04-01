cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	tau_parser
	traversal

	# tests for depreciating rewriter tree
	product_ba_depreciating
	bool_depreciating
	hbdd_depreciating
	rules-bf_execution_depreciating
	rules-bf_parsing_depreciating
	rules-wff_execution_depreciating
	rules-wff_parsing_depreciating
	builders_depreciating
	rewriting_depreciating
	nso_rr_depreciating
	tau_parser_depreciating
	normalizer_depreciating
	type_system_depreciating
	bindings_depreciating
	sbf_ba_binding_depreciating
	traversal_depreciating
	tau_depreciating
	variant_ba_depreciating
	normal_forms_depreciating
	make_node_hook_depreciating
	interpreter_depreciating
)

foreach(X IN LISTS TESTS)
	set(N "test_${X}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()
