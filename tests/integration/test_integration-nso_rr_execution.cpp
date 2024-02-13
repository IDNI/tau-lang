// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../../src/doctest.h"
#include "../../src/nso_rr.h"
#include "../../src/bool.h"
#include "../../src/bdd_handle.h"
#include "../../src/normalizer2.h"

#include "test_integration_helpers-bdd.h"
#include "../unit/test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("rec relations execution: simple cases") {

	TEST_CASE("wff_rec_relation: direct substitution") {
		const char* sample =
			"g[0]($Y) ::= T."
			"g[0](Y).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation: direct substitution") {
		const char* sample =
			"g[0]($Y) := 1."
			"(g[0](Y) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("wff_rec_relation: two substitutions") {
		const char* sample =
			"g[0]($Y) ::= T."
			"g[$n]($Y) ::= g[$n - 1]($Y)."
			"g[1](Y).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation: two substitutions") {
		const char* sample =
			"g[0]($Y) := 1."
			"g[$n]($Y) := g[$n - 1]($Y)."
			"(g[1](Y) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("rec_relations execution: types") {

	TEST_CASE("clashing name nso_rr wff_rec_relation and bf_rec_relation: wff wins") {
		const char* sample =
			"g[0]($Y) := 0."
			"g[0]($Y) ::= T."
			"g[0](Y).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("clashing name nso_rr wff_rec_relation and bf_rec_relation: bf wins") {
		const char* sample =
			"g[0]($Y) := 0."
			"g[0]($Y) ::= T."
			"(g[0](Y) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("2d cases") {

	TEST_CASE("wff_rec_relation 2d: direct substitution") {
		const char* sample =
			"g[0, 0]($Y) ::= T."
			"g[0, 0](Y).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation 2d: direct substitution") {
		const char* sample =
			"g[0, 0]($Y) := 0."
			"(g[0, 0](Y) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("wff_rec_relation 2d: two substitutions 1st coord.") {
		const char* sample =
			"g[0, 0]($Y) ::= T."
			"g[$n, 0]($Y) ::= g[$n - 1, 0]($Y)."
			"g[1, 0](Y).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation 2d: two substitutions 1st coord.") {
		const char* sample =
			"g[0, 0]($Y) := 1."
			"g[$n, 0]($Y) := g[$n - 1, 0]($Y)."
			"(g[1, 0](Y) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("wff_rec_relation 2d: two substitutions 2nd coord.") {
		const char* sample =
			"g[0, 0]($Y) ::= T."
			"g[0, $n]($Y) ::= g[0, $n - 1]($Y)."
			"g[0, 1](Y).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation 2d: two substitutions 2nd coord.") {
		const char* sample =
			"g[0, 0]($Y) := 1."
			"g[0, $n]($Y) := g[0, $n - 1]($Y)."
			"(g[0, 1](Y) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("rec relations execution: several relations") {

	TEST_CASE("direct substitution, wff_rec_relation case") {
		const char* sample =
			"g[0, 0]($Y) ::= T."
			"h[0, 0]($Y) ::= T."
			"(g[0, 0](Y) && h[0, 0](T)).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("direct substitution, bf_rec_relation case") {
		const char* sample =
			"g[0, 0]($Y) := 1."
			"h[0, 0]($Y) := 1."
			"((g[0, 0](Y) & h[0,0](Y)) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("complex substitution, wff_rec_relation case.") {
		const char* sample =
			"g[0, 0]($Y) ::= T."
			"g[$n, 0]($Y) ::= g[$n - 1, 0]($Y)."
			"h[0, 0]($Y) ::= T."
			"h[0, $m]($Y) ::= h[0, $m - 1]($Y)."
			"(g[1, 0](Y) && h[0,1](Y)).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("complex substitution, bf_rec_relation case.") {
		const char* sample =
			"g[0, 0]($Y) := 1."
			"g[$n, 0]($Y) := g[$n - 1, 0]($Y)."
			"h[0, 0]($Y) := 0."
			"h[0, $m]($Y) := h[0, $m - 1]($Y)."
			"((g[1, 0](Y) & h[0,1](Y)) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("rec relations execution: longer offsets") {

	TEST_CASE("wff_rec_relation: case 1") {
		const char* sample =
			"g[0]($Y) ::= T."
			"g[$n]($Y) ::= g[$n - 2]($Y)."
			"g[4](Y).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("wff_rec_relation: case 2") {
		const char* sample =
			"g[0]($Y) ::= T."
			"g[1]($Y) ::= F."
			"g[$n]($Y) ::= g[$n - 2]($Y)."
			"g[5](Y).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation: case 1") {
		const char* sample =
			"g[0]($Y) := 1."
			"g[1]($Y) := 0."
			"g[$n]($Y) := g[$n - 2]($Y)."
			"(g[4](Y) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation: case 2") {
		const char* sample =
			"g[0]($Y) := 1."
			"g[1]($Y) := 0."
			"g[$n]($Y) := g[$n - 1]($Y)."
			"(g[5](Y) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}
}

TEST_SUITE("mutual rec cases") {

	TEST_CASE("wff_rec_relation: case 1") {
		const char* sample =
			"g[0]($Y) ::= T."
			"g[$n]($Y) ::= h[$n - 1]($Y)."
			"h[0]($Y) ::= F."
			"h[$n]($Y) ::= g[$n - 1]($Y)."
			"g[4](Y).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("wff_rec_relation: case 2") {
		const char* sample =
			"g[0]($Y) ::= T."
			"g[$n]($Y) ::= h[$n - 1]($Y)."
			"h[0]($Y) ::= F."
			"h[$n]($Y) ::= g[$n - 1]($Y)."
			"g[5](Y).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f; // wff_t
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation: case 1") {
		const char* sample =
			"g[0]($Y) := 1."
			"g[$n]($Y) := h[$n - 1]($Y)."
			"h[0]($Y) := 0."
			"h[$n]($Y) := g[$n - 1]($Y)."
			"(g[4](Y) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation: case 2") {
		const char* sample =
			"g[0]($Y) := 1."
			"g[$n]($Y) := h[$n - 1]($Y)."
			"h[0]($Y) := 0."
			"h[$n]($Y) := g[$n - 1]($Y)."
			"(g[5](Y) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t; // wff_f
		CHECK( check.has_value() );
	}

}

TEST_SUITE("multiple rec relations") {

	TEST_CASE("wff_rec_relation: case 1") {
		const char* sample =
			"g[0]($Y) ::= T."
			"h[0]($Y) ::= F."
			"(g[0](Y) && h[0](Y)).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("wff_rec_relation: case 2") {
		const char* sample =
			"g[0]($Y) ::= T."
			"g[$n]($Y) ::= g[$n - 1]($Y)."
			"h[0]($Y) ::= F."
			"h[$n]($Y) ::= h[$n - 1]($Y)."
			"(g[1](Y) && h[1](Y)).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("wff_rec_relation: case 3") {
		const char* sample =
			"g[0]($Y) ::= T."
			"g[$n]($Y) ::= g[$n - 1]($Y)."
			"h[0]($Y) ::= T."
			"h[$n]($Y) ::= h[$n - 1]($Y)."
			"(g[1](Y) && h[1](Y)).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}


	TEST_CASE("bf_rec_relation: case 1") {
		const char* sample =
			"g[0]($Y) := 0."
			"h[0]($Y) := 1."
			"((g[0](Y) | h[0](Y)) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation: case 2") {
		const char* sample =
			"g[0]($Y) := 1."
			"g[$n]($Y) := g[$n - 1]($Y)."
			"h[0]($Y) := 0."
			"h[$n]($Y) := h[$n - 1]($Y)."
			"((g[1](Y) & h[1](Y)) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("bf_rec_relation: case 3") {
		const char* sample =
			"g[0]($Y) := 1."
			"g[$n]($Y) := g[$n - 1]($Y)."
			"h[0]($Y) := 1."
			"h[$n]($Y) := h[$n - 1]($Y)."
			"((g[1](Y) & h[1](Y)) = 0).";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		factory_binder<bdd_test_factory, bdd_test> fb(bf);
		auto sample_formula = make_nso_rr_using_factory<factory_binder<bdd_test_factory_t, bdd_test>, bdd_test>(sample_src, fb);
		auto result = normalizer<bdd_test>(sample_formula);
		auto check = result |  tau_parser::wff_f;
		CHECK( check.has_value() );
	}
}