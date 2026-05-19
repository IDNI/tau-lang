set(TESTS
	bool
	hbdd
	hsb
	hsb_splitter
	qint
	qlt
	sbf
	tau_ba
)

foreach(X IN LISTS TESTS)
	add("test_${X}")
	# The inherited add() sets include to CMAKE_CURRENT_SOURCE_DIR/.., which
	# is tests/unit/ from this subdirectory.  The test helpers (test_init.h,
	# test_Bool_helpers.h, …) live one level higher in tests/, so add that too.
	target_include_directories("test_${X}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
endforeach()
