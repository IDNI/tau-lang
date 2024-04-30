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

#include "doctest.h"
#include "nso_rr.h"
#include "bdd_handle.h"
#include "normalizer.h"
#include "satisfiability.h"

#include "test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

static auto T = get_tau_ba("{ : { : F.} = 0. } = 0.");
static auto F = get_tau_ba("{ : { : T.} = 0. } = 0.");

TEST_SUITE("tau_ba operators: negation") {

	TEST_CASE("negation of F") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((~F).nso_rr);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("negation of T") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((~T).nso_rr);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("tau_ba operators: conjunction") {

	TEST_CASE("F & F") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((F&F).nso_rr);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("F & T") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((F&T).nso_rr);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("T & F") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((T&F).nso_rr);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("T & T") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((T&T).nso_rr);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("tau_ba operators: disjunction") {

	TEST_CASE("F | F") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((F|F).nso_rr);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("F | T") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((F|T).nso_rr);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );

	}

	TEST_CASE("T | F") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((T|F).nso_rr);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("T | T") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((T|T).nso_rr);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("tau_ba operators: exclusive or") {

	TEST_CASE("F + F") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((F+F).nso_rr);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("F + T") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((F+T).nso_rr);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("T + F") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((T+F).nso_rr);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("T + T") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((T+T).nso_rr);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("F ^ F") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((F^F).nso_rr);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("F ^ T") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((F^T).nso_rr);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("T ^ F") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((T^F).nso_rr);
		auto check = normalized | tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("T ^ T") {
		auto normalized = normalizer<tau_ba<bdd_test>, bdd_test>((T^T).nso_rr);
		auto check = normalized | tau_parser::wff_f;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("tau_ba check: is_one") {

	TEST_CASE("F is not one") {
		CHECK( !F.is_one() );
	}

	TEST_CASE("T is one") {
		CHECK( T.is_one() );
	}
}

TEST_SUITE("tau_ba check: is_zero") {

	TEST_CASE("F is zero") {
		CHECK( F.is_zero() );
	}

	TEST_CASE("T is not zero") {
		CHECK( !T.is_zero() );
	}
}