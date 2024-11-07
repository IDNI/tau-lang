cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(namespace "TAU")
set(PROJECT_NAME tau)
# TODO (LOW) check if tau is better than TAU
set(PROJECT_LIB_NAME TAU)
project("${PROJECT_NAME}"
	VERSION 0.0.1
	DESCRIPTION "IDNI's tau language"
)

option(TAU_MEASURE                 "Measure tau performance"               OFF)
option(PARSER_MEASURE              "Measure parser performance"            OFF)
option(TAU_CACHE                   "Cache costly computations"             OFF)
option(TAU_BUILD_DOC               "Build documentation"                   OFF)
option(TAU_BUILD_STATIC_LIBRARY    "Build static library"                  OFF)
option(TAU_BUILD_SHARED_LIBRARY    "Build shared library"                  OFF)
option(TAU_BUILD_EXECUTABLE        "Build executable"                      OFF)
option(TAU_BUILD_SHARED_EXECUTABLE "Build shared executable"               OFF)
option(TAU_BUILD_TESTS             "Build all tests"                       OFF)
option(TAU_BUILD_UNIT_TESTS        "Build unit tests"                      OFF)
option(TAU_BUILD_REPL_TESTS        "Build repl tests"                      OFF)
option(TAU_BUILD_INTEGRATION       "Build integration tests"               OFF)
option(TAU_BUILD_REGRESSION        "Build regression tests"                OFF)
option(TAU_BUILD_BENCHMARK         "Build benchmark tests"                 OFF)
option(TAU_GENERATE_PARSERS        "Generates parsers from TGF"            OFF)
set_property(CACHE TAU_BUILD_DOC               PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_STATIC_LIBRARY    PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_SHARED_LIBRARY    PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_EXECUTABLE        PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_SHARED_EXECUTABLE PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_TESTS             PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_INTEGRATION       PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_REGRESSION        PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_BENCHMARK         PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_REPL_TESTS        PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_GENERATE_PARSERS        PROPERTY STRINGS "OFF" "ON")

if(TAU_BUILD_TESTS)
	set(TAU_BUILD_UNIT_TESTS ON)
	set(TAU_BUILD_REPL_TESTS ON)
	set(TAU_BUILD_INTEGRATION ON)
	set(TAU_BUILD_REGRESSION ON)
	set(TAU_BUILD_BENCHMARK ON)
endif()

if(TAU_BUILD_REPL_TESTS)
	set(TAU_BUILD_EXECUTABLE ON)
endif()

if(NOT TAU_BUILD_STATIC_LIBRARY AND NOT TAU_BUILD_SHARED_LIBRARY
	AND NOT TAU_BUILD_EXECUTABLE AND NOT TAU_BUILD_SHARED_EXECUTABLE
	AND NOT TAU_BUILD_UNIT_TESTS AND NOT TAU_BUILD_REGRESSION
	AND NOT TAU_BUILD_INTEGRATION AND NOT TAU_BUILD_REPL_TESTS
	AND NOT TAU_BUILD_BENCHMARK)
		set(TAU_BUILD_EXECUTABLE ON)
endif()

# fine tuning definitions according to build type and options
if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	set(TAU_BUILD_BENCHMARK ON)
	set(TAU_MEASURE ON)
	set(TAU_CACHE ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(TAU_BUILD_BENCHMARK OFF)
	set(TAU_MEASURE OFF)
	set(TAU_CACHE OFF)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
	set(TAU_BUILD_BENCHMARK OFF)
	set(TAU_MEASURE OFF)
	set(TAU_CACHE TRUE)
endif()

if(NOT TAU_BUILD_BENCHMARK)
	set(TAU_MEASURE OFF)
endif()

if(TAU_MEASURE)
	set(PARSER_MEASURE ON)
endif()

# defining available definitions
set(TAU_DEFINITIONS
	TAU_MEASURE
	TAU_CACHE
)

message(STATUS "TAU_MEASURE: ${TAU_MEASURE}")
message(STATUS "PARSER_MEASURE: ${PARSER_MEASURE}")
message(STATUS "TAU_CACHE: ${TAU_CACHE}")
message(STATUS "TAU_BUILD_STATIC_LIBRARY: ${TAU_BUILD_STATIC_LIBRARY}")
message(STATUS "TAU_BUILD_SHARED_LIBRARY: ${TAU_BUILD_SHARED_LIBRARY}")
message(STATUS "TAU_BUILD_EXECUTABLE: ${TAU_BUILD_EXECUTABLE}")
message(STATUS "TAU_BUILD_SHARED_EXECUTABLE: ${TAU_BUILD_SHARED_EXECUTABLE}")
message(STATUS "TAU_BUILD_TESTS: ${TAU_BUILD_TESTS}")
message(STATUS "TAU_BUILD_UNIT_TESTS: ${TAU_BUILD_UNIT_TESTS}")
message(STATUS "TAU_BUILD_REPL_TESTS: ${TAU_BUILD_REPL_TESTS}")
message(STATUS "TAU_BUILD_REGRESSION: ${TAU_BUILD_REGRESSION}")
message(STATUS "TAU_BUILD_INTEGRATION: ${TAU_BUILD_INTEGRATION}")
message(STATUS "TAU_BUILD_BENCHMARK: ${TAU_BUILD_BENCHMARK}")
message(STATUS "TAU_GENERATE_PARSERS: ${TAU_GENERATE_PARSERS}")

#
# Adding Boost log library
#
set(Boost_USE_STATIC_LIBS ON)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND
	CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		find_package(Boost REQUIRED
			PATHS "${CMAKE_CURRENT_SOURCE_DIR}/libboost-mingw-w64"
			COMPONENTS log)
else()
		find_package(Boost REQUIRED COMPONENTS log)
endif()
if (Boost_FOUND)
	message(STATUS "Boost version: ${Boost_VERSION}")
	message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
	message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
else()
	message(FATAL_ERROR "Boost not found")
endif()

#
# Adding parser library submodule directory
#
option(BOOST_LOG_TRIVIAL           "Build logging in parser"   ON)
option(PARSER_BUILD_STATIC_LIBRARY "Build only static library" ON)
add_subdirectory("external/parser")
set(IDNI_PARSER_OBJECT_LIB "idni_parsero")

# TODO (LOW) make it run automatically on grammar modifications
#
# Check https://stackoverflow.com/questions/26193068/how-to-only-build-auto-generated-code-when-the-generator-or-input-changes-in-cma

#
# Generate C++ parser from TGF if requested
#
if (TAU_GENERATE_PARSERS OR TAU_GENERATE_PARSER)
	include(generate-parser)
	generate_parser("${PROJECT_SOURCE_DIR}/parser/tau.tgf")
	generate_parser("${PROJECT_SOURCE_DIR}/parser/bdd.tgf")
endif()

# load common cmake settings and load functions
# 		target_setup(), target_compile_definitions_if() and exclude()
include(tau-common)

#
# Adding src dir
#
add_subdirectory("src")

#
# Testing
#
if (TAU_BUILD_UNIT_TESTS OR TAU_BUILD_REGRESSION
	OR TAU_BUILD_INTEGRATION OR TAU_BUILD_REPL_TESTS)
		enable_testing()
		add_subdirectory(tests)
endif ()

#
# Build documentation
#
# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND AND TAU_BUILD_DOC)
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
else (DOXYGEN_FOUND AND TAU_BUILD_DOC)
	message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND AND TAU_BUILD_DOC)
