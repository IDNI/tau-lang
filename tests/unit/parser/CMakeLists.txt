set(TESTS
	by_grammar
	hsb_grammar
	qint_grammar
	qlt_grammar
	sbf_ba_parsing
	tau_spec
)

foreach(X IN LISTS TESTS)
	add("test_${X}")
	# The inherited add() sets include to CMAKE_CURRENT_SOURCE_DIR/.., which
	# is tests/unit/ from this subdirectory.  The test helpers (test_init.h,
	# test_tau_helpers.h, …) live one level higher in tests/, so add that too.
	target_include_directories("test_${X}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
endforeach()
