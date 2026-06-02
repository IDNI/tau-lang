cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	bv_ba1
	bv_ba2
	sbf_ba
	qint
	hsb
	qlt
	mixed
)

foreach(X IN LISTS TESTS)
	add("test_integration-solver-${X}")
	# From this depth ${CMAKE_CURRENT_SOURCE_DIR}/.. in add() resolves to
	# tests/integration/ — add tests/ root so shared headers remain reachable.
	target_include_directories("test_integration-solver-${X}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
	# Override working directory to tests/ (same as all other integration tests).
	set_tests_properties("test_integration-solver-${X}" PROPERTIES
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)
endforeach()
