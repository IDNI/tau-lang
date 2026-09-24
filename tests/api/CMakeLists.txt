cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TESTS
	tref_api
	string_api
	limits
	memory_budget
	ltl_execution
	normal_forms_xor
	undecided_verdicts
)

foreach(TEST IN LISTS TESTS)
	add("test_api-${TEST}")
endforeach()
