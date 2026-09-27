cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TAU_PYTHON_BINDING_MODULE "tau_nanobind")

message(STATUS "TAU_PYTHON_BINDING_MODULE: ${TAU_PYTHON_BINDING_MODULE}")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	message(FATAL_ERROR "run bindings cmake from root with -DTAU_BUILD_BINDING_PYTHON_NANOBIND=ON")
endif()

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "TAU_SHARED_PREFIX: ${TAU_SHARED_PREFIX}")

if (CMAKE_VERSION VERSION_LESS 3.18)
	set(DEV_MODULE Development)
else()
	set(DEV_MODULE Development.Module)
endif()

# FindPython runs the target interpreter through CMAKE_CROSSCOMPILING_EMULATOR
# only under CMP0190 NEW; the project floor is 3.22, so raise it here.
if(POLICY CMP0190)
	cmake_policy(SET CMP0190 NEW)
endif()

find_package(Python 3.9 COMPONENTS Interpreter ${DEV_MODULE} REQUIRED)

# A cross build runs the target interpreter under the emulator; FindPython does
# this for its own queries, and the nanobind lookup and the tests must too.
set(TAU_PYTHON_EMULATOR "")
if(CMAKE_CROSSCOMPILING AND CMAKE_CROSSCOMPILING_EMULATOR)
	set(TAU_PYTHON_EMULATOR ${CMAKE_CROSSCOMPILING_EMULATOR})
endif()

execute_process(
	COMMAND ${TAU_PYTHON_EMULATOR} "${Python_EXECUTABLE}" -m nanobind --cmake_dir
	OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)
find_package(nanobind CONFIG REQUIRED)


nanobind_add_module(${TAU_PYTHON_BINDING_MODULE} tau_nanobind.cpp)

add_dependencies(${TAU_PYTHON_BINDING_MODULE} tau_header)
set_target_properties(${TAU_PYTHON_BINDING_MODULE} PROPERTIES
	INSTALL_RPATH "${CVC5_LIBDIR}"
	OUTPUT_NAME "tau"
)
target_compile_features(${TAU_PYTHON_BINDING_MODULE} PRIVATE cxx_std_23)
target_compile_definitions_if(${TAU_PYTHON_BINDING_MODULE} PRIVATE "${TAU_DEFINITIONS}")
target_link_libraries(${TAU_PYTHON_BINDING_MODULE} PRIVATE ${TAU_STATIC_LIB_NAME})
target_include_directories(${TAU_PYTHON_BINDING_MODULE} PRIVATE
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/parser"
	"${TAU_PACK_INCLUDE_DIR}"
	"${TAU_GENERATED_PARSER_DIR}"
	"${TAU_SHARED_PREFIX}/cvc5/dist/include"
	"${TAU_SHARED_PREFIX}/boost/dist/include"
)

set_source_files_properties(tau_nanobind.cpp PROPERTIES
	COMPILE_OPTIONS "-Wno-error")

# target_compile_options(${TAU_PYTHON_BINDING_MODULE} PRIVATE -Wfatal-errors)

# BUILD_TESTING mirrors the include(CTest) gate the root CMakeLists.txt
# uses, so these register only when ctest infra is actually active.
if(BUILD_TESTING)
	file(GLOB TAU_PYTHON_BINDING_TEST_SCRIPTS
		"${CMAKE_SOURCE_DIR}/tests/bindings/python/test_*.py")
	foreach(test_script ${TAU_PYTHON_BINDING_TEST_SCRIPTS})
		get_filename_component(test_stem "${test_script}" NAME_WE)
		add_test(NAME "binding-python-${test_stem}"
			COMMAND ${TAU_PYTHON_EMULATOR} ${Python_EXECUTABLE} "${test_script}")
		set_tests_properties("binding-python-${test_stem}" PROPERTIES
			ENVIRONMENT "TAU_PYTHON_MODULE_DIR=$<TARGET_FILE_DIR:${TAU_PYTHON_BINDING_MODULE}>")
	endforeach()
endif()

include(tau-cross-test-environment)
tau_set_cross_test_environment()
