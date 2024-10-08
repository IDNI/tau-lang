cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(GEN_DIR "${PROJECT_SOURCE_DIR}/parser")
set(GEN_EXECUTABLE "${GEN_DIR}/gen")

function(generate_parser tgf_filename)
	execute_process(COMMAND ${GEN_EXECUTABLE} "${tgf_filename}"
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endfunction(generate_parser)
