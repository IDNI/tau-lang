cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# Regenerate one grammar's parser as a build rule rather than at configure time,
# so only a grammar whose .tgf changed is regenerated and whatever includes the
# generated header waits for it.
#
# The output sits beside its .tgf and is committed, so a build without
# TAU_GENERATE_PARSERS never needs the tgf tool.
function(generate_parser tgf_filename)
	get_filename_component(_dir "${tgf_filename}" DIRECTORY)
	get_filename_component(_stem "${tgf_filename}" NAME_WE)
	set(_header "${_dir}/${_stem}_parser.generated.h")

	add_custom_command(
		OUTPUT "${_header}"
		COMMAND "${PROJECT_SOURCE_DIR}/parser/gen" "${tgf_filename}"
		DEPENDS "${tgf_filename}"
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		COMMENT "Generating parser from ${_stem}.tgf"
		VERBATIM)

	list(APPEND TAU_GENERATED_PARSER_HEADERS "${_header}")
	set(TAU_GENERATED_PARSER_HEADERS "${TAU_GENERATED_PARSER_HEADERS}"
		PARENT_SCOPE)
endfunction(generate_parser)
