cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	message(FATAL_ERROR "run bindings cmake from root with -DTAU_BUILD_BINDING_PYTHON_NANOBIND=ON and/or -DTAU_BUILD_BINDING_PYTHON_CTYPE=ON")
endif()

if (TAU_BUILD_BINDING_PYTHON_CTYPE)
	add_subdirectory(ctypes)
endif()

if (TAU_BUILD_BINDING_PYTHON_NANOBIND)
	add_subdirectory(nanobind)
endif()
