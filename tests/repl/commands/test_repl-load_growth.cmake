# --- GitHub #203: loading a ?: chain or a flat || condition ------------------
# The reporter's two rule shapes over bitvector inputs: a chain of N branches
# `(i12 = id) ? clause : rest`, and one branch whose clause is a flat || of k
# terms. A clause chooses o5 = 0 when one of its terms
# `i1 > threshold && !(key stream = key)` holds, and o5 = 1 otherwise. Each
# rule is loaded as a spec and as a revision through `u[t] = i0[t]`, then run
# once without the key (o5 = 0) and once with it (o5 = 1).
#
# The load grew exponentially in N and k: the bitvector case split multiplied
# the instances of inputs sharing one condition, and the validity check of a
# revised rule read its inputs through a guard whose elimination split on
# every guard atom. At these sizes it took from 1 to 5 minutes or ran out of
# memory; each case now runs in about 2 s or less, and the timeout catches a
# return of the growth.

# Sets <out> to the clause of a branch whose terms test the key streams i18,
# i19, ... against the keys first_key, first_key + 1, ... with <thresholds>.
function(issue206_clause out first_key thresholds)
	set(_or "")
	set(_j 0)
	foreach(_thr IN LISTS thresholds)
		math(EXPR _stream "18 + ${_j}")
		math(EXPR _key "${first_key} + ${_j}")
		if(_j GREATER 0)
			string(APPEND _or " || ")
		endif()
		string(APPEND _or "(i1[t]:bv[24] > { ${_thr} }:bv[24] && !(i${_stream}[t]:bv[8] = { ${_key} }:bv[8]))")
		math(EXPR _j "${_j} + 1")
	endforeach()
	set(${out} "((${_or}) ? (o5[t]:bv[24] = { 0 }:bv[24]) : (o5[t]:bv[24] = { 1 }:bv[24]))" PARENT_SCOPE)
endfunction()

# Sets <out> to the chain of <n> branches, branch a with id 4(a-1)+1 and the
# keys after it, and <checks> to the inputs of the two runs on branch n.
function(issue206_chain out checks n)
	set(_rule "(o5[t]:bv[24] = { 1 }:bv[24])")
	# built from the last branch outwards
	foreach(_i RANGE 1 ${n})
		math(EXPR _id "4 * (${n} - ${_i}) + 1")
		math(EXPR _key "${_id} + 1")
		issue206_clause(_clause ${_key} "1000;10000;100000")
		set(_rule "((i12[t]:bv[8] = { ${_id} }:bv[8]) ? ${_clause} : ${_rule})")
	endforeach()
	set(${out} "${_rule}" PARENT_SCOPE)
	# the runs take the last branch
	math(EXPR _id "4 * (${n} - 1) + 1")
	math(EXPR _key "${_id} + 1")
	# i1, i12, i18, i19, i20
	set(${checks} "1001\\n${_id}\\n0\\n0\\n0" "1001\\n${_id}\\n${_key}\\n0\\n0" PARENT_SCOPE)
endfunction()

# Sets <out> to one branch of <k> terms, keys 2, 3, ... and thresholds 1000,
# 2000, ..., and <checks> to the inputs of the two runs.
function(issue206_terms out checks k)
	set(_thresholds "")
	set(_none "")
	set(_all "")
	foreach(_j RANGE 1 ${k})
		math(EXPR _thr "1000 * ${_j}")
		math(EXPR _key "${_j} + 1")
		list(APPEND _thresholds ${_thr})
		string(APPEND _none "\\n0")
		string(APPEND _all "\\n${_key}")
	endforeach()
	issue206_clause(_clause 2 "${_thresholds}")
	set(${out} "((i12[t]:bv[8] = { 1 }:bv[8]) ? ${_clause} : (o5[t]:bv[24] = { 1 }:bv[24]))" PARENT_SCOPE)
	math(EXPR _amount "1000 * ${k} + 1")
	# i1, i12, i18, ..., i(17+k)
	set(${checks} "${_amount}\\n1${_none}" "${_amount}\\n1${_all}" PARENT_SCOPE)
endfunction()

# The rule <rule> loaded as a spec and run for the two input sets of <checks>.
function(issue206_spec_test name rule checks)
	list(GET checks 0 _none)
	list(GET checks 1 _key)
	add_multiline_repl_test(load_growth-${name}_spec
		"o5\\[0\\] := 0.*o5\\[1\\] := 1"
		STDIN "run 2 steps always ((o1[t]:bv[24] = i1[t]:bv[24]) && ${rule})\\n${_none}\\n${_key}\\nq\\n"
		REQUIRES bv TIMEOUT 120)
endfunction()

# The rule <rule> loaded at step 1 as a revision of `always o1 = i1` through
# `u[t] = i0[t]`, then run for the two input sets of <checks>.
function(issue206_revision_test name rule checks)
	list(GET checks 0 _none)
	list(GET checks 1 _key)
	add_multiline_repl_test(load_growth-${name}_revision
		"o5\\[2\\] := 0.*o5\\[3\\] := 1"
		STDIN "run ((!(i0[t] = 0)) ? (u[t] = i0[t] && o0[t] = 0) : o0[t] = 1)\\nalways (o1[t]:bv[24] = i1[t]:bv[24])\\nalways ${rule}\\n0\\nF\\n${_none}\\nF\\n${_key}\\nq\\nq\\n"
		REQUIRES bv TIMEOUT 120)
endfunction()

issue206_terms(_rule _checks 20)
issue206_spec_test(terms_20 "${_rule}" "${_checks}")
issue206_terms(_rule _checks 16)
issue206_revision_test(terms_16 "${_rule}" "${_checks}")
issue206_chain(_rule _checks 48)
issue206_spec_test(chain_48 "${_rule}" "${_checks}")
issue206_chain(_rule _checks 40)
issue206_revision_test(chain_40 "${_rule}" "${_checks}")
