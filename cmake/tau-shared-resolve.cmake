# cmake/tau-resolve.cmake
#
# Single source of truth for two build settings that CMake and scripts/devrc
# both need to agree on:
#
#   TAU_BUILD_JOBS     -D  >  environment  >  auto
#                       auto = min((cores + 1) / 2, available_MiB / TAU_BUILD_JOB_MEMORY_MB), min 1
#                       (TAU_BUILD_JOB_MEMORY_MB itself resolves -D > environment > default;
#                       see _tau_resolve_job_memory_mb below)
#   TAU_SHARED_PREFIX  -D  >  environment  >  $HOME/.tau
#
# include()d from CMakeLists.txt, this module declares the two settings as
# cache variables holding *user intent* and computes the *resolved* value
# into a separate, uncached variable (TAU_BUILD_JOBS_RESOLVED /
# TAU_SHARED_PREFIX_RESOLVED) on every configure. Consumers that want the
# actual number/path to build with must read the _RESOLVED variable, not
# the cache entry - and the module must never write the resolved value
# back into the cache entry (not even with FORCE): doing so would turn
# "auto" into whatever it first resolved to, and the environment variable
# would silently stop being read on the next reconfigure of the same build
# directory. TAU_BUILD_JOBS's cache default is "" (nobody passed -D),
# distinct from an explicit -DTAU_BUILD_JOBS=0: "" falls through to the
# environment, "0" means "auto, ignoring the environment" -- see the
# comment where TAU_BUILD_JOBS_RESOLVED is computed, below.
#
# It also runs standalone as:
#
#   cmake -P cmake/tau-resolve.cmake <TAU_BUILD_JOBS|TAU_SHARED_PREFIX> [<requested>]
#
# which applies the same rule outside of a project() and prints the result -
# and only the result - to stdout, so scripts/devrc can shell out to this
# module instead of reimplementing the resolution rule in bash. <requested>
# plays the role the cache entry plays under CMake: pass the explicit value
# if the caller has one (e.g. a -D from the command line), or omit it (or
# pass "") for none. As with the cache entry above, an omitted/"" <requested>
# is "nothing explicit requested" (fall through to the environment) while an
# explicit "0" (a present 4th argv, distinguishable from a missing one -- see
# scripts/devrc's _tau_resolve) means "auto, ignore the environment".

include_guard(GLOBAL)

# Default memory budget (MiB) per compile job when auto-detecting
# TAU_BUILD_JOBS. Single source for the two places this literal is needed:
# the TAU_BUILD_JOB_MEMORY_MB cache variable's default (include mode) and
# the fallback _tau_resolve_job_memory_mb uses when neither -D nor the
# environment supplied a value (script mode has no cache to default).
set(TAU_BUILD_JOB_MEMORY_MB_DEFAULT 1300)

# Auto-detection path for _tau_resolve_build_jobs, below: half the logical
# core count (rounded up, minimum 1), capped by however many compile jobs
# fit in available memory without swapping. mem_budget is the caller's
# already-resolved TAU_BUILD_JOB_MEMORY_MB (see _tau_resolve_job_memory_mb).
# The memory cap only ever lowers the core-count answer, never raises it,
# and is skipped outright (falling back to the core-count answer alone)
# whenever either input to it is unusable:
#   - mem_budget is 0 or not a positive integer.
#   - AVAILABLE_PHYSICAL_MEMORY comes back empty or 0, which
#     cmake_host_system_information() does on platforms it can't query.
# out_reason is set to a plain-text (unparenthesised) explanation of
# whichever bound won, for callers that want to report it; callers that
# don't care can pass any variable name and ignore it.
function(_tau_resolve_auto_build_jobs mem_budget out_var out_reason)
	include(ProcessorCount)
	ProcessorCount(cores)
	if(cores EQUAL 0)
		message(WARNING
			"Could not detect number of logical CPU cores, defaulting to 1")
		set(cores 1)
	endif()
	math(EXPR cores_jobs "(${cores} + 1) / 2")
	if(cores_jobs LESS 1)
		set(cores_jobs 1)
	endif()

	# Default to the core-count answer; each guard clause below returns
	# early and keeps it whenever the memory cap can't be applied.
	set("${out_var}" "${cores_jobs}" PARENT_SCOPE)
	set("${out_reason}" "half of ${cores} cores" PARENT_SCOPE)

	if(NOT mem_budget MATCHES "^[1-9][0-9]*$")
		# 0 or not a positive integer -- treat as "no cap"
		return()
	endif()

	cmake_host_system_information(RESULT mem_available
		QUERY AVAILABLE_PHYSICAL_MEMORY)
	if(NOT mem_available MATCHES "^[1-9][0-9]*$")
		# unsupported/empty/0 on this platform -- skip the memory cap
		return()
	endif()

	math(EXPR mem_jobs "${mem_available} / ${mem_budget}")
	if(NOT mem_jobs LESS cores_jobs)
		# memory allows at least as many jobs as the core count already
		# does -- keep the core-count answer
		return()
	endif()

	if(mem_jobs LESS 1)
		set(mem_jobs 1)
	endif()
	set("${out_var}" "${mem_jobs}" PARENT_SCOPE)
	set("${out_reason}"
		"memory-limited: ${mem_available} MiB available / ${mem_budget} MiB per job; ${cores_jobs} by core count"
		PARENT_SCOPE)
endfunction()

# -D (or, standalone, the <requested> argument) beats the environment beats
# auto-detection (see _tau_resolve_auto_build_jobs above). requested="0" is
# a real value, not "unspecified": it means "auto-detect" explicitly and,
# per normal -D > env > default precedence, wins over the environment --
# it does NOT fall through to consult TAU_BUILD_JOBS from the environment.
# Only a genuinely absent/empty requested falls through to the environment,
# where (as before) a positive env value is used verbatim and a
# non-positive or unset one means auto. mem_budget_requested is the -D (or
# standalone <requested>) value for TAU_BUILD_JOB_MEMORY_MB; it's resolved
# the same way, via _tau_resolve_job_memory_mb, only if/when the auto-detect
# path actually needs it. out_reason is "" unless jobs came from
# auto-detection, in which case it explains which bound won.
function(_tau_resolve_build_jobs requested mem_budget_requested out_var out_reason)
	set(jobs "${requested}")
	set(reason "")
	if(NOT jobs MATCHES "^[1-9][0-9]*$")
		if(NOT requested STREQUAL "0" AND DEFINED ENV{TAU_BUILD_JOBS} AND "$ENV{TAU_BUILD_JOBS}" MATCHES "^[1-9][0-9]*$")
			set(jobs "$ENV{TAU_BUILD_JOBS}")
		else()
			_tau_resolve_job_memory_mb("${mem_budget_requested}" mem_budget)
			_tau_resolve_auto_build_jobs("${mem_budget}" jobs reason)
		endif()
	endif()
	set("${out_var}" "${jobs}" PARENT_SCOPE)
	set("${out_reason}" "${reason}" PARENT_SCOPE)
endfunction()

# -D (or, standalone, the <requested> argument) beats the environment beats
# ${TAU_BUILD_JOB_MEMORY_MB_DEFAULT}. Unlike TAU_BUILD_JOBS there is no
# "explicit auto" sentinel here: any non-empty requested or env value --
# valid or not -- is passed straight through as-is, and it's
# _tau_resolve_auto_build_jobs's job to treat a non-positive-integer budget
# as "disable the memory cap" (that's the escape hatch: an explicit 0 or
# garbage value, from either -D or the environment, must disable the cap
# rather than fall through to the default). Only a genuinely empty/absent
# value at every level falls through to the next one.
function(_tau_resolve_job_memory_mb requested out_var)
	set(budget "${requested}")
	if(budget STREQUAL "")
		if(DEFINED ENV{TAU_BUILD_JOB_MEMORY_MB} AND NOT "$ENV{TAU_BUILD_JOB_MEMORY_MB}" STREQUAL "")
			set(budget "$ENV{TAU_BUILD_JOB_MEMORY_MB}")
		else()
			set(budget "${TAU_BUILD_JOB_MEMORY_MB_DEFAULT}")
		endif()
	endif()
	set("${out_var}" "${budget}" PARENT_SCOPE)
endfunction()

# -D (or, standalone, the <requested> argument) beats the environment beats
# $HOME/.tau (or %USERPROFILE%\.tau on Windows when HOME is unset).
function(_tau_resolve_shared_prefix requested out_var)
	set(prefix "${requested}")
	if(prefix STREQUAL "")
		if(DEFINED ENV{TAU_SHARED_PREFIX} AND NOT "$ENV{TAU_SHARED_PREFIX}" STREQUAL "")
			set(prefix "$ENV{TAU_SHARED_PREFIX}")
		elseif(DEFINED ENV{HOME} AND NOT "$ENV{HOME}" STREQUAL "")
			set(prefix "$ENV{HOME}/.tau")
		elseif(DEFINED ENV{USERPROFILE} AND NOT "$ENV{USERPROFILE}" STREQUAL "")
			# Native Windows cmake has no HOME; Git bash sets one, cmd does not.
			file(TO_CMAKE_PATH "$ENV{USERPROFILE}/.tau" prefix)
		else()
			message(FATAL_ERROR
				"TAU_SHARED_PREFIX is empty and neither HOME nor USERPROFILE is set; "
				"set TAU_SHARED_PREFIX to the shared dependency prefix")
		endif()
	endif()
	set("${out_var}" "${prefix}" PARENT_SCOPE)
endfunction()

if(CMAKE_SCRIPT_MODE_FILE)
	# Standalone lookup for scripts/devrc. message() has no "plain stdout,
	# no prefix" mode: STATUS adds a leading "-- ", and every other mode
	# writes to stderr. `cmake -E echo` is the only portable plain-stdout
	# write. Do not use /dev/stdout here. A buildkit RUN step has no such
	# device and the open fails.
	if(CMAKE_ARGC LESS 4)
		message(FATAL_ERROR
			"usage: cmake -P tau-resolve.cmake <TAU_BUILD_JOBS|TAU_SHARED_PREFIX> [<requested>]")
	endif()
	set(_tau_resolve_var "${CMAKE_ARGV3}")
	set(_tau_resolve_requested "${CMAKE_ARGV4}")

	if(_tau_resolve_var STREQUAL "TAU_BUILD_JOBS")
		# _tau_resolve_reason is discarded here: stdout must stay exactly
		# the resolved value (see comment above), so the "why" only ever
		# gets printed by the include()d branch below. There's no -D/
		# <requested> equivalent for TAU_BUILD_JOB_MEMORY_MB in script
		# mode (no cache to hold one), so pass "" and let
		# _tau_resolve_job_memory_mb fall through to its environment
		# variable / default -- this is what makes that env var reachable
		# on the cmake -P / devrc path.
		_tau_resolve_build_jobs("${_tau_resolve_requested}" "" _tau_resolve_result _tau_resolve_reason)
	elseif(_tau_resolve_var STREQUAL "TAU_SHARED_PREFIX")
		_tau_resolve_shared_prefix("${_tau_resolve_requested}" _tau_resolve_result)
	else()
		message(FATAL_ERROR "tau-resolve: unknown variable '${_tau_resolve_var}'")
	endif()

	execute_process(COMMAND "${CMAKE_COMMAND}" -E echo "${_tau_resolve_result}")
else()
	# include()d from CMakeLists.txt (directly, or via cmake/use-emscripten.cmake
	# which needs TAU_SHARED_PREFIX_RESOLVED before project()): declare the
	# cache (user-intent) variables and resolve them into separate, uncached
	# variables that get recomputed on every configure.
	set(TAU_BUILD_JOBS_DOC
		"Number of jobs to use for the build (empty to auto-detect from the TAU_BUILD_JOBS env var, or half the cores, capped by TAU_BUILD_JOB_MEMORY_MB; 0 to force auto-detection, ignoring the TAU_BUILD_JOBS env var)")
	set(TAU_BUILD_JOBS "" CACHE STRING "${TAU_BUILD_JOBS_DOC}")
	set_property(CACHE TAU_BUILD_JOBS PROPERTY STRINGS
		"" "0" "1" "2" "3" "4" "5" "6" "7" "8" "9" "10"
		"11" "12" "13" "14" "15" "16" "17" "18" "19" "20")
	set(TAU_BUILD_JOB_MEMORY_MB_DOC
		"Memory budgeted per compile job in MiB, used only when auto-detecting TAU_BUILD_JOBS (empty to auto-detect from the TAU_BUILD_JOB_MEMORY_MB env var, or ${TAU_BUILD_JOB_MEMORY_MB_DEFAULT}; 0 or any other non-positive-integer value -- from either -D or the environment -- disables the memory cap entirely, leaving the core-count answer)")
	set(TAU_BUILD_JOB_MEMORY_MB "" CACHE STRING "${TAU_BUILD_JOB_MEMORY_MB_DOC}")
	# Passing the cache entry straight through here is what keeps a
	# changed TAU_BUILD_JOBS env value honoured across reconfigures of the
	# same build directory, as long as the cache entry itself is still
	# the empty default.
	_tau_resolve_build_jobs("${TAU_BUILD_JOBS}" "${TAU_BUILD_JOB_MEMORY_MB}" TAU_BUILD_JOBS_RESOLVED TAU_BUILD_JOBS_REASON)

	set(TAU_SHARED_PREFIX_DOC
		"Install prefix for shared dependencies (empty to auto-detect from the TAU_SHARED_PREFIX env var, or ~/.tau)")
	set(TAU_SHARED_PREFIX "" CACHE STRING "${TAU_SHARED_PREFIX_DOC}")
	_tau_resolve_shared_prefix("${TAU_SHARED_PREFIX}" TAU_SHARED_PREFIX_RESOLVED)

	unset(TAU_BUILD_JOBS_DOC)
	unset(TAU_BUILD_JOB_MEMORY_MB_DOC)
	unset(TAU_SHARED_PREFIX_DOC)
endif()
