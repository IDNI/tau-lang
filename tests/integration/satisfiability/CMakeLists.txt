cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	test_integration-satisfiability-bool
	test_integration-satisfiability-bv
	test_integration-satisfiability-sbf
	test_integration-satisfiability-tau_ba
	test_integration-satisfiability-mixed
	test_integration-satisfiability-qlt
	test_integration-satisfiability-qint
	test_integration-satisfiability-hsb
)

foreach(X IN LISTS TESTS)
	add(${X})
	# add() sets includes to ${CMAKE_CURRENT_SOURCE_DIR}/.. = tests/integration/
	# We additionally need: tests/ root (for test_init.h, test_tau_helpers.h)
	# and this directory itself (for test_integration-satisfiability_helper.h).
	target_include_directories(${X} PUBLIC
		${PROJECT_SOURCE_DIR}/tests
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/..)
endforeach()
