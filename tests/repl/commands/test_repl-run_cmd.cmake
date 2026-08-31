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

# --- GitHub #76: bitvector-free mixed :tau stream spec ----------------------
# The reporter's 7-line reproducer (two :tau streams, a cross-stream
# assignment under a meet-guarded case split) produced no output within 400 s
# on fd137e86; it was traced to resolve_ex_block's legacy anti_prenex fallback,
# since deleted. Step 0 now completes in well under a second, so the guard is
# simply that step 1 is reached before the timeout. `set charvar off` is what
# lets multi-letter stream names like o0seal parse as one variable.
add_test(NAME "test_repl-run_cmd-issue76_mixed_tau_streams"
	COMMAND bash -c "printf 'set charvar off\\ni1 : tau := in console\\ni2 : tau := in console\\nrun ( (o0seal[0]:tau = { o1[t]=1 -> o2[t]=1 }) && (o0law[0]:tau = { o1[t]=1 -> o2[t]=1 }) && ( (i2[t]:tau != 0) ? ((o0seal[t]:tau = o0law[t-1]:tau) && (o0law[t]:tau = o0law[t-1]:tau)) : ( ((o0law[t-1]:tau & i1[t]:tau) != 0) ? ((o0seal[t]:tau = o0seal[t-1]:tau) && (o0law[t]:tau = o0law[t-1]:tau & i1[t]:tau)) : ((o0seal[t]:tau = o0seal[t-1]:tau) && (o0law[t]:tau = o0law[t-1]:tau)) ) ) )\\no3[t]=1 -> o4[t]=1\\n0\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-issue76_mixed_tau_streams" PROPERTIES
	PASS_REGULAR_EXPRESSION "Execution step: 1"
	TIMEOUT 120)

# --- GitHub #82: state accumulating N clauses in a :tau constant -------------
# The reporter's reproducer with N=20 clauses in the constant K (their
# numbering, o1..o40 -- lexicographic variable order separates each clause's
# pair, which is what exposed the blowup). One step cost 7.9 s at N=20 on
# fd137e86 and 282 s on c70e41ba: the closed implication check and the tau
# constant's complement both fed a conjunction of variable-disjoint clauses to
# one Boole decomposition (2^N). Now decided per component: ~2.3 s in Release,
# ~42 s in a -O0 Debug build, hence the generous timeout.
add_test(NAME "test_repl-run_cmd-issue82_accumulated_tau_constant"
	COMMAND bash -c "printf 'set charvar off\\ni1 : tau := in console\\ni2 : tau := in console\\nrun ( (o0a[0]:tau = { (o1[t]=1 -> o2[t]=1) && (o3[t]=1 -> o4[t]=1) && (o5[t]=1 -> o6[t]=1) && (o7[t]=1 -> o8[t]=1) && (o9[t]=1 -> o10[t]=1) && (o11[t]=1 -> o12[t]=1) && (o13[t]=1 -> o14[t]=1) && (o15[t]=1 -> o16[t]=1) && (o17[t]=1 -> o18[t]=1) && (o19[t]=1 -> o20[t]=1) && (o21[t]=1 -> o22[t]=1) && (o23[t]=1 -> o24[t]=1) && (o25[t]=1 -> o26[t]=1) && (o27[t]=1 -> o28[t]=1) && (o29[t]=1 -> o30[t]=1) && (o31[t]=1 -> o32[t]=1) && (o33[t]=1 -> o34[t]=1) && (o35[t]=1 -> o36[t]=1) && (o37[t]=1 -> o38[t]=1) && (o39[t]=1 -> o40[t]=1) }) && (o0b[0]:tau = { (o1[t]=1 -> o2[t]=1) && (o3[t]=1 -> o4[t]=1) && (o5[t]=1 -> o6[t]=1) && (o7[t]=1 -> o8[t]=1) && (o9[t]=1 -> o10[t]=1) && (o11[t]=1 -> o12[t]=1) && (o13[t]=1 -> o14[t]=1) && (o15[t]=1 -> o16[t]=1) && (o17[t]=1 -> o18[t]=1) && (o19[t]=1 -> o20[t]=1) && (o21[t]=1 -> o22[t]=1) && (o23[t]=1 -> o24[t]=1) && (o25[t]=1 -> o26[t]=1) && (o27[t]=1 -> o28[t]=1) && (o29[t]=1 -> o30[t]=1) && (o31[t]=1 -> o32[t]=1) && (o33[t]=1 -> o34[t]=1) && (o35[t]=1 -> o36[t]=1) && (o37[t]=1 -> o38[t]=1) && (o39[t]=1 -> o40[t]=1) }) && ( (i2[t]:tau != 0) ? ((o0a[t]:tau = o0b[t-1]:tau) && (o0b[t]:tau = o0b[t-1]:tau)) : ((o0a[t]:tau = o0a[t-1]:tau) && (o0b[t]:tau = o0b[t-1]:tau & i1[t]:tau)) ) )\\no3[t]=1 -> o4[t]=1\\n0\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-issue82_accumulated_tau_constant" PROPERTIES
	PASS_REGULAR_EXPRESSION "Execution step: 1"
	TIMEOUT 300)

# --- sbf multiline value continuation ---------------------------------------
# An incomplete sbf value ("x |") keeps the prompt open (the sbf-parser
# unexpected-end check in awaiting_more_input); the next line completes it.
add_test(NAME "test_repl-run_cmd-sbf_multiline_value"
	COMMAND bash -c "printf 'run always o1[t]:sbf = i1[t]\\nx |\\ny\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-run_cmd-sbf_multiline_value" PROPERTIES
	PASS_REGULAR_EXPRESSION "o1\\[0\\] := x \\| x' y")
