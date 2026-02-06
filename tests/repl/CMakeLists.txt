#
# REPL testing
#
set(TAU_BUILD_REPL_TESTS ON CACHE STRING "build the tau-lang repl suite ON")
set_property(CACHE TAU_BUILD_REPL_TESTS PROPERTY STRINGS "OFF" "ON")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if (TAU_BUILD_REPL_TESTS)
	enable_testing()
	add_subdirectory(commands)
endif ()
