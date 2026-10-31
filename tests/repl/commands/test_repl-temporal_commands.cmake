include(add_repl_test)

#
# Every temporal operator under the commands that keep it or refuse it, and
# the core and counter-strategy of each binary operator with a spare conjunct.
#

set(_o "o1[t]:bv[1] = 1")
set(_i "i1[t]:bv[1] = 1")
set(_oa "o1\\[t\\]:bv\\[1\\](' = 0| = 1)")
set(_ia "i1\\[t\\]:bv\\[1\\](' = 0| = 1)")
set(_plain --color false --benchmarks false)

# temporal_commands(<op> <formula> <printed regex>): the normal forms keep the
# operator, a substitution reaches under it, and solve and lgrs refuse it.
function(temporal_commands op formula printed)
	foreach(_cmd cnf mnf "onf o1" qelim)
		string(REPLACE " " "_" _name "${_cmd}")
		add_repl_test(temporal_commands-${op}-${_name}
			"fragment ctl_star. ${_cmd} ${formula}"
			"%1: ${printed}\n" NO_TRACE FLAGS ${_plain} REQUIRES bv)
	endforeach()
	add_repl_test(temporal_commands-${op}-subst
		"${formula}. s %1[${_o} / ${_i}]" "%2: [^\n]*i1\\[t\\]" NO_TRACE
		FAIL_REGEX "%2: [^\n]*o1\\[t\\]" FLAGS ${_plain} REQUIRES bv)
	foreach(_cmd solve lgrs)
		add_repl_test(temporal_commands-${op}-${_cmd}
			"${_cmd} ${formula}" "Found temporal quantifier in formula"
			NO_FAIL_REGEX NO_TRACE REQUIRES bv)
	endforeach()
endfunction()

temporal_commands(always "G (${_o})" "always ${_oa}")
temporal_commands(sometimes "F (${_o})" "F ${_oa}")
temporal_commands(once "G (O (${_o}))" "always \\(T S ${_oa}\\)")
temporal_commands(historically "G (H (${_o}))"
	"always (\\(F T ${_oa}\\)|!\\(T S \\(!${_oa}\\)\\))")
temporal_commands(until "(${_i}) U (${_o})" "${_ia} U ${_oa}")
temporal_commands(release "(${_i}) R (${_o})" "${_ia} R ${_oa}")
temporal_commands(weak_until "(${_o}) W (${_i})" "${_oa} W ${_ia}")
temporal_commands(since "G ((${_i}) S (${_o}))" "always \\(${_ia} S ${_oa}\\)")
temporal_commands(trigger "G ((${_i}) T (${_o}))" "always \\(${_ia} T ${_oa}\\)")
temporal_commands(path_all "A (G (${_o}))" "A always ${_oa}")
temporal_commands(path_some "E (F (${_o}))" "E F ${_oa}")

# An instantiation under the operator folds its atom.
foreach(_case "always;G" "sometimes;F")
	list(GET _case 0 _name)
	list(GET _case 1 _op)
	add_repl_test(temporal_commands-${_name}-inst
		"i (${_op} (${_o}))[o1[t]:bv[1] / 0]" "%1: F\n" NO_TRACE
		FLAGS ${_plain} REQUIRES bv)
endforeach()

# temporal_core(<op> <unrealizable formula> <printed regex>): the conjunct over
# o2 is realizable alone, so the core is the other one.
function(temporal_core op formula printed)
	set(_spec "(${formula}) && (G (o2[t]:bv[1] = 1))")
	add_repl_test(temporal_commands-${op}-unrealizable_core
		"unrealizable_core ${_spec}" "\n${printed}\n"
		NO_TRACE FLAGS ${_plain} REQUIRES ltlsynt bv)
	add_repl_test(temporal_commands-${op}-explain
		"explain ${_spec}"
		"unrealizable core:\n  ${printed}\n.*environment counter-strategy"
		FAIL_REGEX "core:\n[^\n]*\n  o2" NO_TRACE FLAGS ${_plain} REQUIRES ltlsynt bv)
endfunction()

temporal_core(until "(${_o}) U (${_i})" "${_oa} U ${_ia}")
temporal_core(release "(${_o}) R (${_i})" "${_oa} R ${_ia}")
temporal_core(since "G ((${_o}) S (${_i}))" "always \\(${_oa} S ${_ia}\\)")
temporal_core(trigger "G ((${_o}) T (${_i}))" "always \\(${_oa} T ${_ia}\\)")
