set(TAU "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(TAU_HEADERS
	../parser/sbf_parser.generated.h
	../parser/tau_parser.generated.h
	../parser/bitvector_parser.generated.h
	../parser/nlang_parser.generated.h
	boolean_algebras/cvc5/cvc5_options.h    # runtime cvc5 option-set and solver placement parameters
	boolean_algebras/cvc5/cvc5.h
	boolean_algebras/bdds/babdd.h
	boolean_algebras/bdds/bdd_handle.h
	boolean_algebras/bool_ba.h
	boolean_algebras/sbf_ba.h
	boolean_algebras/variant_ba.h
	boolean_algebras/nso_ba.h
	boolean_algebras/tau_ba.h
	boolean_algebras/bv_ba.h
	heuristics/bv_ba_simplification.h		# heuristic for simplifying boolean formulas with bitvectors
	heuristics/ex_subs_based_elimination.h  # heuristic for eliminating existential quantifiers based on substitution
	heuristics/blast_placement.h            # runtime placement parameters for blasting and the solver
	heuristics/bv_predicate_blasting.h      # heuristic for predicate blasting bitvector formulas
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
	solver.h
	ba_types_inference.h
	base_ba_dispatcher.h
	interpreter.h
	pointwise_revision.h
	api.h
	union_find.h       # union find data structure
	union_find_with_sets.h
	undefs.h           # undefines implementation macros
)

# grammar tables, compiled here instead of in every TU including the parser
set(TAU_PARSER_SOURCES
	../parser/tau_parser.generated.cpp
	../parser/sbf_parser.generated.cpp
)

# do not optimize parser sources, they are mostly data and optimization takes too longwhat
set_source_files_properties(${TAU_PARSER_SOURCES}
	PROPERTIES COMPILE_OPTIONS "-O0;-fno-lto")

set(TAU_SOURCES
	${TAU_PARSER_SOURCES}
	boolean_algebras/bool_ba.cpp
	boolean_algebras/bdds/var_dict.cpp
	boolean_algebras/nlang_ba.cpp
	boolean_algebras/qint.cpp
	boolean_algebras/qlt.cpp
	instantiate_test_pack.cpp
	base_ba_dispatcher_bv_sbf.cpp
	base_ba_dispatcher_full_pack.cpp
	base_ba_dispatcher_bv_bool.cpp
)

find_package(CURL REQUIRED)

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
target_link_libraries(${TAU_OBJECT_LIB_NAME} tauparser Boost::log cvc5::cvc5)
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
target_link_libraries(${TAU_STATIC_LIB_NAME} tauparser Boost::log cvc5::cvc5 CURL::libcurl)
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

#
# shared library
#
add_library(${TAU_SHARED_LIB_NAME} SHARED)
add_library(${namespace}::${TAU_SHARED_LIB_NAME} ALIAS ${TAU_SHARED_LIB_NAME})
target_sources(${TAU_SHARED_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_link_libraries(${TAU_SHARED_LIB_NAME} tauparser Boost::log cvc5::cvc5 CURL::libcurl)
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
	target_link_libraries(${TAU_EXECUTABLE_NAME} ${TAU_STATIC_LIB_NAME} cvc5::cvc5)
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
target_link_libraries(tau_codegen ${TAU_STATIC_LIB_NAME} cvc5::cvc5)
target_include_directories(tau_codegen PUBLIC
	$<BUILD_INTERFACE:${TAU}/src>
	$<BUILD_INTERFACE:${TAU}/parser>
	$<BUILD_INTERFACE:${TAU}/external/parser/src>
	$<BUILD_INTERFACE:${TAU}/external/cvc5/include>
	$<BUILD_INTERFACE:${TAU}/external/cvc5/build/include>
)
add_dependencies(tau_codegen tau_header)

# Kept out of the default build when tests are off: it is a full-stack
# translation unit that no non-test target needs (the cpp_codegen tests emit
# and compile code in-process, they do not run this binary). Build it on
# demand with
#   cmake --build <dir> --target tau_codegen
# It is slated to be merged into the tau CLI as a compile/codegen subcommand.
# tests/repl/commands/test_repl-codegen_cli.cmake DOES run this binary
# directly (CG-N6/CG-R1 exit-code coverage), so a *-tests build must include
# it in the default target set.
if(NOT TAU_BUILD_TESTS)
	exclude(tau_codegen)
endif()

#
# executable using shared library
#
if(TAU_BUILD_SHARED_EXECUTABLE)
        add_executable(${TAU_EXE_SHARED_NAME})
        target_sources(${TAU_EXE_SHARED_NAME} PRIVATE ${CLI_SOURCES})
        target_setup(${TAU_EXE_SHARED_NAME})
        target_link_libraries(${TAU_EXE_SHARED_NAME} ${TAU_SHARED_LIB_NAME} cvc5::cvc5)
	target_include_directories(${TAU_EXE_SHARED_NAME} PUBLIC
		$<BUILD_INTERFACE:${TAU}/src>
		$<BUILD_INTERFACE:${TAU}/parser>
		$<BUILD_INTERFACE:${TAU}/external/parser/src>
		$<BUILD_INTERFACE:${TAU}/external/cvc5/include>
		$<BUILD_INTERFACE:${TAU}/external/cvc5/build/include>
	)
	add_dependencies(${TAU_EXE_SHARED_NAME} tau_header)
	if(TAU_PARSER_HAS_FTXUI_DEP)
		target_link_libraries(${TAU_EXE_SHARED_NAME}
			ftxui::screen ftxui::dom ftxui::component)
		target_compile_definitions(${TAU_EXE_SHARED_NAME} PRIVATE TAU_PARSER_HAS_FTXUI)
	endif()
endif(TAU_BUILD_SHARED_EXECUTABLE)

#
# install
#

