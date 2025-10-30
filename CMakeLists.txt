cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/parser/cmake)

# Read version from file and strip prerelease information for cmake project VERSION property
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" TAU_VERSION)
string(STRIP "${TAU_VERSION}" TAU_VERSION)
string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+" TAU_TARGET_VERSION "${TAU_VERSION}")
message(STATUS "Tau version: ${TAU_TARGET_VERSION} / ${TAU_VERSION}")

set(namespace "TAU")
set(PROJECT_NAME tau)
set(PROJECT_LIB_NAME TAU)
project("${PROJECT_NAME}"
	VERSION "${TAU_TARGET_VERSION}"
	DESCRIPTION "IDNI's tau language"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compile commands (clangd)

# Set the number of jobs to use for the build
include(ProcessorCount)
ProcessorCount(PROCESSOR_COUNT)
if(PROCESSOR_COUNT EQUAL 0)
	message(WARNING "Could not detect number of logical CPU cores, defaulting to 1")
	set(PROCESSOR_COUNT 1)
endif()
set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} "${PROCESSOR_COUNT}")

include(FetchContent)
include(ExternalProject)

option(PARSER_MEASURE              "Measure parser performance"            OFF)
option(TAU_MEASURE                 "Measure tau performance"               OFF)
option(TAU_CACHE                   "Cache costly computations"             OFF)
option(TAU_LOG_LINES               "Log file and line numbers"             OFF)
option(TAU_LOG_LINE_PATHS          "Log file with a path and line numbers" OFF)
option(TAU_LOG_CHANNELS            "Enable Debug and Trace logging channels" OFF)
option(TAU_LOG_TRACE_TESTS         "Trace logging severity for tests"      OFF)
option(TAU_BUILD_DOC               "Build documentation"                   OFF)
option(TAU_BUILD_STATIC_LIBRARY    "Build static library"                  OFF)
option(TAU_BUILD_SHARED_LIBRARY    "Build shared library"                  OFF)
option(TAU_BUILD_EXECUTABLE        "Build executable"                      OFF)
option(TAU_BUILD_SHARED_EXECUTABLE "Build shared executable"               OFF)
option(TAU_BUILD_TESTS             "Build all tests"                       OFF)
option(TAU_BUILD_UNIT_TESTS        "Build unit tests"                      OFF)
option(TAU_BUILD_INTEGRATION       "Build integration tests"               OFF)
option(TAU_BUILD_REPL_TESTS        "Build repl tests"                      OFF)
option(TAU_BUILD_BENCHMARK         "Build benchmark tests"                 OFF)
option(TAU_ADDRESS_SANITIZER       "Build with address sanitizer"          OFF)
option(TAU_WINDOWS_PACKAGE         "Build Windows package"                 OFF)
option(TAU_WINDOWS_ZIP_PACKAGE     "Build Windows zip package"             OFF)
option(TAU_GENERATE_PARSERS        "Generates parsers from TGF"            OFF)

set(TAU_SHARED_PREFIX "$ENV{HOME}/.tau" CACHE STRING "Install prefix for shared libraries")

set_property(CACHE PARSER_MEASURE              PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_MEASURE                 PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_CACHE                   PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_LOG_LINES               PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_LOG_LINE_PATHS          PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_LOG_CHANNELS            PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_LOG_TRACE_TESTS         PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_DOC               PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_STATIC_LIBRARY    PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_SHARED_LIBRARY    PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_EXECUTABLE        PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_SHARED_EXECUTABLE PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_TESTS             PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_UNIT_TESTS        PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_INTEGRATION       PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_REPL_TESTS        PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_BENCHMARK         PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_ADDRESS_SANITIZER       PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_WINDOWS_ZIP_PACKAGE     PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_WINDOWS_PACKAGE         PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_GENERATE_PARSERS        PROPERTY STRINGS "OFF" "ON")

# Load local cmake settings
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLocalLists.txt")
	include("${CMAKE_CURRENT_SOURCE_DIR}/CMakeLocalLists.txt")
endif()

if(TAU_BUILD_TESTS)
	set(TAU_BUILD_UNIT_TESTS ON)
	set(TAU_BUILD_REPL_TESTS ON)
	set(TAU_BUILD_INTEGRATION ON)
	set(TAU_BUILD_BENCHMARK ON)
endif()

if(TAU_BUILD_REPL_TESTS)
	set(TAU_BUILD_EXECUTABLE ON)
endif()

if(NOT TAU_BUILD_STATIC_LIBRARY AND NOT TAU_BUILD_SHARED_LIBRARY
	AND NOT TAU_BUILD_EXECUTABLE AND NOT TAU_BUILD_SHARED_EXECUTABLE
	AND NOT TAU_BUILD_UNIT_TESTS AND NOT TAU_BUILD_INTEGRATION
	AND NOT TAU_BUILD_REPL_TESTS AND NOT TAU_BUILD_BENCHMARK)
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
#	set(TAU_LOG_TRACE_TESTS ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(TAU_BUILD_BENCHMARK OFF)
	set(TAU_MEASURE OFF)
	set(TAU_CACHE ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Coverage")
	set(TAU_BUILD_BENCHMARK OFF)
	set(TAU_MEASURE OFF)
	message(STATUS "Building with coverage information")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
	set(TAU_BUILD_UNIT_TESTS ON)
	set(TAU_BUILD_INTEGRATION ON)
	set(TAU_BUILD_REPL_TESTS OFF)
	set(TAU_BUILD_BENCHMARK OFF)
endif()

#
# Adding coverage target if requested
#
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
	find_program(LCOV_EXEC lcov)
	find_program(GENHTML_EXEC genhtml)
	if(LCOV_EXEC AND GENHTML_EXEC)
		add_custom_target(coverage
			COMMAND ${LCOV_EXEC} --directory . --capture --output-file coverage.info
			#COMMAND ${LCOV_EXEC} --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
			COMMAND ${GENHTML_EXEC} -o coverage coverage.info
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			COMMENT "Generating coverage report"
			VERBATIM
		)
	else()
		message(WARNING "lcov or genhtml not found! Coverage target will not be available.")
	endif()
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
	TAU_LOG_LINES
	TAU_LOG_LINE_PATHS
	TAU_LOG_CHANNELS
)

message(STATUS "PROCESSOR_COUNT: ${PROCESSOR_COUNT}")
message(STATUS "PARSER_MEASURE: ${PARSER_MEASURE}")
message(STATUS "TAU_MEASURE: ${TAU_MEASURE}")
message(STATUS "TAU_CACHE: ${TAU_CACHE}")
message(STATUS "TAU_LOG_LINES: ${TAU_LOG_LINES}")
message(STATUS "TAU_LOG_LINE_PATHS: ${TAU_LOG_LINE_PATHS}")
message(STATUS "TAU_LOG_CHANNELS: ${TAU_LOG_CHANNELS}")
message(STATUS "TAU_LOG_TRACE_TESTS: ${TAU_LOG_TRACE_TESTS}")
message(STATUS "TAU_BUILD_STATIC_LIBRARY: ${TAU_BUILD_STATIC_LIBRARY}")
message(STATUS "TAU_BUILD_SHARED_LIBRARY: ${TAU_BUILD_SHARED_LIBRARY}")
message(STATUS "TAU_BUILD_EXECUTABLE: ${TAU_BUILD_EXECUTABLE}")
message(STATUS "TAU_BUILD_SHARED_EXECUTABLE: ${TAU_BUILD_SHARED_EXECUTABLE}")
message(STATUS "TAU_BUILD_TESTS: ${TAU_BUILD_TESTS}")
message(STATUS "TAU_BUILD_UNIT_TESTS: ${TAU_BUILD_UNIT_TESTS}")
message(STATUS "TAU_BUILD_INTEGRATION: ${TAU_BUILD_INTEGRATION}")
message(STATUS "TAU_BUILD_REPL_TESTS: ${TAU_BUILD_REPL_TESTS}")
message(STATUS "TAU_BUILD_BENCHMARK: ${TAU_BUILD_BENCHMARK}")
message(STATUS "TAU_ADDRESS_SANITIZER: ${TAU_ADDRESS_SANITIZER}")
message(STATUS "TAU_WINDOWS_ZIP_PACKAGE: ${TAU_WINDOWS_ZIP_PACKAGE}")
message(STATUS "TAU_WINDOWS_PACKAGE: ${TAU_WINDOWS_PACKAGE}")
message(STATUS "TAU_GENERATE_PARSERS: ${TAU_GENERATE_PARSERS}")
message(STATUS "TAU_SHARED_PREFIX: ${TAU_SHARED_PREFIX}")

if(NOT EXISTS "${TAU_SHARED_PREFIX}/lib")
	execute_process(COMMAND mkdir -p "${TAU_SHARED_PREFIX}/lib")
endif()

if(NOT EXISTS "${TAU_SHARED_PREFIX}/lib64")
	execute_process(COMMAND ln -s lib ${TAU_SHARED_PREFIX}/lib64)
endif()

#
# Add cvc5 as an external project
#
if (NOT EXISTS "${TAU_SHARED_PREFIX}/lib/libcvc5.so")
	message(STATUS "cvc5 not found in ${TAU_SHARED_PREFIX}, building from source...")
	ExternalProject_Add(cvc5_external_project
		SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/cvc5
		CONFIGURE_COMMAND cd ${CMAKE_SOURCE_DIR}/external/cvc5 && ${CMAKE_SOURCE_DIR}/external/cvc5/configure.sh --no-gpl --auto-download --prefix=${TAU_SHARED_PREFIX}
		BUILD_COMMAND make -C ${CMAKE_SOURCE_DIR}/external/cvc5/build -j ${PROCESSOR_COUNT} # && make test && make install
		INSTALL_COMMAND make -C ${CMAKE_SOURCE_DIR}/external/cvc5/build install
		UPDATE_COMMAND ""
		BUILD_BYPRODUCTS ${CMAKE_SOURCE_DIR}/external/cvc5/build/libcvc5.so
	)
else()
	message(STATUS "cvc5 found in ${TAU_SHARED_PREFIX}, using prebuilt library...")
endif()

set(CVC5_FOUND TRUE)
set(CVC5_INCLUDE_DIR "${TAU_SHARED_PREFIX}/include/")
set(CVC5_LIBRARIES "${TAU_SHARED_PREFIX}/lib/libcvc5.so")

add_library(cvc5 STATIC IMPORTED GLOBAL)
set_target_properties(
  cvc5 PROPERTIES IMPORTED_LOCATION "${CVC5_LIBRARIES}"
)
set_target_properties(
  cvc5 PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CVC5_INCLUDE_DIR}"
)
add_dependencies(cvc5 cvc5_external_project)
set(CVC5_LIB cvc5)

#
# Adding Boost log library
#
set(Boost_USE_STATIC_LIBS ON)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND
	CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		find_package(Boost REQUIRED
			PATHS "${CMAKE_CURRENT_SOURCE_DIR}/external/libboost-mingw-w64"
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

# Add relative path of shared libraries to the executable's path (ld token $ORIGIN)
set(CMAKE_BUILD_RPATH "${CMAKE_BUILD_RPATH}:\$ORIGIN:${TAU_SHARED_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH "\$ORIGIN:${TAU_SHARED_PREFIX}/lib")
message(STATUS "CMAKE_BUILD_RPATH: ${CMAKE_BUILD_RPATH}")
message(STATUS "CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}")

#
# Generate C++ parser from TGF if requested
#
if (TAU_GENERATE_PARSERS OR TAU_GENERATE_PARSER)
	include(generate-parser)
	generate_parser("${PROJECT_SOURCE_DIR}/parser/tau.tgf")
	generate_parser("${PROJECT_SOURCE_DIR}/parser/sbf.tgf")
endif()

#
# Generate version_license.h file
#
include(version_license)
version_license(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/version_license.h.template"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/version_license.h")

if(TAU_ADDRESS_SANITIZER)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
	set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
endif()

#
# Add tauparser library
#
message("Building parser")
set(TAU_PARSER_BUILD_STATIC_LIBRARY ON CACHE BOOL "Build static library" FORCE)
add_subdirectory(external/parser)

#
# load common cmake settings and load functions
# 		target_setup(), target_compile_definitions_if() and exclude()
include(tau-common)

#
# Adding src dir
#
message(STATUS "Building src")
add_subdirectory("src")

#
# Testing
#
if (TAU_BUILD_UNIT_TESTS OR TAU_BUILD_INTEGRATION OR TAU_BUILD_REPL_TESTS)
	message(STATUS "Building tests")
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
set(CPACK_PACKAGE_VERSION "${TAU_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "IDNI")
set(CPACK_PACKAGE_CONTACT "contact@idni.org")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/packages")
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
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt" "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
		DESTINATION "."
		COMPONENT "WindowsFiles")
	set(CPACK_GENERATOR "NSIS")
	set(CPACK_COMPONENTS_ALL "WindowsFiles")
	# NSIS (Windows Installer) specific settings
	set(CPACK_NSIS_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.exe")
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "tau-lang-${CPACK_PACKAGE_VERSION}")
	set(CPACK_NSIS_DISPLAY_NAME "Tau Language")
	set(CPACK_NSIS_HELP_LINK "https://github.com/IDNI/tau-lang/blob/main/README.md")
	set(CPACK_NSIS_URL_INFO_ABOUT "${CPACK_PACKAGE_HOMEPAGE_URL}")
	set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
	set(CPACK_NSIS_LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
	set(CPACK_NSIS_INSTALLER_PRIVILEGES "lowest")
	set(CPACK_NSIS_MODIFY_PATH ON)
elseif (TAU_BUILD_EXECUTABLE AND TAU_WINDOWS_ZIP_PACKAGE)
	install(TARGETS ${TAU_EXECUTABLE_NAME}
		RUNTIME DESTINATION "."
		COMPONENT "win-standalone"
		CONFIGURATIONS Release)
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt" "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
		DESTINATION "."
		COMPONENT "win-standalone")
	set(CPACK_GENERATOR "ZIP")
	set(CPACK_COMPONENTS_ALL "win-standalone")
	set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
elseif (TAU_BUILD_EXECUTABLE)
	install(TARGETS ${TAU_EXECUTABLE_NAME}
		RUNTIME DESTINATION "bin"
		COMPONENT "LinuxFiles"
		CONFIGURATIONS Release)
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt" "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
		DESTINATION "share/doc/${PROJECT_NAME}"
		COMPONENT "LinuxFiles")
	set(CPACK_GENERATOR "DEB;RPM")
	set(CPACK_COMPONENTS_ALL "LinuxFiles")
	# DEB specific settings
	set(CPACK_DEBIAN_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.deb")
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "IDNI") # required
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.27)")
	set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
	set(CPACK_DEBIAN_ARCHITECTURE "amd64")
	# RPM specific settings
	set(CPACK_RPM_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.rpm")
	set(CPACK_RPM_PACKAGE_AUTOREQ " no")
	set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
	set(CPACK_RPM_PACKAGE_LICENSE "Tau Language Framework")
	set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
	#set(CPACK_RPM_PACKAGE_REQUIRES "libc >= 2.27")
	set(CPACK_RPM_PACKAGE_DOC_DIR "share/doc/${PROJECT_NAME}")
endif()

# Include the CPack module
include(CPack)