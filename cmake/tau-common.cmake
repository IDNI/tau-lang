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
if(USED_CMAKE_GENERATOR MATCHES "Ninja")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
endif()

# LTO only pays off when something LTO-links it; test targets are all -fno-lto.
# -ffat-lto-objects is what lets those -fno-lto targets link an LTO-built
# library, and em++ has no equivalent, so wasm takes the LTO-off path whole.
if ((TAU_BUILD_EXECUTABLE OR TAU_BUILD_SHARED_EXECUTABLE
	OR TAU_BUILD_SHARED_LIBRARY OR TAU_BUILD_BINDING_PYTHON)
	AND NOT EMSCRIPTEN)
	set(TAU_LTO_COMPILE ";-flto=auto;-ffat-lto-objects")
	set(TAU_LTO_LINK "-flto=auto")
else()
	set(TAU_LTO_COMPILE "")
	set(TAU_LTO_LINK "")
	message(STATUS "LTO off: nothing links with LTO here (tests are -fno-lto)")
endif()

set(TAU_DEVEL_OPTIONS "-O0;-DDEBUG;-g0")
set(TAU_DEBUG_OPTIONS "-O0;-DDEBUG;-ggdb3")
set(TAU_RELEASE_OPTIONS "-O3;-DNDEBUG${TAU_LTO_COMPILE}")
set(TAU_RELWITHDEBINFO_OPTIONS "-O3;-DNDEBUG${TAU_LTO_COMPILE};-g")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(COMPILE_OPTIONS "${TAU_DEBUG_OPTIONS}")
	set(TAU_LINK_OPTIONS "")
elseif (CMAKE_BUILD_TYPE STREQUAL "Devel")
	set(COMPILE_OPTIONS "${TAU_DEVEL_OPTIONS}")
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
if(EMSCRIPTEN)
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
			-Werror           # warning as errors
			-Wfatal-errors    # first error stops compilation
			# -ftemplate-backtrace-limit=0
		)
	else()
		target_compile_options(${target} PRIVATE /W4)
	endif()
	if(EMSCRIPTEN)
		target_compile_options(${target} PRIVATE
			# emsdk ships a newer clang than the host one, which reports
			# unused templates the rest of the toolchains accept
			-Wno-unused-template
			-fwasm-exceptions
			# the standardized wasm-exceptions encoding, not emsdk's
			# legacy-by-default one; compile+link, must match everywhere an
			# object is linked into a wasm artifact (B6/D2,
			# .local/build-emscripten.md)
			-sWASM_LEGACY_EXCEPTIONS=0
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
