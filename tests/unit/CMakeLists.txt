cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	bool
	bindings
	builders
	hbdd
	interpreter
	make_node_hook
	normal_forms
	normalizer
	nso_rr
	product_ba
	rewriting
	rules-bf_execution
	rules-bf_parsing
	rules-wff_execution
	rules-wff_parsing
	sbf_ba_binding
	tau
	tau_parser
	traversal
	type_system
	variant_ba
)

foreach(X IN LISTS TESTS)
	set(N "test_${X}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()
