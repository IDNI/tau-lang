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

set(TESTS_WITHOUT_PARSER
	ba
	bool
	hbdd
)

set(TESTS_WITH_PARSER
	rules-bf_execution
	rules-bf_parsing
	rules-cbf_execution
	rules-cbf_parsing
	rules-wff_execution
	rules-wff_parsing
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

set(TESTS ${TESTS_WITHOUT_PARSER} ${TESTS_WITH_PARSER})

foreach(X IN LISTS TESTS)
	set(N "test_${X}")
	add_executable(${N} "${N}.cpp")
	target_setup(${N})
	target_link_libraries(${N} ${TAU_OBJECT_LIB_NAME} doctest)
	list(FIND TESTS_WITH_PARSER "${X}" _index)
	if (${_index} GREATER -1)
		target_link_libraries(${N} ${IDNI_PARSER_OBJECT_LIB})
	endif()
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
endforeach()

add_executable(debug_tau_normalizer debug_tau_normalizer.cpp)
target_setup(debug_tau_normalizer)
target_link_libraries(debug_tau_normalizer ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB})
