cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
if(NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	message(STATUS
		"${PROJECT_NAME} as a subproject of [${CMAKE_PROJECT_NAME}]")
else()
	message(STATUS "${PROJECT_NAME} as a top project")
endif()

cmake_host_system_information(RESULT CORE_COUNT QUERY NUMBER_OF_LOGICAL_CORES)
if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES Debug Release)
	set(CMAKE_CONFIGURATION_TYPES
		"${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "" FORCE
	)
endif()
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "CMAKE_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID}")
	message(STATUS "CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER}")
endif ()

set(CMAKE_VERBOSE_MAKEFILE true CACHE BOOL "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(USED_CMAKE_GENERATOR
	"${CMAKE_GENERATOR}" CACHE STRING "Expose CMAKE_GENERATOR" FORCE
)

set(TAU_IS_GNU_OR_CLANG OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	set(TAU_IS_GNU_OR_CLANG ON)
endif()

if(USED_CMAKE_GENERATOR MATCHES "Ninja" AND TAU_IS_GNU_OR_CLANG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
endif()

# LTO only pays off when something LTO-links it; test targets are all -fno-lto
# (GNU/Clang) or omit /GL (MSVC). -ffat-lto-objects lets -fno-lto targets link
# an LTO-built library; em++ has no equivalent, so wasm takes LTO-off whole.
set(TAU_LTO_COMPILE_FLAGS "")
set(TAU_LTO_COMPILE "")
set(TAU_LTO_LINK "")
if (TAU_LTO AND (TAU_BUILD_EXECUTABLE OR TAU_BUILD_SHARED_EXECUTABLE
	OR TAU_BUILD_SHARED_LIBRARY OR TAU_BUILD_BINDING_PYTHON_NANOBIND
	OR TAU_BUILD_BINDING_PYTHON_CTYPE)
	AND NOT EMSCRIPTEN)
	if(TAU_IS_GNU_OR_CLANG)
		# AppleClang rejects -ffat-lto-objects under LTO; plain -flto=auto
		# works. The probe needs -flto=auto: without it the flag is accepted
		# everywhere.
		include(CheckCXXCompilerFlag)
		set(CMAKE_REQUIRED_FLAGS "-flto=auto")
		check_cxx_compiler_flag("-ffat-lto-objects" TAU_HAVE_FAT_LTO_OBJECTS)
		unset(CMAKE_REQUIRED_FLAGS)
		if(TAU_HAVE_FAT_LTO_OBJECTS)
			set(TAU_FAT_LTO ";-ffat-lto-objects")
		else()
			set(TAU_FAT_LTO "")
		endif()
		set(TAU_LTO_COMPILE_FLAGS "-flto=auto${TAU_FAT_LTO}")
		set(TAU_LTO_COMPILE ";${TAU_LTO_COMPILE_FLAGS}")
		set(TAU_LTO_LINK "-flto=auto")
	elseif(MSVC)
		set(TAU_LTO_COMPILE_FLAGS "/GL")
		set(TAU_LTO_COMPILE ";${TAU_LTO_COMPILE_FLAGS}")
		set(TAU_LTO_LINK "/LTCG")
	endif()
else()
	message(STATUS "LTO off")
endif()

# cl.exe spells the same optimization levels and debug flags differently.
if(MSVC)
	set(TAU_DEVEL_OPTIONS "/Od;/DDEBUG;/Zi")
	set(TAU_DEBUG_OPTIONS "/Od;/DDEBUG;/Zi")
	set(TAU_RELEASE_OPTIONS "/O2;/DNDEBUG${TAU_LTO_COMPILE}")
	set(TAU_RELWITHDEBINFO_OPTIONS "/O2;/DNDEBUG${TAU_LTO_COMPILE};/Zi")
	set(TAU_COVERAGE_OPTIONS "/Od;/DDEBUG;/Zi")
else()
	set(TAU_DEVEL_OPTIONS "-O0;-DDEBUG;-g0")
	set(TAU_DEBUG_OPTIONS "-O0;-DDEBUG;-ggdb3")
	set(TAU_RELEASE_OPTIONS "-O3;-DNDEBUG${TAU_LTO_COMPILE}")
	set(TAU_RELWITHDEBINFO_OPTIONS "-O3;-DNDEBUG${TAU_LTO_COMPILE};-g")
	# Coverage mirrors Debug semantics. --coverage itself is added in
	# CMakeLists.txt. Without -DDEBUG (and with no -DNDEBUG) asserts stay live
	# while bdd_handle::b is private, which does not compile -- see bdd_handle.h.
	set(TAU_COVERAGE_OPTIONS "-O0;-DDEBUG;-ggdb3")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(COMPILE_OPTIONS "${TAU_DEBUG_OPTIONS}")
	set(TAU_LINK_OPTIONS "")
elseif (CMAKE_BUILD_TYPE STREQUAL "Devel")
	set(COMPILE_OPTIONS "${TAU_DEVEL_OPTIONS}")
	set(TAU_LINK_OPTIONS "")
elseif (CMAKE_BUILD_TYPE STREQUAL "Coverage")
	set(COMPILE_OPTIONS "${TAU_COVERAGE_OPTIONS}")
	set(TAU_LINK_OPTIONS "")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	set(COMPILE_OPTIONS "${TAU_RELWITHDEBINFO_OPTIONS}")
	set(TAU_LINK_OPTIONS "${TAU_LTO_LINK}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
	set(COMPILE_OPTIONS "${TAU_RELEASE_OPTIONS}")
	set(TAU_LINK_OPTIONS "${TAU_LTO_LINK}")
endif()

message(STATUS "COMPILE_OPTIONS ${COMPILE_OPTIONS}")
message(STATUS "TAU_LINK_OPTIONS ${TAU_LINK_OPTIONS}")

# gold links noticeably faster than bfd; use it everywhere when available.
# em++ always links (it ignores -fuse-ld=gold rather than rejecting it), so
# the check below passes there too, but wasm-ld is em++'s only real linker.
# MSVC has no gold; skip the probe entirely.
if(EMSCRIPTEN OR NOT TAU_IS_GNU_OR_CLANG)
	set(TAU_LINKER "")
else()
	include(CheckLinkerFlag)
	check_linker_flag(CXX "-fuse-ld=gold" TAU_HAVE_GOLD)
	if(TAU_HAVE_GOLD)
		set(TAU_LINKER "-fuse-ld=gold")
		set(CMAKE_LINK_DEPENDS_USE_LINKER FALSE)
	else()
		set(TAU_LINKER "")
		message(STATUS "gold not available, linking with the default linker")
	endif()
endif()

include(git-defs) # for ${TAU_GIT_DEFINITIONS}
function(target_git_definitions target)
	target_compile_definitions(${target} PRIVATE ${TAU_GIT_DEFINITIONS})
endfunction()

# passes definitions if they exist
function(target_compile_definitions_if target access project_definitions)
	foreach(X IN LISTS project_definitions)
		if(${X})
			target_compile_definitions(${target} ${access} "-D${X}")
		endif()
	endforeach()
endfunction()

# setups a target: sets COMPILE and LINK options, adds warnings, c++ std req...
function(target_setup target)
	if(NOT MSVC)
		target_compile_options(${target} PRIVATE
			-W -Wall -Wextra -Wpedantic
			-Wformat=2
			-Wcast-align
			-Wstrict-aliasing=2
			-Wfloat-equal
			-Wwrite-strings
			# tau's own targets carry these: no dependency exports its build
			# flags, so nothing else supplies them and a value-losing or sign
			# conversion would go unnoticed.
			-Wconversion -Wsign-conversion
			# warnings as errors in dev configs only, so a newly
			# introduced warning is caught there, not in Release
			$<$<OR:$<CONFIG:Debug>,$<CONFIG:Devel>,$<CONFIG:Coverage>,$<CONFIG:RelWithDebInfo>>:-Werror>
			# GCC 15 reports -Wstrict-overflow at -O0 with --coverage in
			# code no other configuration flags; keep it visible, not fatal
			$<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Coverage>>:-Wno-error=strict-overflow>
			# -ftemplate-backtrace-limit=0
		)
	else()
		# /utf-8: the sources carry UTF-8 in comments and string literals.
		# NOMINMAX: windows.h's min/max macros would break std::min/std::max.
		target_compile_options(${target} PRIVATE
			/W4 /EHsc /utf-8 /bigobj)
		target_compile_definitions(${target} PRIVATE
			NOMINMAX
			WIN32_LEAN_AND_MEAN
			_CRT_DECLARE_NONSTDC_NAMES=0)
	endif()
	if(EMSCRIPTEN)
		target_compile_options(${target} PRIVATE
			# emsdk ships a newer clang than the host one, which reports
			# unused templates the rest of the toolchains accept
			-Wno-unused-template
			-fwasm-exceptions
			# the standardized wasm-exceptions encoding, not emsdk's
			# legacy-by-default one. Every linked object in a wasm artifact
			# must agree on this.
			-sWASM_LEGACY_EXCEPTIONS=0
			# a shift this wide is undefined at wasm32's 32-bit word size and
			# traps at runtime, not a mere warning
			-Werror=shift-count-overflow
			# wasm32 is the only 32-bit target here. A 64-bit value
			# narrowed to size_t there truncates silently instead of trapping
			-Werror=shorten-64-to-32
		)
		target_link_options(${target} PRIVATE
			-fwasm-exceptions
			-sWASM_LEGACY_EXCEPTIONS=0
		)
	endif()
	target_compile_options(${target} PRIVATE "${COMPILE_OPTIONS}")
	target_compile_definitions_if(${target} PRIVATE "${TAU_DEFINITIONS}")
	target_compile_features(${target} PRIVATE cxx_std_23)
	# grammars are compiled ahead of time, so tau never parses .tgf at runtime
	target_compile_definitions(${target} PRIVATE TAU_PARSER_NO_TGF)
	if(TAU_DEPS_FROM_STORE)
		# src/defs.h pulls the parser's defs.h from the SDK package.
		target_compile_definitions(${target} PRIVATE TAU_PARSER_DEFS_INSTALLED)
	else()
		# src/defs.h includes the parser defs through this macro so its text
		# never names the external/parser path an installed SDK must not carry.
		target_compile_definitions(${target} PRIVATE
			"TAU_PARSER_DEFS_INCLUDE=\"../external/parser/src/defs.h\"")
	endif()
	# generated tau_pack.h describing the configured BA pack, and beside it the
	# BA parsers generated from the pack's grammars
	if(TAU_PACK_INCLUDE_DIR)
		target_include_directories(${target} PRIVATE ${TAU_PACK_INCLUDE_DIR})
	endif()
	# generated core parsers, which core includes by bare name
	if(TAU_GENERATED_PARSER_DIR)
		target_include_directories(${target} PRIVATE
			${TAU_GENERATED_PARSER_DIR})
	endif()
	# directories of out-of-tree BAs registered with tau_register_ba()
	if(TAU_BA_INCLUDE_DIRS)
		target_include_directories(${target} PRIVATE ${TAU_BA_INCLUDE_DIRS})
	endif()
	if (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND
		CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			target_compile_options(${target} PRIVATE
				-Wa,-mbig-obj
				-fno-use-linker-plugin
			)
			target_link_libraries(${target}
				${CMAKE_THREAD_LIBS_INIT}
				-static-libgcc
				-static-libstdc++
			)
			# reduce strict overflow level for boost has warnings in mingw
			target_compile_options(${target} PRIVATE
				-Wstrict-overflow=2
			)
	else()
		target_link_libraries(${target} ${CMAKE_THREAD_LIBS_INIT})
		if(NOT MSVC)
			target_compile_options(${target} PRIVATE
				-Wstrict-overflow=5
			)
		endif()
	endif()
	target_link_options(${target} PRIVATE "${TAU_LINK_OPTIONS}" ${TAU_LINKER})
	# Windows default stack is 1 MiB; tau_ba splitters and ocltl decode on
	# moderate k need more (Linux soft limit is typically 8 MiB). Match the
	# wasm STACK_SIZE so MSVC builds do not SIGSEGV / 0xc0000409 on the
	# same cases that pass elsewhere.
	if(MSVC)
		target_link_options(${target} PRIVATE "/STACK:16777216")
	endif()
	target_git_definitions(${target})
	set_target_properties(${target} PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
		PUBLIC_HEADER            "${PROJECT_HEADERS}"
	)
endfunction()

# exclude target from all and default
function(exclude target)
	set_target_properties(${target} PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
	)
endfunction()

# target names
set(TAU_OBJECT_LIB_NAME "${PROJECT_LIB_NAME}o")
set(TAU_STATIC_LIB_NAME "${PROJECT_LIB_NAME}_static")
set(TAU_SHARED_LIB_NAME "${PROJECT_LIB_NAME}")
set(TAU_EXECUTABLE_NAME "${PROJECT_NAME}")
set(TAU_EXE_SHARED_NAME "${PROJECT_NAME}_shared")

# The platform of this build: the build folder name `./dev preset` uses, which
# names the SDK box's install root.
get_filename_component(TAU_PLATFORM_NAME "${CMAKE_BINARY_DIR}" NAME)
# A cross target gets its own box package: it cannot share the host-native
# box's path or toolchain. The suffix is the store target value, not the build
# folder's short preset suffix; a native arm64 build keeps the unsuffixed box.
set(TAU_SDK_PACKAGE_SUFFIX "")
if(TAU_DEPS_TARGET STREQUAL "windows-x86_64-mingw")
	set(TAU_SDK_PACKAGE_SUFFIX "-windows-x86_64-mingw")
elseif(TAU_DEPS_TARGET STREQUAL "wasm32-emscripten")
	set(TAU_SDK_PACKAGE_SUFFIX "-wasm32-emscripten")
elseif(TAU_DEPS_TARGET STREQUAL "linux-arm64" AND CMAKE_CROSSCOMPILING)
	set(TAU_SDK_PACKAGE_SUFFIX "-linux-arm64")
endif()
set(TAU_SDK_PACKAGE_NAME "tau-sdk${TAU_SDK_PACKAGE_SUFFIX}")

include(tau-platform-map)

# Writes the platform names `./dev preset` uses and the tau preset that owns
# each, as a cmake file an SDK consumer reads, for `./dev compile` and the
# cmake script. The map comes from tau_collect_platform_map, so this file and
# the table compiled into tau cannot drift.
function(tau_write_platform_map out_file)
	tau_collect_platform_map()
	set(_content "# Generated from CMakePresets.json at configure time. Do not edit.\n")
	string(APPEND _content "set(TAU_PLATFORM_NAMES \"${TAU_PLATFORM_NAMES}\")\n")
	string(APPEND _content "set(TAU_PRESET_PLATFORM_MAP\n")
	foreach(_entry IN LISTS TAU_PRESET_PLATFORM_MAP)
		string(APPEND _content "\t\"${_entry}\"\n")
	endforeach()
	string(APPEND _content ")\n")
	file(WRITE "${out_file}" "${_content}")
endfunction()
