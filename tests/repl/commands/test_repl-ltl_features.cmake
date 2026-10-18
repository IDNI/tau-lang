#
# LTL features of the 2026-10-16 review: the past duals of sometimes and
# always (once, historically) and forward references (`[t+k]`). Every case
# is over bv[1].
#

# `once φ` is `T S φ`: φ held at some step up to now; `historically φ` is
# `!once !φ`. A grant can follow a request only if one came.
add_repl_test(ltl_features-once_needs_an_earlier_request_unsat
	"sat (G(o1[t]:bv[1] = 0)) && (F(o2[t]:bv[1] = 1)) && (G(o2[t]:bv[1] = 1 -> once o1[t]:bv[1] = 1))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_features-once_after_a_request_sat
	"sat (F(o1[t]:bv[1] = 1)) && (F(o2[t]:bv[1] = 1)) && (G(o2[t]:bv[1] = 1 -> once o1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_features-O_is_once
	"sat (G(o1[t]:bv[1] = 0)) && (F(o2[t]:bv[1] = 1)) && (G(o2[t]:bv[1] = 1 -> O o1[t]:bv[1] = 1))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_features-historically_unsat
	"sat (G(o1[t]:bv[1] = 0)) && (F(historically o1[t]:bv[1] != 0))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_features-H_is_historically
	"sat (G(o1[t]:bv[1] = 1)) && (F(H o1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_features-once_takes_a_parenthesized_connective_warns
	"sat O(o2[t]:bv[1] = 0) && G(o1[t]:bv[1] = 1)"
	"`O\\(A\\) && B` is `O\\(A && B\\)`" REQUIRES ltlsynt)

# A forward reference reads a later step: `G(o1[t+1] = i1[t])` copies each
# input into the next output and leaves o1[0] free.
add_multiline_repl_test(ltl_features-forward_reference_run
	"o1\\[1\\] := 1\n.*o1\\[2\\] := 0"
	STDIN "run 3 steps G(o1[t+1]:bv[1] = i1[t]:bv[1]).\\n1\\n0\\n1\\nq\\nq\\n"
	FAIL_REGEX "unsat|Error")
add_repl_test(ltl_features-forward_reference_leaves_step_zero_free_sat
	"sat (G(o1[t+1]:bv[1] = 1)) && (F(o1[t]:bv[1] = 0 && [t = 0]))"
	": T")
add_repl_test(ltl_features-forward_reference_constrains_step_one_unsat
	"sat (G(o1[t+1]:bv[1] = 1)) && (F(o1[t]:bv[1] = 0 && [t = 1]))"
	": F")
# the always clauses share one start, so a forward reference in one does not
# delay another
add_repl_test(ltl_features-forward_reference_keeps_other_always_at_step_zero_unsat
	"sat (G(o1[t+1]:bv[1] = 1)) && (G(o2[t]:bv[1] = 1)) && (F(o2[t]:bv[1] = 0 && [t = 0]))"
	": F")
# a time constraint moves with its clause
add_repl_test(ltl_features-forward_reference_time_constraint_unsat
	"sat (G([t >= 2] -> o1[t+1]:bv[1] = 1)) && (F(o1[t]:bv[1] = 0 && [t = 3]))"
	": F")
add_repl_test(ltl_features-forward_reference_time_constraint_sat
	"sat (G([t >= 2] -> o1[t+1]:bv[1] = 1)) && (F(o1[t]:bv[1] = 0 && [t = 2]))"
	": T")
add_repl_test(ltl_features-forward_reference_under_until_realizable
	"realizable (o1[t]:bv[1] = 0) U (o1[t+1]:bv[1] = 1)"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_features-forward_reference_with_since_refused
	"sat G((o1[t+1]:bv[1] = 1) S (o2[t]:bv[1] = 1))"
	"forward reference \\[t\\+k\\] in a clause with since or trigger" NO_FAIL_REGEX)

# The unrealizable core keeps only what conflicts, and explain adds the
# environment's counter-strategy for it, with the atom each prop stands for.
add_repl_test(ltl_features-unrealizable_core
	"unrealizable_core (G(o2[t]:bv[1] = 1)) && (G(o1[t]:bv[1] = 1 -> i1[t]:bv[1] = 0)) && (F(o1[t]:bv[1] = 1))"
	"o1\\[t\\]:bv\\[1\\] = 1 -> i1\\[t\\]:bv\\[1\\] = 0.*\n.*F o1" FAIL_REGEX "o2\\[t\\]" REQUIRES ltlsynt)
add_repl_test(ltl_features-unrealizable_core_of_realizable
	"unrealizable_core (G(o1[t]:bv[1] = i1[t]:bv[1]))"
	"realizable" REQUIRES ltlsynt)
add_repl_test(ltl_features-explain
	"explain (G(o2[t]:bv[1] = 1)) && (G(o1[t]:bv[1] = 1 -> i1[t]:bv[1] = 0)) && (F(o1[t]:bv[1] = 1))"
	"unrealizable core:.*environment counter-strategy:\n.*p[0-9] := " REQUIRES ltlsynt)

# The run that checks a realizable verdict keeps a right one
add_repl_test(ltl_features-self_check_keeps_realizable
	"realizable (always i1[t]:bv[1] = 1) -> (always (o1[t]:bv[1] = 1 && i1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_features-self_check_off
	"set ltl-self-check-steps 0. realizable (o1[t]:bv[1] = 0) U (o1[t]:bv[1] = 1)"
	": T" REQUIRES ltlsynt)

# Assume-guarantee: the README's arbiter and response specifications are
# realizable with their assumptions and unrealizable without them
add_repl_test(ltl_features-assume_guarantee_arbiter_realizable
	"realizable ((G !(i1[t]:bv[1] = 1 && i2[t]:bv[1] = 1))) -> ((G (i1[t]:bv[1] = 1 -> o1[t]:bv[1] = 1)) && (G (i2[t]:bv[1] = 1 -> o1[t]:bv[1] = 0)))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_features-assume_guarantee_arbiter_without_assumption_unrealizable
	"realizable (G (i1[t]:bv[1] = 1 -> o1[t]:bv[1] = 1)) && (G (i2[t]:bv[1] = 1 -> o1[t]:bv[1] = 0))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_features-assume_guarantee_response_realizable
	"realizable ((G (F i1[t]:bv[1] = 1))) -> ((G (i1[t]:bv[1] = 1 -> F o1[t]:bv[1] = 1)))"
	": T" REQUIRES ltlsynt)

# The arbiter of five clients, each request served under the assumption that
# each client lets go infinitely often. One proposition stands for both
# values of each bv[1] stream, so ltlsynt sees five inputs and five outputs
# and the verdict takes seconds, well within the timeout.
add_repl_test(ltl_features-assume_guarantee_arbiter_of_five_clients
	"realizable ((G(F(i1[t]:bv[1] = 0))) && (G(F(i2[t]:bv[1] = 0))) && (G(F(i3[t]:bv[1] = 0))) && (G(F(i4[t]:bv[1] = 0))) && (G(F(i5[t]:bv[1] = 0)))) -> ((G(!(o1[t]:bv[1] = 1 && o2[t]:bv[1] = 1) && !(o1[t]:bv[1] = 1 && o3[t]:bv[1] = 1) && !(o1[t]:bv[1] = 1 && o4[t]:bv[1] = 1) && !(o1[t]:bv[1] = 1 && o5[t]:bv[1] = 1) && !(o2[t]:bv[1] = 1 && o3[t]:bv[1] = 1) && !(o2[t]:bv[1] = 1 && o4[t]:bv[1] = 1) && !(o2[t]:bv[1] = 1 && o5[t]:bv[1] = 1) && !(o3[t]:bv[1] = 1 && o4[t]:bv[1] = 1) && !(o3[t]:bv[1] = 1 && o5[t]:bv[1] = 1) && !(o4[t]:bv[1] = 1 && o5[t]:bv[1] = 1))) && (G(i1[t]:bv[1] = 1 -> F(o1[t]:bv[1] = 1))) && (G(i2[t]:bv[1] = 1 -> F(o2[t]:bv[1] = 1))) && (G(i3[t]:bv[1] = 1 -> F(o3[t]:bv[1] = 1))) && (G(i4[t]:bv[1] = 1 -> F(o4[t]:bv[1] = 1))) && (G(i5[t]:bv[1] = 1 -> F(o5[t]:bv[1] = 1))))"
	": T" REQUIRES ltlsynt TIMEOUT 30)
