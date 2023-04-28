cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

function(add_git_submodule dir)
find_package(Git REQUIRED)

if(NOT EXISTS ${dir}/CMakeLists.txt)
  execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${dir}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND_ERROR_IS_FATAL ANY)
endif()

add_subdirectory(${dir})

endfunction(add_git_submodule)
