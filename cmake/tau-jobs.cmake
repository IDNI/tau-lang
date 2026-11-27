# The parallelism of a build and its tests, from the cores, the memory and the
# compiler:
#
#   build jobs  the compile and link jobs Ninja runs at once (TAU_BUILD_JOBS)
#   heavy jobs  how many of them may be heavy compiles (TAU_HEAVY_JOBS, the
#               depth of the tau_heavy pool, cmake/tau-heavy-pool.cmake)
#   test jobs   the tests ctest runs at once (TAU_TEST_JOBS)
#   compile-test slots
#               how many tests that build an emitted artifact run at once:
#               each counts as test jobs / slots of ctest's slots (PROCESSORS)
#
# The jobs may use the logical cores less TAU_HOST_RESERVE_CORES (default 4
# under WSL, whose VM holds every thread of the Windows host, else 0), at most
# TAU_JOBS_MAX, and the available memory (MemAvailable; on macOS the free,
# speculative and inactive pages of vm_stat) less a tenth, at least 2 GiB.
#
# The peak memory of a build of J jobs, H of them heavy, is taken as the lower
# of two bounds, in MiB:
#
#   every job at its worst   H * TAU_HEAVY_JOB_MEMORY_MB + (J - H) * TAU_LIGHT_JOB_MEMORY_MB
#   measured on 20-32 jobs   TAU_BUILD_PEAK_BASE_MB + J * TAU_BUILD_PEAK_JOB_MB  (12000 + 750 J)
#
# The worst translation units take 6000 MiB with gcc and 2800 MiB with clang,
# the light ones 2000 and 1500, but the peaks of different units do not
# coincide: with many jobs the second bound, the same for both compilers, is
# the lower one. The build jobs are the most that fit with one heavy job, and
# the heavy jobs the most that fit beside them, up to half of them. The test
# jobs are as many as fit at TAU_TEST_JOB_MEMORY_MB each, up to the cores the
# jobs may use. A test that builds an artifact takes
# TAU_COMPILE_TEST_MEMORY_MB and TAU_COMPILE_TEST_CORES of the logical cores:
# gcc's -flto=auto link runs on several (8, 3500 MiB), clang's on one (1,
# 1700 MiB). They take at most half of the test jobs. The compiler is CMAKE_CXX_COMPILER_ID at configure; TAU_JOBS_COMPILER
# (gcc or clang) overrides it, and in script mode, where there is none, the
# basename of $CXX decides, gcc when it is not clang.
#
# TAU_JOBS_CORES and TAU_BUILD_MEMORY_MB replace the detected logical cores
# and available memory, to compute the values of another machine.
#
# A value given for any of the three (-D or the environment) is kept and the
# others are computed around it; a build that fits nowhere gets one job, and
# its heavy jobs one. Every input also reads the environment variable of its
# name.
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

# gcc or clang: whose per-job figures apply
function(_tau_jobs_compiler out)
	_tau_jobs_input(TAU_JOBS_COMPILER "" compiler)
	if(compiler STREQUAL "")
		if(CMAKE_CXX_COMPILER_ID)
			set(id "${CMAKE_CXX_COMPILER_ID}")
		elseif(DEFINED ENV{CXX})
			get_filename_component(id "$ENV{CXX}" NAME)
		else()
			set(id "")
		endif()
		# Clang, AppleClang, clang-cl, emcc; GNU, MSVC and an unknown one take
		# the gcc figures, the larger
		if(id MATCHES "[Cc]lang|^em")
			set(compiler clang)
		else()
			set(compiler gcc)
		endif()
	endif()
	if(NOT compiler MATCHES "^(gcc|clang)$")
		message(FATAL_ERROR "TAU_JOBS_COMPILER must be gcc or clang, not '${compiler}'")
	endif()
	set(${out} ${compiler} PARENT_SCOPE)
endfunction()

# The logical cores, the jobs the host reserve and TAU_JOBS_MAX leave of them,
# the memory and the part of it the jobs may use, in MiB.
function(_tau_jobs_machine out_cores out_max out_mem out_budget)
	_tau_jobs_input(TAU_JOBS_CORES "" cores)
	if(NOT cores MATCHES "^[1-9][0-9]*$")
		include(ProcessorCount)
		ProcessorCount(cores)
	endif()
	if(NOT cores GREATER 0)
		set(cores 1)
	endif()
	_tau_jobs_input(TAU_HOST_RESERVE_CORES "" host_reserve)
	if(host_reserve STREQUAL "")
		set(host_reserve 0)
		if(EXISTS "/proc/sys/kernel/osrelease")
			file(READ "/proc/sys/kernel/osrelease" osrelease)
			if(osrelease MATCHES "[Mm]icrosoft")
				set(host_reserve 4)
			endif()
		endif()
	elseif(NOT host_reserve MATCHES "^[0-9]+$")
		message(FATAL_ERROR "TAU_HOST_RESERVE_CORES must be a number, not '${host_reserve}'")
	endif()
	math(EXPR max_jobs "${cores} - ${host_reserve}")
	if(max_jobs LESS 1)
		set(max_jobs 1)
	endif()
	_tau_jobs_input(TAU_JOBS_MAX "" cap)
	_tau_jobs_positive("${cap}" has_cap)
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

# The peak MiB of <j> build jobs with <h> heavy ones, the lower of the two
# bounds in the header.
function(_tau_jobs_peak j h heavy_mb light_mb base_mb job_mb out)
	math(EXPR worst "${h} * ${heavy_mb} + (${j} - ${h}) * ${light_mb}")
	math(EXPR measured "${base_mb} + ${j} * ${job_mb}")
	if(measured LESS worst)
		set(worst ${measured})
	endif()
	set(${out} ${worst} PARENT_SCOPE)
endfunction()

# tau_compute_jobs(<build> <heavy> <test> <out_build> <out_heavy> <out_test> <out_reason>)
# <build>, <heavy> and <test> are the requested values, "" to compute; a
# requested heavy of 0 means no pool, and is returned as 0.
function(tau_compute_jobs req_build req_heavy req_test out_build out_heavy out_test out_reason)
	_tau_jobs_machine(cores max_jobs mem budget)
	_tau_jobs_compiler(compiler)
	if(compiler STREQUAL "clang")
		set(heavy_default 2800)
		set(light_default 1500)
	else()
		set(heavy_default 6000)
		set(light_default 2000)
	endif()
	_tau_jobs_input(TAU_HEAVY_JOB_MEMORY_MB ${heavy_default} heavy_mb)
	_tau_jobs_input(TAU_LIGHT_JOB_MEMORY_MB ${light_default} light_mb)
	_tau_jobs_input(TAU_BUILD_PEAK_BASE_MB 12000 base_mb)
	_tau_jobs_input(TAU_BUILD_PEAK_JOB_MB 750 job_mb)
	_tau_jobs_input(TAU_TEST_JOB_MEMORY_MB 1000 test_mb)

	_tau_jobs_positive("${req_build}" fixed_build)
	_tau_jobs_positive("${req_heavy}" fixed_heavy)
	set(no_pool FALSE)
	if("${req_heavy}" STREQUAL "0")
		set(no_pool TRUE)
	endif()

	# the build jobs: the most that fit with the fewest heavy jobs allowed
	set(fits TRUE)
	if(fixed_build)
		set(best_j ${req_build})
	else()
		set(best_j 0)
		foreach(j RANGE 1 ${max_jobs})
			set(h 1)
			if(fixed_heavy)
				set(h ${req_heavy})
			elseif(no_pool)
				set(h ${j})
			endif()
			if(h GREATER j)
				set(h ${j})
			endif()
			_tau_jobs_peak(${j} ${h} ${heavy_mb} ${light_mb} ${base_mb} ${job_mb} need)
			if(NOT need GREATER budget)
				set(best_j ${j})
			endif()
		endforeach()
		if(best_j EQUAL 0)
			set(best_j 1)
			set(fits FALSE)
		endif()
	endif()

	# the heavy jobs: the most that fit beside them, up to half of them
	if(no_pool)
		set(best_h 0)
	elseif(fixed_heavy)
		set(best_h ${req_heavy})
	else()
		math(EXPR h_hi "(${best_j} + 1) / 2")
		set(best_h 0)
		foreach(h RANGE 1 ${h_hi})
			_tau_jobs_peak(${best_j} ${h} ${heavy_mb} ${light_mb} ${base_mb} ${job_mb} need)
			if(NOT need GREATER budget)
				set(best_h ${h})
			endif()
		endforeach()
		if(best_h EQUAL 0)
			set(best_h 1)
			set(fits FALSE)
		endif()
	endif()

	if(fits)
		set(reason "${budget} of ${mem} MiB, ${cores} cores; ${compiler}")
	elseif(fixed_build)
		set(reason "${budget} MiB fit no heavy job of ${heavy_mb} MiB beside ${req_build} build jobs; ${compiler}")
	else()
		set(reason "${budget} MiB fit no heavy job of ${heavy_mb} MiB; ${compiler}")
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
# the artifact-building tests that fit at once in the logical cores and the
# memory, at most half of <test_jobs> and at least 1, and the ctest slots (PROCESSORS)
# each must count as so that -j <test_jobs> runs no more of them.
function(tau_compute_compile_test_slots test_jobs out_slots out_processors)
	_tau_jobs_machine(cores max_jobs mem budget)
	_tau_jobs_compiler(compiler)
	if(compiler STREQUAL "clang")
		set(cores_default 1)
		set(mb_default 1700)
	else()
		set(cores_default 8)
		set(mb_default 3500)
	endif()
	_tau_jobs_input(TAU_COMPILE_TEST_MEMORY_MB ${mb_default} compile_mb)
	_tau_jobs_input(TAU_COMPILE_TEST_CORES ${cores_default} compile_cores)
	math(EXPR slots "${cores} / ${compile_cores}")
	math(EXPR mem_slots "${budget} / ${compile_mb}")
	if(mem_slots LESS slots)
		set(slots ${mem_slots})
	endif()
	# half of the test jobs at most: the rest of the suite keeps the other half
	math(EXPR half "${test_jobs} / 2")
	if(slots GREATER half)
		set(slots ${half})
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
