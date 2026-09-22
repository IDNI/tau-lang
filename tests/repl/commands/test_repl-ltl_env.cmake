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

# The environment variables are fallbacks of the runtime parameters: the
# variable shows through `get` when the option is unset, and the CLI flag
# wins when both are given.
add_test(NAME "test_repl-ltl_env-timeout_env_is_the_fallback"
	COMMAND bash -c "TAU_LTL_TIMEOUT_SEC=5 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get ltltimeout\"")
set_tests_properties("test_repl-ltl_env-timeout_env_is_the_fallback" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltltimeout: *5s")
add_test(NAME "test_repl-ltl_env-timeout_flag_beats_env"
	COMMAND bash -c "TAU_LTL_TIMEOUT_SEC=5 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --ltl-timeout 9 -e \"get ltltimeout\"")
set_tests_properties("test_repl-ltl_env-timeout_flag_beats_env" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltltimeout: *9s")
add_test(NAME "test_repl-ltl_env-timeout_flag_rejects_garbage"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --ltl-timeout abc -e \"get ltltimeout\" 2>&1")
set_tests_properties("test_repl-ltl_env-timeout_flag_rejects_garbage" PROPERTIES
	PASS_REGULAR_EXPRESSION "expects a non-negative number")
add_test(NAME "test_repl-ltl_env-alg_env_is_the_fallback"
	COMMAND bash -c "TAU_LTL_ALG=D $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get ltlalg\"")
set_tests_properties("test_repl-ltl_env-alg_env_is_the_fallback" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlalg: *D")
add_test(NAME "test_repl-ltl_env-alg_flag_beats_env"
	COMMAND bash -c "TAU_LTL_ALG=D $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --ltl-alg A -e \"get ltlalg\"")
set_tests_properties("test_repl-ltl_env-alg_flag_beats_env" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlalg: *A")
add_test(NAME "test_repl-ltl_env-alg_garbage_reads_as_auto"
	COMMAND bash -c "TAU_LTL_ALG=C $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get ltlalg\" 2>&1")
set_tests_properties("test_repl-ltl_env-alg_garbage_reads_as_auto" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlalg: *auto")
add_test(NAME "test_repl-ltl_env-qe_env_is_validated"
	COMMAND bash -c "TAU_LTL_OMCAT_QE_MAX_VARS=abc $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get ltlqemaxvars\" 2>&1")
set_tests_properties("test_repl-ltl_env-qe_env_is_validated" PROPERTIES
	PASS_REGULAR_EXPRESSION "keeping the default 2")
add_test(NAME "test_repl-ltl_env-qe_flag_beats_env"
	COMMAND bash -c "TAU_LTL_OMCAT_QE_MAX_VARS=4 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --ltl-qe-max-vars 3 -e \"get ltlqemaxvars\"")
set_tests_properties("test_repl-ltl_env-qe_flag_beats_env" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlqemaxvars: *3")
# BA-declared knobs promoted from header constants. The option exists only
# when its algebra is in the pack, so each is gated on its own BA.
tau_repl_unsupported(_tau_skip "get qlt-t3-cap")
if(_tau_skip)
	tau_repl_record_skip("ltl_env-qlt_t3_cap_option")
else()
	add_test(NAME "test_repl-ltl_env-qlt_t3_cap_option"
		COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --qlt-t3-cap 12 -e \"get qlt-t3-cap\"")
	set_tests_properties("test_repl-ltl_env-qlt_t3_cap_option" PROPERTIES
		PASS_REGULAR_EXPRESSION "qlt-t3-cap: *12")
endif()
tau_repl_unsupported(_tau_skip "set nlang-http-timeout")
if(_tau_skip)
	tau_repl_record_skip("ltl_env-nlang_http_timeout_option")
else()
	add_test(NAME "test_repl-ltl_env-nlang_http_timeout_option"
		COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"set nlang-http-timeout 3\"")
	set_tests_properties("test_repl-ltl_env-nlang_http_timeout_option" PROPERTIES
		PASS_REGULAR_EXPRESSION "nlang-http-timeout: *3")
endif()
add_test(NAME "test_repl-ltl_env-refinement_rounds_flag"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --ltl-refinement-rounds 5 -e \"get ltlrefinementrounds\"")
set_tests_properties("test_repl-ltl_env-refinement_rounds_flag" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlrefinementrounds: *5")
add_test(NAME "test_repl-ltl_env-window_max_paths_flag"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --ltl-window-max-paths 0 -e \"get ltlwindowmaxpaths\"")
set_tests_properties("test_repl-ltl_env-window_max_paths_flag" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlwindowmaxpaths: *unlimited")
add_test(NAME "test_repl-ltl_env-pwr_semantic_flag"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --pwr-semantic -e \"get pwrsemantic\"")
set_tests_properties("test_repl-ltl_env-pwr_semantic_flag" PROPERTIES
	PASS_REGULAR_EXPRESSION "pwrsemantic: *on")
tau_repl_unsupported(_tau_skip "get qlt-const-output-max")
if(_tau_skip)
	tau_repl_record_skip("ltl_env-qlt_const_output_max_flag")
else()
	add_test(NAME "test_repl-ltl_env-qlt_const_output_max_flag"
		COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --qlt-const-output-max 3 -e \"get qlt-const-output-max\"")
	set_tests_properties("test_repl-ltl_env-qlt_const_output_max_flag" PROPERTIES
		PASS_REGULAR_EXPRESSION "qlt-const-output-max: *3")
endif()
# The -K short flag belongs to --ba-component-factoring; --ltl-qe-max-vars is -k.
add_test(NAME "test_repl-ltl_env-qe_short_flag_is_lowercase_k"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -k 3 -e \"get ltlqemaxvars\"")
set_tests_properties("test_repl-ltl_env-qe_short_flag_is_lowercase_k" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlqemaxvars: *3")

# Every runtime limit of the pipeline carries all three surfaces: a CLI flag,
# a REPL option and a TAU_* environment fallback. The variable shows through
# `get` when the flag is absent, the flag wins when both are given, and a
# garbage value keeps the default and says so.
add_test(NAME "test_repl-ltl_env-hoa_max_states_env_is_the_fallback"
	COMMAND bash -c "TAU_LTL_HOA_MAX_STATES=7 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get ltlhoamaxstates\"")
set_tests_properties("test_repl-ltl_env-hoa_max_states_env_is_the_fallback" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlhoamaxstates: *7")
add_test(NAME "test_repl-ltl_env-hoa_max_states_flag_beats_env"
	COMMAND bash -c "TAU_LTL_HOA_MAX_STATES=7 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --ltl-hoa-max-states 9 -e \"get ltlhoamaxstates\"")
set_tests_properties("test_repl-ltl_env-hoa_max_states_flag_beats_env" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlhoamaxstates: *9")
add_test(NAME "test_repl-ltl_env-hoa_max_states_garbage_warns"
	COMMAND bash -c "TAU_LTL_HOA_MAX_STATES=abc $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get ltlhoamaxstates\" 2>&1")
set_tests_properties("test_repl-ltl_env-hoa_max_states_garbage_warns" PROPERTIES
	PASS_REGULAR_EXPRESSION "TAU_LTL_HOA_MAX_STATES='abc' is not a non-negative number")
add_test(NAME "test_repl-ltl_env-hoa_max_states_flag_rejects_garbage"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --ltl-hoa-max-states abc -e \"get ltlhoamaxstates\" 2>&1")
set_tests_properties("test_repl-ltl_env-hoa_max_states_flag_rejects_garbage" PROPERTIES
	PASS_REGULAR_EXPRESSION "expects a non-negative number")
add_test(NAME "test_repl-ltl_env-guard_max_cubes_env_is_the_fallback"
	COMMAND bash -c "TAU_LTL_GUARD_MAX_CUBES=6 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get ltlguardmaxcubes\"")
set_tests_properties("test_repl-ltl_env-guard_max_cubes_env_is_the_fallback" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlguardmaxcubes: *6")
add_test(NAME "test_repl-ltl_env-refinement_rounds_env_is_the_fallback"
	COMMAND bash -c "TAU_LTL_REFINEMENT_ROUNDS=5 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get ltlrefinementrounds\"")
set_tests_properties("test_repl-ltl_env-refinement_rounds_env_is_the_fallback" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlrefinementrounds: *5")
add_test(NAME "test_repl-ltl_env-refinement_rounds_flag_beats_env"
	COMMAND bash -c "TAU_LTL_REFINEMENT_ROUNDS=5 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --ltl-refinement-rounds 8 -e \"get ltlrefinementrounds\"")
set_tests_properties("test_repl-ltl_env-refinement_rounds_flag_beats_env" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlrefinementrounds: *8")
# 0 is a value, not an absence: it means unlimited, and the variable can say so.
add_test(NAME "test_repl-ltl_env-window_max_paths_env_is_the_fallback"
	COMMAND bash -c "TAU_LTL_WINDOW_MAX_PATHS=0 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get ltlwindowmaxpaths\"")
set_tests_properties("test_repl-ltl_env-window_max_paths_env_is_the_fallback" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlwindowmaxpaths: *unlimited")
add_test(NAME "test_repl-ltl_env-window_max_paths_flag_beats_env"
	COMMAND bash -c "TAU_LTL_WINDOW_MAX_PATHS=0 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --ltl-window-max-paths 12 -e \"get ltlwindowmaxpaths\"")
set_tests_properties("test_repl-ltl_env-window_max_paths_flag_beats_env" PROPERTIES
	PASS_REGULAR_EXPRESSION "ltlwindowmaxpaths: *12")
# The same three surfaces for the caps an algebra declares about itself; each
# exists only when its algebra is in the pack, so each is gated on its own BA.
tau_repl_unsupported(_tau_skip "get qlt-t3-cap")
if(_tau_skip)
	tau_repl_record_skip("ltl_env-qlt_t3_cap_env")
else()
	add_test(NAME "test_repl-ltl_env-qlt_t3_cap_env_is_the_fallback"
		COMMAND bash -c "TAU_QLT_T3_CAP=14 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get qlt-t3-cap\"")
	set_tests_properties("test_repl-ltl_env-qlt_t3_cap_env_is_the_fallback" PROPERTIES
		PASS_REGULAR_EXPRESSION "qlt-t3-cap: *14")
	add_test(NAME "test_repl-ltl_env-qlt_t3_cap_flag_beats_env"
		COMMAND bash -c "TAU_QLT_T3_CAP=14 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --qlt-t3-cap 12 -e \"get qlt-t3-cap\"")
	set_tests_properties("test_repl-ltl_env-qlt_t3_cap_flag_beats_env" PROPERTIES
		PASS_REGULAR_EXPRESSION "qlt-t3-cap: *12")
	add_test(NAME "test_repl-ltl_env-qlt_t3_cap_garbage_warns"
		COMMAND bash -c "TAU_QLT_T3_CAP=abc $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get qlt-t3-cap\" 2>&1")
	set_tests_properties("test_repl-ltl_env-qlt_t3_cap_garbage_warns" PROPERTIES
		PASS_REGULAR_EXPRESSION "TAU_QLT_T3_CAP='abc' is not a non-negative number")
endif()
tau_repl_unsupported(_tau_skip "get qlt-const-output-max")
if(_tau_skip)
	tau_repl_record_skip("ltl_env-qlt_const_output_max_env")
else()
	add_test(NAME "test_repl-ltl_env-qlt_const_output_max_env_is_the_fallback"
		COMMAND bash -c "TAU_QLT_CONST_OUTPUT_MAX=4 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get qlt-const-output-max\"")
	set_tests_properties("test_repl-ltl_env-qlt_const_output_max_env_is_the_fallback" PROPERTIES
		PASS_REGULAR_EXPRESSION "qlt-const-output-max: *4")
	add_test(NAME "test_repl-ltl_env-qlt_const_output_max_flag_beats_env"
		COMMAND bash -c "TAU_QLT_CONST_OUTPUT_MAX=4 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --qlt-const-output-max 3 -e \"get qlt-const-output-max\"")
	set_tests_properties("test_repl-ltl_env-qlt_const_output_max_flag_beats_env" PROPERTIES
		PASS_REGULAR_EXPRESSION "qlt-const-output-max: *3")
endif()
tau_repl_unsupported(_tau_skip "get nlang-http-timeout")
if(_tau_skip)
	tau_repl_record_skip("ltl_env-nlang_http_timeout_env")
else()
	add_test(NAME "test_repl-ltl_env-nlang_http_timeout_env_is_the_fallback"
		COMMAND bash -c "TAU_NLANG_HTTP_TIMEOUT=9 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get nlang-http-timeout\"")
	set_tests_properties("test_repl-ltl_env-nlang_http_timeout_env_is_the_fallback" PROPERTIES
		PASS_REGULAR_EXPRESSION "nlang-http-timeout: *9")
	add_test(NAME "test_repl-ltl_env-nlang_http_timeout_flag_beats_env"
		COMMAND bash -c "TAU_NLANG_HTTP_TIMEOUT=9 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --nlang-http-timeout 3 -e \"get nlang-http-timeout\"")
	set_tests_properties("test_repl-ltl_env-nlang_http_timeout_flag_beats_env" PROPERTIES
		PASS_REGULAR_EXPRESSION "nlang-http-timeout: *3")
endif()
# A BA-declared count option left unset keeps the algebra's own default: the
# CLI passes nothing, so an algebra's environment fallback is not shadowed.
tau_repl_unsupported(_tau_skip "get qlt-t3-cap")
if(_tau_skip)
	tau_repl_record_skip("ltl_env-qlt_t3_cap_default_without_flag")
else()
	add_test(NAME "test_repl-ltl_env-qlt_t3_cap_default_without_flag"
		COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get qlt-t3-cap\"")
	set_tests_properties("test_repl-ltl_env-qlt_t3_cap_default_without_flag" PROPERTIES
		PASS_REGULAR_EXPRESSION "qlt-t3-cap: *20")
endif()

# TAU_TREF_BUDGET is the memory budget's environment form: it shows through
# `get` when the flag is unset, and the flag wins when both are given.
add_test(NAME "test_repl-tref_budget_env_is_the_fallback"
	COMMAND bash -c "TAU_TREF_BUDGET=4096 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get trefbudget\"")
set_tests_properties("test_repl-tref_budget_env_is_the_fallback" PROPERTIES
	PASS_REGULAR_EXPRESSION "trefbudget: *4096")

add_test(NAME "test_repl-tref_budget_flag_beats_env"
	COMMAND bash -c "TAU_TREF_BUDGET=4096 $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --tref-budget 8192 -e \"get trefbudget\"")
set_tests_properties("test_repl-tref_budget_flag_beats_env" PROPERTIES
	PASS_REGULAR_EXPRESSION "trefbudget: *8192")

# A budget no session can meet refuses the command instead of answering it,
# and says which knob set the cap.
add_test(NAME "test_repl-tref_budget_refuses_when_exhausted"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --tref-budget 1 -e \"dnf (x & y) | z\" 2>&1")
set_tests_properties("test_repl-tref_budget_refuses_when_exhausted" PROPERTIES
	PASS_REGULAR_EXPRESSION "memory budget exhausted.*--tref-budget")

# Unlimited is the shipped default: the same command answers normally.
add_test(NAME "test_repl-tref_budget_unlimited_by_default"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"dnf (x & y) | z\" 2>&1")
set_tests_properties("test_repl-tref_budget_unlimited_by_default" PROPERTIES
	FAIL_REGULAR_EXPRESSION "memory budget exhausted"
	PASS_REGULAR_EXPRESSION "xy\\|z")
