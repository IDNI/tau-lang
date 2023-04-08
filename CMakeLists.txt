cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

cmake_host_system_information(RESULT CORE_COUNT QUERY NUMBER_OF_LOGICAL_CORES)
set(CMAKE_VERBOSE_MAKEFILE true CACHE BOOL "")

set(USED_CMAKE_GENERATOR "${CMAKE_GENERATOR}" CACHE STRING "Expose CMAKE_GENERATOR" FORCE)
if (USED_CMAKE_GENERATOR MATCHES "Ninja")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
endif ()

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif ()

project(tau)
add_subdirectory("src")

#
# Build documentationo
#
option(BUILD_DOC "Build documentation" OFF)
# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND AND BUILD_DOC)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")
    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND AND BUILD_DOC)
	message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND AND BUILD_DOC)

#
# Testing
#
set(BUILD_TESTS ON CACHE STRING "build the tau-lang test suite ON")
set_property(CACHE BUILD_TESTS PROPERTY STRINGS "OFF" "ON")

if (BUILD_TESTS)
	set(DOCTEST_HEADER "${PROJECT_SOURCE_DIR}/src/doctest.h" CACHE PATH "Doctest header")
	if (NOT EXISTS "${DOCTEST_HEADER}")
		message(STATUS "Downloading doctest to '${PROJECT_SOURCE_DIR}'")
		find_package(Wget REQUIRED)
		execute_process(COMMAND "${WGET_EXECUTABLE}" https://raw.githubusercontent.com/onqtam/doctest/master/doctest/doctest.h -P ${PROJECT_SOURCE_DIR}/src)
	endif ()

	add_library(doctest INTERFACE)
	target_compile_definitions(doctest INTERFACE TAU_USE_DOCTEST)
	set(TEST_FRAMEWORK doctest)

	enable_testing()
	add_subdirectory(tests)
endif ()

#
# Building
#
message(STATUS "CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "CMAKE_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID}")
	message(STATUS "CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER}")
endif ()