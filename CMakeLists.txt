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

include(GNUInstallDirs)
include(FetchContent)

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
option(TAU_BUILD_API_TESTS         "Build API tests"                       OFF)
option(TAU_BUILD_REPL_TESTS        "Build repl tests"                      OFF)
option(TAU_BUILD_BENCHMARK         "Build benchmark tests"                 OFF)
option(TAU_BUILD_PIC               "Build position independent code"       OFF)
option(TAU_ADDRESS_SANITIZER       "Build with address sanitizer"          OFF)
option(TAU_DEB_PACKAGE             "Build DEB package"                     OFF)
option(TAU_RPM_PACKAGE             "Build RPM package"                     OFF)
option(TAU_WINDOWS_PACKAGE         "Build Windows package"                 OFF)
option(TAU_WINDOWS_ZIP_PACKAGE     "Build Windows zip package"             OFF)
option(TAU_GENERATE_PARSERS        "Generates parsers from TGF"            OFF)
option(TAU_BUILD_BINDING_PYTHON    "Build Python bindings"                 OFF)

set(TAU_SHARED_PREFIX "$ENV{HOME}/.tau" CACHE STRING "Install prefix for shared libraries")
set(TAU_BUILD_JOBS 0 CACHE STRING "Number of jobs to use for the build (0 to auto-detect)")

message(STATUS "TAU_BUILD_JOBS: ${TAU_BUILD_JOBS}")

# Set the number of jobs to use for the build
if (TAU_BUILD_JOBS EQUAL 0)
	include(ProcessorCount)
	ProcessorCount(TAU_BUILD_JOBS)
	if(TAU_BUILD_JOBS EQUAL 0)
		message(WARNING "Could not detect number of logical CPU cores, defaulting to 1")
		set(TAU_BUILD_JOBS 1)
	else()
		math(EXPR TAU_BUILD_JOBS "( ${TAU_BUILD_JOBS} + 1 ) / 2")
		message(STATUS "Auto setting TAU_BUILD_JOBS: ${TAU_BUILD_JOBS} (half of available cores)")
	endif()
endif()
set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} "${TAU_BUILD_JOBS}")

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
set_property(CACHE TAU_BUILD_API_TESTS         PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_REPL_TESTS        PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_BENCHMARK         PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_ADDRESS_SANITIZER       PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_DEB_PACKAGE             PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_RPM_PACKAGE             PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_WINDOWS_ZIP_PACKAGE     PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_WINDOWS_PACKAGE         PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_GENERATE_PARSERS        PROPERTY STRINGS "OFF" "ON")
set_property(CACHE TAU_BUILD_BINDING_PYTHON    PROPERTY STRINGS "OFF" "ON")

# Load local cmake settings
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLocalLists.txt")
	include("${CMAKE_CURRENT_SOURCE_DIR}/CMakeLocalLists.txt")
endif()

if(TAU_BUILD_TESTS)
	set(TAU_BUILD_UNIT_TESTS ON)
	set(TAU_BUILD_INTEGRATION ON)
	set(TAU_BUILD_API_TESTS ON)
	set(TAU_BUILD_REPL_TESTS ON)
	# set(TAU_BUILD_BENCHMARK ON)
endif()

if(TAU_BUILD_REPL_TESTS)
	set(TAU_BUILD_EXECUTABLE ON)
endif()

if(TAU_BUILD_BINDING_PYTHON)
	set(TAU_BUILD_STATIC_LIBRARY ON)
	set(TAU_BUILD_PIC ON)
endif()

if(TAU_BUILD_SHARED_LIBRARY)
	set(TAU_BUILD_PIC ON)
endif()

if(NOT TAU_BUILD_STATIC_LIBRARY AND NOT TAU_BUILD_SHARED_LIBRARY
	AND NOT TAU_BUILD_EXECUTABLE AND NOT TAU_BUILD_SHARED_EXECUTABLE
	AND NOT TAU_BUILD_UNIT_TESTS AND NOT TAU_BUILD_INTEGRATION
	AND NOT TAU_BUILD_API_TESTS AND NOT TAU_BUILD_REPL_TESTS
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
	set(TAU_BUILD_API_TESTS OFF)
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
message(STATUS "TAU_BUILD_API_TESTS: ${TAU_BUILD_API_TESTS}")
message(STATUS "TAU_BUILD_REPL_TESTS: ${TAU_BUILD_REPL_TESTS}")
message(STATUS "TAU_BUILD_BENCHMARK: ${TAU_BUILD_BENCHMARK}")
message(STATUS "TAU_ADDRESS_SANITIZER: ${TAU_ADDRESS_SANITIZER}")
message(STATUS "TAU_RPM_PACKAGE: ${TAU_RPM_PACKAGE}")
message(STATUS "TAU_DEB_PACKAGE: ${TAU_DEB_PACKAGE}")
message(STATUS "TAU_WINDOWS_ZIP_PACKAGE: ${TAU_WINDOWS_ZIP_PACKAGE}")
message(STATUS "TAU_WINDOWS_PACKAGE: ${TAU_WINDOWS_PACKAGE}")
message(STATUS "TAU_GENERATE_PARSERS: ${TAU_GENERATE_PARSERS}")
message(STATUS "TAU_BUILD_BINDING_PYTHON: ${TAU_BUILD_BINDING_PYTHON}")
message(STATUS "TAU_SHARED_PREFIX: ${TAU_SHARED_PREFIX}")

message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

set(CMAKE_INSTALL_PREFIX "${TAU_SHARED_PREFIX}" CACHE PATH "Install prefix" FORCE)
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_INSTALL_BINDIR: ${CMAKE_INSTALL_BINDIR}")
message(STATUS "CMAKE_INSTALL_LIBDIR: ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "CMAKE_INSTALL_INCLUDEDIR: ${CMAKE_INSTALL_INCLUDEDIR}")

#
# Add cvc5 library
#
set(DEP_CVC5_ARGS
	-DTAU_BUILD_JOBS=${TAU_BUILD_JOBS}
	-DTAU_SHARED_PREFIX=${TAU_SHARED_PREFIX})
set(CVC5_SOURCE_DIR ${TAU_SHARED_PREFIX}/cvc5)
set(CVC5_LIBDIR ${CVC5_SOURCE_DIR}/dist/${CMAKE_INSTALL_LIBDIR})
if (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	list(APPEND DEP_CVC5_ARGS --w64)
	set(CVC5_LIBDIR ${CVC5_SOURCE_DIR}/dist-w64/${CMAKE_INSTALL_LIBDIR})
endif()
message(STATUS "DEP_CVC5_ARGS: ${DEP_CVC5_ARGS}")
message(STATUS "CVC5_LIBDIR: ${CVC5_LIBDIR}")
execute_process(
	COMMAND bash ./dev dep-cvc5.sh ${DEP_CVC5_ARGS}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(cvc5 CONFIG REQUIRED
	PATHS ${CVC5_LIBDIR}/cmake/cvc5)
message(STATUS "cvc5 found: ${cvc5_FOUND}")
message(STATUS "cvc5 version: ${cvc5_VERSION}")

#
# Add Boost log library
#
set(DEP_BOOST_ARGS
	-DTAU_BUILD_JOBS=${TAU_BUILD_JOBS}
	-DTAU_SHARED_PREFIX=${TAU_SHARED_PREFIX}
	-DTAU_BUILD_PIC=${TAU_BUILD_PIC})
set(BOOST_SOURCE_DIR ${TAU_SHARED_PREFIX}/boost)
set(BOOST_LIBDIR ${BOOST_SOURCE_DIR}/dist/${CMAKE_INSTALL_LIBDIR})
if (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	list(APPEND DEP_BOOST_ARGS --w64)
	set(BOOST_LIBDIR ${BOOST_SOURCE_DIR}/dist-w64/${CMAKE_INSTALL_LIBDIR})
endif()
message(STATUS "DEP_BOOST_ARGS: ${DEP_BOOST_ARGS}")
message(STATUS "BOOST_LIBDIR: ${BOOST_LIBDIR}")
execute_process(
	COMMAND bash ./dev dep-boost.sh ${DEP_BOOST_ARGS}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# cmake_policy(SET CMP0167 NEW) is required in future versions of CMake
set(Boost_USE_STATIC_LIBS ON)
if ((CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	OR TAU_BUILD_PIC)
		set(Boost_DIR "${BOOST_LIBDIR}/cmake/Boost-1.86.0")
endif()
find_package(Boost REQUIRED COMPONENTS log)
if (Boost_FOUND)
	message(STATUS "Boost_DIR: ${Boost_DIR}")
	message(STATUS "Boost version: ${Boost_VERSION}")
	message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
	message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
else()
	message(FATAL_ERROR "Boost not found")
endif()

# Set RPATH where to find dependencies
set(CMAKE_BUILD_RPATH "\${ORIGIN}")
set(CMAKE_INSTALL_RPATH "\${ORIGIN}:\${ORIGIN}/../lib/tau:\${ORIGIN}/../lib64/tau")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_RPATH FALSE)
message(STATUS "CMAKE_BUILD_RPATH: ${CMAKE_BUILD_RPATH}")
message(STATUS "CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}")
message(STATUS "CMAKE_INSTALL_RPATH_USE_LINK_PATH: ${CMAKE_INSTALL_RPATH_USE_LINK_PATH}")
message(STATUS "CMAKE_SKIP_BUILD_RPATH: ${CMAKE_SKIP_BUILD_RPATH}")
message(STATUS "CMAKE_BUILD_WITH_RPATH: ${CMAKE_BUILD_WITH_RPATH}")

#
# Generate C++ parsers from TGF if requested
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
if (TAU_BUILD_UNIT_TESTS OR TAU_BUILD_INTEGRATION
 OR TAU_BUILD_API_TESTS OR TAU_BUILD_REPL_TESTS)
	message(STATUS "Building tests")
	include(CTest)
	add_subdirectory(tests)
endif ()

#
# Building Python bindings
#
if (TAU_BUILD_BINDING_PYTHON)
	message(STATUS "Building Python nanobind bindings")
	add_subdirectory(bindings/python/nanobind)
endif()

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
set(CPACK_PACKAGE_DIRECTORY "${TAU_SHARED_PREFIX}/packages")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://idni.org")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)

#
# Set component and destinations
#
set(TAU_PKG_DESTINATION "bin")
set(TAU_PKG_COMPONENT "LinuxFiles")
set(TAU_PKG_DOCS_DESTINATION "share/doc/tau")
if (TAU_WINDOWS_PACKAGE)
	set(TAU_PKG_COMPONENT "WindowsFiles")
elseif (TAU_WINDOWS_ZIP_PACKAGE)
	set(TAU_PKG_COMPONENT "WindowsFilesStandalone")
	set(TAU_PKG_DESTINATION ".")
	set(TAU_PKG_DOCS_DESTINATION ".")
endif()

# Install executable and documentation
if (TAU_BUILD_EXECUTABLE)
install(TARGETS ${TAU_EXECUTABLE_NAME}
	RUNTIME DESTINATION ${TAU_PKG_DESTINATION} COMPONENT ${TAU_PKG_COMPONENT}
	CONFIGURATIONS Release)
install(FILES	"${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md"
		"${CMAKE_CURRENT_SOURCE_DIR}/README.md"
	DESTINATION ${TAU_PKG_DOCS_DESTINATION} COMPONENT ${TAU_PKG_COMPONENT})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/licenses
	DESTINATION ${TAU_PKG_DOCS_DESTINATION} COMPONENT ${TAU_PKG_COMPONENT})

# Install cvc5 runtime dependencies
include(install_cvc5)
if (TAU_RPM_PACKAGE)
	execute_process(COMMAND rpm --eval "%{_lib}"
			OUTPUT_VARIABLE rpm_libdir
			OUTPUT_STRIP_TRAILING_WHITESPACE)
	set(CMAKE_INSTALL_LIBDIR "${rpm_libdir}")
	install_cvc5("${rpm_libdir}/tau" "LinuxFiles" "Linux")
elseif (TAU_DEB_PACKAGE)
	set(CMAKE_INSTALL_LIBDIR "lib")
	install_cvc5("lib/tau" "${TAU_PKG_COMPONENT}" "Linux")
elseif (TAU_WINDOWS_PACKAGE OR TAU_WINDOWS_ZIP_PACKAGE)
	install_cvc5("${TAU_PKG_DESTINATION}" "${TAU_PKG_COMPONENT}" "Windows")
endif()

# Set CPack generators
set(CPACK_COMPONENTS_ALL ${TAU_PKG_COMPONENT})
if (TAU_BUILD_EXECUTABLE AND TAU_WINDOWS_PACKAGE)
	set(CPACK_GENERATOR "NSIS")
	# NSIS (Windows Installer) specific settings
	set(CPACK_NSIS_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.exe")
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "tau-lang-${CPACK_PACKAGE_VERSION}")
	set(CPACK_NSIS_DISPLAY_NAME "Tau Language Framework")
	set(CPACK_NSIS_HELP_LINK "https://github.com/IDNI/tau-lang/blob/main/README.md")
	set(CPACK_NSIS_URL_INFO_ABOUT "${CPACK_PACKAGE_HOMEPAGE_URL}")
	set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
	set(CPACK_NSIS_LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
	set(CPACK_NSIS_INSTALLER_PRIVILEGES "lowest")
	set(CPACK_NSIS_MODIFY_PATH ON)
elseif (TAU_BUILD_EXECUTABLE AND TAU_WINDOWS_ZIP_PACKAGE)
	set(CPACK_GENERATOR "ZIP")
	set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
elseif (TAU_BUILD_EXECUTABLE AND TAU_RPM_PACKAGE)
	set(CPACK_GENERATOR "RPM")
	set(CPACK_RPM_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.rpm")
	set(CPACK_RPM_PACKAGE_AUTOREQ " no")
	set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
	set(CPACK_RPM_PACKAGE_LICENSE "Tau Language Framework")
	set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
	# cvc5 dependencies: glibc, gmp, libstdc++
	set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.27, gmp >= 6.2.1, libstdc++ >= 4.9")
	set(CPACK_RPM_PACKAGE_DOC_DIR "share/doc/${PROJECT_NAME}")
elseif (TAU_BUILD_EXECUTABLE AND TAU_DEB_PACKAGE)
	set(CPACK_GENERATOR "DEB")
	# DEB specific settings
	set(CPACK_DEBIAN_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.deb")
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "IDNI") # required
	# cvc5 dependencies: libc6, libgmp10 (or libgmp), libstdc++6
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.27), libgmp10 (>= 2:6.2.1), libstdc++6 (>= 4.9)")
	set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
	set(CPACK_DEBIAN_ARCHITECTURE "amd64")
endif()

# Include the CPack module
include(CPack)