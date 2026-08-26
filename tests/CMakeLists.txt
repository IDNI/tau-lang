set(DOCTEST_HEADER "${PROJECT_SOURCE_DIR}/src/doctest.h" CACHE PATH "Doctest header")
if (NOT EXISTS "${DOCTEST_HEADER}")
	set(DOCTEST_VERSION "v2.4.11")
	set(DOCTEST_URL "https://raw.githubusercontent.com/doctest/doctest/${DOCTEST_VERSION}/doctest/doctest.h")
	set(DOCTEST_SHA256 "44faa038e9c3f9728efbda143748d01124ea0a27f4bf78f35a15d8fab2e039fb")
	message(STATUS "Downloading doctest ${DOCTEST_VERSION} to '${DOCTEST_HEADER}'")
	file(DOWNLOAD "${DOCTEST_URL}" "${DOCTEST_HEADER}"
		EXPECTED_HASH SHA256=${DOCTEST_SHA256}
		STATUS DOCTEST_DOWNLOAD_STATUS
		TLS_VERIFY ON)
	list(GET DOCTEST_DOWNLOAD_STATUS 0 DOCTEST_DOWNLOAD_RESULT)
	if(NOT DOCTEST_DOWNLOAD_RESULT EQUAL 0)
		list(GET DOCTEST_DOWNLOAD_STATUS 1 DOCTEST_DOWNLOAD_MESSAGE)
		message(FATAL_ERROR "Failed to download doctest ${DOCTEST_VERSION}: ${DOCTEST_DOWNLOAD_MESSAGE}")
	endif()
endif ()

add_library(doctest INTERFACE)
target_compile_definitions(doctest INTERFACE TAU_USE_DOCTEST)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(doctest INTERFACE -Wno-c2y-extensions "-Wno-#warnings")
endif()
# SW-11: this is a doctest PREPROCESSOR macro; a CMake variable had no
# effect. Define it for real on the test compile.
add_compile_definitions(DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE)

set(TAU_SKIP_TESTS
	test_integration-satisfiability-bool
	test_integration-satisfiability-sbf
	test_integration-satisfiability-hsb
	test_integration-satisfiability-qint
	test_integration-satisfiability-qlt
	test_integration-satisfiability-tau_ba
	test_integration-satisfiability-mixed
	test_integration-satisfiability-bv
	test_ltl_benchmarks
	test_ltl_multi_type_oracle
	test_ltl_soundness_regression
	test_ltl_many_sorted
	test_by_grammar
	test_ltl_correctness
	test_integration-interpreter_ltl_multiio
	test_integration-solver-hsb
	test_integration-solver-qint
	test_integration-solver-qlt
	test_integration-solver-mixed
	test_integration-solver-sbf
	test_tau_tree_printers
	test_integration-quantifiers
	test_temporal_connectives
	test_integration-ltl_aba_execution
	test_integration-blasting_correctness_check1
	test_integration-blasting_correctness_check2
	test_integration-blasting_correctness_check3
	test_integration-blasting_correctness_check4
	test_integration-blasting_correctness_check5
	test_ltl_strategy_exec
	CACHE STRING "Test targets skipped because covered by REPL tests")

#
# Which BAs a central suite needs.
#
# A suite a plugin or backend owns declares this in its manifest; these are the
# ones belonging to no single algebra, plus core tests that merely mention one.
# A suite naming an algebra cannot build in a pack without it, so a reduced
# -DTAU_BAS= skips it rather than failing.
#
# Derived once from what each source names, so some entries are broader than the
# suite truly needs -- a mention in a comment reads the same as a use. Dropping
# one is a matter of proving it in a pack without that algebra.
#
set(TAU_TEST_REQUIRES_test_algorithm_a qlt)
set(TAU_TEST_REQUIRES_test_algorithm_b qlt)
set(TAU_TEST_REQUIRES_test_algorithm_d qlt sbf)
set(TAU_TEST_REQUIRES_test_antiprenexing bv sbf)
set(TAU_TEST_REQUIRES_test_ba_constants bv sbf)
set(TAU_TEST_REQUIRES_test_ba_descriptor_pack bv hsb nlang qint qlt sbf)
set(TAU_TEST_REQUIRES_test_ba_types bv sbf)
set(TAU_TEST_REQUIRES_test_ba_types_inference sbf)
set(TAU_TEST_REQUIRES_test_base_ba_dispatcher bv sbf)
set(TAU_TEST_REQUIRES_test_bv_ba_hooks bv)
set(TAU_TEST_REQUIRES_test_by_grammar bv nlang qint qlt sbf)
set(TAU_TEST_REQUIRES_test_cpp_codegen_bench bv)
set(TAU_TEST_REQUIRES_test_cpp_codegen_data_atoms bv qlt)
set(TAU_TEST_REQUIRES_test_hbdd sbf)
set(TAU_TEST_REQUIRES_test_hooks bv)
set(TAU_TEST_REQUIRES_test_integration-ba_types_inference bv sbf)
set(TAU_TEST_REQUIRES_test_integration-bf_hooks sbf)
set(TAU_TEST_REQUIRES_test_integration-bf_normalization sbf)
set(TAU_TEST_REQUIRES_test_integration-ctl_star bv)
set(TAU_TEST_REQUIRES_test_integration-heuristics-simplify_using_equality bv)
set(TAU_TEST_REQUIRES_test_integration-heuristics-trivial_skolem bv)
set(TAU_TEST_REQUIRES_test_integration-bv_stress_check bv)
set(TAU_TEST_REQUIRES_test_integration-interpreter sbf)
set(TAU_TEST_REQUIRES_test_integration-ltl_aba_execution bv sbf)
set(TAU_TEST_REQUIRES_test_integration-pointwise_revision sbf)
set(TAU_TEST_REQUIRES_test_integration-satisfiability-mixed bv hsb qint qlt sbf)
set(TAU_TEST_REQUIRES_test_integration-solver-iterators bv sbf)
set(TAU_TEST_REQUIRES_test_integration-solver-mixed hsb qint qlt)
set(TAU_TEST_REQUIRES_test_integration-splitter bv sbf)
set(TAU_TEST_REQUIRES_test_integration-tau_ba1 bv sbf)
set(TAU_TEST_REQUIRES_test_integration-tau_ba3 bv sbf)
set(TAU_TEST_REQUIRES_test_integration-type_scoped_resolver sbf)
set(TAU_TEST_REQUIRES_test_integration-wff_hooks sbf)
set(TAU_TEST_REQUIRES_test_integration-wff_normalization bv)
set(TAU_TEST_REQUIRES_test_ltl_aba bv nlang qint qlt sbf)
set(TAU_TEST_REQUIRES_test_ltl_benchmarks sbf)
set(TAU_TEST_REQUIRES_test_ltl_correctness bv qlt sbf)
set(TAU_TEST_REQUIRES_test_ltl_many_sorted bv hsb qint qlt sbf)
set(TAU_TEST_REQUIRES_test_ltl_multi_type_oracle bv qint qlt sbf)
set(TAU_TEST_REQUIRES_test_ltl_negative sbf)
set(TAU_TEST_REQUIRES_test_ltl_soundness_regression bv qlt sbf)
set(TAU_TEST_REQUIRES_test_ltl_strategy_exec bv sbf)
set(TAU_TEST_REQUIRES_test_normal_forms bv sbf)
set(TAU_TEST_REQUIRES_test_tau_bdd bv sbf)
set(TAU_TEST_REQUIRES_test_variant_ba bv sbf)

#
# Which external packages a test needs, read the same way.
#
# A test may use a dependency directly rather than through a BA -- cvc5 as a
# cross-validation oracle -- and that dependency is only configured when some BA
# in the pack requires it. Maps a dependency's include prefix to the package name
# the BA manifests declare.
#
set(TAU_TEST_DEP_HEADERS "cvc5/=cvc5" "curl/=CURL")

function(tau_test_required_packages out src)
	set(_req "")
	if(EXISTS "${src}")
		file(READ "${src}" _tau_src)
		foreach(_entry ${TAU_TEST_DEP_HEADERS})
			string(REPLACE "=" ";" _pair "${_entry}")
			list(GET _pair 0 _hdr)
			list(GET _pair 1 _pkg)
			if(_tau_src MATCHES "include[ \t]*<${_hdr}")
				list(APPEND _req ${_pkg})
			endif()
		endforeach()
	endif()
	set(${out} "${_req}" PARENT_SCOPE)
endfunction()

function(add test_name)
	# message(STATUS "adding test: ${test_name}")
	if(test_name IN_LIST TAU_SKIP_TESTS)
		message(STATUS "Skipping C++ test ${test_name} (covered by REPL test)")
		return()
	endif()
	set(N "${test_name}")
	foreach(_id ${TAU_TEST_REQUIRES_${N}})
		if(NOT _id IN_LIST TAU_PACK_BA_IDS)
			message(STATUS
				"Skipping ${N}: needs BA '${_id}', not in TAU_BAS")
			return()
		endif()
	endforeach()
	tau_test_required_packages(_tau_pkgs "${CMAKE_CURRENT_SOURCE_DIR}/${N}.cpp")
	foreach(_pkg ${_tau_pkgs})
		if(NOT _pkg IN_LIST TAU_BA_REQUIRED_PACKAGES)
			message(STATUS
				"Skipping ${N}: needs package '${_pkg}', which no BA in TAU_BAS requires")
			return()
		endif()
	endforeach()
	_tau_add_test_target("${N}" "${CMAKE_CURRENT_SOURCE_DIR}/${N}.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/..")
endfunction()

# Build and register one test target from an explicit source path, so a suite
# can live outside the directory that adds it -- a BA's own tests do.
function(_tau_add_test_target N src work_dir)
	add_executable(${N} "${src}")
	target_setup(${N})
	target_include_directories(${N} PUBLIC ${work_dir})
	# extern template declarations for the test BA packs, instantiated in libTAU
	# test_cvc5 is skipped: its own file-scope TermManager collides with tau.h's
	if(NOT "${N}" STREQUAL "test_cvc5")
		target_compile_options(${N} PUBLIC
			-include ${PROJECT_SOURCE_DIR}/src/extern_template_test_pack.h)
	endif()
	target_link_libraries(${N} ${TAU_STATIC_LIB_NAME} doctest ${TAU_BA_LINK_LIBS})
	add_dependencies(${N} tau_header)
	target_compile_options(${N} PUBLIC -Wno-unused-function)
	target_compile_definitions_if(${N} PUBLIC TAU_LOG_TRACE_TESTS)
	target_compile_options(${N} PUBLIC -fno-lto)
	target_link_options(${N} PUBLIC -fno-lto)
	target_compile_options(${N} PUBLIC -Wstrict-overflow=2 -Wno-error=strict-overflow)
	add_test(NAME ${N} COMMAND "${PROJECT_BINARY_DIR}/${N}")
	# Heavy adversarial / fuzz suites need more than the ctest default;
	# their hundred-plus deep LTL+ABA-oracle cases routinely run >600s
	# on the CI hardware.  Everything else gets the default.
	if("${N}" MATCHES "^test_(ltl_aba|ltl_soundness_regression)$")
		set(_TAU_TEST_TIMEOUT 1500)
	else()
		set(_TAU_TEST_TIMEOUT 600)
	endif()
	set_tests_properties(${N}
		PROPERTIES WORKING_DIRECTORY ${work_dir}
		ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/external/cvc5/build/src:$ENV{LD_LIBRARY_PATH}"
		TIMEOUT ${_TAU_TEST_TIMEOUT})
endfunction()

# Register a backend's own suites, e.g. the cvc5 smoke tests. A backend serves
# whichever algebras ask for it rather than being one, so what decides its
# suites is the package: they are skipped unless a BA of the pack requires it,
# which _tau_add_owned_test() checks from the source's own includes.
function(tau_add_backend_tests)
	file(GLOB _srcs "${PROJECT_SOURCE_DIR}/src/backends/*/tests/*.cpp")
	foreach(_src ${_srcs})
		_tau_add_owned_test("${_src}")
	endforeach()
endfunction()

# Build and register one suite owned by a plugin or a backend: the source lives
# outside tests/, so it needs the shared fixture directories on its include path
# and the package check add() applies to central suites.
function(_tau_add_owned_test src)
	get_filename_component(_n "${src}" NAME_WE)
	if(_n IN_LIST TAU_SKIP_TESTS)
		message(STATUS "Skipping C++ test ${_n} (covered by REPL test)")
		return()
	endif()
	if(NOT EXISTS "${src}")
		message(FATAL_ERROR "declared test does not exist: ${src}")
	endif()
	foreach(_req ${TAU_BA_TEST_REQUIRES_${_n}})
		if(NOT _req IN_LIST TAU_PACK_BA_IDS)
			message(STATUS
				"Skipping ${_n}: needs BA '${_req}', not in TAU_BAS")
			return()
		endif()
	endforeach()
	# a suite may use a dependency directly -- cvc5 as an oracle -- which is
	# configured only when some BA in the pack requires it
	tau_test_required_packages(_pkgs "${src}")
	foreach(_pkg ${_pkgs})
		if(NOT _pkg IN_LIST TAU_BA_REQUIRED_PACKAGES)
			message(STATUS
				"Skipping ${_n}: needs package '${_pkg}', which no BA in TAU_BAS requires")
			return()
		endif()
	endforeach()
	_tau_add_test_target("${_n}" "${src}" "${PROJECT_SOURCE_DIR}/tests")
	# shared fixtures such a suite includes by bare name
	target_include_directories("${_n}" PUBLIC
		${PROJECT_SOURCE_DIR}/tests/unit
		${PROJECT_SOURCE_DIR}/tests/integration
		${PROJECT_SOURCE_DIR}/tests/integration/satisfiability
		${PROJECT_SOURCE_DIR}/tests/integration/solver)
endfunction()

# Register the suites the pack's BAs declare in their manifests. Ownership
# decides them, so unlike add() they need no requirement read from the source.
function(tau_add_owned_ba_tests)
	foreach(_src ${TAU_BA_TESTS_RESOLVED})
		_tau_add_owned_test("${_src}")
	endforeach()
endfunction()

#
# Integration testing
#
set_property(CACHE TAU_BUILD_INTEGRATION PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_INTEGRATION)
	enable_testing()
	add_subdirectory(integration)
endif ()

#
# Unit testing
#
set_property(CACHE TAU_BUILD_UNIT_TESTS PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_UNIT_TESTS)
	enable_testing()
	add_subdirectory(unit)
endif ()

#
# Suites owned by the pack's BAs
#
# One registration for both kinds: a manifest lists what the algebra owns, not
# where it used to live.
if (TAU_BUILD_UNIT_TESTS OR TAU_BUILD_INTEGRATION)
	enable_testing()
	tau_add_owned_ba_tests()
	tau_add_backend_tests()
endif ()

#
# API testing
#
set_property(CACHE TAU_BUILD_API_TESTS PROPERTY STRINGS "OFF" "ON")
if (TAU_BUILD_API_TESTS)
	enable_testing()
	add_subdirectory(api)
endif ()

#
# REPL testing
#
set_property(CACHE TAU_BUILD_REPL_TESTS PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_REPL_TESTS)
	enable_testing()
	add_subdirectory(repl)
endif ()

#
# Benchmark testing
#
if (TAU_BUILD_BENCHMARK)
	enable_testing()
	add_subdirectory(benchmark)
endif ()
