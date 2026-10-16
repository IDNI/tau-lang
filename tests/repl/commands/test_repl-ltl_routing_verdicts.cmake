#
# Which procedure decides a formula, and what the commands say about it: a
# Boolean combination of temporal clauses over inputs, the trace of `ltl`,
# the CTL* fragment gate, the warning on a prefix operator that takes a
# connective, and the counter-strategy of `ltl-witness`. Every case is over
# bv[1], where each verdict has one right answer.
#

# A strategy may wait for the inputs before it picks the disjunct to fulfil:
# the assumption of `(G A) -> (G B)` over an input is one disjunct, and
# neither is realizable alone.
add_repl_test(ltl_routing-implication_over_input_realizable
	"realizable (always i1[t]:bv[1] = 1) -> (always (o1[t]:bv[1] = 1 && i1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_routing-implication_over_input_sat
	"sat (always i1[t]:bv[1] = 1) -> (always (o1[t]:bv[1] = 1 && i1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_routing-two_input_assumption_realizable
	"realizable (always !(i1[t]:bv[1] = 1 && i2[t]:bv[1] = 1)) -> ((always (i1[t]:bv[1] = 1 -> o1[t]:bv[1] = 1)) && (always (i2[t]:bv[1] = 1 -> o1[t]:bv[1] = 0)))"
	": T" REQUIRES ltlsynt)
# `F(o1 = 1)` is equivalent, so `sat` agrees with `realizable`
add_repl_test(ltl_routing-sometimes_disjunction_sat
	"sat (F (i1[t]:bv[1] = 1 && o1[t]:bv[1] = 1)) || (F (i1[t]:bv[1] != 1 && o1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_routing-sometimes_or_always_sat
	"sat (sometimes i1[t]:bv[1] = 0) || (always (o1[t]:bv[1] = 1 && i1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
# valid implies sat
add_repl_test(ltl_routing-valid_disjunction_sat
	"sat (F (i1[t]:bv[1] = 1 && o1[t]:bv[1] = 1)) || (F (i1[t]:bv[1] = 1 && o1[t]:bv[1] != 1)) || (G (i1[t]:bv[1] != 1))"
	": T" REQUIRES ltlsynt)
# each disjunct needs the input to stay put, and the environment moves it
add_repl_test(ltl_routing-always_disjunction_over_input_unrealizable
	"realizable (G (o1[t]:bv[1] = 0 && i1[t]:bv[1] = 0)) || (G (o1[t]:bv[1] = 1 && i1[t]:bv[1] = 1))"
	": F" REQUIRES ltlsynt)
# the run of the implication waits for the inputs: o1 follows the
# assumption while it holds
add_multiline_repl_test(ltl_routing-run_implication_over_input
	"o1\\[0\\] := 1"
	STDIN "run (always i1[t]:bv[1] = 1) -> (always (o1[t]:bv[1] = 1 && i1[t]:bv[1] = 1)).\\n1\\n0\\n1\\n1\\nq\\nq\\n"
	FAIL_REGEX "unsat|Error" REQUIRES ltlsynt)

# The trace of `ltl` ends with the verdict `realizable` gives
add_repl_test(ltl_routing-ltl_trace_implication_realizable
	"ltl (always i1[t]:bv[1] = 1) -> (always (o1[t]:bv[1] = 1 && i1[t]:bv[1] = 1))"
	"Strategy: .*\nREALIZABLE" REQUIRES ltlsynt)
