#
# Where the LTL pipeline places a formula in time: fixed-position atoms under
# the step counter, time constraints, top-level clauses and past operators.
# Each case has one right verdict over bv[1]; the conjuncts carry outer
# parentheses because a prefix temporal operator takes the whole conjunction
# to its right.
#

# A fixed-position atom read by two hoisted conjuncts of different depths
# stands for a different step's data in each: `i1[1]` is `i1[t]` at step 1
# and `i1[t-2]` at step 3. The extra conjunct is satisfiable with
# `o2[1] = 0`, so it changes no verdict, in either position.
add_repl_test(ltl_temporal-shared_positional_atom_first_unrealizable
	"realizable (G(i1[1]:bv[1] = 0 || o2[1]:bv[1] = 0)) && (G(i1[1]:bv[1] = 0 || i1[3]:bv[1] != 0)) && (F(o3[t]:bv[1] = 1))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-shared_positional_atom_second_unrealizable
	"realizable (G(i1[1]:bv[1] = 0 || i1[3]:bv[1] != 0)) && (G(i1[1]:bv[1] = 0 || o2[1]:bv[1] = 0)) && (F(o3[t]:bv[1] = 1))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_temporal-shared_positional_atom_first_realizable
	"realizable (G(i1[1]:bv[1] = 0 || o2[1]:bv[1] = 0)) && (G(i1[1]:bv[1] = 0 || o3[2]:bv[1] = 0)) && (G(o3[2]:bv[1] != 0 || i1[1]:bv[1] != 0)) && (F(o4[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
