#
# REPL testing
#
set(TAU_BUILD_REPL_TESTS ON CACHE STRING "build the tau-lang repl suite ON")
set_property(CACHE TAU_BUILD_REPL_TESTS PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_REPL_TESTS)
	enable_testing()
	add_subdirectory(commands)
	add_subdirectory(qelim)
	add_subdirectory(normalizer)
	add_subdirectory(satisfiability)
	add_subdirectory(execution)
	add_subdirectory(normal_forms)
endif ()
