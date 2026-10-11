cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	message(FATAL_ERROR "run bindings cmake from root with -DTAU_BUILD_BINDING_PYTHON_CTYPE=ON")
endif()

add_library(tau_ctypes SHARED tau_ctypes.cpp)

add_dependencies(tau_ctypes tau_header)
set_target_properties(tau_ctypes PROPERTIES
	INSTALL_RPATH "${CVC5_LIBDIR}"
	CXX_STANDARD 23
)
target_compile_features(tau_ctypes PRIVATE cxx_std_23)
target_link_libraries(tau_ctypes PRIVATE ${TAU_STATIC_LIB_NAME})
# tauparser, Boost, and cvc5 include dirs arrive through TAU_static's link
# interface, as for the nanobind module.
target_include_directories(tau_ctypes PRIVATE
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/parser"
	"${TAU_PACK_INCLUDE_DIR}"
	"${TAU_GENERATED_PARSER_DIR}"
)

if(BUILD_TESTING)
	if(NOT Python_EXECUTABLE)
		find_package(Python COMPONENTS Interpreter REQUIRED)
	endif()
	add_test(NAME binding-python-ctypes-test_ctypes_limits
		COMMAND ${Python_EXECUTABLE}
			"${CMAKE_SOURCE_DIR}/tests/bindings/python/ctypes/test_ctypes_limits.py")
	set_tests_properties(binding-python-ctypes-test_ctypes_limits PROPERTIES
		ENVIRONMENT "TAU_LTL_LIB=$<TARGET_FILE:tau_ctypes>;PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}")
endif()
