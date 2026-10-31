include(add_repl_test)

#
# The nestings of two temporal operators the other suites leave out: for each,
# a formula Spot finds realizable and one it does not, neither of them valid.
# tests/sweep/ltl_sweep.py checks every such formula and the normal forms.
#

set(_o "o1[t]:bv[1] = 1")
set(_i "i1[t]:bv[1] = 1")

# ltl_interplay(<outer> <inner> <realizable formula> <unrealizable formula>)
function(ltl_interplay outer inner real unreal)
	foreach(_case "T|${real}" "F|${unreal}")
		string(REPLACE "|" ";" _parts "${_case}")
		list(GET _parts 0 _verdict)
		list(GET _parts 1 _f)
		add_repl_test(ltl_interplay-${outer}_${inner}-realizable_${_verdict}
			"realizable ${_f}" ": ${_verdict}\n" NO_TRACE REQUIRES ltlsynt)
		add_repl_test(ltl_interplay-${outer}_${inner}-not_valid_${_verdict}
			"valid ${_f}" ": F\n" NO_TRACE REQUIRES ltlsynt)
	endforeach()
endfunction()

ltl_interplay(O U "O ((${_o}) U (${_o}))"
	"O ((${_o}) U (${_i}))")
ltl_interplay(O R "O ((${_o}) R (${_o}))"
	"O ((${_o}) R (${_i}))")
ltl_interplay(O W "O ((${_o}) W (${_o}))"
	"O ((${_i}) W (${_i}))")
ltl_interplay(O S "O ((${_o}) S (${_o}))"
	"O ((${_o}) S (${_i}))")
ltl_interplay(O T "O ((${_o}) T (${_o}))"
	"O ((${_o}) T (${_i}))")
ltl_interplay(H U "H ((${_o}) U (${_o}))"
	"H ((${_o}) U (${_i}))")
ltl_interplay(H R "H ((${_o}) R (${_o}))"
	"H ((${_o}) R (${_i}))")
ltl_interplay(H W "H ((${_o}) W (${_o}))"
	"H ((${_i}) W (${_i}))")
ltl_interplay(H S "H ((${_o}) S (${_o}))"
	"H ((${_o}) S (${_i}))")
ltl_interplay(H T "H ((${_o}) T (${_o}))"
	"H ((${_o}) T (${_i}))")
ltl_interplay(U O "(${_o}) U (O (${_o}))"
	"(${_o}) U (O (${_i}))")
ltl_interplay(U H "(${_o}) U (H (${_o}))"
	"(${_o}) U (H (${_i}))")
ltl_interplay(U T "(${_o}) U ((${_o}) T (${_o}))"
	"(${_o}) U ((${_o}) T (${_i}))")
ltl_interplay(R O "(${_o}) R (O (${_o}))"
	"(${_o}) R (O (${_i}))")
ltl_interplay(R H "(${_o}) R (H (${_o}))"
	"(${_o}) R (H (${_i}))")
ltl_interplay(R S "(${_o}) R ((${_o}) S (${_o}))"
	"(${_o}) R ((${_o}) S (${_i}))")
ltl_interplay(W O "(${_o}) W (O (${_o}))"
	"(${_i}) W (O (${_i}))")
ltl_interplay(W H "(${_o}) W (H (${_o}))"
	"(${_i}) W (H (${_i}))")
ltl_interplay(S O "(${_o}) S (O (${_o}))"
	"(${_o}) S (O (${_i}))")
ltl_interplay(S H "(${_o}) S (H (${_o}))"
	"(${_o}) S (H (${_i}))")
ltl_interplay(T O "(${_o}) T (O (${_o}))"
	"(${_o}) T (O (${_i}))")
ltl_interplay(T H "(${_o}) T (H (${_o}))"
	"(${_o}) T (H (${_i}))")
