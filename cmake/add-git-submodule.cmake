cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

function(add_git_submodule dir)

	find_package(Git REQUIRED)

	if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git" AND NOT EXISTS "${dir}/CMakeLists.txt")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- "${dir}"
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		endif()
	endif()

	add_subdirectory(${dir})

endfunction(add_git_submodule)
