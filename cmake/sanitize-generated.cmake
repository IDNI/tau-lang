# cmake/sanitize-generated.cmake
#
# Rewrite the generator's absolute grammar path in a generated parser header to
# the grammar's base name. Run as a build step after the header is generated, so
# neither the header an installed SDK ships nor the amalgamated tau.h carries a
# build, store, or home path.
#
#   cmake -DHEADER=<generated header> -P sanitize-generated.cmake

if(NOT DEFINED HEADER OR NOT EXISTS "${HEADER}")
	message(FATAL_ERROR "sanitize-generated: HEADER is not set or missing: '${HEADER}'")
endif()

file(READ "${HEADER}" _content)
string(REGEX MATCH "generated from a file ([^\n]*) by" _match "${_content}")
if(_match)
	get_filename_component(_grammar "${CMAKE_MATCH_1}" NAME)
	string(REPLACE "${CMAKE_MATCH_1}" "${_grammar}" _content "${_content}")
	file(WRITE "${HEADER}" "${_content}")
endif()
