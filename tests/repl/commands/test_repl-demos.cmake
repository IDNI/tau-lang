include(add_repl_test)

#
# Every demo runs to its end with no report it does not ask for.
#

# 6.2 shows a formula that has no verdict, so UNKNOWN is part of its output.
set(TAU_DEMOS_SHOWING_UNKNOWN demo_6.2-ltl_satisfiability_and_realizability)

file(GLOB _demos "${CMAKE_SOURCE_DIR}/demos/*.tau")
foreach(_demo ${_demos})
	get_filename_component(_name "${_demo}" NAME_WLE)
	file(READ "${_demo}" _text)
	set(_requires ltlsynt hostfs)
	foreach(_ba bv qint qlt hsb nlang)
		if(_text MATCHES ":${_ba}|--${_ba}[ \t]")
			list(APPEND _requires ${_ba})
		endif()
	endforeach()
	set(_fail "Unknown option|Syntax Error|Found temporal quantifier")
	if(NOT _name IN_LIST TAU_DEMOS_SHOWING_UNKNOWN)
		string(APPEND _fail "|UNKNOWN")
	endif()
	add_raw_repl_test(demos-${_name}
		"\"${CMAKE_CURRENT_SOURCE_DIR}/../run_demo.sh\" ${TAU_RUN} \"${_demo}\""
		"DEMO-END" TIMEOUT 120
		FAIL_REGEX "${_fail}"
		REQUIRES ${_requires})
endforeach()
