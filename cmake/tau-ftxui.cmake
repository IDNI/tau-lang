include_guard(GLOBAL)

# FTXUI resolution for Tau's own targets.
#
# The resolved FTXUI store prefix is on CMAKE_PREFIX_PATH, and a missing
# package is an error.

set(TAU_PARSER_HAS_FTXUI_DEP OFF)
if(TAU_DONT_USE_FTXUI)
	message(STATUS "FTXUI: disabled (TAU_DONT_USE_FTXUI=ON)")
else()
	tau_deps_drop_stale_dirs("${TAU_FTXUI_PREFIX}" "^ftxui_DIR$")
	find_package(ftxui CONFIG REQUIRED NO_CMAKE_FIND_ROOT_PATH)
	set_target_properties(ftxui::screen ftxui::dom ftxui::component
		PROPERTIES IMPORTED_GLOBAL TRUE)
	set(TAU_PARSER_HAS_FTXUI_DEP ON)
	message(STATUS "FTXUI: using installed package at ${ftxui_DIR}")
endif()

set(TAU_PARSER_HAS_FTXUI_DEP ${TAU_PARSER_HAS_FTXUI_DEP} CACHE INTERNAL "FTXUI available")
