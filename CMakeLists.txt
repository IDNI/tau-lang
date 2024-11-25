cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(namespace "TAU")
set(PROJECT_NAME tau)
# TODO (LOW) check if tau is better than TAU
set(PROJECT_LIB_NAME TAU)
project("${PROJECT_NAME}"
	VERSION 0.7
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
option(TAU_WINDOWS_PACKAGE         "Build Windows package"                 OFF)
option(TAU_WINDOWS_ZIP_PACKAGE     "Build Windows zip package"             OFF)
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
set_property(CACHE TAU_WINDOWS_ZIP_PACKAGE     PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_WINDOWS_PACKAGE         PROPERTY STRINGS "OFF" "ON")

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

if(TAU_WINDOW_PACKAGE AND TAU_WINDOWS_ZIP_PACKAGE)
	message(FATAL_ERROR "Cannot build both Windows and Windows zip packages at the same time")
endif()

#
# Fine tuning definitions according to build type and options
#
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

#
# Defining available definitions
#
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
message(STATUS "TAU_WINDOWS_ZIP_PACKAGE: ${TAU_WINDOWS_ZIP_PACKAGE}")
message(STATUS "TAU_WINDOWS_PACKAGE: ${TAU_WINDOWS_PACKAGE}")

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
option(BOOST_LOG_TRIVIAL           "Build logging in parser"   ON)
option(PARSER_BUILD_STATIC_LIBRARY "Build only static library" ON)

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

#
# Generate LICENSE C++ file
#
include(version_license)

#
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

#
# CPack configuration
#

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "IDNI")
set(CPACK_PACKAGE_CONTACT "contact@idni.org")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://idni.org")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)

#
# Specify installation directories and files
#
if (TAU_BUILD_EXECUTABLE AND TAU_WINDOWS_PACKAGE)
	install(TARGETS ${TAU_EXECUTABLE_NAME}
		RUNTIME DESTINATION "bin"
		COMPONENT "WindowsFiles"
		CONFIGURATIONS Release)
	install(FILES "${CMAKE_SOURCE_DIR}/LICENSE.txt" "${CMAKE_SOURCE_DIR}/README.md"
		DESTINATION "."
		COMPONENT "WindowsFiles")
	set(CPACK_GENERATOR "NSIS")
	set(CPACK_COMPONENTS_ALL "WindowsFiles")
	# NSIS (Windows Installer) specific settings
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "tau-lang-${CPACK_PACKAGE_VERSION}")
	set(CPACK_NSIS_DISPLAY_NAME "Tau Language")
	set(CPACK_NSIS_HELP_LINK "https://github.com/IDNI/tau-lang/blob/main/README.md")
	set(CPACK_NSIS_URL_INFO_ABOUT "${CPACK_PACKAGE_HOMEPAGE_URL}")
	set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
	set(CPACK_NSIS_LICENSE_FILE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
	set(CPACK_NSIS_INSTALLER_PRIVILEGES "lowest")
	set(CPACK_NSIS_MODIFY_PATH ON)
elseif (TAU_BUILD_EXECUTABLE AND TAU_WINDOWS_ZIP_PACKAGE)
	install(TARGETS ${TAU_EXECUTABLE_NAME}
		RUNTIME DESTINATION "."
		COMPONENT "ZipFiles"
		CONFIGURATIONS Release)
	install(FILES "${CMAKE_SOURCE_DIR}/LICENSE.txt" "${CMAKE_SOURCE_DIR}/README.md"
		DESTINATION "."
		COMPONENT "ZipFiles")
	set(CPACK_GENERATOR "ZIP")
	set(CPACK_COMPONENTS_ALL "ZipFiles")
	set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
elseif (TAU_BUILD_EXECUTABLE)
	install(TARGETS ${TAU_EXECUTABLE_NAME}
		RUNTIME DESTINATION "bin"
		COMPONENT "LinuxFiles"
		CONFIGURATIONS Release)
	install(FILES "${CMAKE_SOURCE_DIR}/LICENSE.txt" "${CMAKE_SOURCE_DIR}/README.md"
		DESTINATION "share/doc/${PROJECT_NAME}"
		COMPONENT "LinuxFiles")
	set(CPACK_GENERATOR "DEB;RPM")
	set(CPACK_COMPONENTS_ALL "LinuxFiles")
	# DEB specific settings
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "IDNI") # required
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.27)")
	set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
	set(CPACK_DEBIAN_ARCHITECTURE "amd64")
	# RPM specific settings
	set(CPACK_RPM_PACKAGE_AUTOREQ " no")
	set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
	set(CPACK_RPM_PACKAGE_LICENSE "Tau Language Framework")
	set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
	#set(CPACK_RPM_PACKAGE_REQUIRES "libc >= 2.27")
	set(CPACK_RPM_PACKAGE_DOC_DIR "share/doc/${PROJECT_NAME}")
endif()

# Include the CPack module
include(CPack)