# Pack-awareness for the REPL suite.
#
# A REPL case naming a Boolean algebra outside the configured -DTAU_BAS= is
# skipped rather than failed, judged by the spec text and not the file: a
# fifty-case file with three bv mentions still contributes forty-seven.
#
# A case whose need the spec text does not show declares it on its helper
# call: `REQUIRES ltlsynt` for a live ltlsynt on PATH, `REQUIRES hostfs` for a
# real host file, `REQUIRES subprocess` for a host compiler the case spawns,
# and `REQUIRES <ba-id>` for an algebra the text does not name.
#
# Skipped names are recorded in a global property per reason and summarised
# once, so a build reports what it dropped and why instead of quietly running
# fewer tests.

# Configure-time capability probe. Checked once here rather than per test,
# since the REPL binary itself would have to spawn ltlsynt just to answer
# whether it can spawn ltlsynt. HINTS covers the store package configure
# resolved when no ltlsynt was on PATH.
find_program(TAU_LTLSYNT_PROGRAM ltlsynt HINTS "${TAU_SPOT_BIN}")

# A wasm module cannot spawn a host process, so a host ltlsynt found on
# PATH does not count as usable under Emscripten.
if(TAU_LTLSYNT_PROGRAM AND NOT EMSCRIPTEN)
	set(TAU_REPL_LTLSYNT_USABLE TRUE)
else()
	set(TAU_REPL_LTLSYNT_USABLE FALSE)
endif()

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

# A wasm REPL test can open host files through the NODEFS mount the CLI's
# node pre-js installs (src/tau_stdin_node.pre.js) under the Emscripten node
# emulator. The browser REPL page has no such mount, so TAU_BUILD_REPL_BROWSER_TESTS
# turns it off and a hostfs case skips with the same HOSTFS reason.
if(EMSCRIPTEN AND CMAKE_CROSSCOMPILING_EMULATOR
   AND NOT TAU_BUILD_REPL_BROWSER_TESTS)
	set(TAU_REPL_NODEFS_HOSTFS TRUE)
else()
	set(TAU_REPL_NODEFS_HOSTFS FALSE)
endif()

# Every skip reason, and the summary line it prints. A HOSTFS or SUBPROCESS
# case registers disabled, so ctest lists it as not run instead of missing.
set(TAU_REPL_SKIP_REASONS PACK LTLSYNT HOSTFS SUBPROCESS SDK)
set(TAU_REPL_DISABLED_REASONS HOSTFS SUBPROCESS)
set(TAU_REPL_SKIP_WHY_PACK "naming a BA outside TAU_BAS=${TAU_BAS}")
set(TAU_REPL_SKIP_WHY_LTLSYNT "needing ltlsynt, which this build cannot run")
set(TAU_REPL_SKIP_WHY_HOSTFS "opening a host file this build's filesystem cannot reach")
set(TAU_REPL_SKIP_WHY_SUBPROCESS "spawning a host compiler, which a wasm module or tau.exe under wine cannot run")
set(TAU_REPL_SKIP_WHY_SDK "needing a platform SDK this build tree does not hold")

# Sets <out> to the reason a case cannot run, or to an empty string. hostfs
# gates on EMSCRIPTEN and the node emulator, not CMAKE_CROSSCOMPILING: a mingw
# or arm cross-build still runs against a real filesystem on its target.
function(tau_repl_skip_reason out cmd requires)
	tau_repl_unsupported(_pack "${cmd}")
	foreach(_need ${requires})
		if(_need IN_LIST TAU_REGISTERED_BA_IDS AND NOT _need IN_LIST TAU_PACK_BA_IDS)
			set(_pack TRUE)
		endif()
	endforeach()
	# A Windows target on another host runs tau.exe under wine, which cannot
	# start the host cmake and compiler.
	if("subprocess" IN_LIST requires
			AND (EMSCRIPTEN OR (WIN32 AND NOT CMAKE_HOST_WIN32)))
		set(${out} SUBPROCESS PARENT_SCOPE)
	elseif(_pack)
		set(${out} PACK PARENT_SCOPE)
	elseif("ltlsynt" IN_LIST requires AND NOT TAU_REPL_LTLSYNT_USABLE)
		set(${out} LTLSYNT PARENT_SCOPE)
	elseif("hostfs" IN_LIST requires AND EMSCRIPTEN AND NOT TAU_REPL_NODEFS_HOSTFS)
		set(${out} HOSTFS PARENT_SCOPE)
	else()
		set(${out} "" PARENT_SCOPE)
	endif()
endfunction()

# The prologue of every case helper, after `cmake_parse_arguments(PARSE_ARGV
# <n> _tau ...)`: sets _tau_reason, and records and drops a case that must
# not exist in this build. <cmd_var> names the variable the pack check reads,
# as a macro argument loses escapes.
macro(tau_repl_gate_case test cmd_var)
	if(_tau_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR
			"${test}: unexpected argument(s) ${_tau_UNPARSED_ARGUMENTS}")
	endif()
	foreach(_tau_need ${_tau_REQUIRES})
		if(NOT _tau_need MATCHES "^(ltlsynt|hostfs|subprocess)$"
		   AND NOT _tau_need IN_LIST TAU_REGISTERED_BA_IDS)
			message(FATAL_ERROR "${test}: unknown need '${_tau_need}' "
				"(expected ltlsynt, hostfs, subprocess or a registered BA id)")
		endif()
	endforeach()
	tau_repl_skip_reason(_tau_reason "${${cmd_var}}" "${_tau_REQUIRES}")
	if(_tau_reason AND NOT _tau_reason IN_LIST TAU_REPL_DISABLED_REASONS)
		tau_repl_record_skip("${test}" ${_tau_reason})
		return()
	endif()
endmacro()

# The epilogue of every case helper, after its add_test(): a case the gate
# kept although it cannot run registers disabled.
macro(tau_repl_disable_skipped test)
	if(_tau_reason)
		tau_repl_record_skip("${test}" ${_tau_reason})
	endif()
endmacro()

# Records that the test <name> was skipped for <reason>, for the summary.
function(tau_repl_record_skip name reason)
	if(NOT reason IN_LIST TAU_REPL_SKIP_REASONS)
		message(FATAL_ERROR "${name}: unknown skip reason '${reason}' "
			"(expected one of ${TAU_REPL_SKIP_REASONS})")
	endif()
	set_property(GLOBAL APPEND PROPERTY TAU_REPL_SKIPPED_${reason} "${name}")
	if(reason IN_LIST TAU_REPL_DISABLED_REASONS)
		set_tests_properties("${name}" PROPERTIES DISABLED TRUE)
	endif()
endfunction()

# Prints how many REPL cases this build could not run, and why.
function(tau_repl_report_skips)
	foreach(_reason ${TAU_REPL_SKIP_REASONS})
		get_property(_names GLOBAL PROPERTY TAU_REPL_SKIPPED_${_reason})
		list(LENGTH _names _n)
		if(_n GREATER 0)
			message(STATUS "REPL suite: ${_n} case(s) skipped, "
				"${TAU_REPL_SKIP_WHY_${_reason}}")
		endif()
	endforeach()
endfunction()

# add_test() adds CMAKE_CROSSCOMPILING_EMULATOR only ahead of a target
# COMMAND, and a REPL case runs tau inside a shell string, so every case goes
# through this launcher instead.
if(NOT DEFINED TAU_RUN)
	if(CMAKE_CROSSCOMPILING_EMULATOR)
		string(REPLACE ";" " " _tau_run_emulator "${CMAKE_CROSSCOMPILING_EMULATOR}")
		set(_tau_run_emulator "${_tau_run_emulator} ")
	else()
		set(_tau_run_emulator "")
	endif()
	file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/tau-run"
		CONTENT "#!/bin/sh\nexec ${_tau_run_emulator}\"$<TARGET_FILE:${TAU_EXECUTABLE_NAME}>\" \"$@\"\n"
		FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
			GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	set(TAU_RUN "${CMAKE_BINARY_DIR}/tau-run")
endif()

# The launcher of a case that runs tau without a shell. TAU_RUN is a sh
# script, so a Windows host takes the binary itself. A cross host keeps
# TAU_RUN, which puts the emulator in front of tau.exe.
if(CMAKE_HOST_WIN32)
	set(TAU_LAUNCHER "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
else()
	set(TAU_LAUNCHER "${TAU_RUN}")
endif()
