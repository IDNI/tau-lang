// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentiTd cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "../../src/doctest.h"
#include "../../src/nso_rr.h"
#include "../../src/bdd_handle.h"
#include "../../src/normalizer2.h"
#include "../../src/satisfiability.h"

#include "test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

tau_ba<bdd_test> get_tau_ba(const char* src) {
	bdd_test_factory bf;
	tau_factory<bdd_test_factory, bdd_test> fb(bf);
	factory_binder<tau_factory<bdd_test_factory, bdd_test>, tau_ba<bdd_test>, bdd_test> fbinder(fb);
	rr<nso<tau_ba<bdd_test>, bdd_test>> nso_rr = make_nso_rr_using_factory<
			factory_binder<tau_factory<bdd_test_factory, bdd_test>,tau_ba<bdd_test>, bdd_test>,
			tau_ba<bdd_test>, bdd_test>(src, fbinder);
	auto value = nso_rr.main
		| tau_parser::bf_eq
		| tau_parser::bf
		| tau_parser::bf_constant
		| tau_parser::constant
		| only_child_extractor<tau_ba<bdd_test>, bdd_test>;
	return get<tau_ba<bdd_test>>(get<variant<
			tau_ba<bdd_test>, bdd_test>>(value.value()->value));
}

tau_ba<bdd_test> normal(const tau_ba<bdd_test>& f) {
	auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>(f.rr_nso);
	return normalized;
}

TEST_SUITE("complex tau_ba formulas") {

	TEST_CASE("Ohad's example: ex X ( { : ( X = { : (Y = 0).}). } = 0).") {
		const char* sample = "ex X ( { : ( X = { : (Y = 0).}). } = 0).";
		auto normalized = normalize_test_tau(sample);
		auto check = normalized.main | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("tau_ba operators") {
		auto T = get_tau_ba("( { : ({ : F.} = 0). } = 0 ).");
		auto F = get_tau_ba("( { : ({ : T.} = 0). } = 0 ).");
		auto TorF  = T | F;
		auto TandF = T & F;
		cout << "T: "     << T     << " \n\tnormal: " << normal(T)     << "\n";
		cout << "F: "     << F     << " \n\tnormal: " << normal(F)     << "\n";
		cout << "T | F: " << TorF  << " \n\tnormal: " << normal(TorF)  << "\n";
		cout << "T & F: " << TandF << " \n\tnormal: " << normal(TandF) << "\n";

		CHECK( true );
	}
}
