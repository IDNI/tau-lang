set(DOCTEST_HEADER "${PROJECT_SOURCE_DIR}/src/doctest.h" CACHE PATH "Doctest header")
if (NOT EXISTS "${DOCTEST_HEADER}")
	message(STATUS "Downloading doctest to '${PROJECT_SOURCE_DIR}'")
	find_package(Wget REQUIRED)
	# TODO (MEDIUM) we should use a fixed tag or commit instead of master
	execute_process(COMMAND "${WGET_EXECUTABLE}" https://raw.githubusercontent.com/doctest/doctest/master/doctest/doctest.h -P ${PROJECT_SOURCE_DIR}/src)
endif ()

add_library(doctest INTERFACE)
target_compile_definitions(doctest INTERFACE TAU_USE_DOCTEST)
set(DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE "true")

function(add test_name)
	# message(STATUS "adding test: ${test_name}")
	set(N "${test_name}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_include_directories(${N} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
	target_link_libraries(${N} ${TAU_STATIC_LIB_NAME} doctest cvc5::cvc5)
	add_dependencies(${N} tau_header)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	target_compile_definitions_if(${N} PUBLIC TAU_LOG_TRACE_TESTS)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
	set_tests_properties(${N}
		PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
		ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/external/cvc5/build/src:$LD_LIBRARY_PATH")
endfunction()

#
# Integration testing
#
set(TAU_BUILD_INTEGRATION ON CACHE STRING "build the tau-lang integration suite ON")
set_property(CACHE TAU_BUILD_INTEGRATION PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_INTEGRATION)
	enable_testing()
	add_subdirectory(integration)
endif ()

#
# Unit testing
#
set(TAU_BUILD_UNIT_TESTS ON CACHE STRING "build the tau-lang unit suite ON")
set_property(CACHE TAU_BUILD_UNIT_TESTS PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_UNIT_TESTS)
	enable_testing()
	add_subdirectory(unit)
endif ()

#
# API testing
#
set(TAU_BUILD_API_TESTS ON CACHE STRING "build the tau-lang api suite ON")
set_property(CACHE TAU_BUILD_API_TESTS PROPERTY STRINGS "OFF" "ON")
if (TAU_BUILD_API_TESTS)
	enable_testing()
	add_subdirectory(api)
endif ()

#
# REPL testing
#
set(TAU_BUILD_REPL_TESTS ON CACHE STRING "build the tau-lang repl suite ON")
set_property(CACHE TAU_BUILD_REPL_TESTS PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_REPL_TESTS)
	enable_testing()
	add_subdirectory(repl)
endif ()

#
# Benchmark testing
#
if (TAU_BUILD_BENCHMARK)
	enable_testing()
	add_subdirectory(benchmark)
endif ()
