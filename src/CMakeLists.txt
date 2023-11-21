cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
set(TAU "${CMAKE_SOURCE_DIR}/..")
list(APPEND CMAKE_MODULE_PATH ${TAU}/cmake)

set(TAU_HEADERS
	anf.h
	ba.h
	babdd.h
	barr.h
	bdd_handle.h
	defs.h
	bool.h
	rewriting.h
	dict.h
	msba.h
	normalizer.h
	normalizer2.h
	out.h
	formula.h
	seq.h
)

set(TAU_SOURCES
	dict.cpp
)

set(CLI_SOURCES main.cpp)


#
# include common CMAKE settings and target_setup() and exclude()
#
include(tau-common)

# target names
set(OBJECT_LIB_NAME "${PROJECT_LIB_NAME}o")
set(STATIC_LIB_NAME "${PROJECT_LIB_NAME}_static")
set(SHARED_LIB_NAME "${PROJECT_LIB_NAME}")
set(EXECUTABLE_NAME "${PROJECT_NAME}")
set(EXE_SHARED_NAME "${PROJECT_NAME}_shared")

#
# object library
#
add_library(${OBJECT_LIB_NAME} OBJECT)
target_sources(${OBJECT_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_setup(${OBJECT_LIB_NAME})
target_link_libraries(${OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB})
target_compile_options(${OBJECT_LIB_NAME} PRIVATE -fPIC)
target_include_directories(${OBJECT_LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/parser>
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/parser/src>
)

#
# static library
#
add_library(${STATIC_LIB_NAME} STATIC)
target_sources(${STATIC_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_setup(${STATIC_LIB_NAME})
target_link_libraries(${STATIC_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB})
target_include_directories(${STATIC_LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
)
set_target_properties(${STATIC_LIB_NAME} PROPERTIES
	OUTPUT_NAME ${PROJECT_LIB_NAME})
if(NOT BUILD_STATIC_LIBRARY)
	exclude(${STATIC_LIB_NAME})
endif()

#
# shared library
#
add_library(${SHARED_LIB_NAME} SHARED)
add_library(${namespace}::${SHARED_LIB_NAME} ALIAS ${SHARED_LIB_NAME})
target_sources(${SHARED_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_setup(${SHARED_LIB_NAME})
target_link_libraries(${SHARED_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB})
target_include_directories(${SHARED_LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/parser>
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external/parser/src>
	$<INSTALL_INTERFACE:.>
)
set_target_properties(${SHARED_LIB_NAME} PROPERTIES
	EXPORT_NAME ${SHARED_LIB_NAME}
	PUBLIC_HEADER "${TAU_HEADERS}"
)
if(NOT BUILD_SHARED_LIBRARY)
	exclude(${SHARED_LIB_NAME})
endif()

#
# executable
#
if(BUILD_EXECUTABLE)
        add_executable(${EXECUTABLE_NAME})
        target_sources(${EXECUTABLE_NAME} PRIVATE ${CLI_SOURCES})
        target_setup(${EXECUTABLE_NAME})
        target_link_libraries(${EXECUTABLE_NAME} ${STATIC_LIB_NAME})
endif(BUILD_EXECUTABLE)

#
# executable using shared library
#
if(BUILD_SHARED_EXECUTABLE)
        add_executable(${EXE_SHARED_NAME})
        target_sources(${EXE_SHARED_NAME} PRIVATE ${CLI_SOURCES})
        target_setup(${EXE_SHARED_NAME})
        target_link_libraries(${EXE_SHARED_NAME} ${SHARED_LIB_NAME})
endif(BUILD_SHARED_EXECUTABLE)

#
# install
#
include(GNUInstallDirs)

set(BUILD_TARGETS ${OBJECT_LIB_NAME})
if(BUILD_SHARED_LIBRARY)
	set(BUILD_TARGETS "${BUILD_TARGETS}" ${SHARED_LIB_NAME})
endif()
if(BUILD_STATIC_LIBRARY)
	set(BUILD_TARGETS "${BUILD_TARGETS}" ${STATIC_LIB_NAME})
endif()
if(BUILD_EXECUTABLE)
	set(BUILD_TARGETS "${BUILD_TARGETS}" ${EXECUTABLE_NAME})
endif()
if(BUILD_SHARED_EXECUTABLE)
	set(BUILD_TARGETS "${BUILD_TARGETS}" ${EXE_SHARED_NAME})
endif()

install(TARGETS ${BUILD_TARGETS}
	EXPORT TAU
	RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
	INCLUDES      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/TAU
	)
export(EXPORT TAU FILE ${TAU}/tau-config.cmake)
install(EXPORT TAU DESTINATION ${PROJECT_BINARY_DIR})
