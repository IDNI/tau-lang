#
# The smallest formulas of every temporal operator, each with one right
# answer from the semantics of LTL: the operators over the constants T and F
# and over one output, every nesting of two prefix operators, the laws that
# relate the operators, the long spellings, the prefix operators over an
# input, the CTL* quantifiers over constants, and every operator over the
# atoms of one output and one input. The streams are bv[1]; `sat` and
# `realizable` read the inputs universally, `valid` reads them as outputs.
#

set(_o "o1[t]:bv[1] = 1")
set(_o2 "o2[t]:bv[1] = 1")
set(_i "i1[t]:bv[1] = 1")

# ltl_trivial(<name> <command> <T|F> [ltlsynt]): the command answers the
# verdict and never the other one.
function(ltl_trivial name cmd verdict)
	if(verdict STREQUAL "T")
		set(_other F)
	else()
		set(_other T)
	endif()
	set(_requires)
	if("ltlsynt" IN_LIST ARGN)
		set(_requires REQUIRES ltlsynt)
	endif()
	# the result name `%N` carries a color escape before its colon
	add_repl_test(ltl_trivial-${name} "${cmd}" "%[0-9]+[^:\n]*: ${verdict}"
		FAIL_REGEX "Error|%[0-9]+[^:\n]*: ${_other}" ${_requires})
endfunction()

# A prefix operator over a constant is that constant: the hooks fold it.
foreach(_op F G O H)
	foreach(_c T F)
		foreach(_cmd sat valid)
			ltl_trivial(${_op}_${_c}-${_cmd} "${_cmd} ${_op}(${_c})" ${_c})
		endforeach()
	endforeach()
endforeach()

# ltl_trivial_binary(<left> <right> <U> <R> <W> <S> <T>): each of the five
# binary operators over the two operands, as the pair of verdicts of `sat`
# and of `valid` (`TF` is satisfiable and no tautology). `o` stands for the
# output. A top-level formula is read at its first step, and one whose only
# operators are S and T at every step, so `T R o` is o now and `F S o` is
# `G o`.
function(ltl_trivial_binary left right)
	set(_ops U R W S T)
	set(_names until release weak_until since trigger)
	set(_operand_T "T")
	set(_operand_F "F")
	set(_operand_o "${_o}")
	foreach(_k RANGE 4)
		list(GET _ops ${_k} _op)
		list(GET _names ${_k} _name)
		list(GET ARGN ${_k} _pair)
		string(SUBSTRING "${_pair}" 0 1 _sat)
		string(SUBSTRING "${_pair}" 1 1 _valid)
		set(_f "(${_operand_${left}}) ${_op} (${_operand_${right}})")
		# only the formulas the hooks fold to a constant decide without
		# ltlsynt
		set(_needs ltlsynt)
		if(NOT "${left}${right}" MATCHES "o" AND NOT "${left}${_op}${right}" STREQUAL "FWF")
			set(_needs)
		endif()
		ltl_trivial(${left}_${_name}_${right}-sat "sat ${_f}" ${_sat} ${_needs})
		ltl_trivial(${left}_${_name}_${right}-valid "valid ${_f}" ${_valid} ${_needs})
	endforeach()
endfunction()

#                  left right U  R  W  S  T
ltl_trivial_binary(T    T     TT TT TT TT TT)
ltl_trivial_binary(F    F     FF FF FF FF FF)
# T U F needs an F; T R F needs F now; T W F is G T
ltl_trivial_binary(T    F     FF FF TT FF FF)
ltl_trivial_binary(F    T     TT TT TT TT TT)
# T U o is F o, T R o is o now, T W o holds, T S o is G(O o), T T o is G o
ltl_trivial_binary(T    o     TF TF TT TF TF)
# F U o and F W o are o now, F R o is G o, F S o and F T o are G o
ltl_trivial_binary(F    o     TF TF TF TF TF)
ltl_trivial_binary(o    T     TT TT TT TT TT)
# o W F is G o; the others need F at the first step
ltl_trivial_binary(o    F     FF FF TF FF FF)
# each of them is o now, or G o for S and T
ltl_trivial_binary(o    o     TF TF TF TF TF)

# Every nesting of two prefix operators over an output can hold (the output
# stays 1) and can fail (it stays 0).
foreach(_a F G O H)
	foreach(_b F G O H)
		ltl_trivial(${_a}${_b}-sat "sat ${_a}(${_b}(${_o}))" T ltlsynt)
		ltl_trivial(${_a}${_b}-valid "valid ${_a}(${_b}(${_o}))" F ltlsynt)
	endforeach()
endforeach()

# The laws between the operators are valid; the classic non-laws are not.
# The past operators sit under G, so each side is read at every step.
set(_laws
	"GG_is_G@(G(G(${_o}))) <-> (G(${_o}))@T"
	"FF_is_F@(F(F(${_o}))) <-> (F(${_o}))@T"
	"GFG_is_FG@(G(F(G(${_o})))) <-> (F(G(${_o})))@T"
	"FGF_is_GF@(F(G(F(${_o})))) <-> (G(F(${_o})))@T"
	"FG_implies_GF@(F(G(${_o}))) -> (G(F(${_o})))@T"
	"GF_not_implies_FG@(G(F(${_o}))) -> (F(G(${_o})))@F"
	"G_implies_F@(G(${_o})) -> (F(${_o}))@T"
	"F_not_implies_G@(F(${_o})) -> (G(${_o}))@F"
	"not_F_is_G_not@(!(F(${_o}))) <-> (G(!(${_o})))@T"
	"not_G_is_F_not@(!(G(${_o}))) <-> (F(!(${_o})))@T"
	"not_U_is_R@(!((${_o}) U (${_o2}))) <-> ((!(${_o})) R (!(${_o2})))@T"
	"not_R_is_U@(!((${_o}) R (${_o2}))) <-> ((!(${_o})) U (!(${_o2})))@T"
	"W_is_U_or_G@((${_o}) W (${_o2})) <-> (((${_o}) U (${_o2})) || (G(${_o})))@T"
	"R_is_W@((${_o}) R (${_o2})) <-> ((${_o2}) W ((${_o}) && (${_o2})))@T"
	"U_is_W_and_F@((${_o}) U (${_o2})) <-> (((${_o}) W (${_o2})) && (F(${_o2})))@T"
	"U_implies_F@((${_o}) U (${_o2})) -> (F(${_o2}))@T"
	"R_is_G_or_F@((${_o}) R (${_o2})) -> ((G(${_o2})) || (F(${_o})))@T"
	"F_is_T_U@(F(${_o})) <-> ((T) U (${_o}))@T"
	"G_is_F_R@(G(${_o})) <-> ((F) R (${_o}))@T"
	"G_distributes_and@(G((${_o}) && (${_o2}))) <-> ((G(${_o})) && (G(${_o2})))@T"
	"F_distributes_or@(F((${_o}) || (${_o2}))) <-> ((F(${_o})) || (F(${_o2})))@T"
	"G_not_distributes_or@(G((${_o}) || (${_o2}))) <-> ((G(${_o})) || (G(${_o2})))@F"
	"F_not_distributes_and@(F((${_o}) && (${_o2}))) <-> ((F(${_o})) && (F(${_o2})))@F"
	"O_is_T_S@(G(O(${_o}))) <-> (G((T) S (${_o})))@T"
	"H_is_not_O_not@(G(H(${_o}))) <-> (G(!(O(!(${_o})))))@T"
	"T_is_not_S@(G((${_o}) T (${_o2}))) <-> (G(!((!(${_o})) S (!(${_o2})))))@T"
	"GH_is_G@(G(H(${_o}))) <-> (G(${_o}))@T"
	"S_implies_O@(G((${_o}) S (${_o2}))) -> (G(O(${_o2})))@T"
	"H_implies_O@(G(H(${_o}))) -> (G(O(${_o})))@T"
	"O_not_implies_H@(G(O(${_o}))) -> (G(H(${_o})))@F")
foreach(_law IN LISTS _laws)
	string(REPLACE "@" ";" _law "${_law}")
	list(GET _law 0 _name)
	list(GET _law 1 _f)
	list(GET _law 2 _verdict)
	ltl_trivial(law_${_name} "valid ${_f}" ${_verdict} ltlsynt)
endforeach()

# Each long spelling is its letter.
foreach(_pair always:G sometimes:F once:O historically:H)
	string(REPLACE ":" ";" _pair "${_pair}")
	list(GET _pair 0 _word)
	list(GET _pair 1 _letter)
	ltl_trivial(spelling_${_word}
		"valid (G(${_word}(${_o}))) <-> (G(${_letter}(${_o})))" T ltlsynt)
endforeach()
ltl_trivial(spelling_box "valid (G([](${_o}))) <-> (G(G(${_o})))" T ltlsynt)
ltl_trivial(spelling_diamond "valid (G(<>(${_o}))) <-> (G(F(${_o})))" T ltlsynt)
foreach(_pair until:U release:R weak_until:W since:S trigger:T)
	string(REPLACE ":" ";" _pair "${_pair}")
	list(GET _pair 0 _word)
	list(GET _pair 1 _letter)
	ltl_trivial(spelling_${_word}
		"valid (G((${_o}) ${_word} (${_o2}))) <-> (G((${_o}) ${_letter} (${_o2})))"
		T ltlsynt)
endforeach()

# A prefix operator over an input is the environment's to break; the system
# wins only what it does not depend on.
ltl_trivial(input_F-realizable "realizable F(${_i})" F ltlsynt)
ltl_trivial(input_G-realizable "realizable G(${_i})" F ltlsynt)
ltl_trivial(input_GF-realizable "realizable G(F(${_i}))" F ltlsynt)
ltl_trivial(input_FG-realizable "realizable F(G(${_i}))" F ltlsynt)
ltl_trivial(input_O-realizable "realizable O(${_i})" F ltlsynt)
ltl_trivial(input_H-realizable "realizable H(${_i})" F ltlsynt)
ltl_trivial(input_G_or_F_not-realizable
	"realizable (G(${_i})) || (F(!(${_i})))" T ltlsynt)
ltl_trivial(input_echo-realizable "realizable G((${_o}) <-> (${_i}))" T ltlsynt)

# The CTL* quantifiers over constants, and over one stream: A and E range
# over the input sequences, `-` is "unrealizable".
foreach(_case "A_T:A(T):T" "A_F:A(F):F" "E_T:E(T):T" "E_F:E(F):F"
		"semneg_T:-(T):F" "semneg_F:-(F):T")
	string(REPLACE ":" ";" _case "${_case}")
	list(GET _case 0 _name)
	list(GET _case 1 _f)
	list(GET _case 2 _verdict)
	ltl_trivial(ctl_star_${_name}
		"fragment ctl_star. realizable ${_f}" ${_verdict})
endforeach()
ltl_trivial(ctl_star_A_GF_output
	"fragment ctl_star. realizable A(G(F(${_o})))" T ltlsynt)
ltl_trivial(ctl_star_E_G_output
	"fragment ctl_star. realizable E(G(${_o}))" T ltlsynt)
ltl_trivial(ctl_star_E_F_input
	"fragment ctl_star. realizable E(F(${_i}))" T ltlsynt)
ltl_trivial(ctl_star_E_G_input
	"fragment ctl_star. realizable E(G(${_i}))" F ltlsynt)
ltl_trivial(ctl_star_A_F_input
	"fragment ctl_star. realizable A(F(${_i}))" F ltlsynt)
ltl_trivial(ctl_star_semneg_F_input
	"fragment ctl_star. realizable -(F(${_i}))" T ltlsynt)

# The operators over the four atoms of one output and one input: o0 is
# `o1[t] = 0`, o1 `o1[t] = 1`, i0 `i1[t] = 0` and i1 `i1[t] = 1`. Each
# verdict pair is that of `realizable` (which `sat` answers too, the inputs
# being universal) and of `valid` (every stream read as an output). The
# environment owns i1, so a formula the inputs alone must make true is
# unrealizable.
set(_atom_o0 "o1[t]:bv[1] = 0")
set(_atom_o1 "o1[t]:bv[1] = 1")
set(_atom_i0 "i1[t]:bv[1] = 0")
set(_atom_i1 "i1[t]:bv[1] = 1")
set(_atoms o0 o1 i0 i1)

# ltl_trivial_atom_case(<name> <formula> <pair> <commands>...)
function(ltl_trivial_atom_case name f pair)
	string(SUBSTRING "${pair}" 0 1 _real)
	string(SUBSTRING "${pair}" 1 1 _valid)
	foreach(_cmd ${ARGN})
		if(_cmd STREQUAL "valid")
			set(_verdict ${_valid})
		else()
			set(_verdict ${_real})
		endif()
		ltl_trivial(${name}-${_cmd} "${_cmd} ${f}" ${_verdict} ltlsynt)
	endforeach()
endfunction()

# ltl_trivial_atom_prefix(<operators> <o0> <o1> <i0> <i1>): one prefix
# operator, or two nested, over each atom.
function(ltl_trivial_atom_prefix ops)
	string(LENGTH "${ops}" _n)
	set(_commands sat realizable valid)
	if(_n GREATER 1)
		set(_commands realizable valid)
	endif()
	foreach(_k RANGE 3)
		list(GET _atoms ${_k} _a)
		list(GET ARGN ${_k} _pair)
		set(_f "${_atom_${_a}}")
		foreach(_j RANGE ${_n})
			if(_j LESS _n)
				math(EXPR _p "${_n} - 1 - ${_j}")
				string(SUBSTRING "${ops}" ${_p} 1 _op)
				set(_f "${_op}(${_f})")
			endif()
		endforeach()
		ltl_trivial_atom_case(atom_${ops}_${_a} "${_f}" ${_pair} ${_commands})
	endforeach()
endfunction()

# Over an output each one can be met; over an input none can.
#                       o0 o1 i0 i1
ltl_trivial_atom_prefix(F TF TF FF FF)
ltl_trivial_atom_prefix(G TF TF FF FF)
ltl_trivial_atom_prefix(O TF TF FF FF)
ltl_trivial_atom_prefix(H TF TF FF FF)
foreach(_a F G O H)
	foreach(_b F G O H)
		ltl_trivial_atom_prefix(${_a}${_b} TF TF FF FF)
	endforeach()
endforeach()

# ltl_trivial_atom_binary(<op> <left> <o0> <o1> <i0> <i1>): the operator
# with the left atom and each right atom.
function(ltl_trivial_atom_binary op left)
	foreach(_k RANGE 3)
		list(GET _atoms ${_k} _r)
		list(GET ARGN ${_k} _pair)
		ltl_trivial_atom_case(atom_${left}_${op}_${_r}
			"(${_atom_${left}}) ${op} (${_atom_${_r}})" ${_pair}
			sat realizable valid)
	endforeach()
endfunction()

# U, R, S and T are realizable exactly when the right side is an output
# atom: the system makes it true at once, or at every step. None is valid.
foreach(_op U R S T)
	foreach(_left ${_atoms})
		#                                       o0 o1 i0 i1
		ltl_trivial_atom_binary(${_op} ${_left} TF TF FF FF)
	endforeach()
endforeach()
# W holds forever on its left side alone, so an output on the left wins
# too; `x = 0 W x = 1` and `x = 1 W x = 0` are tautologies.
#                         left o0 o1 i0 i1
ltl_trivial_atom_binary(W o0   TF TT TF TF)
ltl_trivial_atom_binary(W o1   TT TF TF TF)
ltl_trivial_atom_binary(W i0   TF TF FF TT)
ltl_trivial_atom_binary(W i1   TF TF TT FF)
