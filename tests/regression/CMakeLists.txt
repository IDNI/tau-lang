cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	test_regression-issue_applying_once_defs
)

foreach(N IN LISTS TESTS)
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()