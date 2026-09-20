#
# Realizability over bv[1], the two-element algebra: tau's semantics coincide
# with the propositional reading, so these verdicts have a single right
# answer. Both were wrong before (found by the CROSS-bv1 fuzz suite):
#   * a bare formula over an input took the whole-query BA sat fast path,
#     which chooses the input existentially (false REALIZABLE);
#   * the oracle emitted no environment assumption for the negative
#     polarities of pure-input atom pairs, so ltlsynt let the environment
#     make both `i = 0` and `i = 1` false (false UNREALIZABLE).
# Needs a live ltlsynt on PATH, like the other realizability tests.
#
add_repl_test(realizable_bv1-input_conjunct_is_unrealizable
	"realizable (o1[t]:bv[1] = 1) && (i1[t]:bv[1] = 0)" ": F")
add_repl_test(realizable_bv1-output_conjunct_is_realizable
	"realizable (o1[t]:bv[1] = 1) && (o2[t]:bv[1] = 0)" ": T")
add_repl_test(realizable_bv1-input_tautology_under_F
	"realizable F ((i1[t]:bv[1] = 0) || (i1[t]:bv[1] = 1))" ": T")
add_repl_test(realizable_bv1-input_tautology_under_U
	"realizable (o1[t]:bv[1] = 1) until ((i1[t]:bv[1] = 0) || (i1[t]:bv[1] = 1))" ": T")
add_repl_test(realizable_bv1-input_contradiction_under_F
	"realizable F ((i1[t]:bv[1] = 0) && (i1[t]:bv[1] = 1))" ": F")
add_repl_test(realizable_bv1-weak_until_tautology
	"realizable ((i1[t]:bv[1] = 0)) weak_until ((i1[t]:bv[1] = 1))" ": T")
