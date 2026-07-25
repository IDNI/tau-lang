# include(add_blasting_test) to register blasting on/off equivalence tests.

# Per-test timeout (s); matches the C++ helper TIME_LIMIT_MS constant.
set(TAU_BLASTING_TEST_TIMEOUT 1200)

# Resolve the checker path HERE, at include time, not inside the function.
set(TAU_BLASTING_CHECKER "${CMAKE_CURRENT_LIST_DIR}/compare_blasting.sh")

function(add_blasting_equiv_test test_name formula)
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash "${TAU_BLASTING_CHECKER}"
			"$<TARGET_FILE:${TAU_EXECUTABLE_NAME}>" "${formula}")
	# No PASS_REGULAR_EXPRESSION: the script's EXIT CODE is the verdict.
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		TIMEOUT ${TAU_BLASTING_TEST_TIMEOUT}
	)
endfunction()
