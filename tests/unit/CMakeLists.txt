cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	# tree independent tests
	bool
	product_ba
	hbdd
	variant_ba

	# new tree
	bindings
	builders
	normal_forms
	nso_rr
	rules-bf_parsing
	rules-wff_parsing
	sbf_ba_parsing
	tau_parser
	traversal

	# empty tests?
	# interpreter
	# normalizer
	# tau
)

set(DEPRECIATING_TESTS
	# tests to implement with new tree
	rules-bf_execution_depreciating
	rules-wff_execution_depreciating
	type_system_depreciating
	normal_forms_depreciating
	make_node_hook_depreciating
	
	# finished
	bindings_depreciating
	builders_depreciating
	nso_rr_depreciating
	rules-bf_parsing_depreciating
	rules-wff_parsing_depreciating
	sbf_ba_binding_depreciating
	tau_parser_depreciating
	traversal_depreciating
	rewriting_depreciating # moved to the parser repository	
)

foreach(X IN LISTS TESTS)
	set(N "test_${X}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()

foreach(X IN LISTS DEPRECIATING_TESTS)
	set(N "test_${X}")
	add_executable(${N} "depreciating/${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()
