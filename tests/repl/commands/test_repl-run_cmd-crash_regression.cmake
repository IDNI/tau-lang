#
# D14 regression: `run` on a formula the shared solving path refuses (a
# positional atom under a temporal operator) used to propagate an uncaught
# std::runtime_error out of interpreter::make_interpreter, terminating the
# whole REPL process with SIGABRT. It must now print the refusal to stderr
# and return to the prompt instead of crashing.
#

add_repl_test_fail(run_cmd-no_crash_on_positional_under_temporal
	"run F (o1[5]:bv[8] = {1}:bv[8])." "refused")
