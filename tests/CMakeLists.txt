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
set(DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE "true")

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
# Which BAs a test needs, read from the test's own source.
#
# A test that names a Boolean algebra cannot build in a pack without it, so a
# reduced -DTAU_BAS= must skip it rather than fail. This is derived at configure
# time instead of from a hand-kept list, so a new test classifies itself and no
# list drifts. The match is deliberately loose on both sides, to catch `sbf_ba`,
# `nlang_ba`, `bv[8]` and `is_tref_bv_type_family` as well as bare ids; only a
# letter or digit adjoining the id rejects it, so `bvector` does not match. The
# cost of a false positive is one test skipped in a reduced pack, which the skip
# message below makes visible. For the full pack nothing is skipped and the build
# is unchanged.
#
# A fixture helper pins its own pack instead of following TAU_BAS, so it is
# scanned along with any test including it. Pack-agnostic helpers name no BA and
# need no entry here.
#
set(TAU_TEST_FIXTURE_HELPERS test_Bool_helpers.h test_sbf_ba_helpers.h)
set(_TAU_TEST_HELPER_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(tau_test_required_bas out src)
	set(_req "")
	if(EXISTS "${src}")
		file(READ "${src}" _tau_src)
		foreach(_h ${TAU_TEST_FIXTURE_HELPERS})
			if(_tau_src MATCHES "${_h}")
				file(READ "${_TAU_TEST_HELPER_DIR}/${_h}" _tau_fixture)
				string(APPEND _tau_src "${_tau_fixture}")
			endif()
		endforeach()
		foreach(_id bv sbf qlt qint hsb nlang)
			if(_tau_src MATCHES "[^A-Za-z0-9]${_id}")
				list(APPEND _req ${_id})
			endif()
		endforeach()
	endif()
	set(${out} "${_req}" PARENT_SCOPE)
endfunction()

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
	tau_test_required_bas(_tau_req "${CMAKE_CURRENT_SOURCE_DIR}/${N}.cpp")
	foreach(_id ${_tau_req})
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
	if("${N}" MATCHES "^test_(ltl_adversarial|ltl_adversarial2|ltl_aba|ltl_fuzz100|ltl_soundness_regression)$")
		set(_TAU_TEST_TIMEOUT 1500)
	else()
		set(_TAU_TEST_TIMEOUT 600)
	endif()
	set_tests_properties(${N}
		PROPERTIES WORKING_DIRECTORY ${work_dir}
		ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/external/cvc5/build/src:$LD_LIBRARY_PATH"
		TIMEOUT ${_TAU_TEST_TIMEOUT})
endfunction()

# Register the suites the pack's BAs declare in their manifests. Ownership
# decides them, so unlike add() they need no requirement read from the source.
function(tau_add_owned_ba_tests)
	foreach(_src ${TAU_BA_TESTS_RESOLVED})
		get_filename_component(_n "${_src}" NAME_WE)
		if(_n IN_LIST TAU_SKIP_TESTS)
			message(STATUS
				"Skipping C++ test ${_n} (covered by REPL test)")
			continue()
		endif()
		if(NOT EXISTS "${_src}")
			message(FATAL_ERROR
				"BA manifest declares a missing test: ${_src}")
		endif()
		set(_skip FALSE)
		foreach(_req ${TAU_BA_TEST_REQUIRES_${_n}})
			if(NOT _req IN_LIST TAU_PACK_BA_IDS)
				message(STATUS
					"Skipping ${_n}: needs BA '${_req}', not in TAU_BAS")
				set(_skip TRUE)
			endif()
		endforeach()
		# a suite may use a dependency directly -- cvc5 as an oracle -- which is
		# configured only when some BA in the pack requires it
		tau_test_required_packages(_pkgs "${_src}")
		foreach(_pkg ${_pkgs})
			if(NOT _pkg IN_LIST TAU_BA_REQUIRED_PACKAGES)
				message(STATUS
					"Skipping ${_n}: needs package '${_pkg}', which no BA in TAU_BAS requires")
				set(_skip TRUE)
			endif()
		endforeach()
		if(_skip)
			continue()
		endif()
		_tau_add_test_target("${_n}" "${_src}"
			"${PROJECT_SOURCE_DIR}/tests")
		# shared fixtures a BA-owned suite includes by bare name
		target_include_directories("${_n}" PUBLIC
			${PROJECT_SOURCE_DIR}/tests/unit
			${PROJECT_SOURCE_DIR}/tests/integration
			${PROJECT_SOURCE_DIR}/tests/integration/satisfiability
			${PROJECT_SOURCE_DIR}/tests/integration/solver)
	endforeach()
endfunction()

#
# Integration testing
#
set(TAU_BUILD_INTEGRATION ON CACHE STRING "build the tau-lang integration suite ON")
set_property(CACHE TAU_BUILD_INTEGRATION PROPERTY STRINGS "OFF" "ON")

if (TAU_BUILD_INTEGRATION)
	enable_testing()
	add_subdirectory(integration)
endif ()

#
# Unit testing
#
set(TAU_BUILD_UNIT_TESTS ON CACHE STRING "build the tau-lang unit suite ON")
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
endif ()

#
# API testing
#
set(TAU_BUILD_API_TESTS ON CACHE STRING "build the tau-lang api suite ON")
set_property(CACHE TAU_BUILD_API_TESTS PROPERTY STRINGS "OFF" "ON")
if (TAU_BUILD_API_TESTS)
	enable_testing()
	add_subdirectory(api)
endif ()

#
# REPL testing
#
set(TAU_BUILD_REPL_TESTS ON CACHE STRING "build the tau-lang repl suite ON")
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
