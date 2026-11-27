# The parallelism of a build and its tests, from the cores and the memory of
# the machine:
#
#   build jobs  the compile and link jobs Ninja runs at once (TAU_BUILD_JOBS)
#   heavy jobs  how many of them may be heavy compiles (TAU_HEAVY_JOBS, the
#               depth of the tau_heavy pool, cmake/tau-heavy-pool.cmake)
#   test jobs   the tests ctest runs at once (TAU_TEST_JOBS)
#   compile-test slots
#               how many tests that build an emitted artifact run at once:
#               each counts as test jobs / slots of ctest's slots (PROCESSORS)
#
# A heavy compile takes TAU_HEAVY_JOB_MEMORY_MB, a light one
# TAU_LIGHT_JOB_MEMORY_MB and a test TAU_TEST_JOB_MEMORY_MB (defaults measured
# with gcc 13 -O3: libTAU's and the heaviest suites' TUs peak at 5 to 7 GB, the
# 90th percentile of the others at 2.3 GB, and a test at 3.2 GB at most and
# almost always far less). The heavy compiles are TAU_HEAVY_SHARE percent of the
# build's CPU time. A test that builds an artifact takes
# TAU_COMPILE_TEST_MEMORY_MB and, while its -flto=auto link runs, about
# TAU_COMPILE_TEST_CORES cores. Of the available memory (MemAvailable; on
# macOS the free, speculative and inactive pages of vm_stat), a tenth (at least
# 2 GiB) is kept back.
#
# The build and heavy jobs are the pair that fits in the memory with the
# shortest estimated time, max(cpu / build_jobs, cpu * share / heavy_jobs); a
# tie goes to the pair with less memory. The test jobs are as many as fit, up
# to the core count. TAU_JOBS_MAX caps the build and the test jobs, for a
# machine that other builds share. A value given for any of the three (-D or
# the environment) is kept and the others are computed around it; a build that
# fits nowhere gets one job. Every input also reads the environment variable of
# its name.
#
# include()d after tau-shared-resolve, it computes TAU_HEAVY_JOBS_RESOLVED,
# TAU_TEST_JOBS_RESOLVED and TAU_COMPILE_TEST_PROCESSORS from
# TAU_BUILD_JOBS_RESOLVED. As a script it prints "<build> <heavy> <test>" for
# scripts/preset.sh:
#
#   cmake [-DTAU_BUILD_JOBS=N] [-DTAU_HEAVY_JOBS=N] [-DTAU_TEST_JOBS=N] -P cmake/tau-jobs.cmake
include_guard(GLOBAL)
cmake_policy(PUSH)
cmake_policy(VERSION 3.10...3.31)

# -D, else the environment, else <default>
function(_tau_jobs_input name default out)
	if(DEFINED ${name} AND NOT "${${name}}" STREQUAL "")
		set(${out} "${${name}}" PARENT_SCOPE)
	elseif(DEFINED ENV{${name}} AND NOT "$ENV{${name}}" STREQUAL "")
		set(${out} "$ENV{${name}}" PARENT_SCOPE)
	else()
		set(${out} "${default}" PARENT_SCOPE)
	endif()
endfunction()

function(_tau_jobs_positive value out)
	if("${value}" MATCHES "^[0-9]+$" AND value GREATER 0)
		set(${out} TRUE PARENT_SCOPE)
	else()
		set(${out} FALSE PARENT_SCOPE)
	endif()
endfunction()

# The MiB that vm_stat's <text> shows free or reclaimable: the free, the
# speculative and the inactive pages ("Pages free" leaves the speculative ones
# out), "" when <text> is not vm_stat output.
function(_tau_jobs_vm_stat_mb text out)
	set(mb "")
	if(text MATCHES "page size of ([0-9]+) bytes")
		set(page ${CMAKE_MATCH_1})
		set(pages 0)
		set(found 0)
		foreach(kind free speculative inactive)
			if(text MATCHES "Pages ${kind}: +([0-9]+)")
				math(EXPR pages "${pages} + ${CMAKE_MATCH_1}")
				math(EXPR found "${found} + 1")
			endif()
		endforeach()
		if(found EQUAL 3)
			math(EXPR mb "${pages} * ${page} / 1048576")
		endif()
	endif()
	set(${out} "${mb}" PARENT_SCOPE)
endfunction()

# The logical cores, the jobs TAU_JOBS_MAX leaves of them, the memory and the
# part of it the jobs may use, in MiB.
function(_tau_jobs_machine out_cores out_max out_mem out_budget)
	include(ProcessorCount)
	ProcessorCount(cores)
	if(NOT cores GREATER 0)
		set(cores 1)
	endif()
	_tau_jobs_input(TAU_JOBS_MAX "" cap)
	_tau_jobs_positive("${cap}" has_cap)
	set(max_jobs ${cores})
	if(has_cap AND cap LESS max_jobs)
		set(max_jobs ${cap})
	endif()

	_tau_jobs_input(TAU_BUILD_MEMORY_MB "" mem)
	if(NOT mem MATCHES "^[1-9][0-9]*$")
		cmake_host_system_information(RESULT mem QUERY AVAILABLE_PHYSICAL_MEMORY)
		# On macOS CMake counts the free pages only, while the system keeps
		# most of the memory as inactive pages it hands back on demand: a 7 GB
		# runner reads as 1.4 GB.
		cmake_host_system_information(RESULT os QUERY OS_NAME)
		if(CMAKE_HOST_APPLE OR os MATCHES "^(macOS|Darwin)$")
			execute_process(COMMAND vm_stat OUTPUT_VARIABLE vm RESULT_VARIABLE rc ERROR_QUIET)
			if(rc EQUAL 0)
				_tau_jobs_vm_stat_mb("${vm}" vm_mb)
				if(vm_mb MATCHES "^[1-9][0-9]*$" AND (NOT mem MATCHES "^[0-9]+$" OR vm_mb GREATER mem))
					set(mem ${vm_mb})
				endif()
			endif()
		endif()
	endif()
	if(NOT mem MATCHES "^[1-9][0-9]*$")
		# unknown memory: the core count alone decides
		math(EXPR mem "1 << 30")
	endif()
	math(EXPR reserve "${mem} / 10")
	if(reserve LESS 2048)
		set(reserve 2048)
	endif()
	math(EXPR budget "${mem} - ${reserve}")
	if(budget LESS 0)
		set(budget 0)
	endif()
	set(${out_cores} ${cores} PARENT_SCOPE)
	set(${out_max} ${max_jobs} PARENT_SCOPE)
	set(${out_mem} ${mem} PARENT_SCOPE)
	set(${out_budget} ${budget} PARENT_SCOPE)
endfunction()

# tau_compute_jobs(<build> <heavy> <test> <out_build> <out_heavy> <out_test> <out_reason>)
# <build>, <heavy> and <test> are the requested values, "" to compute; a
# requested heavy of 0 means no pool, and is returned as 0.
function(tau_compute_jobs req_build req_heavy req_test out_build out_heavy out_test out_reason)
	_tau_jobs_machine(cores max_jobs mem budget)
	_tau_jobs_input(TAU_HEAVY_JOB_MEMORY_MB 6000 heavy_mb)
	_tau_jobs_input(TAU_LIGHT_JOB_MEMORY_MB 2000 light_mb)
	_tau_jobs_input(TAU_TEST_JOB_MEMORY_MB 1000 test_mb)
	_tau_jobs_input(TAU_HEAVY_SHARE 45 share)

	_tau_jobs_positive("${req_build}" fixed_build)
	_tau_jobs_positive("${req_heavy}" fixed_heavy)
	set(no_pool FALSE)
	if("${req_heavy}" STREQUAL "0")
		set(no_pool TRUE)
	endif()

	set(best_t -1)
	set(best_mem 0)
	set(best_j 1)
	set(best_h 1)
	set(j_lo 1)
	set(j_hi ${max_jobs})
	if(fixed_build)
		set(j_lo ${req_build})
		set(j_hi ${req_build})
	endif()
	foreach(j RANGE ${j_lo} ${j_hi})
		set(h_lo 1)
		set(h_hi ${j})
		if(fixed_heavy)
			if(req_heavy GREATER j)
				continue()
			endif()
			set(h_lo ${req_heavy})
			set(h_hi ${req_heavy})
		elseif(no_pool)
			# without a pool every job may be a heavy one
			set(h_lo ${j})
		endif()
		foreach(h RANGE ${h_lo} ${h_hi})
			math(EXPR need "${h} * ${heavy_mb} + (${j} - ${h}) * ${light_mb}")
			if(need GREATER budget)
				continue()
			endif()
			math(EXPR t_all "1000000 / ${j}")
			math(EXPR t_heavy "10000 * ${share} / ${h}")
			set(t ${t_all})
			if(t_heavy GREATER t)
				set(t ${t_heavy})
			endif()
			if(best_t LESS 0 OR t LESS best_t OR (t EQUAL best_t AND need LESS best_mem))
				set(best_t ${t})
				set(best_mem ${need})
				set(best_j ${j})
				set(best_h ${h})
			endif()
		endforeach()
	endforeach()

	if(best_t LESS 0)
		# nothing fits: one job, or the requested ones
		set(best_j 1)
		if(fixed_build)
			set(best_j ${req_build})
		endif()
		set(best_h 1)
		if(fixed_heavy)
			set(best_h ${req_heavy})
		endif()
		if(fixed_build)
			set(reason "${budget} MiB fit no heavy job of ${heavy_mb} MiB beside ${req_build} build jobs")
		else()
			set(reason "${budget} MiB fit no heavy job of ${heavy_mb} MiB")
		endif()
	else()
		set(reason "${budget} of ${mem} MiB, ${cores} cores; ${heavy_mb}/${light_mb} MiB per heavy/light job")
	endif()
	if(no_pool)
		set(best_h 0)
	endif()

	_tau_jobs_positive("${req_test}" fixed_test)
	if(fixed_test)
		set(tests ${req_test})
	else()
		math(EXPR tests "${budget} / ${test_mb}")
		if(tests GREATER max_jobs)
			set(tests ${max_jobs})
		endif()
		if(tests LESS 1)
			set(tests 1)
		endif()
	endif()
	if(NOT max_jobs EQUAL cores)
		string(APPEND reason "; capped at ${max_jobs}")
	endif()

	set(${out_build} ${best_j} PARENT_SCOPE)
	set(${out_heavy} ${best_h} PARENT_SCOPE)
	set(${out_test} ${tests} PARENT_SCOPE)
	set(${out_reason} "${reason}" PARENT_SCOPE)
endfunction()

# tau_compute_compile_test_slots(<test_jobs> <out_slots> <out_processors>):
# the artifact-building tests that fit at once in the cores and the memory,
# at most <test_jobs> and at least 1, and the ctest slots (PROCESSORS) each
# must count as so that -j <test_jobs> runs no more of them.
function(tau_compute_compile_test_slots test_jobs out_slots out_processors)
	_tau_jobs_machine(cores max_jobs mem budget)
	_tau_jobs_input(TAU_COMPILE_TEST_MEMORY_MB 3500 compile_mb)
	_tau_jobs_input(TAU_COMPILE_TEST_CORES 8 compile_cores)
	math(EXPR slots "${cores} / ${compile_cores}")
	math(EXPR mem_slots "${budget} / ${compile_mb}")
	if(mem_slots LESS slots)
		set(slots ${mem_slots})
	endif()
	if(slots GREATER test_jobs)
		set(slots ${test_jobs})
	endif()
	if(slots LESS 1)
		set(slots 1)
	endif()
	math(EXPR processors "${test_jobs} / ${slots}")
	if(processors LESS 1)
		set(processors 1)
	endif()
	set(${out_slots} ${slots} PARENT_SCOPE)
	set(${out_processors} ${processors} PARENT_SCOPE)
endfunction()

if(CMAKE_SCRIPT_MODE_FILE)
	_tau_jobs_input(TAU_BUILD_JOBS "" _b)
	_tau_jobs_input(TAU_HEAVY_JOBS "" _h)
	_tau_jobs_input(TAU_TEST_JOBS "" _t)
	tau_compute_jobs("${_b}" "${_h}" "${_t}" _b _h _t _reason)
	execute_process(COMMAND "${CMAKE_COMMAND}" -E echo "${_b} ${_h} ${_t}")
	if(TAU_JOBS_VERBOSE)
		tau_compute_compile_test_slots(${_t} _slots _procs)
		message(STATUS "tau-jobs: ${_reason}; ${_slots} compile tests at once (PROCESSORS ${_procs})")
	endif()
else()
	set(TAU_HEAVY_JOBS "" CACHE STRING
		"Heavy compiles that run at once under Ninja (empty: computed from the memory and TAU_BUILD_JOBS; 0: no limit)")
	set(TAU_TEST_JOBS "" CACHE STRING
		"Tests ctest runs at once (empty: computed from the memory and the cores)")
	# the cache value, else the environment variable, as the script mode reads them
	_tau_jobs_input(TAU_HEAVY_JOBS "" _tau_heavy_req)
	_tau_jobs_input(TAU_TEST_JOBS "" _tau_test_req)
	if(NOT _tau_heavy_req MATCHES "^[0-9]*$")
		message(FATAL_ERROR "TAU_HEAVY_JOBS must be empty or a number, not '${_tau_heavy_req}'")
	endif()
	tau_compute_jobs("${TAU_BUILD_JOBS_RESOLVED}" "${_tau_heavy_req}" "${_tau_test_req}"
		_tau_build_jobs TAU_HEAVY_JOBS_RESOLVED TAU_TEST_JOBS_RESOLVED _tau_jobs_reason)
	tau_compute_compile_test_slots(${TAU_TEST_JOBS_RESOLVED}
		TAU_COMPILE_TEST_SLOTS TAU_COMPILE_TEST_PROCESSORS)
	message(STATUS "Jobs: build ${TAU_BUILD_JOBS_RESOLVED}, heavy ${TAU_HEAVY_JOBS_RESOLVED}, "
		"test ${TAU_TEST_JOBS_RESOLVED}, compile tests ${TAU_COMPILE_TEST_SLOTS} at once "
		"(${_tau_jobs_reason})")
	unset(_tau_build_jobs)
	unset(_tau_jobs_reason)
	unset(_tau_heavy_req)
	unset(_tau_test_req)
endif()

cmake_policy(POP)
