#
# ltl-compositional: the parts of a conjunction that share no output are
# decided, and run, each for itself.
#

include(add_repl_test)

set(_comp ENV TAU_LTL_COMPOSITIONAL=on)

# two parts, both realizable: the conjunction is
add_repl_test(ltl_compositional-realizable_parts
	"realizable (G (i1[t]:bv[1] = 1 -> F o1[t]:bv[1] = 1)) && (G (F (o2[t]:bv[1] = 1 && o3[t]:bv[1] = 0)))"
	"ltl_compositional_parts:[^\n]*[^0-9]2[^0-9].*: T" ${_comp} REQUIRES ltlsynt)

# one part the environment wins decides the conjunction
add_repl_test(ltl_compositional-unrealizable_part
	"realizable (G (F (o1[t]:bv[1] = 1))) && ((o2[t]:bv[1] = 0) U (i1[t]:bv[1] = 1))"
	"ltl_compositional_parts:[^\n]*[^0-9]2[^0-9].*: F" ${_comp} REQUIRES ltlsynt)

# the always clause starts at its deepest lookback, step 2, in each of its
# parts: o1[0] = 1 does not meet o1[t] = 0
add_repl_test(ltl_compositional-part_keeps_clause_start
	"realizable (always o1[t]:bv[1] = 0 && o2[t]:bv[1] = o2[t-2]:bv[1] && (F (o2[t]:bv[1] = 1))) && (o1[0]:bv[1] = 1) && (G (F (o3[t]:bv[1] = 1)))"
	"ltl_compositional_parts:[^\n]*[^0-9]3[^0-9].*: T" ${_comp} REQUIRES ltlsynt)

# off by default: the conjunction is decided whole
add_repl_test(ltl_compositional-off_by_default
	"realizable (G (i1[t]:bv[1] = 1 -> F o1[t]:bv[1] = 1)) && (G (F (o2[t]:bv[1] = 1 && o3[t]:bv[1] = 0)))"
	": T" FAIL_REGEX "ltl_compositional_parts" REQUIRES ltlsynt)

add_multiline_repl_test(ltl_compositional-set_and_get
	"ltl-compositional: true\n.*ltl-compositional: false"
	"set ltl-compositional on" "set ltl-compositional off"
	"get ltl-compositional")

# each part plays its own strategy on the same input: o2 meets its goal
# with o2[t-1] = 0, o4 releases o3
add_multiline_repl_test(ltl_compositional-run_parts
	"o2\\[1\\] := [01]\n.*o3\\[1\\] := [01]\n.*o4\\[1\\] := [01]\n.*o2\\[4\\] := [01]\n.*o3\\[4\\] := [01]\n.*o4\\[4\\] := [01]"
	STDIN "run 5 steps (G (F (o2[t]:bv[1] = 1 && o2[t-1]:bv[1] = 0))) && ((o3[t]:bv[1] = 0) U (o3[t]:bv[1] = 1 && i1[t-1]:bv[1] = 1 || o4[t]:bv[1] = 1)).\\n1\\n0\\n1\\n1\\n0\\nq\\n"
	FAIL_REGEX "not executable|no values|Segmentation|core dumped"
	${_comp} REQUIRES ltlsynt)

# an update revises the part of o2 only: o2 follows it from step 4, and the
# part of o3 keeps its strategy, whose spec the updated one keeps
add_multiline_repl_test(ltl_compositional-update_revises_one_part
	"Updated specification[^\n]*o3\\[t-1\\][^\n]*\n.*o2\\[4\\] := 0\n.*o2\\[5\\] := 1\n.*o2\\[6\\] := 0"
	STDIN "run (always u[t] = i1[t]) && (G (F (o2[t]:bv[1] = 1))) && (G (F (o3[t]:bv[1] = 1 && o3[t-1]:bv[1] = 0))).\\nF.\\nF.\\nalways o2[t]:bv[1] = 0 || o2[t-1]:bv[1] = 0.\\nF.\\nF.\\nF.\\nq\\nq\\n"
	FAIL_REGEX "not accepted|cannot follow|unsat|not executable"
	${_comp} REQUIRES ltlsynt)
