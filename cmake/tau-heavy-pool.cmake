# The Ninja job pool of the heavy compiles: the targets whose translation units
# take 3 GB or more each with gcc -O3. Ninja starts them as soon as their inputs
# exist, so a parallel build would otherwise run several at once and set its
# peak memory by them alone. The pool keeps TAU_HEAVY_JOBS of them at a time
# while the light compiles fill the other slots. Other generators ignore it.
#
# TAU_HEAVY_JOBS: empty for half of TAU_BUILD_JOBS_RESOLVED (at least 1),
# 0 for no limit.
include_guard(GLOBAL)

set(TAU_HEAVY_JOBS "" CACHE STRING
	"Heavy compiles that run at once under Ninja (empty: half of TAU_BUILD_JOBS; 0: no limit)")

if(TAU_HEAVY_JOBS STREQUAL "")
	set(TAU_HEAVY_JOBS_RESOLVED 0)
	if(TAU_BUILD_JOBS_RESOLVED MATCHES "^[0-9]+$" AND TAU_BUILD_JOBS_RESOLVED GREATER 0)
		math(EXPR TAU_HEAVY_JOBS_RESOLVED "${TAU_BUILD_JOBS_RESOLVED} / 2")
		if(TAU_HEAVY_JOBS_RESOLVED LESS 1)
			set(TAU_HEAVY_JOBS_RESOLVED 1)
		endif()
	endif()
elseif(TAU_HEAVY_JOBS MATCHES "^[0-9]+$")
	set(TAU_HEAVY_JOBS_RESOLVED ${TAU_HEAVY_JOBS})
else()
	message(FATAL_ERROR "TAU_HEAVY_JOBS must be empty or a number, not '${TAU_HEAVY_JOBS}'")
endif()

if(TAU_HEAVY_JOBS_RESOLVED GREATER 0)
	set_property(GLOBAL APPEND PROPERTY JOB_POOLS tau_heavy=${TAU_HEAVY_JOBS_RESOLVED})
	message(STATUS "TAU_HEAVY_JOBS: ${TAU_HEAVY_JOBS_RESOLVED}")
else()
	message(STATUS "TAU_HEAVY_JOBS: no limit")
endif()

# tau_heavy_pool(<target> [LINK]): compile <target> in the pool, and with LINK
# link it there too (an LTO link is as heavy as a compile).
function(tau_heavy_pool target)
	if(NOT TAU_HEAVY_JOBS_RESOLVED GREATER 0)
		return()
	endif()
	set_property(TARGET ${target} PROPERTY JOB_POOL_COMPILE tau_heavy)
	if("LINK" IN_LIST ARGN)
		set_property(TARGET ${target} PROPERTY JOB_POOL_LINK tau_heavy)
	endif()
endfunction()
