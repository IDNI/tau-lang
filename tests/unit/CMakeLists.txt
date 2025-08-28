cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	ba_constants
	ba_types_inference
	bool
	gc
	hbdd
	normal_forms
	nso_rr
	product_ba
	rules-bf_parsing
	rules-wff_execution
	rules-wff_parsing
	sbf_ba_parsing
	tau_tree_from_parser
	tau_tree_printers
	traversal
	variant_ba
	interpreter
	ba_cvc5
	cvc5
	resolver
)

foreach(X IN LISTS TESTS)
	set(N "test_${X}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_include_directories(${N} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest ${CVC5_LIB})
	add_dependencies(${N} cvc5)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	target_compile_definitions_if(${N} PUBLIC TAU_LOG_TRACE_TESTS)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
	set_tests_properties(${N} PROPERTIES
	    ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/external/cvc5/build/src:$LD_LIBRARY_PATH"
    )
endforeach()
