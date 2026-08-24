set(TAU "${CMAKE_CURRENT_SOURCE_DIR}/..")

# The amalgamated tau.h is built from TAU_CORE_HEADERS -- generic core only, so
# its content does not change with -DTAU_BAS=. The pack arrives through the
# generated tau_pack.h, which tau.h.in includes last, so a consumer still needs
# only #include "tau.h". TAU_HEADERS stays the full list for dependency tracking.
set(TAU_CORE_HEADERS
	${TAU_GENERATED_PARSER_DIR}/tau_parser.generated.h
	backends/bdds/babdd.h
	backends/bdds/bdd_handle.h
	boolean_algebras/bool_ba.h	# described but not a TAU_BAS entry
	boolean_algebras/variant_ba.h
	boolean_algebras/nso_ba.h
	heuristics/ex_subs_based_elimination.h  # heuristic for eliminating existential quantifiers based on substitution
	defs.h             # common macro and types definitions
	logging.h          # logging and its macros
	tau_bdd.h          # BDD library over tau trees
	tau_tree.h         # tau tree, node, tree traverser, builders,
	                   # extractors, printers, queries, and
	                   # transformations from the parse tree
	ba_types.h         # BA types
	ba_constants.h     # BA constants pool and binder
	dict.h             # dictionaries of bdd var names and strings
	definitions.h      # Storage for function and predicate definitions
	rr.h               # recurrence relation structure
	nso_rr.h           # nso_rr_apply
	execution.h        # execution of nso rr rules
	normal_forms.h     # normal form transformations
	normal_forms_transformations.h
	normalizer.h
	satisfiability.h
	splitter_types.h
	splitter.h
	solver_types.h
	solver.h
	ba_types_inference.h
	base_ba_dispatcher.h
	interpreter.h
	pointwise_revision.h
	api.h
	union_find.h       # union find data structure
	union_find_with_sets.h
)
# undefs.h is not listed above: it undefines the implementation macros, so it
# has to come after the pack that tau.h.in includes last, not before it.

# core plus the configured pack: what the build depends on and installs
set(TAU_HEADERS ${TAU_CORE_HEADERS} ${TAU_BA_HEADERS})

# Grammar tables are mostly data and optimizing them takes too long. Every
# parser of this build -- core's and the pack's BAs' -- is generated, so the
# list comes from the generation rules rather than being named here.
set_source_files_properties(${TAU_GENERATED_PARSER_SOURCES}
	PROPERTIES COMPILE_OPTIONS "-O0;-fno-lto")

# Only what every pack has. A BA's own translation units are declared by its
# ba.cmake and arrive via TAU_BA_SOURCES_EXTRA; TAU_PACK_CORE_SOURCES is the
# resolver's pack-independent list (bool_ba.cpp stays: described like a BA,
# but in every pack), so a pack without a BA does not compile that BA's own.
set(TAU_SOURCES
	${TAU_GENERATED_PARSER_SOURCES}
	${TAU_BA_SOURCES_EXTRA}
	${TAU_PACK_CORE_SOURCES}
	instantiate_test_pack.cpp
)


set(CLI_SOURCES
	main.cpp
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
		-DTAU_HEADERS="${TAU_CORE_HEADERS}"
		-DTAU_HEADER_TEMPLATE="${TAU_HEADER_TEMPLATE}"
		-DTAU_HEADER="${TAU_HEADER}"
		-DTAU_SOURCE_DIR="${TAU_SOURCE_DIR}"
		-P ${PROJECT_SOURCE_DIR}/cmake/header.cmake
	DEPENDS ${TAU_CORE_HEADERS} ${TAU_HEADER_TEMPLATE}
	COMMENT "Building tau.h file"
)

add_custom_target(tau_header ALL DEPENDS ${TAU_HEADER})

#
# object library
#
add_library(${TAU_OBJECT_LIB_NAME} OBJECT)
target_sources(${TAU_OBJECT_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_setup(${TAU_OBJECT_LIB_NAME})
target_link_libraries(${TAU_OBJECT_LIB_NAME} tauparser Boost::log ${TAU_BA_LINK_LIBS})
if (TAU_BUILD_PIC)
	target_compile_options(${TAU_OBJECT_LIB_NAME} PRIVATE -fPIC)
endif()
target_include_directories(${TAU_OBJECT_LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${TAU}/src>
	$<BUILD_INTERFACE:${TAU}/parser>
	$<BUILD_INTERFACE:${TAU}/external/parser/src>
	$<BUILD_INTERFACE:${TAU}/external/cvc5/include>
	$<BUILD_INTERFACE:${TAU}/external/cvc5/build/include>
)
add_dependencies(${TAU_OBJECT_LIB_NAME} tau_header)

# Nothing links the object library: the static and shared libraries each compile
# TAU_SOURCES themselves, and no other target references it.  Left in the default
# build it recompiles the whole source set -- including the heaviest full-stack
# TUs -- for a result no target consumes, which is also what exhausted memory in
# parallel builds.  Still buildable on demand:
#   cmake --build <dir> --target TAUo
exclude(${TAU_OBJECT_LIB_NAME})

#
# static library
#
add_library(${TAU_STATIC_LIB_NAME} STATIC)
target_sources(${TAU_STATIC_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_setup(${TAU_STATIC_LIB_NAME})
target_link_libraries(${TAU_STATIC_LIB_NAME} tauparser Boost::log ${TAU_BA_LINK_LIBS})
if (TAU_BUILD_PIC)
	target_compile_options(${TAU_STATIC_LIB_NAME} PRIVATE -fPIC)
endif()
target_include_directories(${TAU_STATIC_LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${TAU}/src>
	$<BUILD_INTERFACE:${TAU}/parser>
	$<BUILD_INTERFACE:${TAU}/external/parser/src>
	$<BUILD_INTERFACE:${TAU}/external/cvc5/include>
	$<BUILD_INTERFACE:${TAU}/external/cvc5/build/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(${TAU_STATIC_LIB_NAME} PROPERTIES
	OUTPUT_NAME ${PROJECT_LIB_NAME})
add_dependencies(${TAU_STATIC_LIB_NAME} tau_header)

if(NOT TAU_BUILD_STATIC_LIBRARY)
	exclude(${TAU_STATIC_LIB_NAME})
endif()

# `tau compile` links this library as the emitted artifact's tau_prebuilt, so
# only here -- not the object or shared libraries -- pre-instantiates the
# artifact pack templates (src/artifact_pack.def) the artifact's main.cpp
# extern-declares, when TAU_ARTIFACT_PREINST is on. -O3 regardless of build
# type: an artifact from a devel or debug SDK must still run an optimized
# solve/normalize pipeline; DEBUG/NDEBUG is left untouched so this TU still
# matches the emitting build's ABI.
if(TAU_ARTIFACT_PREINST)
	target_sources(${TAU_STATIC_LIB_NAME} PRIVATE instantiate_artifact_pack.cpp)
	set_source_files_properties(instantiate_artifact_pack.cpp
		PROPERTIES COMPILE_OPTIONS "-O3")
endif()

#
# shared library
#
add_library(${TAU_SHARED_LIB_NAME} SHARED)
add_library(${namespace}::${TAU_SHARED_LIB_NAME} ALIAS ${TAU_SHARED_LIB_NAME})
target_sources(${TAU_SHARED_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_link_libraries(${TAU_SHARED_LIB_NAME} tauparser Boost::log ${TAU_BA_LINK_LIBS})
target_include_directories(${TAU_SHARED_LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${TAU}/src>
	$<BUILD_INTERFACE:${TAU}/parser>
	$<BUILD_INTERFACE:${TAU}/external/parser/src>
	$<BUILD_INTERFACE:${TAU}/external/cvc5/include>
	$<INSTALL_INTERFACE:.>
	$<BUILD_INTERFACE:${TAU}/external/cvc5/build/include>
)
set_target_properties(${TAU_SHARED_LIB_NAME} PROPERTIES
	EXPORT_NAME ${TAU_SHARED_LIB_NAME}
	PUBLIC_HEADER "${TAU_HEADERS}"
)
add_dependencies(${TAU_SHARED_LIB_NAME} tau_header)

if(NOT TAU_BUILD_SHARED_LIBRARY)
	exclude(${TAU_SHARED_LIB_NAME})
endif()

#
# executable
#

include(ftxui)

if(TAU_BUILD_EXECUTABLE)
	add_executable(${TAU_EXECUTABLE_NAME})
	target_sources(${TAU_EXECUTABLE_NAME} PRIVATE ${CLI_SOURCES})
	target_setup(${TAU_EXECUTABLE_NAME})
	target_link_libraries(${TAU_EXECUTABLE_NAME} ${TAU_STATIC_LIB_NAME} ${TAU_BA_LINK_LIBS})
	target_include_directories(${TAU_EXECUTABLE_NAME} PUBLIC
		$<BUILD_INTERFACE:${TAU}/src>
		$<BUILD_INTERFACE:${TAU}/parser>
		$<BUILD_INTERFACE:${TAU}/external/parser/src>
		$<BUILD_INTERFACE:${TAU}/external/cvc5/include>
		$<BUILD_INTERFACE:${TAU}/external/cvc5/build/include>
	)
	add_dependencies(${TAU_EXECUTABLE_NAME} tau_header)
	if(TAU_PARSER_HAS_FTXUI_DEP)
		target_link_libraries(${TAU_EXECUTABLE_NAME}
			ftxui::screen ftxui::dom ftxui::component)
		target_compile_definitions(${TAU_EXECUTABLE_NAME} PRIVATE TAU_PARSER_HAS_FTXUI)
		message(STATUS "tau: FTXUI REPL enabled (legacy via -X)")
	elseif(NOT TAU_DONT_USE_FTXUI)
		message(WARNING "tau: FTXUI not available — legacy terminal REPL will be used")
	endif()
endif(TAU_BUILD_EXECUTABLE)

#
# tau_codegen: tau→C++ program emitter.
# Always built; depends on the static library.
#
add_executable(tau_codegen tau_codegen_cli.cpp)
target_setup(tau_codegen)
target_link_libraries(tau_codegen ${TAU_STATIC_LIB_NAME} ${TAU_BA_LINK_LIBS})
target_include_directories(tau_codegen PUBLIC
	$<BUILD_INTERFACE:${TAU}/src>
	$<BUILD_INTERFACE:${TAU}/parser>
	$<BUILD_INTERFACE:${TAU}/external/parser/src>
	$<BUILD_INTERFACE:${TAU}/external/cvc5/include>
	$<BUILD_INTERFACE:${TAU}/external/cvc5/build/include>
)
add_dependencies(tau_codegen tau_header)

# Kept out of the default build: it is a full-stack translation unit that no
# test or other target needs (the cpp_codegen tests emit and compile code
# in-process, they do not run this binary).  Build it on demand with
#   cmake --build <dir> --target tau_codegen
# It is slated to be merged into the tau CLI as a compile/codegen subcommand.
exclude(tau_codegen)

#
# executable using shared library
#
if(TAU_BUILD_SHARED_EXECUTABLE)
        add_executable(${TAU_EXE_SHARED_NAME})
        target_sources(${TAU_EXE_SHARED_NAME} PRIVATE ${CLI_SOURCES})
        target_setup(${TAU_EXE_SHARED_NAME})
        target_link_libraries(${TAU_EXE_SHARED_NAME} ${TAU_SHARED_LIB_NAME} ${TAU_BA_LINK_LIBS})
	target_include_directories(${TAU_EXE_SHARED_NAME} PUBLIC
		$<BUILD_INTERFACE:${TAU}/src>
		$<BUILD_INTERFACE:${TAU}/parser>
		$<BUILD_INTERFACE:${TAU}/external/parser/src>
		$<BUILD_INTERFACE:${TAU}/external/cvc5/include>
		$<BUILD_INTERFACE:${TAU}/external/cvc5/build/include>
	)
	add_dependencies(${TAU_BUILD_SHARED_EXECUTABLE} tau_header)
	if(TAU_PARSER_HAS_FTXUI_DEP)
		target_link_libraries(${TAU_EXE_SHARED_NAME}
			ftxui::screen ftxui::dom ftxui::component)
		target_compile_definitions(${TAU_EXE_SHARED_NAME} PRIVATE TAU_PARSER_HAS_FTXUI)
	endif()
endif(TAU_BUILD_SHARED_EXECUTABLE)

#
# install
#

set(BUILD_TARGETS "")
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
