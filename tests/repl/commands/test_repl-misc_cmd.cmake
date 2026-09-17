#
# Small REPL commands with no other coverage: `memory` outside a run, and
# `func` declarations (parsed by the grammar, refused by the evaluator because
# the OCFuncs pipeline behind them is a stub).
#
add_repl_test(memory_cmd-no_run "memory" "no run in progress")
add_repl_test_fail(func_cmd-not_implemented "func dynamic f : sbf -> sbf" "not implemented")
