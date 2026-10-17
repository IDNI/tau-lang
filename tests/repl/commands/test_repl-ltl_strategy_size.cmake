#
# The size of a strategy: ltl-minimize-strategy and ltl-strategy-max-states.
#

include(add_repl_test)

set(_o "o1[t]:bv[1]")

# A strategy of three states alternates o1 and o2 independently: the
# minimisation keeps the verdict, the bound at its size keeps it, and a
# bound below it answers UNKNOWN after the SAT minimisation of ltlsynt.
set(_alternation "G(${_o} = 1 -> F(${_o} = 0)) && G(${_o} = 0 -> F(${_o} = 1)) && G(o2[t]:bv[1] = 1 -> F(o2[t]:bv[1] = 0)) && G(o2[t]:bv[1] = 0 -> F(o2[t]:bv[1] = 1))")
add_repl_test(ltl_size-minimize_keeps_verdict
	"realizable ${_alternation}" ": T"
	ENV TAU_LTL_MINIMIZE_STRATEGY=true TAU_LTL_SIMPLIFICATION=no
	REQUIRES ltlsynt bv)
add_repl_test(ltl_size-bound_at_size_keeps_verdict
	"realizable ${_alternation}" ": T"
	ENV TAU_LTL_STRATEGY_MAX_STATES=3 REQUIRES ltlsynt bv)
add_repl_test(ltl_size-bound_below_size_is_unknown
	"realizable ${_alternation}"
	"the strategy has 3 states, above ltl-strategy-max-states \\(2\\)"
	NO_FAIL_REGEX ENV TAU_LTL_STRATEGY_MAX_STATES=2 REQUIRES ltlsynt bv)
add_repl_test(ltl_size-bound_set_get
	"set ltl-strategy-max-states 5. get ltl-strategy-max-states"
	"ltl-strategy-max-states: *5" NO_FAIL_REGEX NO_TRACE)
add_repl_test(ltl_size-minimize_set_get
	"set ltl-minimize-strategy true. get ltl-minimize-strategy"
	"ltl-minimize-strategy: *true" NO_FAIL_REGEX NO_TRACE)

# a minimised strategy still runs: o1 takes both values within 4 steps
add_repl_test(ltl_size-minimized_strategy_runs
	"set ltl-minimize-strategy true. run 4 steps (G(F(${_o} = 0))) && (G(F(${_o} = 1)))."
	"o1\\[[0-3]\\] := 0.*o1\\[[0-3]\\] := 1|o1\\[[0-3]\\] := 1.*o1\\[[0-3]\\] := 0"
	FAIL_REGEX "unsat|not executable"
	ENV TAU_LTL_SIMPLIFICATION=no REQUIRES ltlsynt bv)
