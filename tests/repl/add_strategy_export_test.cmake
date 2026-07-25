# include(add_strategy_export_test) for the SQ1-01 strategy-export case.

# Resolved at include time, not inside the function: CMAKE_CURRENT_LIST_DIR
# expands when the function BODY runs, which is the CALLER's directory.
set(TAU_STRATEGY_EXPORT_CHECKER "${CMAKE_CURRENT_LIST_DIR}/check_strategy_export.sh")

function(add_strategy_export_test test_name test_cmd)
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash "${TAU_STRATEGY_EXPORT_CHECKER}"
			"$<TARGET_FILE:${TAU_EXECUTABLE_NAME}>" "${test_cmd}")
	# The script's EXIT CODE is the verdict, not a pattern.
	set_tests_properties("test_repl-${test_name}" PROPERTIES TIMEOUT 300)
endfunction()
