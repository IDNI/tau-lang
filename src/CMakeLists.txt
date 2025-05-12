set(TAU "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(TAU_HEADERS
	defs.h             # common macro and types definitions
	logging.h          # logging and its macros
	tau_tree.h         # tau tree, node, tree traverser, builders,
	                   # extractors, printers, queries, and 
	                   # transformations from the parse tree
	ba_types.h         # BA types
	ba_constants.h     # BA constants pool and binder
	dict.h             # dictionaries of bdd var names and strings
	rr.h               # recurrence relation structure
	nso_rr.h           # nso_rr_apply
	execution.h        # execution of nso rr rules
	normal_forms.h     # normal form transformations
	normalizer.h
	satisfiability.h
	splitter_types.h
	splitter.h
	solver.h
	interpreter.h
	nso_factory.h      # abstract template interface
	boolean_algebras/bool_ba.h
	boolean_algebras/variant_ba.h
	boolean_algebras/nso_ba.h
	boolean_algebras/tau_ba.h
	boolean_algebras/sbf_ba.h
)

set(TAU_SOURCES
	../external/parser/src/parser.cpp
	../external/parser/src/recoders.cpp
	../external/parser/src/utility/characters.cpp
	../external/parser/src/utility/charclasses.cpp
	../external/parser/src/utility/term.cpp
	boolean_algebras/bool_ba.cpp
	dict.cpp
)

set(CLI_SOURCES
	main.cpp
	../external/parser/src/utility/cli.cpp
)

set(TAU_HEADER_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/tau.h.in)
set(TAU_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/tau.h)
set(TAU_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "TAU_HEADER_TEMPLATE: ${TAU_HEADER_TEMPLATE}")
message(STATUS "TAU_HEADER: ${TAU_HEADER}")
message(STATUS "TAU_SOURCE_DIR: ${TAU_SOURCE_DIR}")

# Custom command to generate the unified header
add_custom_command(
	OUTPUT ${TAU_HEADER}
	COMMAND ${CMAKE_COMMAND}
		-DTAU_HEADERS="${TAU_HEADERS}"
		-DTAU_HEADER_TEMPLATE="${TAU_HEADER_TEMPLATE}"
		-DTAU_HEADER="${TAU_HEADER}"
		-DTAU_SOURCE_DIR="${TAU_SOURCE_DIR}"
		-P ${PROJECT_SOURCE_DIR}/cmake/header.cmake
	DEPENDS ${TAU_HEADERS} ${TAU_HEADER_TEMPLATE}
	COMMENT "Building tau.h file"
)

add_custom_target(tau_header ALL DEPENDS ${TAU_HEADER})

#
# object library
#
add_library(${TAU_OBJECT_LIB_NAME} OBJECT)
target_sources(${TAU_OBJECT_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_setup(${TAU_OBJECT_LIB_NAME})
target_link_libraries(${TAU_OBJECT_LIB_NAME} Boost::log ${Z3_LIB})
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
target_link_libraries(${TAU_STATIC_LIB_NAME} Boost::log ${Z3_LIB})
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
target_link_libraries(${TAU_SHARED_LIB_NAME} Boost::log ${Z3_LIB})
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
