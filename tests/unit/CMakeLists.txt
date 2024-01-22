cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	ba
	bool
	hbdd
	rules-bf_execution
	rules-bf_parsing
	rules-wff_execution
	rules-wff_parsing
	rules-tau_execution
	rules-tau_parsing
	builders
	rewriting
	nso_rr
	tau_parser
	normalizer2
	type_system
	bindings
	iterators
	traversal
	tau
)

foreach(X IN LISTS TESTS)
	set(N "test_${X}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()
