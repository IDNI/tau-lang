include_guard(GLOBAL)

# FTXUI resolution for Tau's own targets.
#
# When the deps come from the store, the resolved FTXUI prefix is on
# CMAKE_PREFIX_PATH and a missing package is an error. Otherwise the parser's
# module resolves it (installed package or the pinned source).

if(TAU_DEPS_FROM_STORE)
	set(TAU_PARSER_HAS_FTXUI_DEP OFF)
	if(TAU_DONT_USE_FTXUI)
		message(STATUS "FTXUI: disabled (TAU_DONT_USE_FTXUI=ON)")
	else()
		find_package(ftxui CONFIG REQUIRED NO_CMAKE_FIND_ROOT_PATH)
		set_target_properties(ftxui::screen ftxui::dom ftxui::component
			PROPERTIES IMPORTED_GLOBAL TRUE)
		set(TAU_PARSER_HAS_FTXUI_DEP ON)
		message(STATUS "FTXUI: using installed package at ${ftxui_DIR}")
	endif()
else()
	# The parser's ftxui module lives beside its own cmake/, so that directory
	# joins the module path only on this path.
	list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/external/parser/cmake")
	include(ftxui)
endif()

set(TAU_PARSER_HAS_FTXUI_DEP ${TAU_PARSER_HAS_FTXUI_DEP} CACHE INTERNAL "FTXUI available")
