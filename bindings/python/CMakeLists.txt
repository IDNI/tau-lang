cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# Minimal C-API shared library for ctypes-based Python bindings (Q40-API2).
# Built unconditionally with -DTAU_BUILD_BINDING_PYTHON=ON (alongside nanobind).

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	message(FATAL_ERROR "run bindings cmake from root with -DTAU_BUILD_BINDING_PYTHON=ON")
endif()

add_library(tau_lang SHARED tau_lang_c_api.cpp)

add_dependencies(tau_lang tau_header)
set_target_properties(tau_lang PROPERTIES
	INSTALL_RPATH "${CVC5_LIBDIR}"
	CXX_STANDARD 23
)
target_compile_features(tau_lang PRIVATE cxx_std_23)
target_link_libraries(tau_lang PRIVATE ${TAU_STATIC_LIB_NAME})
target_include_directories(tau_lang PRIVATE
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/parser"
	"${CMAKE_SOURCE_DIR}/external/parser/src"
	"${TAU_SHARED_PREFIX}/cvc5/dist/include"
	"${TAU_SHARED_PREFIX}/boost/dist/include"
)

# Also build the nanobind bindings (existing target) if opted in.
# Default: skip (nanobind / Python Development module may not be available).
# Enable with -DTAU_BUILD_NANOBIND=ON in addition to TAU_BUILD_BINDING_PYTHON.
option(TAU_BUILD_NANOBIND "Build nanobind Python bindings" OFF)
if (TAU_BUILD_NANOBIND)
	add_subdirectory(nanobind)
endif()
