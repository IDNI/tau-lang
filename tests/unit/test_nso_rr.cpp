// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "doctest.h"
#include "test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

TEST_SUITE("make_library") {

	TEST_CASE("make_library: one rule case") {
		const auto sample = BF_TO_DNF_0;
		auto lib = make_library<Bool>(sample);
		CHECK( lib.size() == 1 );
	}
}

// TODO (VERY_LOW) write more unit tests for make_library
// TODO (VERY_LOW) write tests to check make_rule
// TODO (VERY_LOW) write tests to check make_tau_source
// TODO (VERY_LOW) write tests to check make_tau_code
// TODO (VERY_LOW) write tests to check make_builder
// TODO (VERY_LOW) write tests for make_nso_rr_using_factory
// TODO (VERY_LOW) write tests for make_nso_rr_using_bindings
// TODO (VERY_LOW) write tests for make_tau_source
// TODO (VERY_LOW) write tests for nso_rr_apply
// TODO (VERY_LOW) write tests for tau_apply_builder
//
// They are tagged as VERY_LOW because they are extensively tested inderectly in
// other unit/integration tests. However, it is better to have explicit tests
// for each of them.

// TODO (VERY_LOW) write tests for operator<<(ostream, tau<...>)
// TODO (VERY_LOW) write tests for operator<<(ostream, sp_tau_source_node)
//
// They are tagged as VERY_LOW because they should be remove once we have a proper
// UI and I/O api.

// TODO (LOW) write tests for is_non_terminal
// TODO (LOW) write tests for is_terminal
// TODO (LOW) write tests for value_extractor
// TODO (LOW) write tests for terminal_extractor
// TODO (LOW) write tests for non_terminal_extractor
// TODO (LOW) write tests for ba_extractor
// TODO (LOW) write tests for stringify
// TODO (LOW) write tests for make_string
//
// All of them are extensively tested inderectly in other unit/integration tests.
// However, it is better to have explicit tests for each of them.

TEST_SUITE("callbacks") {


	// TODO (MEDIUM) write tests for execution bf_has_clashing_subformulas_cb
	// TODO (MEDIUM) write tests for execution bf_has_subformula_cb
	// TODO (MEDIUM) write tests for execution bf_remove_fexistential_cb
	// TODO (MEDIUM) write tests for execution bf_remove_funiversal_cb

	// TODO (MEDIUM) write tests for execution wff_remove_existential_cb
	// TODO (MEDIUM) write tests for execution wff_remove_bexistential_cb
	// TODO (MEDIUM) write tests for execution wff_remove_buniversal_cb.
	// TODO (MEDIUM) write tests for execution wff_has_clashing_subformulas_cb
	// TODO (MEDIUM) write tests for execution wff_has_subformula_cb
	//
	// Callbacks are a crucial part of the execution of the normalizer and should
	// be tested properly. However, they are extensively tested inderectly in
	// other unit/integration tests. However, it is better to have explicit tests
	// for each of them.

}
