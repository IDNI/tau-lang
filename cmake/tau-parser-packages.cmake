cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
include_guard(GLOBAL)

# Consume the parser SDK that tau_deps_resolve_parser() took from the LOCAL
# store. Its prefix is already on CMAKE_PREFIX_PATH. The host tgf is the
# packaged binary, so this build never compiles the parser or tgf.
#
# The SDK is prebuilt, so its recorded lto and compiler id must match this
# configure. A mismatch is a hard error: the SDK is built from this same preset,
# so a difference means the store entry came from another toolchain.

list(PREPEND CMAKE_PREFIX_PATH "${TAU_PARSER_SDK_PREFIX}")
find_package(tauparser CONFIG REQUIRED NO_CMAKE_FIND_ROOT_PATH)

if(NOT TARGET tauparser)
	if(NOT TARGET idni::tauparser_static)
		message(FATAL_ERROR "the parser SDK did not export idni::tauparser_static")
	endif()
	add_library(tauparser ALIAS idni::tauparser_static)
endif()

message(STATUS "parser SDK at ${TAU_PARSER_SDK_PREFIX}")
message(STATUS "host tgf at ${TAU_TGF_PACKAGE_PREFIX}")

# Read the SDK manifest's recorded lto and compiler id.
function(_tau_parser_sdk_recorded out_lto out_compiler)
	get_filename_component(_entry "${TAU_PARSER_SDK_PREFIX}" DIRECTORY)
	set(_manifest "${_entry}/manifest.json")
	if(NOT EXISTS "${_manifest}")
		message(FATAL_ERROR "the parser SDK manifest is missing: ${_manifest}")
	endif()
	file(READ "${_manifest}" _json)
	string(JSON _lto ERROR_VARIABLE _err GET "${_json}" "fields" "lto")
	if(NOT _err STREQUAL "NOTFOUND")
		message(FATAL_ERROR "the parser SDK manifest has no lto field: ${_err}")
	endif()
	string(JSON _cid ERROR_VARIABLE _cerr GET "${_json}" "fields" "compiler_id")
	if(NOT _cerr STREQUAL "NOTFOUND")
		message(FATAL_ERROR "the parser SDK manifest has no compiler_id field: ${_cerr}")
	endif()
	set(${out_lto} "${_lto}" PARENT_SCOPE)
	set(${out_compiler} "${_cid}" PARENT_SCOPE)
endfunction()

# Map CMake's compiler id to the value the producers record.
function(_tau_configure_compiler_id out)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(_id GNU)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
		set(_id AppleClang)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set(_id Clang)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set(_id MSVC)
	else()
		set(_id unknown)
	endif()
	set(${out} "${_id}" PARENT_SCOPE)
endfunction()

function(tau_parser_packages_check_lto tau_lto)
	_tau_parser_sdk_recorded(_sdk_lto _sdk_compiler)
	if(NOT _sdk_lto STREQUAL "${tau_lto}")
		message(FATAL_ERROR
			"the parser SDK records lto=${_sdk_lto}, but Tau resolves lto=${tau_lto}; "
			"the SDK is built from this preset's TAU_LTO, so they must match")
	endif()
	_tau_configure_compiler_id(_tau_compiler)
	if(NOT _sdk_compiler STREQUAL "${_tau_compiler}")
		message(FATAL_ERROR
			"the parser SDK was built with compiler_id=${_sdk_compiler}, but this "
			"configure resolves ${_tau_compiler}; rebuild the SDK with this compiler")
	endif()
endfunction()
