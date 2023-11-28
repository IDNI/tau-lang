cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(DOCTEST_HEADER "${PROJECT_SOURCE_DIR}/src/doctest.h" CACHE PATH "Doctest header")
if (NOT EXISTS "${DOCTEST_HEADER}")
	message(STATUS "Downloading doctest to '${PROJECT_SOURCE_DIR}'")
	find_package(Wget REQUIRED)
	# TODO (MEDIUM) we should use a fixed tag instead of master
	execute_process(COMMAND "${WGET_EXECUTABLE}" https://raw.githubusercontent.com/doctest/doctest/master/doctest/doctest.h -P ${PROJECT_SOURCE_DIR}/src)
endif ()

target_compile_definitions(doctest INTERFACE TAU_USE_DOCTEST)
set(DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE "true")

add_link_options("-flto")

set(REGRESSION_SAMPLE regression_sample.cpp)
add_executable(regression_sample ${REGRESSION_SAMPLE})
target_setup(regression_sample)
target_link_libraries(regression_sample TAUo doctest)


add_test(NAME regression_sample COMMAND "${PROJECT_BINARY_DIR}/regression_sample")
