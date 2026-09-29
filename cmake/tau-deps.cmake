# cmake/tau-deps.cmake
# Store packages are content-addressed by the preset's compiler and flags; a
# missing one is built from the preset, never a hand-picked mode.

include_guard(GLOBAL)

# The store module owns the on-disk layout and the eviction rules; this file only
# decides when a producer runs.
include("${CMAKE_CURRENT_LIST_DIR}/../external/parser/cmake/tau-store.cmake")

# Compiler and flag arguments every producer receives, so no producer reads a
# compiler from the environment. The values are the preset's build-type flags,
# so switching the build type selects a different package. An optional target
# names the store target of the package, and defaults to TAU_DEPS_TARGET.
function(_tau_deps_toolchain_args out)
	set(_target "${TAU_DEPS_TARGET}")
	if(ARGC GREATER 1)
		set(_target "${ARGV1}")
	endif()
	string(TOUPPER "${CMAKE_BUILD_TYPE}" _build_type)
	set(_cflags_var "CMAKE_C_FLAGS_${_build_type}")
	set(_cxxflags_var "CMAKE_CXX_FLAGS_${_build_type}")
	set(_args
		"-DTAU_DEP_CC=${CMAKE_C_COMPILER}"
		"-DTAU_DEP_CXX=${CMAKE_CXX_COMPILER}"
		"-DTAU_DEP_CFLAGS=${CMAKE_C_FLAGS} ${${_cflags_var}}"
		"-DTAU_DEP_CXXFLAGS=${CMAKE_CXX_FLAGS} ${${_cxxflags_var}}"
		"-DTAU_DEP_TARGET=${_target}")
	# A cross toolchain travels as a file; macOS and MSVC build with the host
	# compiler the preset picked and pass none.
	if(CMAKE_TOOLCHAIN_FILE)
		list(APPEND _args "-DTAU_DEP_TOOLCHAIN=${CMAKE_TOOLCHAIN_FILE}")
	endif()
	set(${out} "${_args}" PARENT_SCOPE)
endfunction()

# A cross-toolchain target builds the host tgf natively, because the target
# compiler cannot produce a binary this build host can run. A native arm64
# build is not cross and keeps the target compiler.
function(_tau_deps_target_is_cross_toolchain out)
	if(CMAKE_CROSSCOMPILING
			OR TAU_DEPS_TARGET STREQUAL "windows-x86_64-mingw"
			OR TAU_DEPS_TARGET STREQUAL "wasm32-emscripten")
		set(${out} TRUE PARENT_SCOPE)
	else()
		set(${out} FALSE PARENT_SCOPE)
	endif()
endfunction()

# The host tool tgf must run on the build machine, so it is built with native
# clang and the build type's own flags; the target's global flags may carry
# wasm-only options native clang rejects.
function(_tau_deps_host_toolchain_args out)
	string(TOUPPER "${CMAKE_BUILD_TYPE}" _build_type)
	set(_cflags_var "CMAKE_C_FLAGS_${_build_type}")
	set(_cxxflags_var "CMAKE_CXX_FLAGS_${_build_type}")
	set(_args
		"-DTAU_DEP_CC=${TAU_HOST_C_COMPILER}"
		"-DTAU_DEP_CXX=${TAU_HOST_CXX_COMPILER}"
		"-DTAU_DEP_CFLAGS=${${_cflags_var}}"
		"-DTAU_DEP_CXXFLAGS=${${_cxxflags_var}}")
	set(${out} "${_args}" PARENT_SCOPE)
endfunction()

# The command that runs a target producer with the flag environment cleared and
# this configure's compiler and flags. Callers append package-specific options.
# An optional target overrides the store target, as in _tau_deps_toolchain_args.
function(tau_deps_producer_command out script)
	if(NOT TAU_BASH)
		message(FATAL_ERROR "running a dependency producer needs bash")
	endif()
	_tau_deps_toolchain_args(_toolchain ${ARGN})
	# Git Bash rewrites an argument that starts with / as a path, which mangles
	# the /D flags in TAU_DEP_CFLAGS when a producer starts the nested cmake.
	# Exclude the flag arguments only: a real path, such as the toolchain file,
	# must still convert.
	# A producer resolves its own job count as -D > env > auto, so hand it this
	# configure's resolved value as the -D the environment cannot shadow.
	set(${out}
		"${CMAKE_COMMAND}" -E env
			--unset=CPPFLAGS --unset=CFLAGS --unset=CXXFLAGS --unset=LDFLAGS
			"MSYS2_ARG_CONV_EXCL=-DTAU_DEP_CFLAGS=\;-DTAU_DEP_CXXFLAGS=\;-DCMAKE_C_FLAGS=\;-DCMAKE_CXX_FLAGS="
			"TAU_SHARED_PREFIX=${TAU_SHARED_PREFIX_RESOLVED}"
			"TAU_BUILD_JOBS=${TAU_BUILD_JOBS_RESOLVED}"
			"${TAU_BASH}" "${script}" ${_toolchain}
				"-DTAU_BUILD_JOBS=${TAU_BUILD_JOBS_RESOLVED}"
		PARENT_SCOPE)
endfunction()

function(tau_deps_host_producer_command out script)
	if(NOT TAU_BASH)
		message(FATAL_ERROR "running a dependency producer needs bash")
	endif()
	_tau_deps_host_toolchain_args(_toolchain)
	# Git Bash rewrites an argument that starts with / as a path, which mangles
	# the /D flags in TAU_DEP_CFLAGS when a producer starts the nested cmake.
	# Exclude the flag arguments only: a real path, such as the toolchain file,
	# must still convert.
	set(${out}
		"${CMAKE_COMMAND}" -E env
			--unset=CPPFLAGS --unset=CFLAGS --unset=CXXFLAGS --unset=LDFLAGS
			"MSYS2_ARG_CONV_EXCL=-DTAU_DEP_CFLAGS=\;-DTAU_DEP_CXXFLAGS=\;-DCMAKE_C_FLAGS=\;-DCMAKE_CXX_FLAGS="
			"TAU_SHARED_PREFIX=${TAU_SHARED_PREFIX_RESOLVED}"
			"TAU_BUILD_JOBS=${TAU_BUILD_JOBS_RESOLVED}"
			"${TAU_BASH}" "${script}" ${_toolchain}
				"-DTAU_BUILD_JOBS=${TAU_BUILD_JOBS_RESOLVED}"
		PARENT_SCOPE)
endfunction()

# Run one producer and set <out_var> to its printed prefix. Extra arguments are
# appended to the producer command line. A leading OPTIONAL turns a producer
# failure into a warning and an empty <out_var>.
function(_tau_deps_run_producer dep producer_cmd out_var)
	set(_args ${ARGN})
	set(_optional FALSE)
	if(_args)
		list(GET _args 0 _first)
		if(_first STREQUAL "OPTIONAL")
			set(_optional TRUE)
			list(REMOVE_AT _args 0)
		endif()
	endif()
	execute_process(
		COMMAND ${producer_cmd} ${_args}
		RESULT_VARIABLE _rc
		OUTPUT_VARIABLE _out
		ERROR_VARIABLE _err)
	if(NOT _rc EQUAL 0)
		if(_optional)
			message(WARNING "cannot resolve the optional ${dep} package.\n${_err}\n${_out}")
			set(${out_var} "" PARENT_SCOPE)
			return()
		endif()
		message(FATAL_ERROR "cannot resolve the ${dep} package.\n${_err}\n${_out}")
	endif()
	if(NOT _out MATCHES "package prefix: (.*)")
		message(FATAL_ERROR "no prefix in the ${dep} producer output:\n${_out}")
	endif()
	set(_prefix "${CMAKE_MATCH_1}")
	string(STRIP "${_prefix}" _prefix)
	if(NOT IS_DIRECTORY "${_prefix}")
		message(FATAL_ERROR "the ${dep} prefix is not a directory: '${_prefix}'")
	endif()
	get_filename_component(_entry "${_prefix}" DIRECTORY)
	tau_store_use("${_entry}")
	set(${out_var} "${_prefix}" PARENT_SCOPE)
endfunction()

function(tau_deps_ensure_prefix dep script out_prefix)
	tau_deps_producer_command(_cmd "${script}")
	_tau_deps_run_producer("${dep}" "${_cmd}" _prefix ${ARGN})
	set(${out_prefix} "${_prefix}" PARENT_SCOPE)
endfunction()

# As tau_deps_ensure_prefix, but the package takes the store id of <target>.
function(tau_deps_ensure_prefix_as dep script target out_prefix)
	tau_deps_producer_command(_cmd "${script}" "${target}")
	_tau_deps_run_producer("${dep}" "${_cmd}" _prefix ${ARGN})
	set(${out_prefix} "${_prefix}" PARENT_SCOPE)
endfunction()

function(tau_deps_ensure_host_prefix dep script out_prefix)
	tau_deps_host_producer_command(_cmd "${script}")
	_tau_deps_run_producer("${dep}" "${_cmd}" _prefix ${ARGN})
	set(${out_prefix} "${_prefix}" PARENT_SCOPE)
endfunction()

# Resolve the parser-side dependencies in order and publish the prefixes Tau
# needs. unordered_dense and ftxui first: the parser SDK id embeds their ids.
function(tau_deps_resolve_parser)
	set(_scripts "${PROJECT_SOURCE_DIR}/external/parser/scripts")
	set(_parser "${_scripts}/dep-parser-sdk.sh")
	tau_deps_ensure_prefix(unordered_dense
		"${_scripts}/dep-unordered-dense.sh" TAU_UNORDERED_DENSE_PREFIX)
	tau_deps_ensure_prefix(ftxui
		"${_scripts}/dep-ftxui.sh" TAU_FTXUI_PREFIX)
	list(APPEND CMAKE_PREFIX_PATH "${TAU_FTXUI_PREFIX}")
	_tau_deps_target_is_cross_toolchain(_cross)
	if(_cross)
		find_program(TAU_HOST_C_COMPILER clang NO_CACHE)
		find_program(TAU_HOST_CXX_COMPILER clang++ NO_CACHE)
		if(NOT TAU_HOST_C_COMPILER OR NOT TAU_HOST_CXX_COMPILER)
			message(FATAL_ERROR "the native host tgf needs clang and clang++")
		endif()
	endif()
	set(_parser_args "-DTAU_PARSER_PACKAGE=sdk"
		"-DTAU_PARSER_LTO=${TAU_LTO}" "-DTAU_PARSER_LTO_FAT=ON")
	if(_cross)
		list(APPEND _parser_args
			"-DTAU_PARSER_HOST_C_COMPILER=${TAU_HOST_C_COMPILER}"
			"-DTAU_PARSER_HOST_CXX_COMPILER=${TAU_HOST_CXX_COMPILER}")
	endif()
	tau_deps_ensure_prefix(parser-sdk "${_parser}" TAU_PARSER_SDK_PREFIX
		${_parser_args})
	if(_cross)
		tau_deps_ensure_host_prefix(tgf "${_parser}" TAU_TGF_PACKAGE_PREFIX
			"-DTAU_PARSER_PACKAGE=tgf"
			"-DTAU_PARSER_LTO=${TAU_LTO}" "-DTAU_PARSER_LTO_FAT=ON")
	else()
		tau_deps_ensure_prefix(tgf "${_parser}" TAU_TGF_PACKAGE_PREFIX
			"-DTAU_PARSER_PACKAGE=tgf"
			"-DTAU_PARSER_LTO=${TAU_LTO}" "-DTAU_PARSER_LTO_FAT=ON")
	endif()
	set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)
	set(TAU_UNORDERED_DENSE_PREFIX "${TAU_UNORDERED_DENSE_PREFIX}" PARENT_SCOPE)
	set(TAU_FTXUI_PREFIX "${TAU_FTXUI_PREFIX}" PARENT_SCOPE)
	set(TAU_PARSER_SDK_PREFIX "${TAU_PARSER_SDK_PREFIX}" PARENT_SCOPE)
	set(TAU_TGF_PACKAGE_PREFIX "${TAU_TGF_PACKAGE_PREFIX}" PARENT_SCOPE)
endfunction()

# Curl is a system package on native Linux and macOS, which both ship
# libcurl. The Windows targets have none, so they build the pinned static curl
# and define CURL::libcurl for nlang.
function(tau_deps_resolve_curl)
	if(NOT "CURL" IN_LIST TAU_BA_REQUIRED_PACKAGES)
		return()
	endif()
	if(NOT TAU_DEPS_TARGET STREQUAL "windows-x86_64-mingw"
			AND NOT TAU_DEPS_TARGET STREQUAL "windows-x86_64-msvc")
		return()
	endif()
	tau_deps_ensure_prefix(curl
		"${PROJECT_SOURCE_DIR}/scripts/dep-curl-package.sh" TAU_CURL_PREFIX)
	if(NOT TARGET CURL::libcurl)
		add_library(CURL::libcurl STATIC IMPORTED)
		# MSVC names its import/static library .lib; MinGW names it .a.
		set(_curl_lib "${TAU_CURL_PREFIX}/lib/libcurl.a")
		if(TAU_DEPS_TARGET STREQUAL "windows-x86_64-msvc")
			set(_curl_lib "${TAU_CURL_PREFIX}/lib/libcurl.lib")
		endif()
		set_target_properties(CURL::libcurl PROPERTIES
			IMPORTED_LOCATION "${_curl_lib}"
			INTERFACE_INCLUDE_DIRECTORIES "${TAU_CURL_PREFIX}/include"
			INTERFACE_COMPILE_DEFINITIONS "CURL_STATICLIB"
			INTERFACE_LINK_LIBRARIES
				"ws2_32;crypt32;secur32;bcrypt;iphlpapi")
	endif()
	set(TAU_CURL_PREFIX "${TAU_CURL_PREFIX}" PARENT_SCOPE)
	set(TAU_HAVE_STORE_CURL TRUE PARENT_SCOPE)
endfunction()

# Spot is a host tool Tau only execs (ltlsynt, autfilt, ltlfilt); nothing
# links it. A host that already has ltlsynt on PATH -- apt, brew, conda --
# uses that. Otherwise the store package supplies the tools, and configure
# publishes its bin directory as TAU_SPOT_BIN. Both Windows targets always take
# the package: a host PATH tool cannot carry the runtime DLLs of the PE, and wine
# cannot load an ELF.
#
# windows-x86_64-mingw takes the windows-x86_64-msvc entry, the Windows PE a
# MinGW tau execs. Under that id the producer applies the msvc host guard
# before it reads any store, so a configure on a Linux host always misses. A
# miss warns and leaves TAU_SPOT_BIN empty, and the LTL suites skip, because
# spot is an optional test-time tool. Do not fall back to a host ltlsynt: an ELF under an .exe name
# would make the suites run and fail instead of skip.
function(tau_deps_resolve_spot)
	# A wasm module cannot exec a host process, so no spot package serves it.
	# The LTL suites skip there through ltlsynt_available().
	if(TAU_DEPS_TARGET STREQUAL "wasm32-emscripten")
		message(STATUS "Spot not used on ${TAU_DEPS_TARGET}")
		return()
	endif()
	set(_windows_targets "windows-x86_64-msvc" "windows-x86_64-mingw")
	find_program(TAU_HOST_LTLSYNT ltlsynt NO_CACHE)
	if(TAU_HOST_LTLSYNT AND NOT TAU_DEPS_TARGET IN_LIST _windows_targets)
		message(STATUS "Spot from PATH: ${TAU_HOST_LTLSYNT}")
		return()
	endif()
	if(TAU_DEPS_TARGET STREQUAL "windows-x86_64-mingw")
		tau_deps_ensure_prefix_as(spot
			"${PROJECT_SOURCE_DIR}/scripts/dep-spot-package.sh"
			"windows-x86_64-msvc" TAU_SPOT_PREFIX OPTIONAL)
		if(NOT TAU_SPOT_PREFIX)
			message(STATUS "Spot not resolved: the LTL suites skip")
			return()
		endif()
	else()
		tau_deps_ensure_prefix(spot
			"${PROJECT_SOURCE_DIR}/scripts/dep-spot-package.sh" TAU_SPOT_PREFIX)
	endif()
	set(TAU_SPOT_PREFIX "${TAU_SPOT_PREFIX}" PARENT_SCOPE)
	set(TAU_SPOT_BIN "${TAU_SPOT_PREFIX}/bin" PARENT_SCOPE)
	message(STATUS "Spot from store: TAU_SPOT_BIN=${TAU_SPOT_PREFIX}/bin")
endfunction()

# Evict stale store entries once every dependency this configure needs is
# resolved. The resolved entries are in use and are never evicted.
function(tau_deps_evict_store)
	if(NOT DEFINED TAU_STORE_KEEP OR TAU_STORE_KEEP EQUAL 0)
		return()
	endif()
	set(_in_use "")
	foreach(_var TAU_PARSER_SDK_PREFIX TAU_TGF_PACKAGE_PREFIX
			TAU_UNORDERED_DENSE_PREFIX TAU_FTXUI_PREFIX
			CVC5_STORE_PREFIX BOOST_STORE_PREFIX TAU_CURL_PREFIX
			TAU_SPOT_PREFIX)
		if(DEFINED ${_var} AND NOT "${${_var}}" STREQUAL "")
			get_filename_component(_entry "${${_var}}" DIRECTORY)
			list(APPEND _in_use "${_entry}")
		endif()
	endforeach()
	tau_store_evict("${TAU_SHARED_PREFIX_RESOLVED}" "${TAU_STORE_KEEP}"
		${_in_use})
endfunction()
