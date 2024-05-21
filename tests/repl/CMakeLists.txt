#
# REPL testing
#
set(TAU_BUILD_REPL_TESTS ON CACHE STRING "build the tau-lang repl suite ON")
set_property(CACHE TAU_BUILD_REPL_TESTS PROPERTY STRINGS "OFF" "ON")

function(add_repl_test test_name test_cmd test_regex)
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash -c "echo \"${test_cmd}. q\" | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${test_regex}")
	# add_test(NAME "test_repl-e-${test_name}"
	# 	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"${test_cmd}\"")
	# set_tests_properties("test_repl-e-${test_name}" PROPERTIES
	# 	PASS_REGULAR_EXPRESSION "${test_regex}")
endfunction()

if (TAU_BUILD_REPL_TESTS)
	enable_testing()
	add_subdirectory(commands)
	add_subdirectory(qelim)
	add_subdirectory(normalizer)
	add_subdirectory(satisfiability)
	add_subdirectory(execution)
	add_subdirectory(normal_forms)
	add_subdirectory(substitution)
endif ()
