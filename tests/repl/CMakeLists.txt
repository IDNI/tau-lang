#
# REPL testing
#
set_property(CACHE TAU_BUILD_REPL_TESTS PROPERTY STRINGS "OFF" "ON")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if (TAU_BUILD_REPL_TESTS)
	enable_testing()
	add_subdirectory(commands)
endif ()
