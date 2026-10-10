# Standalone build of bv_no_arith_bench in its own build directory: the
# repository root comes in as a subdirectory with the static library on and
# every test suite off, so no existing CMake file or build is involved.
cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
project(bv_no_arith_bench LANGUAGES C CXX)

get_filename_component(TAU_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)

set(TAU_BUILD_STATIC_LIBRARY ON CACHE BOOL "Build static library" FORCE)
foreach(suite TESTS UNIT_TESTS INTEGRATION API_TESTS REPL_TESTS BENCHMARK)
	set(TAU_BUILD_${suite} OFF CACHE BOOL "" FORCE)
endforeach()
# The benchmark has no REPL, so the FTXUI dependency is not fetched.
set(TAU_DONT_USE_FTXUI ON CACHE BOOL "" FORCE)

add_subdirectory("${TAU_ROOT}" tau)

# target_setup() reads the root's compile settings from its caller's scope.
foreach(var COMPILE_OPTIONS TAU_LINK_OPTIONS TAU_LINKER TAU_DEFINITIONS
		TAU_GIT_DEFINITIONS TAU_STATIC_LIB_NAME CMAKE_THREAD_LIBS_INIT)
	get_directory_property(${var} DIRECTORY "${TAU_ROOT}" DEFINITION ${var})
endforeach()
foreach(var IN LISTS TAU_DEFINITIONS)
	get_directory_property(${var} DIRECTORY "${TAU_ROOT}" DEFINITION ${var})
endforeach()

# Only engine.cpp includes the tau library; the rest rebuilds in seconds.
add_executable(bv_no_arith_bench
	main.cpp
	program.cpp
	runner.cpp
	engine.cpp
	family_7.cpp
	family_8.cpp
	family_1.cpp
	family_2.cpp
	family_4a.cpp
	family_4b.cpp
)
target_setup(bv_no_arith_bench)
target_include_directories(bv_no_arith_bench PRIVATE "${TAU_ROOT}/tests")
target_link_libraries(bv_no_arith_bench ${TAU_STATIC_LIB_NAME})
add_dependencies(bv_no_arith_bench tau_header)
