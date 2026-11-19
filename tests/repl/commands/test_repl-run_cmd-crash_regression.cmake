include(add_repl_test)

#
# D14 regression: `run` on a positional atom under a temporal operator used to
# propagate an uncaught std::runtime_error out of
# interpreter::make_interpreter, terminating the whole REPL process with
# SIGABRT. The atom now gets a latch (apply_step_counter_encoding), and the
# run sets o1 at step 5.
#

add_repl_test(run_cmd-no_crash_on_positional_under_temporal
	"run 6 steps F (o1[5]:bv[8] = {1}:bv[8])." "o1\\[5\\] := 1" REQUIRES ltlsynt)
