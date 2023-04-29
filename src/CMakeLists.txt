cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
set(CMAKE_VERBOSE_MAKEFILE true)

project(tau-lang)

if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES Debug Release)
	set(CMAKE_CONFIGURATION_TYPES
		"${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "" FORCE
	)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(TAU "${CMAKE_CURRENT_SOURCE_DIR}/..")
list(APPEND CMAKE_MODULE_PATH ${TAU}/cmake)
include(tau-common)

set(TAU_HEADERS
	anf.h
	ba.h
	babdd.h
	barr.h
	bdd_handle.h
	combinators.h
	defs.h
	bool.h
	builder.h
	dict.h
	msba.h
	normalizer.h
	out.h
	seq.h
)

set(TAU_SOURCES
	dict.cpp
)

set(CLI_SOURCES tau.cpp)

set(TAU_DEBUG_OPTIONS "-O0;-DDEBUG;-ggdb3")
set(TAU_RELEASE_OPTIONS "-O3;-DNDEBUG")

set(TAU_COMPILE_OPTIONS
	"$<IF:$<CONFIG:Debug>,${TAU_DEBUG_OPTIONS},${TAU_RELEASE_OPTIONS}>"
	"$<$<CONFIG:Release>:-flto=auto>"
)
set(TAU_LINK_OPTIONS "-flto=auto")

#
# object library
#
add_library(TAUo OBJECT)
target_setup(TAUo)
target_compile_options(TAUo     PRIVATE ${TAU_COMPILE_OPTIONS} -fPIC)
target_link_options(TAUo        PRIVATE ${TAU_LINK_OPTIONS})
target_sources(TAUo             PRIVATE ${TAU_SOURCES} ${TAU_HEADERS})
target_link_libraries(TAUo      ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(TAUo
	${OBJECT_LIB_NAME} idni_parser)
target_include_directories(TAUo PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/parser>	
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/parser/src>)	
target_include_directories(TAUo PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#
# shared library
#
add_library(TAUso SHARED)
add_library(TAU::TMLso ALIAS TAUso)
target_setup(TAUso)
target_compile_options(TAUso     PRIVATE ${TAU_COMPILE_OPTIONS})
target_link_options(TAUso        PRIVATE ${TAU_LINK_OPTIONS})
target_link_libraries(TAUso      TAUo)
target_link_libraries(TAUso
	${OBJECT_LIB_NAME} idni_parser)
target_include_directories(TAUso PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/parser>	
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/parser/src>)	
target_include_directories(TAUso PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:.>
)
set_target_properties(TAUso PROPERTIES
	EXPORT_NAME TAUso
	PUBLIC_HEADER "${TAU_HEADERS}"
)

#
# static library
#
add_library(TAUs STATIC)
add_library(TAU::TAUs ALIAS TAUs)
target_setup(TAUs)
target_compile_options(TAUs     PRIVATE ${TAU_COMPILE_OPTIONS})
target_link_options(TAUs        PRIVATE ${TAU_LINK_OPTIONS})
target_link_libraries(TAUs      TAUo)
target_include_directories(TAUs 
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
set_target_properties(TAUs PROPERTIES OUTPUT_NAME "TAU")

#
# executable
#
add_executable(tau)
target_setup(tau)
target_compile_options(tau PRIVATE ${TAU_COMPILE_OPTIONS})
target_link_options(tau    PRIVATE ${TAU_LINK_OPTIONS})
target_sources(tau         PRIVATE ${CLI_SOURCES})
target_link_libraries(tau  TAUs)
if (WITH_EXCEPTIONS)
	target_compile_definitions(tau PRIVATE "-DWITH_EXCEPTIONS")
endif ()

#
# executable using shared library
#
add_executable(tau_shared)
target_setup(tau_shared)
target_compile_options(tau_shared PRIVATE ${TAU_COMPILE_OPTIONS})
target_link_options(tau_shared    PRIVATE ${TAU_LINK_OPTIONS})
target_sources(tau_shared         PRIVATE ${CLI_SOURCES})
target_link_libraries(tau_shared  TAUso)
exclude(tau_shared)

#
# install
#
include(GNUInstallDirs)
install(TARGETS tau TAUso TAUo
	EXPORT TAU
	LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/TAU
	RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}
)
export(EXPORT TAU FILE ${TAU}/tau-config.cmake)
install(EXPORT TAU DESTINATION ${PROJECT_BINARY_DIR})
