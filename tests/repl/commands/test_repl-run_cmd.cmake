#
# run command (repl_evaluator::run_cmd / continue_running / eval's pending resume)
#
# Coverage-driven additions (2026-08-01). repl_evaluator.tmpl.h measured 64.5%
# line coverage -- the largest remaining gap in src/ -- and its single biggest
# cold region is the `run` command: run_cmd() plus continue_running() plus the
# pending-request resume path in eval(), roughly 120 uncovered lines. There was
# no test for `run` at all.
#
# `run` is interactive: continue_running() steps the interpreter and, when a
# console input stream needs a value, SUSPENDS by setting `pending` and
# reprompting rather than blocking. The next eval() call treats its input as the
# answer to that request instead of as a new command. So driving it needs a
# sequence of lines on stdin, which is what these tests do via -X (the legacy
# terminal REPL, the branch that works without a tty).
#
# Each input sequence below was confirmed by hand against the built binary
# before being turned into an assertion.
#

# --- the main step loop ------------------------------------------------------
# `always` spec: every step needs an input, so this exercises the stream_value
# pending path repeatedly and prints an output per step.
add_test(NAME "test_repl-run_cmd-steps"
	COMMAND bash -c "printf 'run o[t] = i[t].\\nT.\\nF.\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-steps" PROPERTIES
	PASS_REGULAR_EXPRESSION "o\\[0\\] := T")

# The per-step prompt carries the stream name, time point and BA type, built in
# continue_running() from the awaiting repl_pending_input_stream.
add_test(NAME "test_repl-run_cmd-prompt_label"
	COMMAND bash -c "printf 'run o[t] = i[t].\\nT.\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-prompt_label" PROPERTIES
	PASS_REGULAR_EXPRESSION "i\\[0\\] : tau :=")

# Successive steps advance the time point in both the prompt and the output.
add_test(NAME "test_repl-run_cmd-advances_time_point"
	COMMAND bash -c "printf 'run o[t] = i[t].\\nT.\\nF.\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-advances_time_point" PROPERTIES
	PASS_REGULAR_EXPRESSION "o\\[1\\] := F")

# --- more than one console input stream in a single step ---------------------
# read() aborts the whole step at the FIRST stream with no value yet, so a step
# needing N console inputs is entered N times, once per value the REPL collects.
# Each of those attempts re-reads the streams that already answered, so the
# pending stream must hand the same value out again for the same time point;
# consuming it once made the run re-prompt i1 after i2 was given, burn 2^N-1
# input lines per step and then use the wrong values.
add_test(NAME "test_repl-run_cmd-two_input_streams"
	COMMAND bash -c "printf 'run (o1[t] = i1[t]) && (o2[t] = i2[t]).\\nT.\\nF.\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-two_input_streams" PROPERTIES
	PASS_REGULAR_EXPRESSION "o1\\[0\\] := T")

add_test(NAME "test_repl-run_cmd-two_input_streams-second_value"
	COMMAND bash -c "printf 'run (o1[t] = i1[t]) && (o2[t] = i2[t]).\\nT.\\nF.\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-two_input_streams-second_value" PROPERTIES
	PASS_REGULAR_EXPRESSION "o2\\[0\\] := F")

# Two streams, two steps: the re-delivered value must be dropped once the step
# it belongs to completes, otherwise step 1 silently reuses step 0's inputs.
add_test(NAME "test_repl-run_cmd-two_input_streams-next_step"
	COMMAND bash -c "printf 'run (o1[t] = i1[t]) && (o2[t] = i2[t]).\\nT.\\nF.\\nF.\\nT.\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-two_input_streams-next_step" PROPERTIES
	PASS_REGULAR_EXPRESSION "o1\\[1\\] := F")

# --- rejected value re-asks the same step -----------------------------------
# An unparseable value leaves the step unsatisfied. continue_running() is
# re-entered with the previous request as `retry`, so the SAME time point is
# prompted again rather than the run ending. No FAIL_REGULAR_EXPRESSION here:
# the parse error is the expected output.
add_test(NAME "test_repl-run_cmd-retry_on_bad_value"
	COMMAND bash -c "printf 'run o[t] = i[t].\\nzzz.\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-retry_on_bad_value" PROPERTIES
	PASS_REGULAR_EXPRESSION "Failed to parse input value")

# --- the continue-or-quit gate and finish_running ----------------------------
# A spec constraining only time point 0 stops needing input, so the step loop
# reaches the "no awaiting stream" case and asks whether to continue. Answering
# q runs finish_running(), which prints the run's benchmark totals.
add_test(NAME "test_repl-run_cmd-continue_or_quit_prompt"
	COMMAND bash -c "printf 'run o[0] = i[0].\\nT.\\n\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-continue_or_quit_prompt" PROPERTIES
	PASS_REGULAR_EXPRESSION "continue\\?")

add_test(NAME "test_repl-run_cmd-quit_finishes_run"
	COMMAND bash -c "printf 'run o[0] = i[0].\\nT.\\n\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-quit_finishes_run" PROPERTIES
	PASS_REGULAR_EXPRESSION "run: ")

# --- a specification that cannot be run -------------------------------------
# run_cmd returns early when the argument does not yield a formula or an
# interpreter; the REPL must stay usable rather than crash or hang.
add_test(NAME "test_repl-run_cmd-invalid_spec"
	COMMAND bash -c "printf 'run x ) ( invalid\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-invalid_spec" PROPERTIES
	PASS_REGULAR_EXPRESSION "Quit")

# --- issue 20: a definition whose head cannot bind its body's relative offset -
# `f` declares no offset, so the `n` in `r[n](x)` is free. Unfolding `f(1)` then
# rewrites r[n] -> r[n-1] -> r[n-1-1] -> ... forever, which is what hung the
# runner with no output and no message. Written out directly the same spec is
# already rejected ("Main ... cannot contain a relative offset"); hiding it
# behind a definition must be rejected too, and at definition time, so the bad
# definition never reaches the runner or the definition list.
add_test(NAME "test_repl-run_cmd-unbindable_relative_offset"
	COMMAND bash -c "printf 'r[0](x) := 1.\\nr[n](x) := r[n-1](x).\\nf(x) := o1[n] = r[n](x).\\nrun f(1)\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-unbindable_relative_offset" PROPERTIES
	PASS_REGULAR_EXPRESSION "its head declares no offset to bind it")

# The same definition with an offset on the head binds `n` and stays accepted.
add_test(NAME "test_repl-run_cmd-bound_relative_offset_accepted"
	COMMAND bash -c "printf 'r[0](x) := 1.\\nr[n](x) := r[n-1](x).\\nf[n](x) := o1[n] = r[n](x).\\ndefs\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-bound_relative_offset_accepted" PROPERTIES
	PASS_REGULAR_EXPRESSION "\\[3\\] f\\[n\\]\\(x\\)"
	FAIL_REGULAR_EXPRESSION "Error")
