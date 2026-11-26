# The Ninja job pool of the heavy compiles: the targets whose translation units
# take 3 GB or more each with gcc -O3. Ninja starts them as soon as their inputs
# exist, so a parallel build would otherwise run several at once and set its
# peak memory by them alone. The pool keeps TAU_HEAVY_JOBS of them at a time
# while the light compiles fill the other slots. Other generators ignore it.
#
# The depth is TAU_HEAVY_JOBS_RESOLVED (cmake/tau-jobs.cmake); 0 means no pool.
include_guard(GLOBAL)
include(tau-jobs)

if(TAU_HEAVY_JOBS_RESOLVED GREATER 0)
	set_property(GLOBAL APPEND PROPERTY JOB_POOLS tau_heavy=${TAU_HEAVY_JOBS_RESOLVED})
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
