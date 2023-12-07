cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	test_ba
	test_bool
	test_hbdd
	test_rules-bf_execution
	test_rules-bf_parsing
	test_rules-wff_execution
	test_rules-wff_parsing
	test_builders
	test_rewriting
	test_formula
	test_tau_parser
	test_normalizer2
	test_type_system
	test_bindings
	test_traversal
)

foreach(N IN LISTS TESTS)
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} TAUo doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()
