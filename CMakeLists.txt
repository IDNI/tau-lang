cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(namespace "TAU")
set(PROJECT_NAME tau_lang)
set(PROJECT_LIB_NAME TAU)
project("${PROJECT_NAME}"
	VERSION 0.0.1
	DESCRIPTION "IDNI's tau language"
)

option(BUILD_DOC               "Build documentation"         OFF)
option(BUILD_STATIC_LIBRARY    "Build static library"        OFF)
option(BUILD_SHARED_LIBRARY    "Build shared library"        OFF)
option(BUILD_EXECUTABLE        "Build executable"            OFF)
option(BUILD_SHARED_EXECUTABLE "Build shared executable"     OFF)
option(BUILD_TESTS             "Build tests"                 OFF)
option(BUILD_REGRESSION        "Build regression tests"      OFF)
#option(BUILD_EXAMPLES          "Build examples"              OFF)
option(GENERATE_PARSER         "Generates parser from TGF"   OFF)
set_property(CACHE BUILD_DOC               PROPERTY STRINGS "OFF" "ON")
set_property(CACHE BUILD_STATIC_LIBRARY    PROPERTY STRINGS "OFF" "ON")
set_property(CACHE BUILD_SHARED_LIBRARY    PROPERTY STRINGS "OFF" "ON")
set_property(CACHE BUILD_EXECUTABLE        PROPERTY STRINGS "OFF" "ON")
set_property(CACHE BUILD_SHARED_EXECUTABLE PROPERTY STRINGS "OFF" "ON")
set_property(CACHE BUILD_TESTS             PROPERTY STRINGS "OFF" "ON")
set_property(CACHE BUILD_REGRESSION        PROPERTY STRINGS "OFF" "ON")
#set_property(CACHE BUILD_EXAMPLES          PROPERTY STRINGS "OFF" "ON")
set_property(CACHE GENERATE_PARSER         PROPERTY STRINGS "OFF" "ON")
if(NOT BUILD_STATIC_LIBRARY AND NOT BUILD_SHARED_LIBRARY
	AND NOT BUILD_EXECUTABLE AND NOT BUILD_SHARED_EXECUTABLE
	AND NOT BUILD_TESTS) # AND NOT BUILD_EXAMPLES)
		set(BUILD_STATIC_LIBRARY true)
		set(BUILD_EXECUTABLE true)
		set(BUILD_TESTS true)
		#set(BUILD_EXAMPLES true)
endif()

message(STATUS "BUILD_STATIC_LIBRARY: ${BUILD_STATIC_LIBRARY}")
message(STATUS "BUILD_SHARED_LIBRARY: ${BUILD_SHARED_LIBRARY}")
message(STATUS "BUILD_EXECUTABLE: ${BUILD_EXECUTABLE}")
message(STATUS "BUILD_SHARED_EXECUTABLE: ${BUILD_SHARED_EXECUTABLE}")
message(STATUS "BUILD_TESTS: ${BUILD_TESTS}")
message(STATUS "BUILD_REGRESSION: ${BUILD_REGRESSION}")
#message(STATUS "BUILD_EXAMPLES: ${BUILD_EXAMPLES}")
message(STATUS "GENERATE_PARSER: ${GENERATE_PARSER}")

#
# Adding parser library's git submodule
#
include(add-git-submodule)
add_git_submodule("external/parser")
set(IDNI_PARSER_OBJECT_LIB "idni_parsero")

#
# Generate C++ parser from TGF if requested
#
if (GENERATE_PARSER)
	include(generate-parser)
	generate_parser("${PROJECT_SOURCE_DIR}/parser/tau.tgf" tau_parser)
	generate_parser("${PROJECT_SOURCE_DIR}/parser/bdd.tgf" bdd_parser)
endif()

# load common cmake settings and load functions
# 		target_setup(), target_compile_definitions_if() and exclude()
include(tau-common)
#
# Adding src dir
#
add_subdirectory("src")

#if (BUILD_EXAMPLES)
#	add_subdirectory("examples")
#endif(BUILD_EXAMPLES)

#
# Testing
#
if (BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif ()

#
# Build documentation
#
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
	add_custom_target(doc_doxygen ALL
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM
	)
else (DOXYGEN_FOUND AND BUILD_DOC)
	message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND AND BUILD_DOC)
