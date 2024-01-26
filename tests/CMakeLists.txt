set(DOCTEST_HEADER "${PROJECT_SOURCE_DIR}/src/doctest.h" CACHE PATH "Doctest header")
if (NOT EXISTS "${DOCTEST_HEADER}")
	message(STATUS "Downloading doctest to '${PROJECT_SOURCE_DIR}'")
	find_package(Wget REQUIRED)
	# TODO (MEDIUM) we should use a fixed tag or commit instead of master
	execute_process(COMMAND "${WGET_EXECUTABLE}" https://raw.githubusercontent.com/doctest/doctest/master/doctest/doctest.h -P ${PROJECT_SOURCE_DIR}/src)
endif ()

add_library(doctest INTERFACE)
target_compile_definitions(doctest INTERFACE TAU_USE_DOCTEST)
set(DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE "true")

add_link_options("-flto")

#
# Regression testing
#
set(TAU_BUILD_REGRESSION ON CACHE STRING "build the tau-lang regression suite ON")
set_property(CACHE TAU_BUILD_REGRESSION PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_REGRESSION)
	enable_testing()
	add_subdirectory(regression)
endif ()

#
# Integration testing
#
set(TAU_BUILD_INTEGRATION ON CACHE STRING "build the tau-lang integration suite ON")
set_property(CACHE TAU_BUILD_INTEGRATION PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_INTEGRATION)
	enable_testing()
	add_subdirectory(integration)
endif ()

#
# Unit testing
#
set(TAU_BUILD_TESTS ON CACHE STRING "build the tau-lang unit suite ON")
set_property(CACHE TAU_BUILD_TESTS PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_TESTS)
	enable_testing()
	add_subdirectory(unit)
endif ()

add_executable(debug-tau_normalizer-tau debug-tau_normalizer-tau.cpp)
target_setup(debug-tau_normalizer-tau)
target_link_libraries(debug-tau_normalizer-tau ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB})

add_executable(debug-tau_normalizer-bdd debug-tau_normalizer-bdd.cpp)
target_setup(debug-tau_normalizer-bdd)
target_link_libraries(debug-tau_normalizer-bdd ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB})

add_executable(debug-bdd_binding debug-bdd_binding.cpp)
target_setup(debug-bdd_binding)
target_link_libraries(debug-bdd_binding ${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB})
