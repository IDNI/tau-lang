# include(add_repl_test) to use new tree

function(add_repl_test test_name test_cmd test_regex)
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"${test_cmd}\" -S trace")
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${test_regex}"
		FAIL_REGULAR_EXPRESSION "Error"
	)
endfunction()

function(add_repl_test_fail test_name test_cmd test_regex)
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"${test_cmd}\" -S trace")
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${test_regex}"
	)
endfunction()

function(add_echo_repl_test test_name test_cmd test_regex)
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash -c "echo \"${test_cmd}. q\" | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${test_regex}"
		FAIL_REGULAR_EXPRESSION "Error"
	)
endfunction()

function(add_echo_repl_test_fail test_name test_cmd test_regex)
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash -c "echo \"${test_cmd}. q\" | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${test_regex}"
	)
endfunction()

# add_multiline_repl_test(<test_name> <test_regex> <line1> [<line2> ...])
#
# add_echo_repl_test pipes a single "<cmd>. q" line via `echo`, so it cannot
# carry an embedded newline: every command in it lands in ONE parse. Use this
# helper instead for a case that needs each argument on its OWN REPL line
# (e.g. a `type` declaration on one line whose effect a LATER, separately
# parsed line must see). Each of <line1>... is piped, one per line, via
# `printf` into `tau -X` (interactive mode, like commands/test_repl-adt.cmake's
# other raw printf-driven add_test entries, e.g. its lines 176/203/255/265/302),
# followed by a trailing `q` to exit cleanly. Argument order is
# (name, regex, lines...) -- unlike
# add_repl_test/add_echo_repl_test's (name, cmd, regex) -- since CMake's
# variadic tail (ARGN) must come last in the parameter list.
function(add_multiline_repl_test test_name test_regex)
	set(lines ${ARGN})
	string(REPLACE ";" "\\n" joined_lines "${lines}")
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash -c "printf '${joined_lines}\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${test_regex}"
		FAIL_REGULAR_EXPRESSION "Error"
	)
endfunction()
