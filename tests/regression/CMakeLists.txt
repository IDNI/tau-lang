cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	regression_sample
)

foreach(N IN LISTS TESTS)
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} TAUo doctest)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()