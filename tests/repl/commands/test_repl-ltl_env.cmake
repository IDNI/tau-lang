#
# LT-27 / SY-RT4: the diagnostic environment paths of the ltlsynt call.
# Each one is observed through the REPL `ltl` command with the variable
# set for that single invocation.  NOTE: never use `(.*\n)*` to bridge two
# markers here -- the HOA dump is long enough that ctest's regex engine
# backtracks for minutes; match one marker only.
# The spec must go through ltlsynt (a plain
# `always` takes the safety path and never spawns it).  All need a live
# ltlsynt on PATH (as the
# other `ltl` REPL tests do).
#

# TAU_LTL_EXPORT_STRATEGY=hoa prints the strategy to stderr.
add_test(NAME "test_repl-ltl_env-export_hoa"
	COMMAND bash -c "TAU_LTL_EXPORT_STRATEGY=hoa $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"ltl F (o1[t] = 1)\" 2>&1")
set_tests_properties("test_repl-ltl_env-export_hoa" PROPERTIES
	PASS_REGULAR_EXPRESSION "=== STRATEGY HOA ===")

# TAU_LTL_EXPORT_STRATEGY=dot prints dot (or falls back to HOA when autfilt
# is missing) -- either way a STRATEGY banner appears.
add_test(NAME "test_repl-ltl_env-export_dot"
	COMMAND bash -c "TAU_LTL_EXPORT_STRATEGY=dot $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"ltl F (o1[t] = 1)\" 2>&1")
set_tests_properties("test_repl-ltl_env-export_dot" PROPERTIES
	PASS_REGULAR_EXPRESSION "=== STRATEGY (DOT|HOA \\(dot unavailable\\)) ===")

# Without the variable nothing is exported.
add_test(NAME "test_repl-ltl_env-export_off_by_default"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"ltl F (o1[t] = 1)\" 2>&1")
set_tests_properties("test_repl-ltl_env-export_off_by_default" PROPERTIES
	FAIL_REGULAR_EXPRESSION "=== STRATEGY"
	PASS_REGULAR_EXPRESSION "REALIZABLE")

# TAU_LTL_WITNESS=1 on an UNREALIZABLE spec prints the environment's
# counter-strategy (the negated, role-swapped game is realizable).
add_test(NAME "test_repl-ltl_env-witness"
	COMMAND bash -c "TAU_LTL_WITNESS=1 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"ltl F (i1[t] = 1)\" 2>&1")
set_tests_properties("test_repl-ltl_env-witness" PROPERTIES
	PASS_REGULAR_EXPRESSION "=== ENV COUNTER-STRATEGY \\(UNREAL witness\\) ===")

# A bad TAU_LTL_SIMPLIFICATION value is an ltlsynt usage error (exit 2):
# no verdict, reported as UNKNOWN -- never REALIZABLE or UNREALIZABLE.
add_test(NAME "test_repl-ltl_env-bad_simplification_is_unknown"
	COMMAND bash -c "TAU_LTL_SIMPLIFICATION=definitely-not-a-level $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"ltl F (o1[t] = 1)\" 2>&1")
set_tests_properties("test_repl-ltl_env-bad_simplification_is_unknown" PROPERTIES
	FAIL_REGULAR_EXPRESSION "[^N]REALIZABLE|UNREALIZABLE"
	PASS_REGULAR_EXPRESSION "UNKNOWN")

# TAU_LTL_TIMEOUT_SEC garbage keeps the default and says so.
add_test(NAME "test_repl-ltl_env-timeout_garbage_warns"
	COMMAND bash -c "TAU_LTL_TIMEOUT_SEC=abc $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"ltl F (o1[t] = 1)\" 2>&1")
set_tests_properties("test_repl-ltl_env-timeout_garbage_warns" PROPERTIES
	PASS_REGULAR_EXPRESSION "TAU_LTL_TIMEOUT_SEC='abc' is not a non-negative number")
