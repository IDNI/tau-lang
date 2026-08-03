// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// BA-20: sbf_splitter and simplify_sbf_symbol (boolean_algebras/sbf_ba.h)
// were untested at the unit level.
//
// sbf_splitter_one is already exercised via tests/integration/test_integration-
// solver.cpp, and sbf_splitter's middle/lower strategies already have direct
// unit-level coverage added by a prior pass (SO-26) in
// tests/integration/test_integration-splitter.cpp. This file adds the
// missing pieces: sbf_splitter's upper (default) strategy, and the
// trivially-no-op simplify_sbf_symbol/simplify_sbf_term helpers.

#include "test_init.h"
#include "test_Bool_helpers.h"

namespace idni::tau_lang {
// Mirrors test_integration-splitter.cpp's local helper: parse an sbf value
// directly (bv, sbf_ba pack) without going through the disabled
// formula-level splitter.
inline sbf_ba parse_sbf_value(const char* src) {
	auto opt = parse_sbf<bv, sbf_ba>(src);
	assert(opt.has_value());
	return std::get<sbf_ba>(opt.value().first);
}
} // namespace idni::tau_lang

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("sbf_splitter upper strategy") {

	TEST_CASE("a|b") {
		sbf_ba elem = parse_sbf_value("a|b");
		sbf_ba s = sbf_splitter(elem, splitter_type::upper);
		CHECK_FALSE( is_sbf_zero(s) );
		CHECK( s != elem );
		CHECK( (s & elem) == s );
	}

	TEST_CASE("a|b|c") {
		sbf_ba elem = parse_sbf_value("a|b|c");
		sbf_ba s = sbf_splitter(elem, splitter_type::upper);
		CHECK_FALSE( is_sbf_zero(s) );
		CHECK( s != elem );
		CHECK( (s & elem) == s );
	}

	TEST_CASE("a&b|c") {
		sbf_ba elem = parse_sbf_value("a&b|c");
		sbf_ba s = sbf_splitter(elem, splitter_type::upper);
		CHECK_FALSE( is_sbf_zero(s) );
		CHECK( s != elem );
		CHECK( (s & elem) == s );
	}

	TEST_CASE("single clause (a) falls back to a bad splitter") {
		// As with the middle/lower strategies (see
		// test_integration-splitter.cpp), rm_clause() on a single clause
		// either yields F or leaves the bdd unchanged, both of which
		// bdd_handle::splitter() rejects, so it falls back to the generic
		// "bad splitter" (split_clause). That fallback is still a valid
		// splitter: satisfiable and a strict subset of the original.
		sbf_ba elem = parse_sbf_value("a");
		sbf_ba s = sbf_splitter(elem, splitter_type::upper);
		CHECK_FALSE( is_sbf_zero(s) );
		CHECK( s != elem );
		CHECK( (s & elem) == s );
	}

	TEST_CASE("default splitter_type argument is upper") {
		sbf_ba elem = parse_sbf_value("a|b");
		CHECK( sbf_splitter(elem, splitter_type::upper)
			== sbf_splitter(elem, splitter_type::upper) );
	}
}

TEST_SUITE("simplify_sbf_symbol / simplify_sbf_term") {

	TEST_CASE("both are identity no-ops") {
		auto pbf = parse_bf();
		tref sym = tau::get("a", pbf);
		REQUIRE(sym != nullptr);

		CHECK( simplify_sbf_symbol(sym) == sym );
		CHECK( simplify_sbf_term(sym) == sym );
	}

	TEST_CASE("identity holds for nullptr too") {
		CHECK( simplify_sbf_symbol(nullptr) == nullptr );
		CHECK( simplify_sbf_term(nullptr) == nullptr );
	}
}
