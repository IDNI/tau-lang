cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# Generate one grammar's parser into the build tree.
#
# A build rule rather than a configure-time step, so only a grammar whose .tgf
# changed is regenerated, and whatever includes the header waits for it. The
# outputs live in the build tree, so nothing is written into the source tree and
# each build directory keeps its own copy.
#
# `--header-only false` puts the grammar table in a .cpp compiled once, instead
# of in the header where every including TU pays for it.
function(generate_parser tgf_filename out_dir)
	get_filename_component(_stem "${tgf_filename}" NAME_WE)
	set(_header "${out_dir}/${_stem}_parser.generated.h")
	set(_source "${out_dir}/${_stem}_parser.generated.cpp")

	add_custom_command(
		OUTPUT "${_header}" "${_source}"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${out_dir}"
		COMMAND $<TARGET_FILE:tgf> "${tgf_filename}" gen
			--header-only false --output-dir "${out_dir}"
		DEPENDS tgf "${tgf_filename}"
		COMMENT "Generating parser from ${_stem}.tgf"
		VERBATIM)

	list(APPEND TAU_GENERATED_PARSER_HEADERS "${_header}")
	list(APPEND TAU_GENERATED_PARSER_SOURCES "${_source}")
	set(TAU_GENERATED_PARSER_HEADERS "${TAU_GENERATED_PARSER_HEADERS}"
		PARENT_SCOPE)
	set(TAU_GENERATED_PARSER_SOURCES "${TAU_GENERATED_PARSER_SOURCES}"
		PARENT_SCOPE)
endfunction(generate_parser)
