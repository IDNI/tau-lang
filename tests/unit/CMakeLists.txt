cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	ba
	bool
	hbdd
	rules-bf_execution
	rules-bf_parsing
	rules-wff_execution
	rules-wff_parsing
	builders
	rewriting
	formula
	tau_parser
	normalizer2
	type_system
	bindings
	traversal
	rec_relations-execution
)

foreach(X IN LISTS TESTS)
	set(N "test_${X}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} TAUo doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()
