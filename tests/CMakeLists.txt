set(DOCTEST_HEADER "${PROJECT_SOURCE_DIR}/src/doctest.h" CACHE PATH "Doctest header")
if (NOT EXISTS "${DOCTEST_HEADER}")
	set(DOCTEST_VERSION "v2.4.11")
	set(DOCTEST_URL "https://raw.githubusercontent.com/doctest/doctest/${DOCTEST_VERSION}/doctest/doctest.h")
	set(DOCTEST_SHA256 "44faa038e9c3f9728efbda143748d01124ea0a27f4bf78f35a15d8fab2e039fb")
	message(STATUS "Downloading doctest ${DOCTEST_VERSION} to '${DOCTEST_HEADER}'")
	file(DOWNLOAD "${DOCTEST_URL}" "${DOCTEST_HEADER}"
		EXPECTED_HASH SHA256=${DOCTEST_SHA256}
		STATUS DOCTEST_DOWNLOAD_STATUS
		TLS_VERIFY ON)
	list(GET DOCTEST_DOWNLOAD_STATUS 0 DOCTEST_DOWNLOAD_RESULT)
	if(NOT DOCTEST_DOWNLOAD_RESULT EQUAL 0)
		list(GET DOCTEST_DOWNLOAD_STATUS 1 DOCTEST_DOWNLOAD_MESSAGE)
		message(FATAL_ERROR "Failed to download doctest ${DOCTEST_VERSION}: ${DOCTEST_DOWNLOAD_MESSAGE}")
	endif()
endif ()

add_library(doctest INTERFACE)
target_compile_definitions(doctest INTERFACE TAU_USE_DOCTEST)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(doctest INTERFACE -Wno-c2y-extensions "-Wno-#warnings")
endif()
# SW-11: this is a doctest PREPROCESSOR macro; a CMake variable had no
# effect. Define it for real on the test compile.
add_compile_definitions(DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE)

set(TAU_SKIP_TESTS
	test_integration-satisfiability-bool
	test_integration-satisfiability-sbf
	test_integration-satisfiability-hsb
	test_integration-satisfiability-qint
	test_integration-satisfiability-qlt
	test_integration-satisfiability-tau_ba
	test_integration-satisfiability-mixed
	test_integration-satisfiability-bv
	test_ltl_benchmarks
	test_ltl_multi_type_oracle
	test_ltl_soundness_regression
	test_ltl_many_sorted
	test_by_grammar
	test_ltl_correctness
	test_integration-interpreter_ltl_multiio
	test_integration-solver-hsb
	test_integration-solver-qint
	test_integration-solver-qlt
	test_integration-solver-mixed
	test_integration-solver-sbf
	test_tau_tree_printers
	test_integration-quantifiers
	test_temporal_connectives
	test_integration-ltl_aba_execution
	test_integration-blasting_correctness_check1
	test_integration-blasting_correctness_check2
	test_integration-blasting_correctness_check3
	test_integration-blasting_correctness_check4
	test_integration-blasting_correctness_check5
	test_ltl_strategy_exec
	CACHE STRING "Test targets skipped because covered by REPL tests")

function(add test_name)
	# message(STATUS "adding test: ${test_name}")
	if(test_name IN_LIST TAU_SKIP_TESTS)
		message(STATUS "Skipping C++ test ${test_name} (covered by REPL test)")
		return()
	endif()
	set(N "${test_name}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_include_directories(${N} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
	# extern template declarations for the test BA packs, instantiated in libTAU
	# test_cvc5 is skipped: its own file-scope TermManager collides with tau.h's
	if(NOT "${N}" STREQUAL "test_cvc5")
		target_compile_options(${N} PUBLIC
			-include ${PROJECT_SOURCE_DIR}/src/extern_template_test_pack.h)
	endif()
	target_link_libraries(${N} ${TAU_STATIC_LIB_NAME} doctest cvc5::cvc5)
	add_dependencies(${N} tau_header)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	target_compile_definitions_if(${N} PUBLIC TAU_LOG_TRACE_TESTS)
	target_compile_options(${N} PUBLIC -fno-lto)
	target_link_options(${N} PUBLIC -fno-lto)
	target_compile_options(${N} PUBLIC -Wstrict-overflow=2 -Wno-error=strict-overflow)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
	# Heavy adversarial / fuzz suites need more than the ctest default;
	# their hundred-plus deep LTL+ABA-oracle cases routinely run >600s
	# on the CI hardware.  Everything else gets the default.
	if("${N}" MATCHES "^test_(ltl_aba|ltl_soundness_regression)$")
		set(_TAU_TEST_TIMEOUT 1500)
	else()
		set(_TAU_TEST_TIMEOUT 600)
	endif()
	set_tests_properties(${N}
		PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
		ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/external/cvc5/build/src:$ENV{LD_LIBRARY_PATH}"
		TIMEOUT ${_TAU_TEST_TIMEOUT})
endfunction()

#
# Integration testing
#
set_property(CACHE TAU_BUILD_INTEGRATION PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_INTEGRATION)
	enable_testing()
	add_subdirectory(integration)
endif ()

#
# Unit testing
#
set_property(CACHE TAU_BUILD_UNIT_TESTS PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_UNIT_TESTS)
	enable_testing()
	add_subdirectory(unit)
endif ()

#
# API testing
#
set_property(CACHE TAU_BUILD_API_TESTS PROPERTY STRINGS "OFF" "ON")
if (TAU_BUILD_API_TESTS)
	enable_testing()
	add_subdirectory(api)
endif ()

#
# REPL testing
#
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
