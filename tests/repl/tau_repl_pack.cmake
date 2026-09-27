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
# The ids come from the registry, so an algebra added in-tree or registered from
# outside is matched without editing a list here.
#
# Every word holding a '/' is dropped before matching: file paths say where the
# checkout and its data live, not what the spec needs, and a checkout under a
# directory such as `fix-qlt` must not hide cases from a qlt-less pack. The
# separators keep a Tau fraction's type in view: `{1/2}:qlt` still names qlt.
function(tau_repl_unsupported out cmd)
	string(REGEX REPLACE
		"[^][ \t\r\n\"'();:=,{}<>|&]*/[^][ \t\r\n\"'();:=,{}<>|&]*"
		"" _spec "${cmd}")
	foreach(_id ${TAU_REGISTERED_BA_IDS})
		if("${_spec}" MATCHES "[^A-Za-z0-9_]${_id}"
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
		list(JOIN _skipped "\n  " _names)
		message(VERBOSE "REPL suite skipped:\n  ${_names}")
	endif()
endfunction()
