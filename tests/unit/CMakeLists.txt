cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	# tree independent tests
	bool
	product_ba
	hbdd
	variant_ba

	# new tree
	ba_constants
	bindings
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

	# obsoleted
	# builders       # covered code is not used anymore
)

foreach(X IN LISTS TESTS)
	set(N "test_${X}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()
