cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TAU_PYTHON_BINDING_MODULE "tau_nanobind")

message(STATUS "TAU_PYTHON_BINDING_MODULE: ${TAU_PYTHON_BINDING_MODULE}")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	message(FATAL_ERROR "run bindings cmake from root with -DTAU_BUILD_BINDING_PYTHON=ON")
endif()

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "TAU_SHARED_PREFIX: ${TAU_SHARED_PREFIX}")

if (CMAKE_VERSION VERSION_LESS 3.18)
	set(DEV_MODULE Development)
else()
	set(DEV_MODULE Development.Module)
endif()

find_package(Python 3.9 COMPONENTS Interpreter ${DEV_MODULE} REQUIRED)

execute_process(
	COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
	OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)
find_package(nanobind CONFIG REQUIRED)


nanobind_add_module(${TAU_PYTHON_BINDING_MODULE} tau_nanobind.cpp)

add_dependencies(${TAU_PYTHON_BINDING_MODULE} tau_header)
set_target_properties(${TAU_PYTHON_BINDING_MODULE} PROPERTIES
	INSTALL_RPATH "${CVC5_LIBDIR}"
	OUTPUT_NAME "tau"
)
target_compile_features(${TAU_PYTHON_BINDING_MODULE} PRIVATE cxx_std_23)
target_link_libraries(${TAU_PYTHON_BINDING_MODULE} PRIVATE ${TAU_STATIC_LIB_NAME})
target_include_directories(${TAU_PYTHON_BINDING_MODULE} PRIVATE
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/parser"
	"${CMAKE_SOURCE_DIR}/external/parser/src"
	"${TAU_SHARED_PREFIX}/cvc5/dist/include"
	"${TAU_SHARED_PREFIX}/boost/dist/include"
)

# target_compile_options(${TAU_PYTHON_BINDING_MODULE} PRIVATE -Wfatal-errors)
