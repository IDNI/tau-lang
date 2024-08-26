cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	product_ba
	bool
	hbdd
	rules-bf_execution
	rules-bf_parsing
	rules-wff_execution
	rules-wff_parsing
	builders
	rewriting
	nso_rr
	tau_parser
	normalizer
	type_system
	bindings
	bdd_binding
	traversal
	tau
	variant_ba
	normal_forms
)

foreach(X IN LISTS TESTS)
	set(N "test_${X}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()
