cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

function(add_valgrind_file_test test_name test_file)
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash -c "cat \"${test_file}\" | valgrind --tool=callgrind --callgrind-out-file=${PROJECT_BINARY_DIR}/test_benchmark-${test_name}-${GIT_COMMIT_HASH}.callgrind.out $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
endfunction()

function(add_valgrind_command_test test_name test_cmd)
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash -c "echo \"${test_cmd}. q\" | valgrind --tool=callgrind --callgrind-out-file=${PROJECT_BINARY_DIR}/test_benchmark-${test_name}-${GIT_COMMIT_HASH}.callgrind.out $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
endfunction()

set(BENCHMARK_TESTS
	wff_normalization
	interpreter
	cvc5
)

# adding benchmark tests
foreach(X IN LISTS BENCHMARK_TESTS)
	set(N "test_benchmark-${X}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB})
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()

# adding valgrind tests for wff normalization

add_valgrind_command_test(luccas_example "n ex a ex b ex c ex d ex f ex e (ax + bx' = cy + dy' || ax + bx' != ey + fy') <-> (ax + bx' = cy + gy')")
add_valgrind_command_test(ohads_example "n all a all b all c all d all p all q all r all s all m all j all k all l (all x ex y f(x,y)=0 || (g(x,y)=0 && h(x,y)!=0)) && !(all y0 all y1 all z0 all z1 ex x f(x,y1x+y0'x)=0 && (g(x,y1x+y0'x)!=0 && h(x,y1x+y0'x)=0))")
