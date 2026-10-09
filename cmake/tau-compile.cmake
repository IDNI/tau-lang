# To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md
#
# The build half of `tau compile`: configure, build and copy one emitted
# artifact. All build logic lives here, so the CLI, `./dev compile` and a
# hand-written cmake invocation share it.
#
# Inputs (-D before -P):
#   TAU_ARTIFACT_DIR  required; the directory holding main.cpp and CMakeLists.txt
#   TAU_NATIVE        build for this machine: the SDK the script lives in,
#                     cmake's default compiler, a Release build type
#   TAU_PRESET        platform or tau preset name deciding the build
#                     (default release; unused with TAU_NATIVE)
#   TAU_EXTRA_ARGS    list of -D... and -G <gen> items passed to the configure
#   TAU_OUTPUT        executable destination (default <artifact>/<TAU_EXE_NAME>)
#   TAU_EXE_NAME      artifact target name (default program)
#   TAU_CXX           C++ compiler override (optional; the preset or cmake
#                     otherwise decides)
#   TAU_SDK_DIR       directory holding TauConfig.cmake (optional; else the
#                     box this script lives in)
#   TAU_SDK_DEPS      file of -D package hints for the SDK (optional)
#   TAU_BUILD_DIR     build directory without a presets file (optional)
#
# Exit codes: 0 built and copied; 2 bad argument; 3 configure failed; 4 build
# failed; 5 executable not found; 6 copy failed. A cmake older than 3.29 cannot
# exit a -P script with a chosen code, so it collapses every failure to 1.
#
# The platform list and the preset-to-platform map come from the SDK the script
# lives in; no list is repeated here.

# A -P script sets no policy by itself: cmake 3.x then rejects IN_LIST.
cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# cmake_language(EXIT) is the only way a -P script chooses its own exit code.
# Older cmake collapses every failure to 1, so a success must return instead
# of falling into the FATAL_ERROR branch.
macro(tau_exit code)
	if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.29")
		cmake_language(EXIT ${code})
	elseif(${code} EQUAL 0)
		return()
	else()
		message(FATAL_ERROR "tau-compile failed (stage ${code})")
	endif()
endmacro()

# Prints the last lines of a log, so a failure a reader sees names the cause
# even where the log file is not at hand (a CI job's output).
function(tau_print_log_tail path)
	if(NOT EXISTS "${path}")
		return()
	endif()
	file(STRINGS "${path}" _lines)
	list(LENGTH _lines _count)
	if(_count GREATER 20)
		math(EXPR _start "${_count} - 20")
		list(SUBLIST _lines ${_start} -1 _lines)
	endif()
	foreach(_line IN LISTS _lines)
		message("  ${_line}")
	endforeach()
endfunction()

# The SDK package a cross platform's box ships in, named by the store target
# value rather than the build folder's short preset tag.
function(tau_sdk_package_suffix platform out_var)
	set(_suffix "")
	if(platform MATCHES "-w64$")
		set(_suffix "-windows-x86_64-mingw")
	elseif(platform MATCHES "wasm")
		set(_suffix "-wasm32-emscripten")
	elseif(platform MATCHES "-arm64$")
		set(_suffix "-linux-arm64")
	endif()
	set(${out_var} "${_suffix}" PARENT_SCOPE)
endfunction()

# The toolchain a cross platform's box needs, and the version that box
# recorded when it was built. A missing toolchain is reported in out_missing
# for the caller to exit on: the artifact cannot configure without it. A
# version mismatch is only a warning, since a newer compiler normally still
# links the archive.
function(tau_check_toolchain platform sdk_dir out_missing)
	set(${out_missing} "" PARENT_SCOPE)
	set(_host_id "")
	set(_host_version "")
	if(platform MATCHES "-w64$")
		find_program(_mingw_gxx x86_64-w64-mingw32-g++)
		if(NOT _mingw_gxx)
			set(${out_missing}
				"x86_64-w64-mingw32-g++; install g++-mingw-w64-x86-64"
				PARENT_SCOPE)
			return()
		endif()
		set(_host_id "GNU")
		execute_process(COMMAND "${_mingw_gxx}" -dumpfullversion
			OUTPUT_VARIABLE _host_version
			OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
	elseif(platform MATCHES "wasm")
		set(_emsdk_dir "${EMSCRIPTEN_DIR}")
		if(NOT _emsdk_dir)
			if(DEFINED ENV{TAU_SHARED_PREFIX}
				AND NOT "$ENV{TAU_SHARED_PREFIX}" STREQUAL "")
				set(_emsdk_dir
					"$ENV{TAU_SHARED_PREFIX}/emsdk/upstream/emscripten")
			elseif(DEFINED ENV{HOME} AND NOT "$ENV{HOME}" STREQUAL "")
				set(_emsdk_dir "$ENV{HOME}/.tau/emsdk/upstream/emscripten")
			endif()
		endif()
		if(NOT _emsdk_dir OR NOT EXISTS "${_emsdk_dir}/emcc")
			set(${out_missing}
				"emsdk; install emscripten or run ./dev dep-emsdk"
				PARENT_SCOPE)
			return()
		endif()
		set(_host_id "Emscripten")
		if(EXISTS "${_emsdk_dir}/emscripten-version.txt")
			file(READ "${_emsdk_dir}/emscripten-version.txt" _host_version)
			string(REGEX REPLACE "[^0-9.]" "" _host_version
				"${_host_version}")
		endif()
	elseif(platform MATCHES "-arm64$")
		find_program(_aarch64_clang clang)
		find_program(_aarch64_clangxx clang++)
		if(NOT _aarch64_clang OR NOT _aarch64_clangxx)
			set(${out_missing}
				"clang and clang++; install clang"
				PARENT_SCOPE)
			return()
		endif()
		if(NOT IS_DIRECTORY "/usr/aarch64-linux-gnu")
			set(${out_missing}
				"the aarch64 cross runtime; install g++-aarch64-linux-gnu"
				PARENT_SCOPE)
			return()
		endif()
		set(_host_id "Clang")
		execute_process(COMMAND "${_aarch64_clangxx}" --version
			OUTPUT_VARIABLE _clang_banner
			OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
		string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" _host_version
			"${_clang_banner}")
	else()
		return()
	endif()
	if(NOT EXISTS "${sdk_dir}/tau-toolchain.txt")
		return()
	endif()
	include("${sdk_dir}/tau-toolchain.txt")
	if(NOT TAU_SDK_TOOLCHAIN_ID STREQUAL _host_id
		OR (NOT TAU_SDK_TOOLCHAIN_VERSION STREQUAL ""
			AND NOT _host_version STREQUAL ""
			AND NOT TAU_SDK_TOOLCHAIN_VERSION STREQUAL _host_version))
		message(WARNING "tau-compile: this ${platform} SDK was built with "
			"${TAU_SDK_TOOLCHAIN_ID} ${TAU_SDK_TOOLCHAIN_VERSION}, but the "
			"host has ${_host_id} ${_host_version}; the build may fail")
	endif()
endfunction()

if(NOT TAU_ARTIFACT_DIR)
	message("tau-compile: TAU_ARTIFACT_DIR is required")
	tau_exit(2)
endif()
# The configure and build steps run with this directory as their working
# directory, so a relative form would resolve against itself and double.
get_filename_component(TAU_ARTIFACT_DIR "${TAU_ARTIFACT_DIR}" ABSOLUTE)
if(NOT EXISTS "${TAU_ARTIFACT_DIR}/CMakeLists.txt")
	message("tau-compile: no CMakeLists.txt in ${TAU_ARTIFACT_DIR}")
	tau_exit(2)
endif()
if(NOT TAU_EXE_NAME)
	set(TAU_EXE_NAME "program")
endif()
if(NOT TAU_OUTPUT)
	set(TAU_OUTPUT "${TAU_ARTIFACT_DIR}/${TAU_EXE_NAME}")
endif()
set(_native FALSE)
if(TAU_NATIVE)
	set(_native TRUE)
endif()
if(NOT TAU_PRESET AND NOT _native)
	set(TAU_PRESET "release")
endif()

set(_platform "")
if(NOT _native)
	if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/tau-platforms.cmake")
		message("tau-compile: this SDK carries no cmake/tau-platforms.cmake")
		tau_exit(2)
	endif()
	include("${CMAKE_CURRENT_LIST_DIR}/tau-platforms.cmake")

	# A platform name is used as is; a tau preset name maps through the SDK's
	# map.
	if(TAU_PRESET IN_LIST TAU_PLATFORM_NAMES)
		set(_platform "${TAU_PRESET}")
	else()
		foreach(_entry IN LISTS TAU_PRESET_PLATFORM_MAP)
			string(FIND "${_entry}" "${TAU_PRESET}|" _pos)
			if(_pos EQUAL 0)
				string(LENGTH "${TAU_PRESET}|" _len)
				string(SUBSTRING "${_entry}" ${_len} -1 _platform)
				break()
			endif()
		endforeach()
	endif()
	if("${_platform}" STREQUAL "")
		message("tau-compile: unknown preset '${TAU_PRESET}'; platforms are: "
			"${TAU_PLATFORM_NAMES}")
		tau_exit(2)
	endif()

	# An MSVC platform cannot configure without the developer environment that
	# puts cl on PATH; the child inherits only what this process has.
	string(FIND "${_platform}" "msvc" _msvc_pos)
	if(NOT _msvc_pos EQUAL -1)
		find_program(_tau_cl cl)
		if(NOT _tau_cl)
			message("tau-compile: open the MSVC developer shell for "
				"${_platform}; cl is not on PATH")
			tau_exit(3)
		endif()
	endif()
endif()

set(_sdk "${TAU_SDK_DIR}")
if(NOT _sdk)
	# The box this script lives in: its cmake/ sits beside TauConfig.cmake.
	set(_sdk "${CMAKE_CURRENT_LIST_DIR}/..")
endif()
if(NOT EXISTS "${_sdk}/TauConfig.cmake")
	if(_native)
		message("tau-compile: no tau SDK at ${_sdk}")
	else()
		tau_sdk_package_suffix("${_platform}" _tau_sdk_package_suffix)
		message("tau-compile: no SDK for ${_platform}; install tau-sdk"
			"${_tau_sdk_package_suffix} or build it with ./dev preset "
			"${_platform}")
	endif()
	tau_exit(3)
endif()

if(NOT _native)
	tau_check_toolchain("${_platform}" "${_sdk}" _tau_toolchain_missing)
	if(_tau_toolchain_missing)
		message("tau-compile: the ${_platform} SDK needs "
			"${_tau_toolchain_missing}")
		tau_exit(3)
	endif()
endif()

set(_extra_args ${TAU_EXTRA_ARGS})
set(_extra_args_configure ${_extra_args})

set(_build_dir "${TAU_BUILD_DIR}")
if(NOT _build_dir)
	if(_native)
		set(_build_dir "${TAU_ARTIFACT_DIR}/build/native")
	else()
		set(_build_dir "${TAU_ARTIFACT_DIR}/build/${_platform}")
	endif()
endif()

set(_configure_log "${TAU_ARTIFACT_DIR}/configure.log")
set(_config "Release")
if(_native)
	# The SDK of the running tau builds for this machine, with cmake's default
	# compiler and a Release build type unless the caller names another.
	set(_configure_invocation
		-S "${TAU_ARTIFACT_DIR}"
		-B "${_build_dir}"
		"-DTau_DIR:PATH=${_sdk}"
		"-DTAU_ARTIFACT_EXE_NAME:STRING=${TAU_EXE_NAME}")
	set(_has_build_type FALSE)
	foreach(_arg IN LISTS TAU_EXTRA_ARGS)
		if(_arg MATCHES "^-DCMAKE_BUILD_TYPE(:[A-Za-z]+)?=(.*)$")
			set(_has_build_type TRUE)
			set(_config "${CMAKE_MATCH_2}")
		endif()
	endforeach()
	if(NOT _has_build_type)
		list(APPEND _configure_invocation
			"-DCMAKE_BUILD_TYPE:STRING=Release")
	endif()
	# A multi-config generator, as Visual Studio, ignores CMAKE_BUILD_TYPE
	# and builds Debug unless the build names the configuration.
	set(_build_invocation --build "${_build_dir}" --config "${_config}")
elseif(EXISTS "${TAU_ARTIFACT_DIR}/CMakePresets.json")
	# The preset decides the generator, the compiler and the build type.
	set(_configure_invocation
		--preset "${_platform}"
		"-DTau_DIR:PATH=${_sdk}"
		"-DTAU_ARTIFACT_EXE_NAME:STRING=${TAU_EXE_NAME}")
	set(_build_invocation --build "${_build_dir}")
else()
	# An artifact whose preset file is gone falls back to -S/-B; a generator
	# can then only come from TAU_EXTRA_ARGS, so cmake's default is used
	# otherwise.
	set(_configure_invocation
		-S "${TAU_ARTIFACT_DIR}"
		-B "${_build_dir}"
		"-DTau_DIR:PATH=${_sdk}"
		"-DTAU_ARTIFACT_EXE_NAME:STRING=${TAU_EXE_NAME}")
	set(_build_invocation --build "${_build_dir}")
endif()

# A build-tree SDK names the store packages it was built against; an installed
# SDK carries them, so no hints file exists and find_package resolves there.
if(NOT TAU_SDK_DEPS AND EXISTS "${_sdk}/tau-sdk-deps.cmake")
	set(TAU_SDK_DEPS "${_sdk}/tau-sdk-deps.cmake")
endif()
if(TAU_SDK_DEPS AND EXISTS "${TAU_SDK_DEPS}")
	include("${TAU_SDK_DEPS}")
	if(TAU_SDK_CONFIGURE_ARGS)
		list(APPEND _extra_args_configure ${TAU_SDK_CONFIGURE_ARGS})
	endif()
endif()
if(TAU_CXX AND NOT EXISTS "${TAU_CXX}")
	# A Visual Studio generator ignores CMAKE_CXX_COMPILER and builds with
	# its own compiler, so a compiler that does not exist is refused here.
	find_program(_tau_cxx_program NAMES "${TAU_CXX}" NO_CACHE)
	if(NOT _tau_cxx_program)
		file(WRITE "${_configure_log}"
			"the C++ compiler ${TAU_CXX} was not found\n")
		message("tau-compile: configure failed")
		tau_print_log_tail("${_configure_log}")
		tau_exit(3)
	endif()
endif()
if(NOT TAU_CXX AND _native AND EXISTS "${_sdk}/tau-toolchain.txt")
	# A native artifact links the SDK's archive, whose explicit
	# instantiations mangle C++20 constrained templates per compiler. Only
	# the compiler that built the archive resolves them, so it is the
	# default here; an explicit TAU_CXX still wins.
	include("${_sdk}/tau-toolchain.txt")
	if(TAU_SDK_TOOLCHAIN_CXX AND EXISTS "${TAU_SDK_TOOLCHAIN_CXX}")
		set(TAU_CXX "${TAU_SDK_TOOLCHAIN_CXX}")
	endif()
endif()
if(TAU_CXX)
	list(APPEND _extra_args_configure
		"-DCMAKE_CXX_COMPILER:PATH=${TAU_CXX}")
endif()
# The Visual Studio generator, cmake's default on Windows, builds with cl
# unless a toolset is named, whatever CMAKE_CXX_COMPILER says. An SDK built by
# clang-cl links only what clang-cl compiles. Unless the caller chose a
# generator or a toolset, Ninja builds with the compiler named above; without
# Ninja the ClangCL toolset of Visual Studio is named.
if(_native AND CMAKE_HOST_WIN32 AND TAU_CXX MATCHES "clang-cl(\\.exe)?$"
		AND NOT DEFINED ENV{CMAKE_GENERATOR})
	set(_names_generator FALSE)
	foreach(_arg IN LISTS TAU_EXTRA_ARGS)
		if(_arg MATCHES "^-[GT]")
			set(_names_generator TRUE)
		endif()
	endforeach()
	if(NOT _names_generator)
		find_program(_tau_ninja_program NAMES ninja NO_CACHE)
		if(_tau_ninja_program)
			list(APPEND _extra_args_configure -G Ninja)
		else()
			list(APPEND _extra_args_configure -T ClangCL)
		endif()
	endif()
endif()

execute_process(COMMAND "${CMAKE_COMMAND}" ${_configure_invocation}
		${_extra_args_configure}
	WORKING_DIRECTORY "${TAU_ARTIFACT_DIR}"
	RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
file(WRITE "${_configure_log}" "${_out}${_err}")
if(NOT _rc EQUAL 0)
	message("tau-compile: configure failed")
	tau_print_log_tail("${_configure_log}")
	tau_exit(3)
endif()

set(_build_log "${TAU_ARTIFACT_DIR}/build.log")
execute_process(COMMAND "${CMAKE_COMMAND}" ${_build_invocation}
	WORKING_DIRECTORY "${TAU_ARTIFACT_DIR}"
	RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
file(WRITE "${_build_log}" "${_out}${_err}")
if(NOT _rc EQUAL 0)
	message("tau-compile: build failed")
	tau_print_log_tail("${_build_log}")
	tau_exit(4)
endif()

string(FIND "${_platform}" "wasm" _wasm_pos)
set(_is_wasm FALSE)
if(NOT _wasm_pos EQUAL -1)
	set(_is_wasm TRUE)
endif()

# Single-config generators put the executable straight in the build dir; a
# config subdir covers a multi-config one. Windows adds .exe, wasm .js.
set(_built "")
if(_is_wasm)
	set(_candidates "${_build_dir}/${TAU_EXE_NAME}.js")
else()
	set(_candidates
		"${_build_dir}/${TAU_EXE_NAME}"
		"${_build_dir}/${TAU_EXE_NAME}.exe"
		"${_build_dir}/${_config}/${TAU_EXE_NAME}"
		"${_build_dir}/${_config}/${TAU_EXE_NAME}.exe")
endif()
foreach(_candidate IN LISTS _candidates)
	if(EXISTS "${_candidate}")
		set(_built "${_candidate}")
		break()
	endif()
endforeach()
if(NOT _built)
	message("tau-compile: no executable named ${TAU_EXE_NAME} under "
		"${_build_dir}")
	tau_exit(5)
endif()

get_filename_component(_output_dir "${TAU_OUTPUT}" DIRECTORY)
if(_output_dir)
	file(MAKE_DIRECTORY "${_output_dir}")
endif()
if(_is_wasm)
	# The js loader and its wasm sit beside each other under one name.
	foreach(_suffix ".js" ".wasm")
		get_filename_component(_stem "${_built}" NAME_WE)
		set(_src "${_build_dir}/${_stem}${_suffix}")
		execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different
				"${_src}" "${TAU_OUTPUT}${_suffix}"
			RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
		if(NOT _rc EQUAL 0)
			message("tau-compile: cannot copy ${_src} to "
				"${TAU_OUTPUT}${_suffix}\n${_out}${_err}")
			tau_exit(6)
		endif()
	endforeach()
else()
	execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different
			"${_built}" "${TAU_OUTPUT}"
		RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
	if(NOT _rc EQUAL 0)
		message("tau-compile: cannot copy ${_built} to ${TAU_OUTPUT}\n${_out}${_err}")
		tau_exit(6)
	endif()
	# Windows loads a DLL from the folder of the program, so the DLLs the
	# SDK links go beside it.
	if(TAU_SDK_RUNTIME_DLLS)
		if(NOT _output_dir)
			set(_output_dir ".")
		endif()
		execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different
				${TAU_SDK_RUNTIME_DLLS} "${_output_dir}"
			RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
		if(NOT _rc EQUAL 0)
			message("tau-compile: cannot copy the runtime DLLs to "
				"${_output_dir}\n${_out}${_err}")
			tau_exit(6)
		endif()
	endif()
endif()

tau_exit(0)
