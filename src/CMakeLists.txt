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
	heuristics/preprocess_placement.h       # runtime placement parameters for preprocessing and the solver
	heuristics/simplify_options.h           # runtime option-set for bv simplification heuristics
	defs.h             # common macro and types definitions
	tau_string_hash.h  # string hash of names and constants
	logging.h          # logging and its macros
	env_limits.h       # environment fallback of a numeric runtime limit
	reset_hooks.h      # caches api::reset empties
	tau_bdd.h          # BDD library over tau trees
	tau_tree.h         # tau tree, node, tree traverser, builders,
	                   # extractors, printers, queries, and
	                   # transformations from the parse tree
	ba_types.h         # BA types
	adt/adt_types.h    # ADT type registry
	adt/adt_flatten.h  # ADT flattener
	ba_constants.h     # BA constants pool and binder
	dict.h             # dictionaries of bdd var names and strings
	definitions.h      # Storage for function and predicate definitions
	rr.h               # recurrence relation structure
	nso_rr.h           # nso_rr_apply
	execution.h        # execution of nso rr rules
	normal_forms.h     # normal form transformations
	antiprenexing/antiprenexing.h  # antiprenexing: eliminability, block atom
	                                # profile/squeeze, boole atom analysis
	antiprenexing/leaf_clause.h    # leaf-clause analysis used by normal_forms.h
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
	tau_diagnostics.h  # idni::tau_lang aliases for the parser library's diagnostics types
	tau_memory_budget.h # cap on the interned tree store, checked on entry to each api call
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
	instantiate_pack.cpp
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
# a file-level DEPENDS across directory scopes is not enough for Unix Makefiles
add_dependencies(tau_header tau_parsers)

#
# object library
#
add_library(${TAU_OBJECT_LIB_NAME} OBJECT)
target_sources(${TAU_OBJECT_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_setup(${TAU_OBJECT_LIB_NAME})
target_link_libraries(${TAU_OBJECT_LIB_NAME} tauparser ${TAU_BOOST_LIBS} ${TAU_BA_LINK_LIBS})
if (TAU_BUILD_PIC AND NOT MSVC)
	target_compile_options(${TAU_OBJECT_LIB_NAME} PRIVATE -fPIC)
endif()
target_include_directories(${TAU_OBJECT_LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${TAU}/src>
	$<BUILD_INTERFACE:${TAU}/parser>
	${TAU_PARSER_SOURCE_INCLUDE}
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
target_link_libraries(${TAU_STATIC_LIB_NAME} tauparser ${TAU_BOOST_LIBS} ${TAU_BA_LINK_LIBS})
if (TAU_BUILD_PIC AND NOT MSVC)
	target_compile_options(${TAU_STATIC_LIB_NAME} PRIVATE -fPIC)
endif()
# An instrumented archive needs the gcov runtime wherever it is linked, the
# program a `tau compile` builds included.
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
	target_link_options(${TAU_STATIC_LIB_NAME} INTERFACE --coverage)
endif()
target_include_directories(${TAU_STATIC_LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${TAU}/src>
	$<BUILD_INTERFACE:${TAU}/parser>
	${TAU_PARSER_SOURCE_INCLUDE}
	# Generated headers an artifact compiles against: the pack header, the core
	# parsers and the pack's own, plus any registered out-of-tree BA's.
	$<BUILD_INTERFACE:${TAU_PACK_INCLUDE_DIR}>
	$<BUILD_INTERFACE:${TAU_GENERATED_PARSER_DIR}>
)
if(TAU_BA_INCLUDE_DIRS)
	target_include_directories(${TAU_STATIC_LIB_NAME} PUBLIC
		$<BUILD_INTERFACE:${TAU_BA_INCLUDE_DIRS}>)
endif()
set_target_properties(${TAU_STATIC_LIB_NAME} PROPERTIES
	OUTPUT_NAME ${PROJECT_LIB_NAME})
add_dependencies(${TAU_STATIC_LIB_NAME} tau_header)

if(NOT TAU_BUILD_STATIC_LIBRARY)
	exclude(${TAU_STATIC_LIB_NAME})
endif()

# `tau compile` links this library as the emitted artifact's tau_prebuilt, so
# only here -- not the object or shared libraries -- pre-instantiates the
# artifact pack templates (src/artifact_pack.def) the artifact's main.cpp
# extern-declares, when TAU_ARTIFACT_PREINST is on.
if(TAU_ARTIFACT_PREINST)
	target_sources(${TAU_STATIC_LIB_NAME} PRIVATE instantiate_artifact_pack.cpp)
	if(MSVC)
		# /GL+LTCG rejects this TU (LNK1377 on tau_ba::tau_ba). It
		# instantiates the whole artifact pack; keep it out of LTCG.
		set_source_files_properties(instantiate_artifact_pack.cpp
			PROPERTIES COMPILE_OPTIONS "/GL-")
	endif()
endif()

# The exported target carries what an artifact needs to compile against the
# archive with the same ABI: the C++ standard, the interface definitions the
# archive was built with, and DEBUG where the archive has it. Shared by the
# build-tree SDK and the install export.
target_compile_features(${TAU_STATIC_LIB_NAME} INTERFACE cxx_std_23)
target_compile_definitions(${TAU_STATIC_LIB_NAME} INTERFACE
	${TAU_RESOLVED_COMPILE_DEFINITIONS}
	TAU_PARSER_NO_TGF
	"TAU_PACK_BOOL_CARRIERS=\"${TAU_PACK_BOOL_CARRIERS_RESOLVED}\"")
if(TAU_ARTIFACT_PREINST)
	target_compile_definitions(${TAU_STATIC_LIB_NAME} INTERFACE
		TAU_CODEGEN_ARTIFACT_PREINST)
endif()
if(CMAKE_BUILD_TYPE MATCHES "^(Debug|Devel|Coverage)$")
	# An artifact must not compile NDEBUG into headers the archive compiled
	# with asserts live.
	target_compile_definitions(${TAU_STATIC_LIB_NAME} INTERFACE DEBUG)
	if(MSVC)
		target_compile_options(${TAU_STATIC_LIB_NAME} INTERFACE "/UNDEBUG")
	else()
		target_compile_options(${TAU_STATIC_LIB_NAME} INTERFACE "-UNDEBUG")
	endif()
endif()
if(EMSCRIPTEN)
	# Every linked object must agree on the exception encoding and on -pthread;
	# the archive and the artifact TU would otherwise fail wasm-ld.
	target_compile_options(${TAU_STATIC_LIB_NAME} INTERFACE
		-fwasm-exceptions -sWASM_LEGACY_EXCEPTIONS=0)
	target_link_options(${TAU_STATIC_LIB_NAME} INTERFACE
		-fwasm-exceptions -sWASM_LEGACY_EXCEPTIONS=0)
	if(TAU_WASM_PTHREADS)
		target_compile_options(${TAU_STATIC_LIB_NAME} INTERFACE -pthread)
		target_link_options(${TAU_STATIC_LIB_NAME} INTERFACE -pthread)
	endif()
endif()
if(TAU_ADDRESS_SANITIZER AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	# The archive references the asan runtime, so an artifact must link it too.
	target_compile_options(${TAU_STATIC_LIB_NAME} INTERFACE -fsanitize=address)
	target_link_options(${TAU_STATIC_LIB_NAME} INTERFACE -fsanitize=address)
endif()
set_target_properties(${TAU_STATIC_LIB_NAME} PROPERTIES EXPORT_NAME TAU)

# cvc5 and curl are dependencies only when the pack this library was built for
# links them.
if("cvc5" IN_LIST TAU_BA_REQUIRED_PACKAGES)
	set(TAU_SDK_FIND_CVC5 "find_dependency(cvc5 CONFIG)")
else()
	set(TAU_SDK_FIND_CVC5
		"# cvc5 is not required by TAU_BAS=${TAU_BAS}; this SDK does not need it")
endif()
if("CURL" IN_LIST TAU_BA_REQUIRED_PACKAGES)
	set(TAU_SDK_FIND_CURL "find_dependency(CURL)")
else()
	set(TAU_SDK_FIND_CURL
		"# curl is not required by TAU_BAS=${TAU_BAS}; this SDK does not need it")
endif()
# Every box installs under its own root, so two platforms' SDKs coexist in one
# prefix instead of overwriting each other's headers and libraries. The
# platform is the build folder name, the same value `./dev preset` names.
set(TAU_SDK_INSTALL_ROOT "${CMAKE_INSTALL_LIBDIR}/tau/sdk/${TAU_PLATFORM_NAME}")
set(TAU_SDK_CMAKE_DIR "${TAU_SDK_INSTALL_ROOT}/lib/cmake/Tau")
include(CMakePackageConfigHelpers)

#
# Build-tree SDK: the compile script, the platform map and a package config, so
# `tau compile` and the codegen tests resolve a box without an install; `tau
# gen` needs none, since it carries the artifact templates itself. Keyed on the
# archive target, not on TAU_BUILD_STATIC_LIBRARY: the target is built whenever
# the executable or a test links it, even when the option only excludes it from
# the default build.
#
if(TARGET ${TAU_STATIC_LIB_NAME} AND TAU_DEPS_FROM_STORE)
	# The export set `export(EXPORT)` names is created here; the install rule
	# it implies only matters to `cmake --install`, and must land where the
	# installed export says it does.
	install(TARGETS ${TAU_STATIC_LIB_NAME}
		EXPORT TauTargets
		ARCHIVE DESTINATION ${TAU_SDK_INSTALL_ROOT}/lib
		COMPONENT "Library"
		INCLUDES DESTINATION ${TAU_SDK_INSTALL_ROOT}/include)
	set(TAU_SDK_DIR "${CMAKE_BINARY_DIR}/sdk")
	file(MAKE_DIRECTORY "${TAU_SDK_DIR}/cmake")
	export(EXPORT TauTargets
		FILE "${TAU_SDK_DIR}/TauTargets.cmake"
		NAMESPACE TAU::)
	configure_package_config_file(
		"${PROJECT_SOURCE_DIR}/cmake/TauConfig.cmake.in"
		"${TAU_SDK_DIR}/TauConfig.cmake"
		INSTALL_DESTINATION "sdk")
	write_basic_package_version_file(
		"${TAU_SDK_DIR}/TauConfigVersion.cmake"
		VERSION "${PROJECT_VERSION}"
		COMPATIBILITY AnyNewerVersion)
	configure_file("${PROJECT_SOURCE_DIR}/cmake/tau-compile.cmake"
		"${TAU_SDK_DIR}/cmake/tau-compile.cmake" COPYONLY)
	# The artifact's wasm template includes these before project().
	configure_file("${PROJECT_SOURCE_DIR}/cmake/tau-shared-emscripten.cmake"
		"${TAU_SDK_DIR}/cmake/tau-shared-emscripten.cmake" COPYONLY)
	configure_file("${PROJECT_SOURCE_DIR}/cmake/tau-shared-resolve.cmake"
		"${TAU_SDK_DIR}/cmake/tau-shared-resolve.cmake" COPYONLY)
	# The artifact presets, their include and their toolchains travel inside
	# tau (cmake/tau_bas.cmake), so only the platform map an SDK consumer reads
	# at compile time is written here.
	tau_write_platform_map("${TAU_SDK_DIR}/cmake/tau-platforms.cmake")
	# The platform this SDK serves, so a box whose path names no platform
	# (the flat Windows zip) can still be resolved. Also the toolchain this box
	# was built with: tau-compile.cmake warns on a host mismatch.
	file(WRITE "${TAU_SDK_DIR}/cmake/tau-this-platform.txt"
		"${TAU_PLATFORM_NAME}\n")
	if(EMSCRIPTEN)
		set(_tau_sdk_toolchain_id "Emscripten")
		set(_tau_sdk_toolchain_version "")
		if(EXISTS "${EMSCRIPTEN_DIR}/emscripten-version.txt")
			file(READ "${EMSCRIPTEN_DIR}/emscripten-version.txt"
				_tau_sdk_toolchain_version)
			string(REGEX REPLACE "[^0-9.]" "" _tau_sdk_toolchain_version
				"${_tau_sdk_toolchain_version}")
		endif()
	else()
		set(_tau_sdk_toolchain_id "${CMAKE_CXX_COMPILER_ID}")
		set(_tau_sdk_toolchain_version "${CMAKE_CXX_COMPILER_VERSION}")
	endif()
	file(WRITE "${TAU_SDK_DIR}/tau-toolchain.txt"
		"set(TAU_SDK_TOOLCHAIN_ID \"${_tau_sdk_toolchain_id}\")\n"
		"set(TAU_SDK_TOOLCHAIN_VERSION \"${_tau_sdk_toolchain_version}\")\n"
		"set(TAU_SDK_TOOLCHAIN_CXX \"${CMAKE_CXX_COMPILER}\")\n")
	# Where this build resolved each dependency, so a build-tree SDK consumer
	# finds the same tauparser/Boost/cvc5. Kept in the SDK, never in `tau`.
	set(_tau_sdk_dep_args "")
	foreach(_entry IN LISTS TAU_SDK_PACKAGE_DIRS)
		list(APPEND _tau_sdk_dep_args "-D${_entry}")
	endforeach()
	if(DEFINED Boost_DIR AND NOT "${Boost_DIR}" MATCHES "NOTFOUND$")
		list(APPEND _tau_sdk_dep_args "-DBoost_DIR=${Boost_DIR}")
	endif()
	if(DEFINED tauparser_DIR AND NOT "${tauparser_DIR}" MATCHES "NOTFOUND$")
		list(APPEND _tau_sdk_dep_args "-Dtauparser_DIR=${tauparser_DIR}")
	endif()
	file(WRITE "${TAU_SDK_DIR}/tau-sdk-deps.cmake"
		"set(TAU_SDK_CONFIGURE_ARGS \"${_tau_sdk_dep_args}\")\n")
endif()

#
# Install the SDK, its dependencies and its build script: installed parser
# mode only, so the parser headers come from the parser SDK instead of the
# source tree.
#
if(TAU_BUILD_STATIC_LIBRARY AND TAU_DEPS_FROM_STORE)
	# Strip build-environment prefixes from __FILE__ and debug info so libTAU.a
	# carries no absolute build, store, or home path. The store prefix may be a
	# symlink, so map its resolved path too.
	if(NOT MSVC)
		get_filename_component(_tau_shared_real
			"${TAU_SHARED_PREFIX_RESOLVED}" REALPATH)
		get_filename_component(_tau_store_real
			"${TAU_SHARED_PREFIX_RESOLVED}/store" REALPATH)
		target_compile_options(${TAU_STATIC_LIB_NAME} PRIVATE
			"-ffile-prefix-map=${PROJECT_SOURCE_DIR}="
			"-ffile-prefix-map=${CMAKE_BINARY_DIR}="
			"-ffile-prefix-map=${TAU_SHARED_PREFIX_RESOLVED}="
			"-ffile-prefix-map=${_tau_shared_real}="
			"-ffile-prefix-map=${_tau_store_real}=")
	endif()

	# Public headers: the whole src/ tree, minus the test-only doctest header.
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
		DESTINATION ${TAU_SDK_INSTALL_ROOT}/include
		COMPONENT "Library"
		FILES_MATCHING
			PATTERN "*.h"
			PATTERN "doctest.h" EXCLUDE
			PATTERN "CMakeFiles" EXCLUDE
			PATTERN "*.cmake" EXCLUDE)
	# Generated parsers: core ones by bare name, BA ones under boolean_algebras/.
	install(DIRECTORY "${TAU_GENERATED_PARSER_DIR}/"
		DESTINATION ${TAU_SDK_INSTALL_ROOT}/include
		COMPONENT "Library"
		FILES_MATCHING PATTERN "*.generated.h")
	install(DIRECTORY "${TAU_PACK_INCLUDE_DIR}/"
		DESTINATION ${TAU_SDK_INSTALL_ROOT}/include
		COMPONENT "Library"
		FILES_MATCHING PATTERN "*.generated.h")
	install(FILES "${TAU_PACK_INCLUDE_DIR}/tau_pack.h"
		DESTINATION ${TAU_SDK_INSTALL_ROOT}/include
		COMPONENT "Library")
	# tau_compile.tmpl.h includes this generated header, so an installed SDK
	# that carries that header must carry this one too.
	install(FILES "${TAU_PACK_INCLUDE_DIR}/tau_artifact_template.h"
		DESTINATION ${TAU_SDK_INSTALL_ROOT}/include
		COMPONENT "Library")

	install(EXPORT TauTargets
		FILE TauTargets.cmake
		NAMESPACE TAU::
		DESTINATION ${TAU_SDK_CMAKE_DIR}
		COMPONENT "Library")

	# INSTALL_PREFIX is the box root, not the install prefix, so
	# PACKAGE_PREFIX_DIR resolves to the box and the bundled parser config at
	# <box>/cmake stays reachable from <box>/lib/cmake/Tau.
	configure_package_config_file(
		"${PROJECT_SOURCE_DIR}/cmake/TauConfig.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/TauConfig.cmake"
		INSTALL_DESTINATION "lib/cmake/Tau"
		INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/${TAU_SDK_INSTALL_ROOT}")
	write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/TauConfigVersion.cmake"
		VERSION "${PROJECT_VERSION}"
		COMPATIBILITY AnyNewerVersion)
	install(FILES
		"${CMAKE_CURRENT_BINARY_DIR}/TauConfig.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/TauConfigVersion.cmake"
		DESTINATION ${TAU_SDK_CMAKE_DIR}
		COMPONENT "Library")
	# The compile script, in the cmake/ subdir `tau` resolves the SDK from.
	install(FILES
		"${PROJECT_SOURCE_DIR}/cmake/tau-compile.cmake"
		"${PROJECT_SOURCE_DIR}/cmake/tau-shared-emscripten.cmake"
		"${PROJECT_SOURCE_DIR}/cmake/tau-shared-resolve.cmake"
		DESTINATION "${TAU_SDK_CMAKE_DIR}/cmake"
		COMPONENT "Library")
	# The platform map a consumer reads at compile time, and the platform and
	# toolchain this box was built for.
	install(FILES "${TAU_SDK_DIR}/cmake/tau-platforms.cmake"
		DESTINATION "${TAU_SDK_CMAKE_DIR}/cmake"
		COMPONENT "Library")
	install(FILES "${TAU_SDK_DIR}/cmake/tau-this-platform.txt"
		DESTINATION "${TAU_SDK_CMAKE_DIR}/cmake"
		COMPONENT "Library")
	install(FILES "${TAU_SDK_DIR}/tau-toolchain.txt"
		DESTINATION ${TAU_SDK_CMAKE_DIR}
		COMPONENT "Library")

	# Bundle the dependency SDKs under this box root so an installed tau-sdk
	# works without the build host's store. Each keeps its own CMake package
	# and layout; TauConfig prepends this prefix to CMAKE_PREFIX_PATH.
	install(DIRECTORY "${TAU_PARSER_SDK_PREFIX}/include/"
		DESTINATION ${TAU_SDK_INSTALL_ROOT}/include
		COMPONENT "Library")
	install(DIRECTORY "${TAU_PARSER_SDK_PREFIX}/cmake/"
		DESTINATION "${TAU_SDK_INSTALL_ROOT}/cmake"
		COMPONENT "Library")
	if(TARGET idni::tauparser_static)
		install(FILES $<TARGET_FILE:idni::tauparser_static>
			DESTINATION ${TAU_SDK_INSTALL_ROOT}/lib
			COMPONENT "Library")
	endif()
	install(DIRECTORY "${BOOST_STORE_PREFIX}/include/"
		DESTINATION ${TAU_SDK_INSTALL_ROOT}/include
		COMPONENT "Library")
	install(DIRECTORY "${BOOST_STORE_PREFIX}/lib/cmake/"
		DESTINATION "${TAU_SDK_INSTALL_ROOT}/lib/cmake"
		COMPONENT "Library")
	# Boost::log's own config also requires atomic, chrono, filesystem, thread
	# and headers, so take every static archive; the shared ones stay out.
	install(DIRECTORY "${BOOST_STORE_PREFIX}/lib/"
		DESTINATION ${TAU_SDK_INSTALL_ROOT}/lib
		COMPONENT "Library"
		FILES_MATCHING PATTERN "*.a" PATTERN "*.lib")
	if("cvc5" IN_LIST TAU_BA_REQUIRED_PACKAGES)
		install(DIRECTORY "${CVC5_STORE_PREFIX}/include/"
			DESTINATION ${TAU_SDK_INSTALL_ROOT}/include
			COMPONENT "Library")
		install(DIRECTORY "${CVC5_STORE_PREFIX}/lib/cmake/"
			DESTINATION "${TAU_SDK_INSTALL_ROOT}/lib/cmake"
			COMPONENT "Library")
		# cvc5 links libpoly beside itself; take the whole library directory
		# so the SDK carries that closure too.
		install(DIRECTORY "${CVC5_STORE_PREFIX}/lib/"
			DESTINATION ${TAU_SDK_INSTALL_ROOT}/lib
			COMPONENT "Library"
			FILES_MATCHING PATTERN "*.so*" PATTERN "*.dylib"
				PATTERN "*.a" PATTERN "*.lib" PATTERN "*.dll")
	endif()
endif()

#
# shared library
#
add_library(${TAU_SHARED_LIB_NAME} SHARED)
add_library(${namespace}::${TAU_SHARED_LIB_NAME} ALIAS ${TAU_SHARED_LIB_NAME})
target_sources(${TAU_SHARED_LIB_NAME} PRIVATE ${TAU_SOURCES})
target_link_libraries(${TAU_SHARED_LIB_NAME} tauparser ${TAU_BOOST_LIBS} ${TAU_BA_LINK_LIBS})
target_include_directories(${TAU_SHARED_LIB_NAME} PUBLIC
	$<BUILD_INTERFACE:${TAU}/src>
	$<BUILD_INTERFACE:${TAU}/parser>
	${TAU_PARSER_SOURCE_INCLUDE}
	$<INSTALL_INTERFACE:.>
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

include(tau-ftxui)

if(TAU_BUILD_EXECUTABLE)
	add_executable(${TAU_EXECUTABLE_NAME})
	target_sources(${TAU_EXECUTABLE_NAME} PRIVATE ${CLI_SOURCES})
	target_setup(${TAU_EXECUTABLE_NAME})
	target_link_libraries(${TAU_EXECUTABLE_NAME} ${TAU_STATIC_LIB_NAME} ${TAU_BA_LINK_LIBS})
	target_include_directories(${TAU_EXECUTABLE_NAME} PUBLIC
		$<BUILD_INTERFACE:${TAU}/src>
		$<BUILD_INTERFACE:${TAU}/parser>
		${TAU_PARSER_SOURCE_INCLUDE}
	)
	add_dependencies(${TAU_EXECUTABLE_NAME} tau_header)
	if(EMSCRIPTEN)
		# the JS binding owns tau.js, so the CLI emits tau_repl.js to
		# share the build dir.
		set_target_properties(${TAU_EXECUTABLE_NAME} PROPERTIES OUTPUT_NAME "tau_repl")
		# grammar tables overflow the default 64KiB wasm stack at static init.
		# Shared memory cannot grow at startup, so INITIAL_MEMORY must hold static
		# data plus the stack, with growth added for the varying heap use.
		target_link_options(${TAU_EXECUTABLE_NAME} PRIVATE
			"SHELL:-s STACK_SIZE=16777216"
			"SHELL:-s INITIAL_MEMORY=33554432"
			"SHELL:-s ALLOW_MEMORY_GROWTH=1")
		# NODEFS backs the full-root /host mount tau_stdin_node.pre.js installs,
		# so a wasm REPL test under the node emulator can open host files.
		target_link_options(${TAU_EXECUTABLE_NAME} PRIVATE
			"SHELL:-lnodefs.js")
		# with no Module["stdin"], Emscripten maps stdin to a tty device,
		# so a piped run never sees EOF and the line editor spins
		target_link_options(${TAU_EXECUTABLE_NAME} PRIVATE
			"SHELL:--pre-js ${CMAKE_CURRENT_SOURCE_DIR}/tau_stdin_node.pre.js")
		set_property(TARGET ${TAU_EXECUTABLE_NAME} APPEND PROPERTY LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tau_stdin_node.pre.js)
	endif()
	if(TAU_PARSER_HAS_FTXUI_DEP)
		target_link_libraries(${TAU_EXECUTABLE_NAME}
			ftxui::screen ftxui::dom ftxui::component)
		target_compile_definitions(${TAU_EXECUTABLE_NAME} PRIVATE TAU_PARSER_HAS_FTXUI)
		if(EMSCRIPTEN AND TAU_WASM_REPL_MODEL STREQUAL "sleep")
			# FTXUI's Emscripten input listener calls emscripten_sleep,
			# which needs JSPI at link time.
			target_link_options(${TAU_EXECUTABLE_NAME} PRIVATE "SHELL:-s JSPI")
		endif()
		message(STATUS "tau: FTXUI REPL enabled (legacy via -X)")
	elseif(NOT TAU_DONT_USE_FTXUI)
		message(WARNING "tau: FTXUI not available — legacy terminal REPL will be used")
	endif()
endif(TAU_BUILD_EXECUTABLE)

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
		${TAU_PARSER_SOURCE_INCLUDE}
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

