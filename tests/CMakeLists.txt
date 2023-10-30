cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(DOCTEST_HEADER "${PROJECT_SOURCE_DIR}/src/doctest.h" CACHE PATH "Doctest header")
if (NOT EXISTS "${DOCTEST_HEADER}")
	message(STATUS "Downloading doctest to '${PROJECT_SOURCE_DIR}'")
	find_package(Wget REQUIRED)
	# TODO we should use a fixed tag instead of master
	execute_process(COMMAND "${WGET_EXECUTABLE}" https://raw.githubusercontent.com/doctest/doctest/master/doctest/doctest.h -P ${PROJECT_SOURCE_DIR}/src)
endif ()

add_library(doctest INTERFACE)
target_compile_definitions(doctest INTERFACE TAU_USE_DOCTEST)
set(DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE "true")

add_link_options("-flto")

set(DEBUG_TAU_NORMALIZER debug_tau_normalizer.cpp)
add_executable(debug_tau_normalizer ${DEBUG_TAU_NORMALIZER})
target_setup(debug_tau_normalizer)
target_link_libraries(debug_tau_normalizer TAUo)

set(TEST_BA test_ba.cpp)
add_executable(test_ba ${TEST_BA})
target_setup(test_ba)
target_link_libraries(test_ba TAUo doctest)

set(TEST_RULES test_rules.cpp)
add_executable(test_rules ${TEST_RULES})
target_setup(test_rules)
target_link_libraries(test_rules TAUo doctest)

set(TEST_BUILDERS test_builders.cpp)
add_executable(test_builders ${TEST_BUILDERS})
target_setup(test_builders)
target_link_libraries(test_builders TAUo doctest)

set(TEST_REWRITING test_rewriting.cpp)
add_executable(test_rewriting ${TEST_REWRITING})
target_setup(test_rewriting)
target_link_libraries(test_rewriting TAUo doctest)

set(TEST_FORMULA test_formula.cpp)
add_executable(test_formula ${TEST_FORMULA})
target_setup(test_formula)
target_link_libraries(test_formula TAUo doctest)

set(TEST_TAU_PARSER test_tau_parser.cpp)
add_executable(test_tau_parser ${TEST_TAU_PARSER})
target_setup(test_tau_parser)
target_link_libraries(test_tau_parser TAUo doctest)

set(TEST_NORMALIZER2 test_normalizer2.cpp)
add_executable(test_normalizer2 ${TEST_NORMALIZER2})
target_setup(test_normalizer2)
target_link_libraries(test_normalizer2 TAUo doctest)

set(TEST_TYPE_SYSTEM test_type_system.cpp)
add_executable(test_type_system ${TEST_TYPE_SYSTEM})
target_setup(test_type_system)
target_link_libraries(test_type_system TAUo doctest)

set(TEST_BINDINGS test_bindings.cpp)
add_executable(test_bindings ${TEST_BINDINGS})
target_setup(test_bindings)
target_link_libraries(test_bindings TAUo doctest)

set(TEST_TRAVERSAL test_traversal.cpp)
add_executable(test_traversal ${TEST_TRAVERSAL})
target_setup(test_traversal)
target_link_libraries(test_traversal TAUo doctest)

set(TEST_INTEGRATION test_integration.cpp)
add_executable(test_integration ${TEST_INTEGRATION})
target_setup(test_integration)
target_link_libraries(test_integration TAUo doctest)

add_test(NAME test_rules COMMAND "${PROJECT_BINARY_DIR}/test_rules")
add_test(NAME test_builders COMMAND "${PROJECT_BINARY_DIR}/test_builders")
add_test(NAME test_ba COMMAND "${PROJECT_BINARY_DIR}/test_ba")
add_test(NAME test_rewriting COMMAND "${PROJECT_BINARY_DIR}/test_rewriting")
add_test(NAME test_formula COMMAND "${PROJECT_BINARY_DIR}/test_formula")
add_test(NAME test_tau_parser COMMAND "${PROJECT_BINARY_DIR}/test_tau_parser")
add_test(NAME test_normalizer2 COMMAND "${PROJECT_BINARY_DIR}/test_normalizer2")
add_test(NAME test_type_system COMMAND "${PROJECT_BINARY_DIR}/test_type_system")
add_test(NAME test_bindings COMMAND "${PROJECT_BINARY_DIR}/test_bindings")
add_test(NAME test_traversal COMMAND "${PROJECT_BINARY_DIR}/test_traversal")
add_test(NAME test_integration COMMAND "${PROJECT_BINARY_DIR}/test_integration")
