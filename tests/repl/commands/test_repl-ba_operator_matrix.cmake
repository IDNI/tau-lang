include(add_repl_test)

#
# The operators past G and F, the negative decisions, a run and a revision over
# each algebra, with the verdicts of the bv[1] table in
# test_repl-ltl_operator_matrix.cmake: an output atom the system can always
# meet and an input atom it cannot.
#

# ba_operators(<ba> <output atom> <input atom>)
function(ba_operators ba o i)
	foreach(_case
			"once|G(O(${o}))|G(O(${i}))"
			"historically|G(H(${o}))|G(H(${i}))"
			"trigger|G((${i}) T (${o}))|G((${o}) T (${i}))"
			"since|G((${i}) S (${o}))|G((${o}) S (${i}))"
			"release|(${i}) R (${o})|(${o}) R (${i})"
			"weak_until|(${o}) W (${i})|(${i}) W (G(!(${i})))")
		string(REPLACE "|" ";" _parts "${_case}")
		list(GET _parts 0 _op)
		list(GET _parts 1 _real)
		list(GET _parts 2 _unreal)
		add_repl_test(ba_operators-${ba}-${_op}-realizable
			"realizable ${_real}" ": T\n" NO_TRACE REQUIRES ltlsynt ${ba})
		add_repl_test(ba_operators-${ba}-${_op}-unrealizable
			"realizable ${_unreal}" ": F\n" NO_TRACE REQUIRES ltlsynt ${ba})
	endforeach()
	set(_clash "(G (${o})) && (F (!(${o})))")
	add_repl_test(ba_operators-${ba}-unsat
		"unsat ${_clash}" ": T\n" NO_TRACE REQUIRES ltlsynt ${ba})
	add_repl_test(ba_operators-${ba}-unrealizable
		"unrealizable ${_clash}" ": T\n" NO_TRACE REQUIRES ltlsynt ${ba})
	add_repl_test(ba_operators-${ba}-valid_tautology
		"valid G((${o}) || !(${o}))" ": T\n" NO_TRACE REQUIRES ltlsynt ${ba})
	add_repl_test(ba_operators-${ba}-valid_not_tautology
		"valid G(${o})" ": F\n" NO_TRACE REQUIRES ltlsynt ${ba})
endfunction()

foreach(_ba tau sbf qint hsb nlang)
	ba_operators(${_ba} "o1[t]:${_ba} = 1" "i1[t]:${_ba} = 1")
endforeach()
# A qlt stream holds points, so its typed 1 is no value of a stream.
ba_operators(qlt "o1[t]:qlt > {0}:qlt" "i1[t]:qlt > {0}:qlt")

# ba_run(<ba> <bottom printed> <top printed>): the run takes the typed 0, and a
# revision on stdin moves it to the typed 1 from the next step.
function(ba_run ba bottom top)
	add_repl_test(ba_run-${ba}-always
		"run 2 steps G (o1[t]:${ba} = 0)" "o1\\[1\\] := ${bottom}"
		NO_TRACE REQUIRES ${ba})
	add_multiline_repl_test(ba_run-${ba}-revision
		"o1\\[0\\] := ${bottom}.*Updated specification.*o1\\[1\\] := ${top}"
		STDIN "disable benchmarks\\nrun 2 steps u[t] = i1[t] && o1[t]:${ba} = 0.\\no1[t]:${ba} = 1.\\nF.\\nq\\nq\\n"
		REQUIRES ${ba})
endfunction()

ba_run(qint bot top)
ba_run(hsb bot top)
ba_run(nlang nothing everything)

# The program of each algebra with no emitter of its own prints what run prints.
include(add_compile_test)
add_compile_run_test(ba_compile-hsb 2 "G (o1[t]:hsb = {top}:hsb).")
add_compile_run_test(ba_compile-qint 2 "G (o1[t]:qint = { [1,2) }:qint).")
add_compile_run_test(ba_compile-nlang 2 "G (o1[t]:nlang = {it rains}:nlang).")
