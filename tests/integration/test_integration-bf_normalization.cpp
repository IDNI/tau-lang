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

#include "doctest.h"
#include "nso_rr.h"
#include "bool_ba.h"
#include "bdd_handle.h"
#include "normalizer.h"
#include "bdd_binding.h"

#include "test_integration_helpers-bdd.h"
#include "../unit/test_helpers.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("Normalize Boolean function without recurrence relation | simple cases") {

	TEST_CASE("True and False") {
		const char* sample = "1 & 0";
		tau_parser::parse_options options;
		options.start = tau_parser::bf;
		auto sample_src = make_tau_source(sample, options);
		auto sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
		auto result = bf_normalizer_without_rec_relation<bdd_binding>(sample_formula.main);
		auto check = result |  tau_parser::bf_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("True or False") {
		const char* sample = "1 | 0";
		tau_parser::parse_options options;
		options.start = tau_parser::bf;
		auto sample_src = make_tau_source(sample, options);
		auto sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
		auto result = bf_normalizer_without_rec_relation<bdd_binding>(sample_formula.main);
		auto check = result |  tau_parser::bf_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("False and True") {
		const char* sample = "0 & 1";
		tau_parser::parse_options options;
		options.start = tau_parser::bf;
		auto sample_src = make_tau_source(sample, options);
		auto sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
		auto result = bf_normalizer_without_rec_relation<bdd_binding>(sample_formula.main);
		auto check = result |  tau_parser::bf_f;
		CHECK( check.has_value() );
	}

	TEST_CASE("False or True") {
		const char* sample = "0 | 1";
		tau_parser::parse_options options;
		options.start = tau_parser::bf;
		auto sample_src = make_tau_source(sample, options);
		auto sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
		auto result = bf_normalizer_without_rec_relation<bdd_binding>(sample_formula.main);
		auto check = result |  tau_parser::bf_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("X or X") {
		const char* sample = "X | X";
		tau_parser::parse_options options;
		options.start = tau_parser::bf;
		auto sample_src = make_tau_source(sample, options);
		auto sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
		auto result = bf_normalizer_without_rec_relation<bdd_binding>(sample_formula.main);
		auto check = result |  tau_parser::variable;
		CHECK( check.has_value() );
	}

	TEST_CASE("X and X") {
		const char* sample = "X & X";
		tau_parser::parse_options options;
		options.start = tau_parser::bf;
		auto sample_src = make_tau_source(sample, options);
		auto sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
		auto result = bf_normalizer_without_rec_relation<bdd_binding>(sample_formula.main);
		auto check = result |  tau_parser::variable;
		CHECK( check.has_value() );
	}

	TEST_CASE("X or X'") {
		const char* sample = "X | X'";
		tau_parser::parse_options options;
		options.start = tau_parser::bf;
		auto sample_src = make_tau_source(sample, options);
		auto sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
		auto result = bf_normalizer_without_rec_relation<bdd_binding>(sample_formula.main);
		auto check = result |  tau_parser::bf_t;
		CHECK( check.has_value() );
	}

	TEST_CASE("X and X'") {
		const char* sample = "X & X'";
		tau_parser::parse_options options;
		options.start = tau_parser::bf;
		auto sample_src = make_tau_source(sample, options);
		auto sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
		auto result = bf_normalizer_without_rec_relation<bdd_binding>(sample_formula.main);
		auto check = result |  tau_parser::bf_f;
		CHECK( check.has_value() );
	}
}


TEST_SUITE("Normalize Boolean function without recurrence relation | Simple SAT problems") {
	TEST_CASE("4 variables") {
		const char* sample = "ex x ex y ex v ex w (x' & y & v & w') != 0.";
		auto sample_src = make_tau_source(sample);
		auto sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
		auto result = normalizer<bdd_binding>(sample_formula);
		auto check = result |  tau_parser::wff_t;
		CHECK( check.has_value() );
	}

	//TEST_CASE("Quantifier Alternation") {
	//	const char* sample = "all x ex y all v ex w ((x' | y) & (y' | x) &  (v' | w) & (w' | v)) != 0.";
	//	auto sample_src = make_tau_source(sample);
	//	bdd_binding_factory bf;
	//	auto sample_formula = make_nso_rr_using_factory<bdd_binding_factory_t, bdd_binding>(sample_src, bf);
	//	auto result = normalizer<bdd_binding>(sample_formula);
	//	auto check = result |  tau_parser::wff_t;
	//	CHECK( check.has_value() );
	//}
}

TEST_SUITE("Normalize Boolean function with recurrence relation") {
	TEST_CASE("Alternating negation") {
		const char* rec =
			"h[n](X) := h[n - 1](X)'."
			"h[0](X) := X.";
		tau_parser::parse_options options;
		options.start = tau_parser::rec_relations;
		auto rec_src = make_tau_source(rec, options);
		auto rec_formula = make_nso_rr_using_factory<bdd_binding>(rec_src);
		rec_formula = infer_ref_types(rec_formula);
		const char* sample = "h[8](Y)";
	 	options.start = tau_parser::bf;
		auto sample_src = make_tau_source(sample, options);
		auto sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
		sample_formula.rec_relations = rec_formula.rec_relations;
		auto result = bf_normalizer_with_rec_relation<bdd_binding>(sample_formula);
		auto check = result | tau_parser::variable;
		CHECK( check.has_value() );
	}

	TEST_CASE("Dependend recurrence relations") {
	 	const char* rec =
	 		"h[n](X) := g[n - 1](X)'."
			"h[0](X) := X."
	 		"g[n](Y) := h[n - 1](Y)'."
	 		"g[0](Y) := Y'.";

	 	tau_parser::parse_options options;
	 	options.start = tau_parser::rec_relations;
	 	auto rec_src = make_tau_source(rec, options);
	 	auto rec_formula = make_nso_rr_using_factory<bdd_binding>(rec_src);
		rec_formula = infer_ref_types(rec_formula);
	 	const char* sample = "h[8](Y)";
	 	options.start = tau_parser::bf;
	 	auto sample_src = make_tau_source(sample, options);
	 	auto sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
	 	sample_formula.rec_relations = rec_formula.rec_relations;
	 	auto result = bf_normalizer_with_rec_relation<bdd_binding>(sample_formula);
	 	auto check = result | tau_parser::variable;
	 	CHECK( check.has_value() );
	 }
}

TEST_SUITE("BDD expressions") {
	TEST_CASE("X or Y") {
		bdd_init<Bool>();
		const char* sample = "{bdd : X} | {bdd : Y}";
		tau_parser::parse_options options;
		options.start = tau_parser::bf;
		auto sample_src = make_tau_source(sample, options);
		auto sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src);
		auto result = bf_normalizer_without_rec_relation<bdd_binding>(sample_formula.main);
		auto check = result |  tau_parser::bf_constant;
		CHECK( check.has_value() );
	}
}
