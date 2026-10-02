// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for hsb_splitter (Strategy 3: dual-SMT midpoint split).
//
// Each test verifies one or more of the three core splitter properties:
//   (P1) s ≠ bot            — the splitter is satisfiable
//   (P2) s & ~x = bot       — the splitter is a sub-element of x
//   (P3) x & ~s ≠ bot       — the complement of s within x is non-empty

#include "test_init.h"
#include "boolean_algebras/hsb/hsb.h"

using idni::tau_lang::hsb;
using idni::tau_lang::hsb_halfspace;
using idni::tau_lang::hsb_splitter;
using idni::tau_lang::hsb_splitter_one;
using idni::tau_lang::is_hsb_zero;
using idni::tau_lang::is_hsb_one;
using idni::tau_lang::splitter_type;

// --- helpers -----------------------------------------------------------------

static hsb make_hs(std::vector<double> w, double b) {
	hsb_halfspace h; h.w = std::move(w); h.b = b;
	return hsb::from_halfspace(h);
}

// Checks P1 + P2 + P3 for a given x in one call.
static void check_partition(const hsb& x) {
	auto s = hsb_splitter(x, splitter_type::lower);
	CHECK(!is_hsb_zero(s));          // P1
	CHECK(is_hsb_zero(s & ~x));      // P2
	CHECK(!is_hsb_zero(x & ~s));     // P3
}

// =============================================================================

TEST_SUITE("hsb_splitter — trivial inputs") {

	TEST_CASE("bot maps to bot") {
		auto s = hsb_splitter(hsb::bottom(), splitter_type::lower);
		CHECK(s == hsb::bottom());
	}

	TEST_CASE("top yields non-trivial halfspace") {
		auto s = hsb_splitter(hsb::top(), splitter_type::lower);
		CHECK(!is_hsb_zero(s));
		CHECK(!is_hsb_one(s));
	}

	TEST_CASE("splitter_one is a non-trivial halfspace") {
		auto s = hsb_splitter_one();
		CHECK(s.root_kind() == hsb::kind::halfspace);
		CHECK(!is_hsb_zero(s));
		CHECK(!is_hsb_one(s));
	}

	TEST_CASE("splitter_one partitions top") {
		auto s     = hsb_splitter_one();
		auto other = hsb::top() & ~s;
		CHECK(!is_hsb_zero(s));     // P1
		CHECK(!is_hsb_zero(other)); // P3 applied to top
	}

} // TEST_SUITE trivial inputs

// =============================================================================

TEST_SUITE("hsb_splitter — 1D partition (P1+P2+P3)") {

	TEST_CASE("strict open halfspace x[0] < 5") {
		check_partition(make_hs({1.0}, -5.0));
	}

	TEST_CASE("non-strict halfspace x[0] >= 3") {
		// w=[-1], b=-3, s(w)=-1 => non-strict: -x[0]-3 <= 0 => x[0] >= 3
		check_partition(make_hs({-1.0}, -3.0));
	}

	TEST_CASE("complement of halfspace") {
		check_partition(~make_hs({1.0}, -5.0));
	}

	TEST_CASE("wide 1D interval [-100, 0)") {
		// x[0] < 0  and  x[0] >= -100
		auto x = make_hs({1.0}, 0.0) & make_hs({-1.0}, -100.0);
		check_partition(x);
	}

	TEST_CASE("narrow 1D interval (-eps, 0)") {
		auto x = make_hs({1.0}, 0.0) & make_hs({-1.0}, -0.001);
		check_partition(x);
	}

	TEST_CASE("halfspace with large positive bias") {
		check_partition(make_hs({1.0}, -1000.0));
	}

} // TEST_SUITE 1D

// =============================================================================

TEST_SUITE("hsb_splitter — 2D partition (P1+P2+P3)") {

	TEST_CASE("axis-aligned 2D box [0,10)^2") {
		auto x = make_hs({1.0, 0.0}, -10.0) & make_hs({-1.0, 0.0}, 0.0)
		       & make_hs({0.0, 1.0}, -10.0) & make_hs({0.0, -1.0}, 0.0);
		check_partition(x);
	}

	TEST_CASE("pure x[1] halfspace: split on highest axis") {
		// Only x[1] appears; Strategy 3 should find thickness on axis 1 first.
		auto x = make_hs({0.0, 1.0}, -5.0);
		check_partition(x);
	}

	TEST_CASE("conjunction along x[0] only") {
		auto x = make_hs({1.0, 0.0}, -5.0) & make_hs({-1.0, 0.0}, 0.0);
		check_partition(x);
	}

	TEST_CASE("diagonal halfspace x[0]+x[1] < 10") {
		check_partition(make_hs({1.0, 1.0}, -10.0));
	}

	TEST_CASE("anti-diagonal halfspace x[0]-x[1] < 5") {
		check_partition(make_hs({1.0, -1.0}, -5.0));
	}

	TEST_CASE("conjunction of two diagonal halfspaces") {
		auto x = make_hs({1.0, 1.0}, -8.0) & make_hs({1.0, -1.0}, -8.0);
		check_partition(x);
	}

	TEST_CASE("complement in 2D") {
		check_partition(~make_hs({1.0, 0.0}, -3.0));
	}

} // TEST_SUITE 2D

// =============================================================================

TEST_SUITE("hsb_splitter — 3D partition (P1+P2+P3)") {

	TEST_CASE("3D axis-aligned box") {
		auto x = make_hs({1.0, 0.0, 0.0}, -3.0)
		       & make_hs({0.0, 1.0, 0.0}, -3.0)
		       & make_hs({0.0, 0.0, 1.0}, -3.0);
		check_partition(x);
	}

	TEST_CASE("pure x[2] halfspace: split on highest axis") {
		auto x = make_hs({0.0, 0.0, 1.0}, -5.0);
		check_partition(x);
	}

	TEST_CASE("3D slab (x[1] only)") {
		auto x = make_hs({0.0, 1.0, 0.0}, -5.0) & make_hs({0.0, -1.0, 0.0}, -5.0);
		check_partition(x);
	}

} // TEST_SUITE 3D

// =============================================================================

TEST_SUITE("hsb_splitter — disjunctive formulas") {

	TEST_CASE("disjunction of two 1D intervals: partition") {
		// x[0] < -5  OR  x[0] >= 5
		auto left  = make_hs({1.0},  5.0);   // x[0] + 5 < 0  =>  x[0] < -5
		auto right = make_hs({-1.0}, -5.0);  // -x[0] - 5 <= 0 => x[0] >= 5
		auto x = left | right;
		auto s = hsb_splitter(x, splitter_type::lower);
		CHECK(!is_hsb_zero(s));          // P1
		CHECK(is_hsb_zero(s & ~x));      // P2
		CHECK(!is_hsb_zero(x & ~s));     // P3
	}

	TEST_CASE("disjunction of three intervals: partition") {
		auto a = make_hs({1.0},  10.0);  // x[0] < -10
		auto b = make_hs({-1.0},  0.0) & make_hs({1.0}, -2.0);  // 0 <= x[0] < 2 (ish)
		auto c = make_hs({-1.0}, -5.0);  // x[0] >= 5
		auto x = a | b | c;
		auto s = hsb_splitter(x, splitter_type::lower);
		CHECK(!is_hsb_zero(s));
		CHECK(is_hsb_zero(s & ~x));
		CHECK(!is_hsb_zero(x & ~s));
	}

	TEST_CASE("disjunction where one clause is infeasible") {
		auto good   = make_hs({1.0}, -5.0);           // x[0] < 5
		// structurally bot after short-circuit: x[0]<0 & x[0]>=0
		auto contra = make_hs({1.0}, 0.0) & ~make_hs({1.0}, 0.0);
		auto x = good | contra;
		auto s = hsb_splitter(x, splitter_type::lower);
		CHECK(!is_hsb_zero(s));   // P1: at least the good clause is splittable
	}

} // TEST_SUITE disjunctions

// =============================================================================

TEST_SUITE("hsb_splitter — atomlessness (repeated splitting)") {

	TEST_CASE("1D: 10 rounds stay non-empty") {
		auto x = make_hs({1.0}, -100.0);
		for (int i = 0; i < 10; ++i) {
			auto s = hsb_splitter(x, splitter_type::lower);
			CHECK(!is_hsb_zero(s));
			x = s;
		}
	}

	TEST_CASE("2D: 6 rounds stay non-empty") {
		auto x = make_hs({1.0, 0.0}, -10.0) & make_hs({0.0, 1.0}, -10.0);
		for (int i = 0; i < 6; ++i) {
			auto s = hsb_splitter(x, splitter_type::lower);
			CHECK(!is_hsb_zero(s));
			x = s;
		}
	}

	TEST_CASE("complement half is also splittable") {
		auto x  = make_hs({1.0}, -10.0);
		auto s  = hsb_splitter(x, splitter_type::lower);
		auto rest = x & ~s;
		CHECK(!is_hsb_zero(s));
		CHECK(!is_hsb_zero(rest));
		// Both halves can be split again
		CHECK(!is_hsb_zero(hsb_splitter(s,    splitter_type::lower)));
		CHECK(!is_hsb_zero(hsb_splitter(rest, splitter_type::lower)));
	}

	TEST_CASE("top splits indefinitely") {
		auto h = hsb::top();
		for (int i = 0; i < 5; ++i) {
			auto s = hsb_splitter(h, splitter_type::lower);
			CHECK(!is_hsb_zero(s));
			CHECK(!is_hsb_one(s));
			h = s;
		}
	}

} // TEST_SUITE atomlessness

// =============================================================================

// The structural factories (mk_and/mk_or/mk_not) skip the short-circuit
// simplifications of operator&/|/~, so trees holding top, bot or not_ nodes
// below the root reach the LRA helpers only when built through them.
TEST_SUITE("hsb_splitter — unsimplified trees") {

	using idni::tau_lang::hsb_tree;
	using idni::tau_lang::tref;
	namespace hd = idni::tau_lang::detail;

	static tref hs_ref(std::vector<double> w, double b) {
		return make_hs(std::move(w), b).root_ref();
	}

	TEST_CASE("collect_conjunction: top and bot leaves, nested and_, or_") {
		tref a = hs_ref({1.0}, -5.0);
		std::vector<hd::linear_constraint> cs;
		CHECK(!hd::collect_conjunction(nullptr, cs));
		CHECK(hd::collect_conjunction(hsb::mk_top(), cs));
		CHECK(cs.empty());
		CHECK(!hd::collect_conjunction(hsb::mk_bot(), cs));
		CHECK(hd::collect_conjunction(hsb::mk_and(a, hsb::mk_top()), cs));
		CHECK(cs.size() == 1);
		CHECK(cs[0].strict);
		cs.clear();
		CHECK(!hd::collect_conjunction(hsb::mk_and(a, hsb::mk_bot()), cs));
		cs.clear();
		// An or_ node is not a conjunction: accepted, but contributes nothing.
		CHECK(hd::collect_conjunction(hsb::mk_or(a, a), cs));
		CHECK(cs.empty());
	}

	TEST_CASE("to_dnf: bot, top, not_ and null") {
		tref a = hs_ref({1.0}, -5.0);
		std::vector<std::vector<tref>> dnf;
		hd::to_dnf(hsb::mk_bot(), dnf);
		CHECK(dnf.empty());
		hd::to_dnf(hsb::mk_top(), dnf);
		REQUIRE(dnf.size() == 1);
		CHECK(dnf[0].empty());
		dnf.clear();
		hd::to_dnf(hsb::mk_not(a), dnf);
		REQUIRE(dnf.size() == 1);
		REQUIRE(dnf[0].size() == 1);
		CHECK(dnf[0][0] == (~make_hs({1.0}, -5.0)).root_ref());
		hd::to_dnf(nullptr, dnf);
		CHECK(dnf.empty());
	}

	TEST_CASE("push_neg: constants, double negation and De Morgan") {
		tref a = hs_ref({1.0}, -5.0);
		tref b = hs_ref({-1.0}, -1.0);
		CHECK(hd::push_neg(nullptr) == hsb::mk_bot());
		CHECK(hd::push_neg(hsb::mk_bot()) == hsb::mk_top());
		CHECK(hd::push_neg(hsb::mk_top()) == hsb::mk_bot());
		CHECK(hd::push_neg(hsb::mk_not(a)) == a);
		tref na = (~make_hs({1.0}, -5.0)).root_ref();
		tref nb = (~make_hs({-1.0}, -1.0)).root_ref();
		CHECK(hd::push_neg(hsb::mk_or(a, b)) == hsb::mk_and(na, nb));
		CHECK(hd::push_neg(hsb::mk_and(a, b)) == hsb::mk_or(na, nb));
	}

	TEST_CASE("infer_dim: not_ reads its child, constants have none") {
		CHECK(hd::infer_dim(nullptr) == 0);
		CHECK(hd::infer_dim(hsb::mk_bot()) == 0);
		CHECK(hd::infer_dim(hsb::mk_top()) == 0);
		CHECK(hd::infer_dim(hsb::mk_not(hs_ref({0.0, 0.0, 1.0}, -1.0))) == 3);
	}

	TEST_CASE("find_feasible_point: no constraint is the origin") {
		auto pt = hd::find_feasible_point({}, 2);
		REQUIRE(pt.has_value());
		CHECK(*pt == std::vector<double>{0.0, 0.0});
	}

	TEST_CASE("find_feasible_point: the point satisfies the constraints") {
		// -x[0] + 3 <= 0, i.e. x[0] >= 3
		std::vector<hd::linear_constraint> cs{ { {-1.0}, 3.0, false } };
		auto pt = hd::find_feasible_point(cs, 1);
		REQUIRE(pt.has_value());
		CHECK((*pt)[0] >= 3.0);
	}

	TEST_CASE("find_second_feasible_point: below the given value") {
		// x[0] >= 0
		std::vector<hd::linear_constraint> cs{ { {-1.0}, 0.0, false } };
		CHECK(!hd::find_second_feasible_point(cs, 1, 0, 0.0, false));
		auto pt = hd::find_second_feasible_point(cs, 1, 0, 5.0, false);
		REQUIRE(pt.has_value());
		CHECK((*pt)[0] >= 0.0);
		CHECK((*pt)[0] < 5.0);
	}

	TEST_CASE("a contradiction under an and_ root is zero and not split") {
		tref a = hs_ref({1.0}, -5.0);
		hsb x(hsb::mk_and(a, hsb::mk_not(a)));
		CHECK(x.root_kind() == hsb::kind::and_);
		CHECK(is_hsb_zero(x));
		// No feasible clause: the input comes back unchanged.
		CHECK(hsb_splitter(x, splitter_type::lower) == x);
	}

	TEST_CASE("a disjunction of bottoms is zero") {
		hsb x(hsb::mk_or(hsb::mk_bot(), hsb::mk_bot()));
		CHECK(x.root_kind() == hsb::kind::or_);
		CHECK(is_hsb_zero(x));
		CHECK(hsb_splitter(x, splitter_type::lower) == x);
	}

	TEST_CASE("a disjunction of tops is one and splits on a fresh axis") {
		hsb x(hsb::mk_or(hsb::mk_top(), hsb::mk_top()));
		CHECK(!is_hsb_zero(x));
		CHECK(is_hsb_one(x));
		auto s = hsb_splitter(x, splitter_type::lower);
		CHECK(s != x);
		CHECK(!is_hsb_zero(s));          // P1
		CHECK(is_hsb_zero(s & ~x));      // P2
		CHECK(!is_hsb_zero(x & ~s));     // P3
	}
}
