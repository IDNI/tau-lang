# To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

# The static CaDiCaL library for the windows-x86_64-msvc cvc5 package, built
# with clang-cl. The producer copies this file into the CaDiCaL source tree as
# its CMakeLists.txt, so the CaDiCaL source stays unpatched. CaDiCaL's own
# build expects a Unix compiler and ar, which the MSVC shell does not have.
cmake_minimum_required(VERSION 3.16)
project(cadical CXX)

set(CVC5_MSVC_DIR "" CACHE PATH "The cmake/cvc5-msvc folder of tau")
if(NOT EXISTS "${CVC5_MSVC_DIR}/compat.h")
	message(FATAL_ERROR "CVC5_MSVC_DIR must name the folder of compat.h, got '${CVC5_MSVC_DIR}'")
endif()

# cadical.cpp and mobical.cpp hold the two command line tools, not the library.
file(GLOB _cadical_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(FILTER _cadical_sources EXCLUDE REGEX "/(cadical|mobical)\\.cpp$")

add_library(cadical STATIC ${_cadical_sources})
# CaDiCaL keys its Windows code on the MinGW macro __WIN32. NUNLOCKED and
# NCLOSEFROM are what cvc5's FindCaDiCaL adds when the CRT lacks getc_unlocked
# and closefrom, and NBUILD gives version 2.1.3 without a generated build.hpp.
target_compile_definitions(cadical PRIVATE
	__WIN32 QUIET NDEBUG NBUILD NUNLOCKED NCLOSEFROM)
target_include_directories(cadical PRIVATE "${CVC5_MSVC_DIR}/include")
target_compile_options(cadical PRIVATE "/FI${CVC5_MSVC_DIR}/compat.h" -w)

install(TARGETS cadical ARCHIVE DESTINATION lib)
install(FILES src/cadical.hpp src/tracer.hpp DESTINATION include/cadical)
