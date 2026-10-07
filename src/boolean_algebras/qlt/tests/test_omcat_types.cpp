// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for omcat::enumerate_qlt_T1 and friends — foundation for
// Algorithm D (direct parity-game construction).

#include "test_init.h"
#include <climits>
#include "omcat_types.h"
#include "boolean_algebras/qlt/omcat_constants.h"

using namespace idni::tau_lang;
using omcat::qlt_type1;
using omcat::rational;
using omcat::enumerate_qlt_T1;
using omcat::qlt_type_of;
using omcat::cmp;

TEST_SUITE("omcat type enumeration") {

	TEST_CASE("zero constants: |T_1| = 1") {
		auto ts = enumerate_qlt_T1({});
		CHECK(ts.size() == 1u);
		CHECK(ts[0].is_interval());
		CHECK(ts[0].index() == 0);
	}

	TEST_CASE("one constant (c=0): |T_1| = 3") {
		auto ts = enumerate_qlt_T1({rational(0, 1)});
		CHECK(ts.size() == 3u);
		// positions: 0=(-∞,0), 1={0}, 2=(0,+∞)
		CHECK(ts[0].is_interval());
		CHECK(ts[1].is_point());
		CHECK(ts[2].is_interval());
	}

	TEST_CASE("two constants (0, 1): |T_1| = 5") {
		auto ts = enumerate_qlt_T1({rational(0, 1), rational(1, 1)});
		CHECK(ts.size() == 5u);
		CHECK(ts[0].is_interval()); // (-∞, 0)
		CHECK(ts[1].is_point());    // {0}
		CHECK(ts[2].is_interval()); // (0, 1)
		CHECK(ts[3].is_point());    // {1}
		CHECK(ts[4].is_interval()); // (1, +∞)
	}

	TEST_CASE("dedup and sort constants") {
		auto ts = enumerate_qlt_T1(
			{rational(1, 1), rational(0, 1), rational(1, 1)});
		CHECK(ts.size() == 5u); // still 2 distinct constants ⇒ 5 types
	}

	TEST_CASE("realize: point type returns the constant") {
		auto ts = enumerate_qlt_T1({rational(3, 7)});
		auto w = ts[1].realize();
		CHECK(cmp(w, rational(3, 7)) == 0);
	}

	TEST_CASE("realize: interval type returns value in the interval") {
		auto ts = enumerate_qlt_T1({rational(0, 1), rational(1, 1)});
		auto w = ts[2].realize(); // (0, 1)
		CHECK(cmp(w, rational(0, 1)) > 0);
		CHECK(cmp(w, rational(1, 1)) < 0);
	}

	TEST_CASE("realize: (-∞, c) returns value below c") {
		auto ts = enumerate_qlt_T1({rational(5, 1)});
		auto w = ts[0].realize();
		CHECK(cmp(w, rational(5, 1)) < 0);
	}

	TEST_CASE("realize: (c, +∞) returns value above c") {
		auto ts = enumerate_qlt_T1({rational(5, 1)});
		auto w = ts[2].realize();
		CHECK(cmp(w, rational(5, 1)) > 0);
	}

	// AL-R2: `cmp` was widened to 128-bit because long long
	// cross-products overflow for parse-reachable magnitudes, but
	// `realize()` kept multiplying the same p/q pairs in 64 bits.  Two
	// constants with denominators around 3.04e9 make the old midpoint
	// denominator `2*a.q*b.q` overflow (signed UB), and the corrupted
	// witness then mis-orders T2/T3 enumeration.  The witness must lie
	// strictly inside the interval and `qlt_type_of` must invert it.
	TEST_CASE("[AL-R2] realize: interval witness does not overflow for large denominators") {
		std::vector<rational> cs = { rational(1, 3037000499LL), rational(1, 3037000500LL) };
		auto ts = enumerate_qlt_T1(cs);
		REQUIRE(ts.size() == 5u);
		// sorted: 1/3037000500 < 1/3037000499
		auto w = ts[2].realize();
		CHECK(cmp(w, rational(1, 3037000500LL)) > 0);
		CHECK(cmp(w, rational(1, 3037000499LL)) < 0);
		for (const auto& t : ts) {
			auto r = t.realize();
			CHECK(r.q > 0);
			CHECK(qlt_type_of(r, t.constants) == t.index());
		}
		// Outer intervals with extreme numerators: c-1 / c+1 must not wrap.
		std::vector<rational> big = { rational(LLONG_MAX - 1, 1), rational(LLONG_MIN + 2, 1) };
		auto tb = enumerate_qlt_T1(big);
		REQUIRE(tb.size() == 5u);
		CHECK(cmp(tb[0].realize(), rational(LLONG_MIN + 2, 1)) < 0);
		CHECK(cmp(tb[4].realize(), rational(LLONG_MAX - 1, 1)) > 0);
		CHECK(cmp(tb[2].realize(), rational(LLONG_MIN + 2, 1)) > 0);
		CHECK(cmp(tb[2].realize(), rational(LLONG_MAX - 1, 1)) < 0);
	}

	TEST_CASE("qlt_type_of inverts realize") {
		std::vector<rational> cs = {
			rational(-1, 2), rational(0, 1),
			rational(3, 4)
		};
		auto ts = enumerate_qlt_T1(cs);
		// cs is already sorted for type_of.
		for (const auto& t : ts) {
			auto w = t.realize();
			int idx = qlt_type_of(w, t.constants);
			CHECK(idx == t.index());
		}
	}

	TEST_CASE("T_2 enumeration: zero constants") {
		// Without constants, |T_1|=1 (one interval) ⇒ for (m,x) both in that
		// interval, three relations (LT, EQ, GT) are consistent: |T_2|=3.
		auto t2 = omcat::enumerate_qlt_T2({});
		CHECK(t2.size() == 3u);
	}

	TEST_CASE("T_2 enumeration: one constant") {
		// |T_1|=3. Pairs (m,x) with consistent relations:
		//   (0,0) same interval (-∞,c): 3 relations
		//   (0,1), (0,2), (1,2): forced LT            → 3
		//   (1,0), (2,0), (2,1): forced GT            → 3
		//   (1,1) same point {c}: forced EQ           → 1
		//   (2,2) same interval (c,+∞): 3 relations   → 3
		//   total = 3 + 3 + 3 + 1 + 3 = 13.
		auto t2 = omcat::enumerate_qlt_T2({omcat::rational(0, 1)});
		CHECK(t2.size() == 13u);
	}

	TEST_CASE("T_2 restriction") {
		auto t2 = omcat::enumerate_qlt_T2({omcat::rational(0, 1)});
		for (const auto& s : t2) {
			CHECK(s.restrict_m().index() == s.pos_m);
			CHECK(s.restrict_x().index() == s.pos_x);
		}
	}

	TEST_CASE("ordering predicates") {
		auto ts = enumerate_qlt_T1({rational(0, 1), rational(1, 1)});
		// ts[0] = (-∞, 0): < 0 < 1
		CHECK(ts[0].less_than(0) == true);
		CHECK(ts[0].less_than(1) == true);
		CHECK(ts[0].greater_than(0) == false);
		CHECK(ts[0].equal_to(0) == false);
		// ts[1] = {0}: = 0
		CHECK(ts[1].equal_to(0) == true);
		CHECK(ts[1].less_than(1) == true);
		// ts[2] = (0, 1)
		CHECK(ts[2].greater_than(0) == true);
		CHECK(ts[2].less_than(1) == true);
		// ts[4] = (1, +∞)
		CHECK(ts[4].greater_than(0) == true);
		CHECK(ts[4].greater_than(1) == true);
	}
}

TEST_SUITE("omcat: parse_rat_literal") {
	TEST_CASE("integer") {
		auto r = omcat::parse_rat_literal("42");
		REQUIRE(r.has_value());
		CHECK(omcat::cmp(r.value(), omcat::rational(42, 1)) == 0);
	}
	TEST_CASE("fraction") {
		auto r = omcat::parse_rat_literal("3/7");
		REQUIRE(r.has_value());
		CHECK(omcat::cmp(r.value(), omcat::rational(3, 7)) == 0);
	}
	TEST_CASE("negative fraction") {
		auto r = omcat::parse_rat_literal("-1/2");
		REQUIRE(r.has_value());
		CHECK(omcat::cmp(r.value(), omcat::rational(-1, 2)) == 0);
	}
	TEST_CASE("decimal") {
		auto r = omcat::parse_rat_literal("0.25");
		REQUIRE(r.has_value());
		CHECK(omcat::cmp(r.value(), omcat::rational(1, 4)) == 0);
	}
	TEST_CASE("decimal 0.45") {
		auto r = omcat::parse_rat_literal("0.45");
		REQUIRE(r.has_value());
		CHECK(omcat::cmp(r.value(), omcat::rational(9, 20)) == 0);
	}
	TEST_CASE("negative decimals") {
		auto half = omcat::parse_rat_literal("-0.5");
		REQUIRE(half.has_value());
		CHECK(omcat::cmp(half.value(), omcat::rational(-1, 2)) == 0);
		auto r = omcat::parse_rat_literal("-1.5");
		REQUIRE(r.has_value());
		CHECK(omcat::cmp(r.value(), omcat::rational(-3, 2)) == 0);
	}
	TEST_CASE("decimal at the least long long") {
		auto r = omcat::parse_rat_literal("-9223372036854775808.");
		REQUIRE(r.has_value());
		CHECK(omcat::cmp(r.value(), omcat::rational(LLONG_MIN, 1)) == 0);
		CHECK_FALSE(omcat::parse_rat_literal("-9223372036854775808.1")
			.has_value());
	}
	TEST_CASE("an out-of-range or malformed numeral is an invalid argument") {
		for (const char* src : { "9223372036854775808",
			"99999999999999999999/3", "3/99999999999999999999",
			"99999999999999999999.5", "0.1234567890123456789", "x", "1/x",
			"-", "1.x" })
		{
			CAPTURE(src);
			auto r = omcat::parse_rat_literal(src);
			CHECK(!r.has_value());
			CHECK(report_has_code(r.report(), code::invalid_argument));
		}
	}
	TEST_CASE("a negative denominator is an invalid argument") {
		for (const char* src : { "3/-4", "1/-9223372036854775808",
			"-9223372036854775808/-1" })
		{
			CAPTURE(src);
			auto r = omcat::parse_rat_literal(src);
			CHECK(!r.has_value());
			CHECK(report_has_code(r.report(), code::invalid_argument));
		}
	}
	TEST_CASE("invalid is an invalid argument") {
		auto r = omcat::parse_rat_literal("not-a-rational");
		CHECK(!r.has_value());
		CHECK(report_has_code(r.report(), code::invalid_argument));
	}
}

// BA2-12: the T3 layer (enumerate_qlt_T3, forced_rel_between,
// rel3_consistent, qlt_type3::restrict_*) had no direct coverage -- its only
// production caller is semantic_pwr, whose tests hand-build the vectors.
TEST_SUITE("qlt T3 enumeration (BA2-12)") {

	TEST_CASE("no constants: T3 over positions is transitively consistent") {
		auto t3 = omcat::enumerate_qlt_T3({});
		REQUIRE( !t3.empty() );
		for (const auto& t : t3) {
			// every member restricts to a valid T1
			CHECK( t.restrict_m().pos == t.pos_m );
			CHECK( t.restrict_x().pos == t.pos_x );
			CHECK( t.restrict_y().pos == t.pos_y );
			// the stored relations are mutually transitive
			CHECK( omcat::rel3_consistent(
				t.rel_mx, t.rel_xy, t.rel_my) );
		}
	}

	TEST_CASE("one constant: forced relations are honored") {
		auto t3 = omcat::enumerate_qlt_T3({ omcat::rational(1, 2) });
		REQUIRE( !t3.empty() );
		for (const auto& t : t3) {
			// a forced pair (-1 = free; else 0/1/2 = LT/EQ/GT)
			// always stores exactly the forced relation
			int f = omcat::forced_rel_between(
				t.restrict_m(), t.restrict_x());
			if (f >= 0) CHECK( (int) t.rel_mx == f );
		}
	}
}
