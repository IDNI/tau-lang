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

# `realizable` and `unrealizable` refuse A/E/- outside `fragment ctl_star`,
# as `sat` does
add_repl_test(ltl_routing-realizable_ctl_star_needs_fragment
	"realizable A G(o1[t]:bv[1] = 0)"
	"require the ctl_star fragment" NO_FAIL_REGEX)
add_repl_test(ltl_routing-unrealizable_ctl_star_needs_fragment
	"unrealizable A G(o1[t]:bv[1] = 0)"
	"require the ctl_star fragment" NO_FAIL_REGEX)

# `G(A) && F(B)` is `G(A && F(B))`, and the parse says so; the parenthesized
# conjunction and `G(A) && G(B)`, which means the same either way, do not warn
add_repl_test(ltl_routing-prefix_operator_takes_conjunction_warns
	"sat G(o2[t]:bv[1] = 0) && F(o1[t]:bv[1] = 1)"
	"`G\\(A\\) && B` is `G\\(A && B\\)`" REQUIRES ltlsynt)
add_repl_test(ltl_routing-prefix_operator_takes_disjunction_warns
	"sat F(o2[t]:bv[1] = 0) || G(o1[t]:bv[1] = 1)"
	"`F\\(A\\) \\|\\| B` is `F\\(A \\|\\| B\\)`" REQUIRES ltlsynt)
add_repl_test(ltl_routing-parenthesized_conjunction_no_warning
	"sat (G(o2[t]:bv[1] = 0)) && (F(o1[t]:bv[1] = 1))"
	": T" FAIL_REGEX "takes everything" REQUIRES ltlsynt)
add_repl_test(ltl_routing-always_conjunction_no_warning
	"sat G(o2[t]:bv[1] = 0) && G(o1[t]:bv[1] = 1)"
	": T" FAIL_REGEX "takes everything")

# The counter-strategy of an unrealizable spec is the environment's, and
# names the atom each prop stands for
add_repl_test(ltl_routing-witness_names_atoms
	"set ltl-witness on. realizable (G (o1[t]:bv[1] = 1 -> i1[t]:bv[1] = 0)) && (F (o1[t]:bv[1] = 1))"
	"ENV COUNTER-STRATEGY.*\n.*p[0-9] := [^\n]*i1\\[t\\]" REQUIRES ltlsynt)

# The `ltl` trace and the witness legend print each atom as written, not in
# its normal form `o1[t]:bv[1]' = 0`
add_repl_test(ltl_routing-ltl_atoms_as_written
	"ltl G(F(o1[t]:bv[1] = 1 && i1[t]:bv[1] = 0))"
	"p[0-9]  :=  o1\\[t\\]:bv\\[1\\] = 1\n"
	FAIL_REGEX "Error|:=  [^\n]*'" NO_TRACE REQUIRES ltlsynt)
add_repl_test(ltl_routing-witness_atoms_as_written
	"set ltl-witness on. realizable (G (o1[t]:bv[1] = 1 -> i1[t]:bv[1] = 0)) && (F (o1[t]:bv[1] = 1))"
	"p[0-9] := o1\\[t\\]:bv\\[1\\] = 1\n"
	FAIL_REGEX "Error|p[0-9] := [^\n]*'" NO_TRACE REQUIRES ltlsynt)
