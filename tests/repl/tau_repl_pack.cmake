# Pack-awareness for the REPL suite.
#
# A REPL case is a spec string handed to the tau binary. If it names a Boolean
# algebra the configured -DTAU_BAS= does not include, the binary cannot answer it
# and the case must be skipped rather than fail. Gating on the spec text rather
# than on the file it lives in keeps the granularity right: a file of fifty cases
# where three mention bv still contributes forty-seven to a bv-less pack.
#
# Skipped names are recorded in a global property and summarised once, so a
# reduced pack reports what it dropped instead of quietly running fewer tests.

# Sets <out> to TRUE when <cmd> names a BA that is not in the configured pack.
function(tau_repl_unsupported out cmd)
	foreach(_id bv sbf qlt qint hsb nlang)
		if("${cmd}" MATCHES "[^A-Za-z0-9_]${_id}"
			AND NOT _id IN_LIST TAU_PACK_BA_IDS)
			set(${out} TRUE PARENT_SCOPE)
			return()
		endif()
	endforeach()
	set(${out} FALSE PARENT_SCOPE)
endfunction()

# Records that <name> was skipped, for the summary.
function(tau_repl_record_skip name)
	set_property(GLOBAL APPEND PROPERTY TAU_REPL_SKIPPED "${name}")
endfunction()

# Prints how many REPL cases the configured pack could not run.
function(tau_repl_report_skips)
	get_property(_skipped GLOBAL PROPERTY TAU_REPL_SKIPPED)
	list(LENGTH _skipped _n)
	if(_n GREATER 0)
		message(STATUS
			"REPL suite: ${_n} case(s) skipped, naming a BA outside "
			"TAU_BAS=${TAU_BAS}")
	endif()
endfunction()
