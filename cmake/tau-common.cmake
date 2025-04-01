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

set(TAU_DEBUG_OPTIONS "-O0;-DDEBUG;-ggdb3")
set(TAU_RELEASE_OPTIONS "-O3;-DNDEBUG;-flto=auto")
set(TAU_RELWITHDEBINFO_OPTIONS "-O3;-DNDEBUG;-flto=auto;-g")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(COMPILE_OPTIONS "${TAU_DEBUG_OPTIONS}")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	set(COMPILE_OPTIONS "${TAU_RELWITHDEBINFO_OPTIONS}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
	set(COMPILE_OPTIONS "${TAU_RELEASE_OPTIONS}")
endif()

set(LINK_OPTIONS "-flto=auto")

message(STATUS "COMPILE_OPTIONS ${COMPILE_OPTIONS}")
message(STATUS "LINK_OPTIONS ${LINK_OPTIONS}")

include(git-defs) # for ${GIT_DEFINITIONS}
function(target_git_definitions target)
	target_compile_definitions(${target} PRIVATE ${GIT_DEFINITIONS})
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
			-Wstrict-overflow=5
			-Wfloat-equal
			-Wwrite-strings
			# -Werror
			-Wfatal-errors
		)
	else()
		target_compile_options(${target} PRIVATE /W4)
	endif()
	target_compile_options(${target} PRIVATE "${COMPILE_OPTIONS}")
	target_compile_definitions_if(${target} PRIVATE "${TAU_DEFINITIONS}")
	if (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND
		CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			target_compile_features(${target} PRIVATE cxx_std_20)
			target_compile_options(${target} PRIVATE
				-Wa,-mbig-obj
				-fno-use-linker-plugin
			)
			target_link_libraries(${target}
				${CMAKE_THREAD_LIBS_INIT}
				-static-libgcc
				-static-libstdc++
			)
	else()
		target_compile_features(${target} PRIVATE cxx_std_23)
		target_link_libraries(${target} ${CMAKE_THREAD_LIBS_INIT})
	endif()
	target_link_options(${target} PRIVATE "${LINK_OPTIONS}")
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
