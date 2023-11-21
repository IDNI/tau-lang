cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(DOCTEST_HEADER "${PROJECT_SOURCE_DIR}/src/doctest.h" CACHE PATH "Doctest header")
if (NOT EXISTS "${DOCTEST_HEADER}")
	message(STATUS "Downloading doctest to '${PROJECT_SOURCE_DIR}'")
	find_package(Wget REQUIRED)
	# TODO (HIGH) we should use a fixed tag instead of master
	execute_process(COMMAND "${WGET_EXECUTABLE}" https://raw.githubusercontent.com/doctest/doctest/master/doctest/doctest.h -P ${PROJECT_SOURCE_DIR}/src)
endif ()

add_library(doctest INTERFACE)
target_compile_definitions(doctest INTERFACE TAU_USE_DOCTEST)
set(DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE "true")

add_link_options("-flto")

set(TESTS
	ba
	bool
	hbdd
	rules
	builders
	rewriting
	formula
	tau_parser
	normalizer2
	type_system
	bindings
	traversal
	integration
)

foreach(X IN LISTS TESTS)
	set(N "test_${X}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N}
		${OBJECT_LIB_NAME}
		${IDNI_PARSER_OBJECT_LIB}
		doctest
	)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()

add_executable(debug_tau_normalizer debug_tau_normalizer.cpp)
target_setup(debug_tau_normalizer)
target_link_libraries(debug_tau_normalizer ${OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB})
