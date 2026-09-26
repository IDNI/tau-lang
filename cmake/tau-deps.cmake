# cmake/tau-deps.cmake
# Store packages are content-addressed by the preset's compiler and flags; a
# missing one is built from the preset, never a hand-picked mode.

include_guard(GLOBAL)

# The store module owns the on-disk layout and the eviction rules; this file only
# decides when a producer runs.
include("${CMAKE_CURRENT_LIST_DIR}/../external/parser/cmake/tau-store.cmake")

# Compiler and flag arguments every producer receives, so no producer reads a
# compiler from the environment. The values are the preset's build-type flags,
# so switching the build type selects a different package.
function(_tau_deps_toolchain_args out)
	string(TOUPPER "${CMAKE_BUILD_TYPE}" _build_type)
	set(_cflags_var "CMAKE_C_FLAGS_${_build_type}")
	set(_cxxflags_var "CMAKE_CXX_FLAGS_${_build_type}")
	set(_args
		"-DTAU_DEP_CC=${CMAKE_C_COMPILER}"
		"-DTAU_DEP_CXX=${CMAKE_CXX_COMPILER}"
		"-DTAU_DEP_CFLAGS=${CMAKE_C_FLAGS} ${${_cflags_var}}"
		"-DTAU_DEP_CXXFLAGS=${CMAKE_CXX_FLAGS} ${${_cxxflags_var}}"
		"-DTAU_DEP_TARGET=${TAU_DEPS_TARGET}")
	# A cross toolchain travels as a file; macOS and MSVC build with the host
	# compiler the preset picked and pass none.
	if(CMAKE_TOOLCHAIN_FILE)
		list(APPEND _args "-DTAU_DEP_TOOLCHAIN=${CMAKE_TOOLCHAIN_FILE}")
	endif()
	set(${out} "${_args}" PARENT_SCOPE)
endfunction()

# A cross-toolchain target builds the host tgf natively, because the target
# compiler cannot produce a binary this build host can run.
function(_tau_deps_target_is_cross_toolchain out)
	if(TAU_DEPS_TARGET STREQUAL "w64" OR TAU_DEPS_TARGET STREQUAL "wasm")
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
function(tau_deps_producer_command out script)
	if(NOT TAU_BASH)
		message(FATAL_ERROR "running a dependency producer needs bash")
	endif()
	_tau_deps_toolchain_args(_toolchain)
	set(${out}
		"${CMAKE_COMMAND}" -E env
			--unset=CPPFLAGS --unset=CFLAGS --unset=CXXFLAGS --unset=LDFLAGS
			"TAU_SHARED_PREFIX=${TAU_SHARED_PREFIX_RESOLVED}"
			"TAU_BUILD_JOBS=${TAU_BUILD_JOBS_RESOLVED}"
			"${TAU_BASH}" "${script}" ${_toolchain}
		PARENT_SCOPE)
endfunction()

function(tau_deps_host_producer_command out script)
	if(NOT TAU_BASH)
		message(FATAL_ERROR "running a dependency producer needs bash")
	endif()
	_tau_deps_host_toolchain_args(_toolchain)
	set(${out}
		"${CMAKE_COMMAND}" -E env
			--unset=CPPFLAGS --unset=CFLAGS --unset=CXXFLAGS --unset=LDFLAGS
			"TAU_SHARED_PREFIX=${TAU_SHARED_PREFIX_RESOLVED}"
			"TAU_BUILD_JOBS=${TAU_BUILD_JOBS_RESOLVED}"
			"${TAU_BASH}" "${script}" ${_toolchain}
		PARENT_SCOPE)
endfunction()

# Run one producer and set <out_var> to its printed prefix. Extra arguments are
# appended to the producer command line.
function(_tau_deps_run_producer dep producer_cmd out_var)
	execute_process(
		COMMAND ${producer_cmd} ${ARGN}
		RESULT_VARIABLE _rc
		OUTPUT_VARIABLE _out
		ERROR_VARIABLE _err)
	if(NOT _rc EQUAL 0)
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
	if(NOT TAU_DEPS_TARGET STREQUAL "w64"
			AND NOT TAU_DEPS_TARGET STREQUAL "win-msvc-x64")
		return()
	endif()
	tau_deps_ensure_prefix(curl
		"${PROJECT_SOURCE_DIR}/scripts/dep-curl-package.sh" TAU_CURL_PREFIX)
	if(NOT TARGET CURL::libcurl)
		add_library(CURL::libcurl STATIC IMPORTED)
		# MSVC names its import/static library .lib; MinGW names it .a.
		set(_curl_lib "${TAU_CURL_PREFIX}/lib/libcurl.a")
		if(TAU_DEPS_TARGET STREQUAL "win-msvc-x64")
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
# publishes its bin directory as TAU_SPOT_BIN. win-msvc-x64 always takes the
# package: its MSYS2 build needs the runtime DLLs copied beside the exes, and
# a Windows PATH ltlsynt would not carry them.
function(tau_deps_resolve_spot)
	find_program(TAU_HOST_LTLSYNT ltlsynt NO_CACHE)
	if(TAU_HOST_LTLSYNT AND NOT TAU_DEPS_TARGET STREQUAL "win-msvc-x64")
		message(STATUS "Spot from PATH: ${TAU_HOST_LTLSYNT}")
		return()
	endif()
	tau_deps_ensure_prefix(spot
		"${PROJECT_SOURCE_DIR}/scripts/dep-spot-package.sh" TAU_SPOT_PREFIX)
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
