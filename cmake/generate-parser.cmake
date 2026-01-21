cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

function(generate_parser tgf_filename)
	execute_process(COMMAND "${PROJECT_SOURCE_DIR}/parser/gen" "${tgf_filename}"
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endfunction(generate_parser)
