#
# Pointwise revision of a running specification for every temporal operator:
# the spec holds o1 at 0 under the operator, and the update, read from the
# input of the u stream at step 1, asks o1 = 1 under an operator. The update
# is applied at step 1, so from step 2 on the outputs meet it. The pairs of
# one operator on both sides come first, then the pairs of two operators that
# revise() handles by a case of its own, then a clause the update does not
# touch, which the revision keeps, and the updates with U, R, W, S or T that
# only the data game plays, and the CTL* fragment.
#

include(add_repl_test)

set(_z "o1[t]:bv[1] = 0")
set(_one "o1[t]:bv[1] = 1")
set(_z2 "o1[t]:bv[1] = 0 && o2[t]:bv[1] = 0")
set(_one2 "o1[t]:bv[1] = 1 && o2[t]:bv[1] = 1")

# pwr_run(<name> <spec> <update> <regex>): run 5 steps of the spec next to
# the u stream, update it at step 1, and match the outputs.
function(pwr_run name spec update regex)
	add_multiline_repl_test(pwr_matrix-${name}
		"Updated specification[^\n]*\n.*${regex}"
		STDIN "run 5 steps (always u[t] = i1[t]) && (${spec}).\\nF.\\n${update}.\\nF.\\nF.\\nF.\\nq\\nq\\n"
		FAIL_REGEX "Error|UNKNOWN|no update was performed" REQUIRES ltlsynt)
endfunction()

set(_o1_after "o1\\[2\\] := 1.*o1\\[3\\] := 1.*o1\\[4\\] := 1")
set(_o12_at_2 "o1\\[2\\] := 1\n.*o2\\[2\\] := 1\n")

# one operator on both sides
pwr_run(G_G "G(${_z})" "G(${_one})" "${_o1_after}")
pwr_run(F_G "F(${_z})" "G(${_one})" "${_o1_after}")
pwr_run(GF_G "G(F(${_z}))" "G(${_one})" "${_o1_after}")
pwr_run(O_O "G(O(${_z}))" "G(O(${_one}))" "o1\\[2\\] := 1")
pwr_run(H_H "G(H(${_z}))" "G(H(${_one}))" "${_o1_after}")
pwr_run(U_U "(${_z}) U (${_z2})" "(${_one}) U (${_one2})" "o1\\[2\\] := 1")
pwr_run(R_R "(${_z}) R (${_z2})" "(${_one}) R (${_one2})" "${_o12_at_2}")
pwr_run(W_W "(${_z}) W (${_z2})" "(${_one}) W (${_one2})" "o1\\[2\\] := 1")
pwr_run(S_S "G((${_z}) S (${_z2}))" "G((${_one}) S (${_one2}))"
	"${_o12_at_2}.*o1\\[3\\] := 1.*o1\\[4\\] := 1")
pwr_run(T_T "G((${_z}) T (${_z2}))" "G((${_one}) T (${_one2}))"
	"${_o12_at_2}")

# two operators: a G spec against an R update revises the invariant of R,
# an R spec against a G update keeps R, an F spec meets a U or a W update
# at once, and a mismatch keeps the update
pwr_run(R_G "(${_z}) R (${_z2})" "G(${_one})" "${_o1_after}")
pwr_run(F_W "F(${_z})" "(${_one}) W (${_one2})" "o1\\[2\\] := 1")
pwr_run(F_U "F(${_z})" "(${_one}) U (${_one2})" "o1\\[2\\] := 1")
pwr_run(U_R "(${_z}) U (${_z2})" "(${_one}) R (${_one2})" "${_o12_at_2}")
pwr_run(H_G "G(H(${_z}))" "G(${_one})" "${_o1_after}")

# a clause over o3 is no part of the conflict and survives the revision
pwr_run(keep_U "(G(o3[t]:bv[1] = 1)) && ((${_z}) U (${_z2}))"
	"(${_one}) U (${_one2})"
	"o1\\[2\\] := 1.*o3\\[2\\] := 1.*o3\\[3\\] := 1.*o3\\[4\\] := 1")
pwr_run(keep_S "(G(o3[t]:bv[1] = 1)) && (G((${_z}) S (${_z2})))"
	"G((${_one}) S (${_one2}))"
	"${_o12_at_2}.*o3\\[2\\] := 1.*o3\\[3\\] := 1.*o3\\[4\\] := 1")

# A step-formula spec against an update with U, R, W, S or T under G or at
# the top level: only the data game plays such an update, and the run goes
# on without an undecided step continuation.
pwr_run(G_GT "G(${_z})" "G((${_one}) T (${_one2}))" "${_o12_at_2}")
pwr_run(G_GS "G(${_z})" "G((${_one}) S (${_one2}))" "${_o12_at_2}")
pwr_run(G_GU "G(${_z})" "G((${_one}) U (${_one2}))" "o1\\[2\\] := 1")
pwr_run(G_GR "G(${_z})" "G((${_one}) R (${_one2}))" "${_o12_at_2}")
pwr_run(G_GO "G(${_z})" "G(O(${_one}))" "o1\\[2\\] := 1")
pwr_run(G_GH "G(${_z})" "G(H(${_one}))" "o1\\[2\\] := 1.*o1\\[3\\] := 1")
pwr_run(G_S "G(${_z})" "(${_one}) S (${_one2})" "${_o12_at_2}")
pwr_run(G_T "G(${_z})" "(${_one}) T (${_one2})" "${_o12_at_2}")
# an update on another output adds its part and keeps the spec
pwr_run(G_GT_other_output "G(o2[t]:bv[1] = 0)" "G((${_one}) T (${_one}))"
	"o1\\[2\\] := 1\n.*o2\\[2\\] := 0\n.*o1\\[3\\] := 1\n.*o2\\[3\\] := 0")

# The CTL* fragment: an A in the update is played as the LTL formula it
# reduces to, a spec with an E gives way to a realizable update, and an E
# update replaces the conflicting part, of a step-formula spec or of one
# with an E, whose witnesses it keeps apart from its own.
function(pwr_run_ctl_star name spec update regex)
	add_multiline_repl_test(pwr_matrix-${name}
		"Updated specification[^\n]*\n.*${regex}"
		STDIN "fragment ctl_star.\\nrun 4 steps (always u[t] = i1[t]) && (${spec}).\\nF.\\n${update}.\\nF.\\nF.\\nq\\nq\\n"
		FAIL_REGEX "Error|UNKNOWN|No solution|no update was performed"
		REQUIRES ltlsynt)
endfunction()

pwr_run_ctl_star(ctl_G_AG "G(${_z})" "A(G(${_one}))"
	"o1\\[2\\] := 1.*o1\\[3\\] := 1")
pwr_run_ctl_star(ctl_AG_AG "A(G(${_z}))" "A(G(${_one}))"
	"o1\\[2\\] := 1.*o1\\[3\\] := 1")
pwr_run_ctl_star(ctl_AG_G "A(G(${_z}))" "G(${_one})"
	"o1\\[2\\] := 1.*o1\\[3\\] := 1")
pwr_run_ctl_star(ctl_EG_G "E(G(${_z}))" "G(${_one})"
	"o1\\[2\\] := 1.*o1\\[3\\] := 1")
pwr_run_ctl_star(ctl_G_EF "G(${_z})" "E(F(${_one}))" "o1\\[2\\] := 1")
pwr_run_ctl_star(ctl_EF_EG "E(F(${_z}))" "E(G(${_one}))"
	"o1\\[2\\] := 1.*o1\\[3\\] := 1")
pwr_run_ctl_star(ctl_EG_EG "E(G(${_z}))" "E(G(${_one}))"
	"o1\\[2\\] := 1.*o1\\[3\\] := 1")
pwr_run_ctl_star(ctl_EG_E_other_output "E(G(${_z}))"
	"E(G(o2[t]:bv[1] = 1))"
	"o1\\[2\\] := 0\n.*o2\\[2\\] := 1\n.*o1\\[3\\] := 0\n.*o2\\[3\\] := 1")
pwr_run_ctl_star(ctl_keep_EF_EG "(G(o2[t]:bv[1] = 1)) && (E(F(${_z})))"
	"E(G(${_one}))"
	"o1\\[2\\] := 1\n.*o2\\[2\\] := 1\n.*o1\\[3\\] := 1\n.*o2\\[3\\] := 1")
