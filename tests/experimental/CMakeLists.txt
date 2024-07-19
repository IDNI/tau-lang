cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	execution
)

foreach(X IN LISTS TESTS)
	set(N "test_${X}")
	set(T "experimental-test_${X}")
	add_executable(${T} "${N}.cpp")
	target_setup(${T})
	target_link_libraries(${T} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB} doctest)
	target_compile_options(${T} PUBLIC -Wno-unused-function)
	add_test(NAME ${T} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()
