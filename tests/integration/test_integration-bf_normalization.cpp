// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

bool bf_normalize_and_check(const char* sample_, typename node_t::type nt) {
	auto pbf = parse_bf();
	tref formula = tau::get(sample_, pbf).value_or(nullptr);
	if (!formula) return false;
	auto nso_rr = get_nso_rr<node_t>(formula);
	if (!nso_rr.has_value()) return false;
	auto result = bf_normalizer_without_rec_relation<node_t>(
						nso_rr.value().main->get());
	if (!result.has_value()) return false;
	return tau::get(result.value()).child_is(nt);
}

TEST_SUITE("Normalize Boolean function without recurrence relation | simple cases") {

	TEST_CASE("True and False") {
		const char* sample = "1 & 0";
		CHECK( bf_normalize_and_check(sample, tau::bf_f) );
	}

	TEST_CASE("True or False") {
		const char* sample = "1 | 0";
		CHECK( bf_normalize_and_check(sample, tau::bf_t) );
	}

	TEST_CASE("False and True") {
		const char* sample = "0 & 1";
		CHECK( bf_normalize_and_check(sample, tau::bf_f) );
	}

	TEST_CASE("False or True") {
		const char* sample = "0 | 1";
		CHECK( bf_normalize_and_check(sample, tau::bf_t) );
	}

	TEST_CASE("X or X") {
		const char* sample = "X | X";
		CHECK( bf_normalize_and_check(sample, tau::variable) );
	}

	TEST_CASE("X and X") {
		const char* sample = "X & X";
		CHECK( bf_normalize_and_check(sample, tau::variable) );
	}

	TEST_CASE("X or X'") {
		const char* sample = "X | X'";
		CHECK( bf_normalize_and_check(sample, tau::bf_t) );
	}

	TEST_CASE("X and X'") {
		const char* sample = "X & X'";
		CHECK( bf_normalize_and_check(sample, tau::bf_f) );
	}
}


TEST_SUITE("Normalize Boolean function without recurrence relation | Simple SAT problems") {
	TEST_CASE("4 variables") {
		const char* sample = "ex x ex y ex v ex w (x' & y & v & w') != 0.";
		tref s = tau::get(sample).value_or(nullptr);
		CHECK( s != nullptr );
		if (!s) return;
		auto formula = get_nso_rr<node_t>(s);
		CHECK( formula.has_value() );
		if (!formula.has_value()) return;
		auto normalized = normalizer<node_t>(formula.value());
		CHECK( normalized.has_value() );
		if (!normalized.has_value()) return;
		tref result = normalized.value();
		CHECK( tau::get(result).child_is(tau::wff_t) );
	}

	TEST_CASE("Quantifier Alternation") {
		const char* sample = "all x ex y all v ex w ((x' | y) & (y' | x) &  (v' | w) & (w' | v)) != 0.";
		tref s = tau::get(sample).value_or(nullptr);
		CHECK( s != nullptr );
		if (!s) return;
		auto formula = get_nso_rr<node_t>(s);
		CHECK( formula.has_value() );
		if (!formula.has_value()) return;
		auto normalized = normalizer<node_t>(formula.value());
		CHECK( normalized.has_value() );
		if (!normalized.has_value()) return;
		tref result = normalized.value();
		CHECK( tau::get(result).child_is(tau::wff_t) );
	}
}

TEST_SUITE("Normalize Boolean function with recurrence relation") {

	TEST_CASE("Simple case (y1)") {
		const char* rec =
			"h(X):tau := 1.";
		const char* sample = "h(Y)";
		auto nso_rr = get_bf_nso_rr(rec, sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		auto result = bf_normalizer_with_rec_relation<node_t>(nso_rr.value());
		REQUIRE( result.has_value() );
		CHECK( tau::get(result.value()).child_is(tau::bf_t) );
	}

	TEST_CASE("Simple case (y2)") {
		const char* rec =
			"h(X):tau := X.";
		const char* sample = "h(Y)";
		auto nso_rr = get_bf_nso_rr(rec, sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		auto result = bf_normalizer_with_rec_relation<node_t>(nso_rr.value());
		REQUIRE( result.has_value() );
		CHECK( tau::get(result.value()).child_is(tau::variable) );
	}

	TEST_CASE("Simple case (y3)") {
		const char* rec =
			"f(X):tau := a x ^ b x.";
		const char* sample = "f(y)";
		auto nso_rr = get_bf_nso_rr(rec, sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		auto result = bf_normalizer_with_rec_relation<node_t>(nso_rr.value());
		REQUIRE( result.has_value() );
		CHECK( tau::get(result.value()).child_is(tau::bf_or) );
	}

	TEST_CASE("Alternating negation") {
		const char* rec =
			"h[n](X):tau := h[n - 1](X)'."
			"h[0](X):tau := X.";
		const char* sample = "h[8](Y)";
		auto nso_rr = get_bf_nso_rr(rec, sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		auto result = bf_normalizer_with_rec_relation<node_t>(nso_rr.value());
		REQUIRE( result.has_value() );
		CHECK( tau::get(result.value()).child_is(tau::variable) );
	}

	TEST_CASE("Dependend recurrence relations") {
	 	const char* rec =
	 		"h[n](X):tau := g[n - 1](X)'."
			"h[0](X):tau := X."
	 		"g[n](Y):tau := h[n - 1](Y)'."
	 		"g[0](Y):tau := Y'.";
	 	const char* sample = "h[8](Y)";
		auto nso_rr = get_bf_nso_rr(rec, sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		auto result = bf_normalizer_with_rec_relation<node_t>(nso_rr.value());
		REQUIRE( result.has_value() );
		CHECK( tau::get(result.value()).child_is(tau::variable) );
	 }
}

TEST_SUITE("SBF expressions") {
	TEST_CASE("X or Y") {
		bdd_init<Bool>();
		const char* sample = "{X}:sbf | {Y}:sbf";
		CHECK( bf_normalize_and_check(sample, tau::ba_constant) );
	}
}


TEST_SUITE("constant folds across an untyped operand") {

	// `1 | 0`, `0 | 1`, `1 & 0` and `0 & 1` with one side untyped fold to a
	// constant of the typed side's type.
	TEST_CASE("the typed operand gives the folded constant its type") {
		const size_t t = tau_type_id<node_t>();
		for (tref fm : { tau::build_bf_or(tau::_1(t), tau::_0(0)),
				tau::build_bf_or(tau::_0(0), tau::_1(t)) })
		{
			CHECK( tau::get(fm).equals_1() );
			CHECK( find_ba_type<node_t>(fm) == t );
		}
		for (tref fm : { tau::build_bf_and(tau::_1(t), tau::_0(0)),
				tau::build_bf_and(tau::_0(0), tau::_1(t)) })
		{
			CHECK( tau::get(fm).equals_0() );
			CHECK( find_ba_type<node_t>(fm) == t );
		}
	}

	// Two distinct concrete types are not folded.
	TEST_CASE("operands of two types keep the raw node") {
		const size_t t = tau_type_id<node_t>(), s = sbf_type_id<node_t>();
		CHECK( tau::get(tau::build_bf_or(tau::_1(t), tau::_0(s)))
			.child_is(tau::bf_or) );
		CHECK( tau::get(tau::build_bf_and(tau::_1(t), tau::_0(s)))
			.child_is(tau::bf_and) );
		tref x = build_bf_variable<node_t>("x", s);
		CHECK( tau::get(tau::build_bf_nlteq(tau::_0(t), x))
			.child_is(tau::bf_nlteq) );
		CHECK( tau::get(tau::build_bf_nlt(x, tau::_0(t)))
			.child_is(tau::bf_nlt) );
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
