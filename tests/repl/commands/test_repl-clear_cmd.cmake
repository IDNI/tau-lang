#
# clear command
#
# Coverage-driven addition (2026-08-01). The include existed but was commented
# out. `clear` emits terminal control sequences rather than text, so the
# assertion is simply that the REPL survives it and goes on to process the next
# command -- which is what the eval_cmd clear arm has to guarantee.
#

add_test(NAME "test_repl-clear_cmd"
	COMMAND bash -c "printf 'clear\\nversion\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-clear_cmd" PROPERTIES
	PASS_REGULAR_EXPRESSION "Tau Language Framework"
	FAIL_REGULAR_EXPRESSION "Error")
