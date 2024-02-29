set(TAU "${CMAKE_CURRENT_SOURCE_DIR}/..")

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
	nso_rr.h
	seq.h
	satisfiability.h
	tau.h
	iterators.h
	executor.h
)

set(TAU_SOURCES
	bool.cpp
	dict.cpp
	iterators.cpp
	nso_rr.cpp
)

set(CLI_SOURCES
	main.cpp
	repl_evaluator.cpp
	../external/parser/src/cli.cpp)

#
# object library
#
add_library(${TAU_OBJECT_LIB_NAME} OBJECT)
target_sources(${TAU_OBJECT_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_setup(${TAU_OBJECT_LIB_NAME})
target_link_libraries(${TAU_OBJECT_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB})
target_compile_options(${TAU_OBJECT_LIB_NAME} PRIVATE -fPIC)
target_include_directories(${TAU_OBJECT_LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${TAU}/src>
	$<BUILD_INTERFACE:${TAU}/parser>
	$<BUILD_INTERFACE:${TAU}/external/parser/src>
)

#
# static library
#
add_library(${TAU_STATIC_LIB_NAME} STATIC)
target_sources(${TAU_STATIC_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_setup(${TAU_STATIC_LIB_NAME})
target_link_libraries(${TAU_STATIC_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB})
target_include_directories(${TAU_STATIC_LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${TAU}/src>
	$<BUILD_INTERFACE:${TAU}/parser>
	$<BUILD_INTERFACE:${TAU}/external/parser/src>
)
set_target_properties(${TAU_STATIC_LIB_NAME} PROPERTIES
	OUTPUT_NAME ${PROJECT_LIB_NAME})
if(NOT TAU_BUILD_STATIC_LIBRARY)
	exclude(${TAU_STATIC_LIB_NAME})
endif()

#
# shared library
#
add_library(${TAU_SHARED_LIB_NAME} SHARED)
add_library(${namespace}::${TAU_SHARED_LIB_NAME} ALIAS ${TAU_SHARED_LIB_NAME})
target_sources(${TAU_SHARED_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_setup(${TAU_SHARED_LIB_NAME})
target_link_libraries(${TAU_SHARED_LIB_NAME} ${IDNI_PARSER_OBJECT_LIB})
target_include_directories(${TAU_SHARED_LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${TAU}/src>
	$<BUILD_INTERFACE:${TAU}/parser>
	$<BUILD_INTERFACE:${TAU}/external/parser/src>
	$<INSTALL_INTERFACE:.>
)
set_target_properties(${TAU_SHARED_LIB_NAME} PROPERTIES
	EXPORT_NAME ${TAU_SHARED_LIB_NAME}
	PUBLIC_HEADER "${TAU_HEADERS}"
)
if(NOT TAU_BUILD_SHARED_LIBRARY)
	exclude(${TAU_SHARED_LIB_NAME})
endif()

#
# executable
#
if(TAU_BUILD_EXECUTABLE)
        add_executable(${TAU_EXECUTABLE_NAME})
        target_sources(${TAU_EXECUTABLE_NAME} PRIVATE ${CLI_SOURCES})
        target_setup(${TAU_EXECUTABLE_NAME})
        target_link_libraries(${TAU_EXECUTABLE_NAME} ${TAU_STATIC_LIB_NAME})
	target_include_directories(${TAU_EXECUTABLE_NAME} PUBLIC
		$<BUILD_INTERFACE:${TAU}/src>
		$<BUILD_INTERFACE:${TAU}/parser>
		$<BUILD_INTERFACE:${TAU}/external/parser/src>
	)
endif(TAU_BUILD_EXECUTABLE)

#
# executable using shared library
#
if(TAU_BUILD_SHARED_EXECUTABLE)
        add_executable(${TAU_EXE_SHARED_NAME})
        target_sources(${TAU_EXE_SHARED_NAME} PRIVATE ${CLI_SOURCES})
        target_setup(${TAU_EXE_SHARED_NAME})
        target_link_libraries(${TAU_EXE_SHARED_NAME} ${TAU_SHARED_LIB_NAME})
	target_include_directories(${TAU_EXE_SHARED_NAME} PUBLIC
		$<BUILD_INTERFACE:${TAU}/src>
		$<BUILD_INTERFACE:${TAU}/parser>
		$<BUILD_INTERFACE:${TAU}/external/parser/src>
	)
endif(TAU_BUILD_SHARED_EXECUTABLE)

#
# install
#
include(GNUInstallDirs)

set(BUILD_TARGETS ${TAU_OBJECT_LIB_NAME})
if(BUILD_SHARED_LIBRARY)
	set(BUILD_TARGETS "${BUILD_TARGETS}" ${TAU_SHARED_LIB_NAME})
endif()
if(BUILD_STATIC_LIBRARY)
	set(BUILD_TARGETS "${BUILD_TARGETS}" ${TAU_STATIC_LIB_NAME})
endif()
if(BUILD_EXECUTABLE)
	set(BUILD_TARGETS "${BUILD_TARGETS}" ${TAU_EXECUTABLE_NAME})
endif()
if(BUILD_SHARED_EXECUTABLE)
	set(BUILD_TARGETS "${BUILD_TARGETS}" ${TAU_EXE_SHARED_NAME})
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
