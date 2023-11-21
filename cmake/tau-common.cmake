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

set(DEBUG_OPTIONS "-O0;-DDEBUG;-ggdb3")
set(RELEASE_OPTIONS "-O3;-DNDEBUG")
set(COMPILE_OPTIONS
	"$<IF:$<CONFIG:Debug>,${DEBUG_OPTIONS},${RELEASE_OPTIONS}>"
	"$<$<CONFIG:Release>:-flto=auto>"
)
set(LINK_OPTIONS "-flto=auto")

include(git-defs) # for ${GIT_DEFINITIONS}
function(target_git_definitions target)
	target_compile_definitions(${target} PRIVATE ${GIT_DEFINITIONS})
endfunction()

# setups a target: sets COMPILE and LINK options, adds warnings, c++20 req...
function(target_setup target)
	target_compile_features(${target} PRIVATE cxx_std_20)
	if(NOT MSVC)
		target_compile_options(${target} PRIVATE
			-W -Wall -Wextra -Wpedantic
			-Wformat=2
			-Wcast-align
			-Wstrict-aliasing=2
			-Wstrict-overflow=5
			-Wfloat-equal
			-Wwrite-strings
			-Wno-unused-function
			#-Werror
			#-Wfatal-errors
		)
	else()
		target_compile_options(${target} PRIVATE /W4)
	endif()
	target_compile_options(${target} PRIVATE "${COMPILE_OPTIONS}")
	target_link_libraries(${target} ${CMAKE_THREAD_LIBS_INIT})
	target_link_options(${target} PRIVATE "${LINK_OPTIONS}")
	target_git_definitions(${target})
	foreach(X IN LISTS PROJECT_DEFINITIONS)
		if(${X})
			target_compile_definitions(${target} PRIVATE "-D${X}")
		endif()
	endforeach()
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
