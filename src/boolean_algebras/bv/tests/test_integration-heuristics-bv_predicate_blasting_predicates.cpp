// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "parser_helper.h"

#include "boolean_algebras/bv/bv_ba.h"
#include "boolean_algebras/bv/heuristics/bv_predicate_blasting.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}

	TEST_CASE("logging") {
		logging::trace();
	}
}

// A bitvector variable and a constant of width `w`.
static tref bv_var(size_t w) {
	return tau::build_bf_variable(bv_type_id<node_t>(w));
}
static tref bv_cte(size_t w, size_t v) {
	return tau::get(tau::bf, bv_constant<node_t>(w, v));
}

// Rewrites `call` with `rules` until no recurrence reference is left.
static tref apply_rules(const rewriter::rules& rules, tref call) {
	auto applied = nso_rr_apply(make_rr<node_t>(rules, call));
	REQUIRE( applied.has_value() );
	REQUIRE( applied.value() != nullptr );
	CHECK( tau::get(applied.value()).find_top(is<node_t, tau::ref>) == nullptr );
	return applied.value();
}

// Satisfiability of `fm` once each variable takes its value; variables a
// predicate introduces besides these are left free, so they are existential.
static bool holds_at(tref fm, const std::vector<std::pair<tref, tref>>& values) {
	for (const auto& [var, value] : values)
		fm = tau::build_wff_and(fm, tau::build_bf_eq(var, value));
	auto sat = is_non_temp_nso_satisfiable<node_t>(fm);
	REQUIRE( sat.has_value() );
	return sat.value();
}

// Whether the binary predicate `pred(x, y)` over 2-bit operands holds
// exactly where `expected(a, b)` does, over every pair of values.
template <typename P, typename E>
static void check_binary_2bit(P pred, E expected) {
	auto x = bv_var(2), y = bv_var(2);
	tref fm = pred(x, y);
	for (size_t a = 0; a < 4; ++a) for (size_t b = 0; b < 4; ++b) {
		CAPTURE(a); CAPTURE(b);
		CHECK( holds_at(fm, { { x, bv_cte(2, a) }, { y, bv_cte(2, b) } })
			== expected(a, b) );
	}
}

TEST_SUITE("bitblasting") {

	TEST_CASE("bvlt_rule(s)") {
		CHECK( bvlt_rules<node_t>(2).size() == 2 );
		auto rule = bvlt_rule<node_t>(2);
		REQUIRE( rule.has_value() );
		check_binary_2bit([&](tref x, tref y) {
			return apply_rules({ rule.value() },
				make_bvlt_call_from_index<node_t>(x, y, 1));
		}, [](size_t a, size_t b) { return a < b; });
	}

	TEST_CASE("bvgt is bvlt with the operands swapped") {
		check_binary_2bit([&](tref x, tref y) {
			auto gt = bvgt<node_t>(x, y);
			REQUIRE( gt.has_value() );
			return gt.value();
		}, [](size_t a, size_t b) { return a > b; });
	}

	TEST_CASE("bvshift_by_one_rule, left") {
		auto rule = bvshift_by_one_rule<node_t>(2, bv_shift_dir::left);
		REQUIRE( rule.has_value() );
		check_binary_2bit([&](tref x, tref y) {
			return apply_rules({ rule.value() },
				make_bvshift_by_one_call<node_t>(bv_shift_dir::left, x, y));
		}, [](size_t a, size_t b) { return b == ((a << 1) & 3); });
	}

	TEST_CASE("bvshift_by_one_rule, right") {
		auto rule = bvshift_by_one_rule<node_t>(2, bv_shift_dir::right);
		REQUIRE( rule.has_value() );
		check_binary_2bit([&](tref x, tref y) {
			return apply_rules({ rule.value() },
				make_bvshift_by_one_call<node_t>(bv_shift_dir::right, x, y));
		}, [](size_t a, size_t b) { return b == (a >> 1); });
	}

	TEST_CASE("a shift-by-one rule asked again for its width is the cached one") {
		using node = node_t;
		for (size_t i = 0; i < 2; ++i) {
			CHECK(bvshift_by_one_rule<node>(3, bv_shift_dir::left).has_value());
			CHECK(bvshift_by_one_rule<node>(3, bv_shift_dir::right).has_value());
		}
	}

	TEST_CASE("is_bit_zero_rule") {
		auto x = bv_var(2);
		tref fm = apply_rules({ is_bit_zero_rule<node_t>(1, 2) },
			make_is_bit_zero_call_from_index<node_t>(x, 1));
		for (size_t a = 0; a < 4; ++a) {
			CAPTURE(a);
			CHECK( holds_at(fm, { { x, bv_cte(2, a) } }) == ((a & 2) == 0) );
		}
	}

	TEST_CASE("is_bit_one_rule") {
		auto x = bv_var(2);
		tref fm = apply_rules({ is_bit_one_rule<node_t>(1, 2) },
			make_is_bit_one_call_from_index<node_t>(x, 1));
		for (size_t a = 0; a < 4; ++a) {
			CAPTURE(a);
			CHECK( holds_at(fm, { { x, bv_cte(2, a) } }) == ((a & 2) != 0) );
		}
	}

	TEST_CASE("bit_rule") {
		// _bit[1](x) is x masked to its bit 1: { 2 } when it is set, 0 if not.
		auto x = bv_var(2);
		tref masked = apply_rules({ bit_rule<node_t>(1, 2) },
			make_bit_call_from_index<node_t>(x, 1));
		for (size_t a = 0; a < 4; ++a) {
			CAPTURE(a);
			CHECK( holds_at(tau::build_bf_eq(masked, bv_cte(2, a & 2)),
				{ { x, bv_cte(2, a) } }) );
			CHECK( !holds_at(tau::build_bf_eq(masked, bv_cte(2, (a & 2) ^ 2)),
				{ { x, bv_cte(2, a) } }) );
		}
	}

	TEST_CASE("bit: extracting bit 0 gives a mask-and term of the right shape") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		tref bit0 = bit<node>(x, 0).value_or(nullptr);
		// bit(x, 0) is the AND of x with the { 1 } bitmask (bit 0 selector)
		CHECK( tau::get(bit0).find_top(is<node, tau::bf_and>) != nullptr );
	}

	TEST_CASE("bvshl_by_one and bvshr_by_one produce distinct satisfiable constraints") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto shifted = tau::build_bf_variable(bv_type_id<node>(4));
		auto shl1_r = bvshl_by_one<node>(x, shifted);
		auto shr1_r = bvshr_by_one<node>(x, shifted);
		REQUIRE( shl1_r.has_value() );
		REQUIRE( shr1_r.has_value() );
		tref shl1 = shl1_r.value();
		tref shr1 = shr1_r.value();
		CHECK( tau::get(shl1) != tau::get(shr1) );
		CHECK( is_non_temp_nso_satisfiable<node>(shl1).value() );
		CHECK( is_non_temp_nso_satisfiable<node>(shr1).value() );
	}

	TEST_CASE("bvadd") {
		// z = x + y modulo 16.
		trefs aux;
		auto x = bv_var(4), y = bv_var(4), z = bv_var(4);
		auto fm = bvadd<node_t>(x, y, z, aux);
		REQUIRE( fm.has_value() );
		CHECK( holds_at(fm.value(), { { x, bv_cte(4, 3) }, { y, bv_cte(4, 5) }, { z, bv_cte(4, 8) } }) );
		CHECK( !holds_at(fm.value(), { { x, bv_cte(4, 3) }, { y, bv_cte(4, 5) }, { z, bv_cte(4, 9) } }) );
		CHECK( holds_at(fm.value(), { { x, bv_cte(4, 9) }, { y, bv_cte(4, 9) }, { z, bv_cte(4, 2) } }) );
		CHECK( !holds_at(fm.value(), { { x, bv_cte(4, 9) }, { y, bv_cte(4, 9) }, { z, bv_cte(4, 3) } }) );
	}

	TEST_CASE("bvsub") {
		// z = x - y modulo 16.
		trefs aux;
		auto x = bv_var(4), y = bv_var(4), z = bv_var(4);
		auto fm = bvsub<node_t>(x, y, z, aux);
		REQUIRE( fm.has_value() );
		CHECK( holds_at(fm.value(), { { x, bv_cte(4, 5) }, { y, bv_cte(4, 3) }, { z, bv_cte(4, 2) } }) );
		CHECK( !holds_at(fm.value(), { { x, bv_cte(4, 5) }, { y, bv_cte(4, 3) }, { z, bv_cte(4, 3) } }) );
		CHECK( holds_at(fm.value(), { { x, bv_cte(4, 3) }, { y, bv_cte(4, 5) }, { z, bv_cte(4, 14) } }) );
		CHECK( !holds_at(fm.value(), { { x, bv_cte(4, 3) }, { y, bv_cte(4, 5) }, { z, bv_cte(4, 2) } }) );
	}

	TEST_CASE("bvshift_rule, left") {
		// The shift by { 1 }:bv[2] relates each 2-bit value to its shift.
		auto count = bv_cte(2, 1);
		auto rule = bvshift_rule<node_t>(count, bv_shift_dir::left);
		REQUIRE( rule.has_value() );
		check_binary_2bit([&](tref x, tref y) {
			return apply_rules({ rule.value() },
				make_bvshift_call<node_t>(bv_shift_dir::left, x, count, y));
		}, [](size_t a, size_t b) { return b == ((a << 1) & 3); });
	}

	TEST_CASE("bvshift_rule, right") {
		// The shift by { 1 }:bv[2] relates each 2-bit value to its shift.
		auto count = bv_cte(2, 1);
		auto rule = bvshift_rule<node_t>(count, bv_shift_dir::right);
		REQUIRE( rule.has_value() );
		check_binary_2bit([&](tref x, tref y) {
			return apply_rules({ rule.value() },
				make_bvshift_call<node_t>(bv_shift_dir::right, x, count, y));
		}, [](size_t a, size_t b) { return b == (a >> 1); });
	}

	TEST_CASE("bvmul") {
		// z = x * 2 modulo 16.
		trefs aux;
		auto x = bv_var(4), z = bv_var(4);
		auto fm = bvmul<node_t>(x, bv_cte(4, 2), z, aux);
		REQUIRE( fm.has_value() );
		CHECK( holds_at(fm.value(), { { x, bv_cte(4, 3) }, { z, bv_cte(4, 6) } }) );
		CHECK( !holds_at(fm.value(), { { x, bv_cte(4, 3) }, { z, bv_cte(4, 7) } }) );
		CHECK( holds_at(fm.value(), { { x, bv_cte(4, 9) }, { z, bv_cte(4, 2) } }) );
		CHECK( !holds_at(fm.value(), { { x, bv_cte(4, 9) }, { z, bv_cte(4, 3) } }) );
	}

	TEST_CASE("bvdiv") {
		// z = x / 2.
		trefs aux;
		auto x = bv_var(4), z = bv_var(4);
		auto fm = bvdiv<node_t>(x, bv_cte(4, 2), z, aux);
		REQUIRE( fm.has_value() );
		CHECK( holds_at(fm.value(), { { x, bv_cte(4, 7) }, { z, bv_cte(4, 3) } }) );
		CHECK( !holds_at(fm.value(), { { x, bv_cte(4, 7) }, { z, bv_cte(4, 4) } }) );
		CHECK( holds_at(fm.value(), { { x, bv_cte(4, 14) }, { z, bv_cte(4, 7) } }) );
		CHECK( !holds_at(fm.value(), { { x, bv_cte(4, 14) }, { z, bv_cte(4, 6) } }) );
	}

	TEST_CASE("bvmod") {
		// z = x % 2.
		trefs aux;
		auto x = bv_var(4), z = bv_var(4);
		auto fm = bvmod<node_t>(x, bv_cte(4, 2), z, aux);
		REQUIRE( fm.has_value() );
		CHECK( holds_at(fm.value(), { { x, bv_cte(4, 7) }, { z, bv_cte(4, 1) } }) );
		CHECK( !holds_at(fm.value(), { { x, bv_cte(4, 7) }, { z, bv_cte(4, 0) } }) );
		CHECK( holds_at(fm.value(), { { x, bv_cte(4, 14) }, { z, bv_cte(4, 0) } }) );
		CHECK( !holds_at(fm.value(), { { x, bv_cte(4, 14) }, { z, bv_cte(4, 1) } }) );
	}

	// bved is not called anywhere else in the library (atomic_blasting
	// dispatches "/" and "%" to bvdiv/bvmod separately, never fusing them
	// into a single bved call), so it has no WFF-level coverage either.
	// Check it directly: 10 = 3*3 + 1, so quotient=3, remainder=1 must
	// satisfy the combined constraint, while a wrong remainder must not.
	TEST_CASE("bved: 10 / 3 gives quotient=3, remainder=1") {
		using node = node_t;
		using tau = tree<node>;

		trefs aux;
		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto q = tau::build_bf_variable(bv_type_id<node>(4));
		auto r = tau::build_bf_variable(bv_type_id<node>(4));
		auto c = tau::get(tau::bf, bv_constant<node_t>(4, 3));
		tref constraint = bved<node>(x, c, q, r, aux).value_or(nullptr);
		tref x10 = tau::build_bf_eq(x, tau::get(tau::bf, bv_constant<node_t>(4, 10)));
		tref q3 = tau::build_bf_eq(q, tau::get(tau::bf, bv_constant<node_t>(4, 3)));
		tref r1 = tau::build_bf_eq(r, tau::get(tau::bf, bv_constant<node_t>(4, 1)));
		tref r2 = tau::build_bf_eq(r, tau::get(tau::bf, bv_constant<node_t>(4, 2)));
		tref good = tau::build_wff_and(tau::build_wff_and(constraint, x10),
			tau::build_wff_and(q3, r1));
		tref bad = tau::build_wff_and(tau::build_wff_and(constraint, x10),
			tau::build_wff_and(q3, r2));
		CHECK( is_non_temp_nso_satisfiable<node>(good).value() );
		CHECK( !is_non_temp_nso_satisfiable<node>(bad).value() );
	}

	TEST_CASE("bvneq_rule") {
		CHECK( bvneq_rules<node_t>(2).size() == 2 );
		auto rule = bvneq_rule<node_t>(2);
		REQUIRE( rule.has_value() );
		check_binary_2bit([&](tref x, tref y) {
			return apply_rules({ rule.value() },
				make_bvneq_call_from_index<node_t>(x, y, 1));
		}, [](size_t a, size_t b) { return a != b; });
	}

	// wff_predicate_blasting's dispatcher never calls bvlteq/bvgteq/bvnlteq/
	// bvnlt/bvngt/bvngteq/bvneq: "<="/">="/"!<="/"!<"/"!>"/"!>=" surface
	// syntax routes through lteq_predicate/gteq_predicate/nlteq_predicate/
	// nlt_predicate/ngt_predicate/ngteq_predicate instead, each of which
	// independently negates lt_predicate/gt_predicate rather than calling
	// these wrappers, and "!=" routes through neq_predicate ->
	// keep_comparison_predicate rather than bvneq. So this whole family of
	// public wrapper functions has zero coverage anywhere else. Check
	// directly that each delegates to the wrapper its definition names, and
	// that bvneq (a real recurrence, not a 1-line forward) computes the
	// right truth value for concrete operands.
	TEST_CASE("bvlteq delegates to bvgt") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		CHECK( tau::get(bvlteq<node>(x, y).value_or(nullptr))
			== tau::get(tau::build_wff_neg(bvgt<node>(x, y).value_or(nullptr))) );
	}

	TEST_CASE("bvgteq delegates to bvlt") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		CHECK( tau::get(bvgteq<node>(x, y).value_or(nullptr))
			== tau::get(tau::build_wff_neg(bvlt<node>(x, y).value_or(nullptr))) );
	}

	TEST_CASE("bvnlteq delegates to bvgt") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		CHECK( tau::get(bvnlteq<node>(x, y).value_or(nullptr)) == tau::get(bvgt<node>(x, y).value_or(nullptr)) );
	}

	TEST_CASE("bvnlt delegates to bvgteq") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		CHECK( tau::get(bvnlt<node>(x, y).value_or(nullptr)) == tau::get(bvgteq<node>(x, y).value_or(nullptr)) );
	}

	TEST_CASE("bvngt delegates to bvlteq") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		CHECK( tau::get(bvngt<node>(x, y).value_or(nullptr)) == tau::get(bvlteq<node>(x, y).value_or(nullptr)) );
	}

	TEST_CASE("bvngteq delegates to bvlt") {
		using node = node_t;
		using tau = tree<node>;

		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		auto y = tau::build_bf_variable(bv_type_id<node>(4));
		CHECK( tau::get(bvngteq<node>(x, y).value_or(nullptr)) == tau::get(bvlt<node>(x, y).value_or(nullptr)) );
	}

	TEST_CASE("bvneq: x != x is never satisfiable") {
		using node = node_t;
		using tau = tree<node>;

		// Operands must carry proper bv type metadata (as a variable built
		// via bv_type_id does) -- a bare ba_constant does not carry a
		// bitwidth get_bv_type_bitwidth can read, so the recurrence built
		// around it never resolves.
		auto x = tau::build_bf_variable(bv_type_id<node>(4));
		tref pred_same = bvneq<node>(x, x).value_or(nullptr);
		auto normalized = normalizer<node>(pred_same);
		REQUIRE( normalized.has_value() );
		CHECK( tau::get(normalized.value()).equals_F() );
	}
}

TEST_SUITE("bvcast") {

	TEST_CASE("bvcast: zero-extension (bv[2] -> bv[4])") {
		auto src = bv_var(2), res = bv_var(4);
		auto fm = bvcast<node_t>(src, res);
		REQUIRE( fm.has_value() );
		for (size_t a = 0; a < 4; ++a) for (size_t b = 0; b < 16; ++b) {
			CAPTURE(a); CAPTURE(b);
			CHECK( holds_at(fm.value(),
				{ { src, bv_cte(2, a) }, { res, bv_cte(4, b) } }) == (b == a) );
		}
	}

	TEST_CASE("bvcast: truncation (bv[4] -> bv[2])") {
		auto src = bv_var(4), res = bv_var(2);
		auto fm = bvcast<node_t>(src, res);
		REQUIRE( fm.has_value() );
		for (size_t a = 0; a < 16; ++a) for (size_t b = 0; b < 4; ++b) {
			CAPTURE(a); CAPTURE(b);
			CHECK( holds_at(fm.value(),
				{ { src, bv_cte(4, a) }, { res, bv_cte(2, b) } })
				== (b == (a & 3)) );
		}
	}
}

// The comparison dispatch of wff_predicate_blasting: every surface operator
// on ground bitvectors is blasted and decided. lt/gt/lteq/gteq/nlt/ngt/
// nlteq/ngteq/neq_predicate had no coverage in the full suite (2026-09-16)
// because nothing exercised the non-`=` operators through the blaster.
TEST_SUITE("bv predicate blasting: every comparison operator") {
	using node = node_t;
	static bool blasted_is_T(const char* spec) {
		tref fm = get_nso_rr<node>(tau::get(spec).value_or(nullptr)).value().main->get();
		tref blasted = bv_predicate_blasting<node>(fm).value_or(nullptr);
		REQUIRE(blasted != nullptr);
		auto n = normalizer<node>(blasted);
		REQUIRE(n.has_value());
		return tau::get(n.value()).equals_T();
	}
	TEST_CASE("< and !< on ground values") {
		CHECK( blasted_is_T("ex x (x = { 2 }:bv[3] && x < { 3 }:bv[3]).") );
		CHECK_FALSE( blasted_is_T("ex x (x = { 3 }:bv[3] && x < { 3 }:bv[3]).") );
		CHECK( blasted_is_T("ex x (x = { 3 }:bv[3] && x !< { 3 }:bv[3]).") );
	}
	TEST_CASE("> and !> on ground values") {
		CHECK( blasted_is_T("ex x (x = { 5 }:bv[3] && x > { 3 }:bv[3]).") );
		CHECK_FALSE( blasted_is_T("ex x (x = { 3 }:bv[3] && x > { 3 }:bv[3]).") );
		CHECK( blasted_is_T("ex x (x = { 3 }:bv[3] && x !> { 3 }:bv[3]).") );
	}
	TEST_CASE("<= and >= and their negations") {
		CHECK( blasted_is_T("ex x (x = { 3 }:bv[3] && x <= { 3 }:bv[3]).") );
		CHECK( blasted_is_T("ex x (x = { 3 }:bv[3] && x >= { 3 }:bv[3]).") );
		CHECK_FALSE( blasted_is_T("ex x (x = { 4 }:bv[3] && x <= { 3 }:bv[3]).") );
		CHECK( blasted_is_T("ex x (x = { 4 }:bv[3] && x !<= { 3 }:bv[3]).") );
		CHECK( blasted_is_T("ex x (x = { 2 }:bv[3] && x !>= { 3 }:bv[3]).") );
		CHECK_FALSE( blasted_is_T("ex x (x = { 3 }:bv[3] && x !>= { 3 }:bv[3]).") );
	}
	TEST_CASE("!= keeps the comparison and decides it") {
		CHECK( blasted_is_T("ex x (x = { 2 }:bv[3] && x != { 3 }:bv[3]).") );
		CHECK_FALSE( blasted_is_T("ex x (x = { 3 }:bv[3] && x != { 3 }:bv[3]).") );
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}

