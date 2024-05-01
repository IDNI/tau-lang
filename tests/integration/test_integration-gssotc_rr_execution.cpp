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
#include "satisfiability.h"
#include "bool_ba.h"
#include "bdd_handle.h"
#include "normalizer.h"

#include "test_integration_helpers-tau.h"

using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

TEST_SUITE("function execution: simple cases") {

	TEST_CASE("tau_rec_relation: direct substitution y1") {
		const char* sample =
			"g($Y) :::= {T}."
			"g(Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("tau_rec_relation: direct substitution y2") {
		const char* sample =
			"g($Y) :::= {F}."
			"g(Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}
}

TEST_SUITE("rec relations execution: simple cases") {

	TEST_CASE("tau_rec_relation: direct substitution y1") {
		const char* sample =
			"g[0]($Y) :::= {T}."
			"g[0](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("tau_rec_relation: direct substitution y2") {
		const char* sample =
			"g[0]($Y) :::= {F}."
			"g[0](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("tau_rec_relation: two substitutions") {
		const char* sample =
			"g[0]($Y) :::= {T}."
			"g[$n]($Y) :::= g[$n - 1]($Y)."
			"g[1](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}
}

TEST_SUITE("tau_rec_relations execution: types") {

	TEST_CASE("clashing names, gssotc wins") {
		const char* sample =
			"g[0]($Y) := 0."
			"g[0]($Y) ::= F."
			"g[0]($Y) :::= {T}."
			"g[0](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("clashing names, gssotc wins y2") {
		const char* sample =
			"g[0]($Y) := 1."
			"g[0]($Y) ::= T."
			"g[0]($Y) :::= {F}."
			"g[0](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("clashing names, nso wins") {
		const char* sample =
			"g[0]($Y) := 0."
			"g[0]($Y) ::= T."
			"g[0]($Y) :::= {F}."
			"{g[0](Y)};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("clashing names, nso wins y2") {
		const char* sample =
			"g[0]($Y) := 1."
			"g[0]($Y) ::= F."
			"g[0]($Y) :::= {T}."
			"{g[0](Y)};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("clashing names, bf wins, base case 1") {
		const char* sample =
			"g[0]($Y) := 1."
			"g[0]($Y) ::= T."
			"g[0]($Y) :::= {T}."
			"{(g[0](Y) = 0)};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("clashing names, bf wins") {
		const char* sample =
			"g[0]($Y) := 1."
			"g[0]($Y) ::= F."
			"g[0]($Y) :::= {F}."
			"{(g[0](Y) = 0)};";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}
}

TEST_SUITE("2d cases") {

	TEST_CASE("tau_rec_relation 2d: direct substitution") {
		const char* sample =
			"g[0, 0]($Y) :::= {T}."
			"g[0, 0](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("tau_rec_relation 2d: two substitutions 1st coord.") {
		const char* sample =
			"g[0, 0]($Y) :::= {T}."
			"g[$n, 0]($Y) :::= g[$n - 1, 0]($Y)."
			"g[1, 0](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("tau_rec_relation 2d: two substitutions 2nd coord.") {
		const char* sample =
			"g[0, 0]($Y) :::= {T}."
			"g[0, $n]($Y) :::= g[0, $n - 1]($Y)."
			"g[0, 1](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}
}

TEST_SUITE("rec relations execution: several relations") {

	TEST_CASE("direct substitution, tau_rec_relation case") {
		const char* sample =
			"g[0, 0]($Y) :::= {T}."
			"h[0, 0]($Y) :::= {T}."
			"g[0, 0](Y) &&& h[0, 0](1);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("complex substitution, tau_rec_relation case.") {
		const char* sample =
			"g[0, 0]($Y) :::= {T}."
			"g[$n, 0]($Y) :::= g[$n - 1, 0]($Y)."
			"h[0, 0]($Y) :::= {T}."
			"h[0, $m]($Y) :::= h[0, $m - 1]($Y)."
			"g[1, 0](Y) &&& h[0,1](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

}

TEST_SUITE("rec relations execution: longer offsets") {

	TEST_CASE("tau_rec_relation: case 1") {
		const char* sample =
			"g[0]($Y) :::= {T}."
			"g[$n]($Y) :::= g[$n - 2]($Y)."
			"g[4](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("tau_rec_relation: case 2") {
		const char* sample =
			"g[0]($Y) :::= {T}."
			"g[1]($Y) :::= {F}."
			"g[$n]($Y) :::= g[$n - 2]($Y)."
			"g[5](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}
}

TEST_SUITE("mutual rec cases") {

	TEST_CASE("tau_rec_relation: case 1") {
		const char* sample =
			"g[0]($Y) :::= {T}."
			"g[$n]($Y) :::= h[$n - 1]($Y)."
			"h[0]($Y) :::= {F}."
			"h[$n]($Y) :::= g[$n - 1]($Y)."
			"g[4](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("tau_rec_relation: case 2") {
		const char* sample =
			"g[0]($Y) :::= {T}."
			"g[$n]($Y) :::= h[$n - 1]($Y)."
			"h[0]($Y) :::= {F}."
			"h[$n]($Y) :::= g[$n - 1]($Y)."
			"g[5](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}
}

TEST_SUITE("multiple rec relations") {

	TEST_CASE("tau_rec_relation: case 1") {
		const char* sample =
			"g[0]($Y) :::= {T}."
			"h[0]($Y) :::= {F}."
			"g[0](Y) &&& h[0](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("tau_rec_relation: case 2") {
		const char* sample =
			"g[0]($Y) :::= {T}."
			"g[$n]($Y) :::= g[$n - 1]($Y)."
			"h[0]($Y) :::= {F}."
			"h[$n]($Y) :::= h[$n - 1]($Y)."
			"g[1](Y) &&& h[1](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( !is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}

	TEST_CASE("tau_rec_relation: case 3") {
		const char* sample =
			"g[0]($Y) :::= {T}."
			"g[$n]($Y) :::= g[$n - 1]($Y)."
			"h[0]($Y) :::= {T}."
			"h[$n]($Y) :::= h[$n - 1]($Y)."
			"g[1](Y) &&& h[1](Y);";
		auto sample_src = make_tau_source(sample);
		bdd_test_factory bf;
		auto sample_formula = make_tau_spec_using_factory<bdd_test_factory, bdd_test>(sample_src, bf);
		CHECK( is_tau_spec_satisfiable<bdd_test>(sample_formula) );
	}
}