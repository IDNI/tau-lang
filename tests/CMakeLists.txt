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
set(DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE "true")

function(add test_name)
	# message(STATUS "adding test: ${test_name}")
	set(N "${test_name}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_include_directories(${N} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
	# Force-include extern template declarations for the test BA packs.
	# Their bodies are explicitly instantiated in libTAU's
	# instantiate_test_pack.cpp, so each test TU links to one shared
	# copy instead of re-instantiating the heaviest pipeline templates
	# per binary.  Production binaries (main, tau_codegen) use the same
	# full pack but are intentionally not force-included here.
	#
	# Skip the force-include for test_cvc5 — it declares its own
	# `static TermManager cvc5_term_manager` at file scope, which
	# would collide with the one transitively brought in by tau.h.
	if(NOT "${N}" STREQUAL "test_cvc5")
		target_compile_options(${N} PUBLIC
			-include ${PROJECT_SOURCE_DIR}/src/extern_template_test_pack.h)
	endif()
	target_link_libraries(${N} ${TAU_STATIC_LIB_NAME} doctest cvc5::cvc5)
	add_dependencies(${N} tau_header)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	target_compile_definitions_if(${N} PUBLIC TAU_LOG_TRACE_TESTS)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
	# Heavy adversarial / fuzz suites need more than the ctest default;
	# their hundred-plus deep LTL+ABA-oracle cases routinely run >600s
	# on the CI hardware.  Everything else gets the default.
	if("${N}" MATCHES "^test_(ltl_adversarial|ltl_adversarial2|ltl_aba|ltl_fuzz100|ltl_soundness_regression)$")
		set(_TAU_TEST_TIMEOUT 1500)
	else()
		set(_TAU_TEST_TIMEOUT 600)
	endif()
	set_tests_properties(${N}
		PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
		ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/external/cvc5/build/src:$LD_LIBRARY_PATH"
		TIMEOUT ${_TAU_TEST_TIMEOUT})
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
