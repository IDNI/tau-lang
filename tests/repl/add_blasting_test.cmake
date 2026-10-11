include(tau_repl_pack)

# include(add_blasting_test) to register blasting on/off equivalence tests.

# Per-test timeout (s).
set(TAU_BLASTING_TEST_TIMEOUT 1200)

# Resolve the checker path HERE, at include time, not inside the function.
set(TAU_BLASTING_CHECKER "${CMAKE_CURRENT_LIST_DIR}/compare_blasting.sh")

# add_blasting_equiv_test(<name> <formula> [REQUIRES ltlsynt|hostfs|<ba-id> ...])
function(add_blasting_equiv_test test_name formula)
	cmake_parse_arguments(PARSE_ARGV 2 _tau "" "" REQUIRES)
	tau_repl_gate_case("test_repl-${test_name}" formula)
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash "${TAU_BLASTING_CHECKER}"
			"${TAU_RUN}" "${formula}")
	# No PASS_REGULAR_EXPRESSION: the script's EXIT CODE is the verdict.
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		TIMEOUT ${TAU_BLASTING_TEST_TIMEOUT})
	if(TAU_ADDRESS_SANITIZER)
		# The asan shadow memory needs more address space than the limit.
		set_tests_properties("test_repl-${test_name}" PROPERTIES
			ENVIRONMENT "TAU_BLASTING_NO_MEMORY_LIMIT=1")
	endif()
	tau_repl_disable_skipped("test_repl-${test_name}")
endfunction()
