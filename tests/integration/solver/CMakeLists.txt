cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	bv_ba1
	bv_ba2
	sbf_ba
)

foreach(X IN LISTS TESTS)
	set(TARGET "test_integration-solver-${X}")
	add(${TARGET})
	# The add() function uses ${CMAKE_CURRENT_SOURCE_DIR}/.. which resolves to
	# tests/integration/ from this depth; add the tests/ root explicitly so
	# test_init.h, test_tau_helpers.h, and test_sbf_ba_helpers.h remain reachable.
	target_include_directories(${TARGET} PUBLIC ${PROJECT_SOURCE_DIR}/tests)
endforeach()
