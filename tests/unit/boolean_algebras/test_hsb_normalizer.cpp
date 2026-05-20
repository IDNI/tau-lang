// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for normalize_hsb and the canonical decomposition algorithm.
//
// Properties verified:
//   (N1) Semantic equivalence  — normalize(x) represents the same region as x.
//   (N2) Idempotency           — normalize(normalize(x)) == normalize(x).
//   (N3) Canonical form        — two semantically equal formulas produce ==
//                                outputs from normalize_hsb.
//   (N4) Disjointness          — every pair of output clauses has empty meet.
//   (N5) Phase-1 correctness   — hsb_halfspace::normalize() divides by the
//                                absolute value of the highest-indexed coeff.

#include "test_init.h"
#include "boolean_algebras/hsb.h"

using idni::tau_lang::hsb;
using idni::tau_lang::hsb_halfspace;
using idni::tau_lang::normalize_hsb;
using idni::tau_lang::is_hsb_zero;
using idni::tau_lang::is_hsb_one;

// =============================================================================
// Helpers
// =============================================================================

static hsb make_hs(std::vector<double> w, double b) {
	hsb_halfspace h; h.w = std::move(w); h.b = b;
	return hsb::from_halfspace(h);
}

static hsb_halfspace make_raw_hs(std::vector<double> w, double b) {
	hsb_halfspace h; h.w = std::move(w); h.b = b;
	return h;
}

// Count top-level OR clauses in a formula (1 if not an or_ node).
static size_t count_top_clauses(const hsb& f) {
	if (!f.root) return 0;
	if (f.root->k != hsb::kind::or_) return 1;
	return 1 + count_top_clauses(hsb{f.root->rhs})
	         + count_top_clauses(hsb{f.root->lhs}) - 1;
}

// Return true iff the formula tree contains any kind::not_ node.
static bool has_negation(const hsb::node_ptr& n) {
	if (!n) return false;
	if (n->k == hsb::kind::not_) return true;
	return has_negation(n->lhs) || has_negation(n->rhs) || has_negation(n->inner);
}

// Collect top-level disjuncts of an hsb formula into a flat vector.
static void collect_clauses(const hsb& f, std::vector<hsb>& out) {
	if (!f.root) return;
	if (f.root->k == hsb::kind::or_) {
		collect_clauses(hsb{f.root->lhs}, out);
		collect_clauses(hsb{f.root->rhs}, out);
	} else {
		out.push_back(f);
	}
}

// =============================================================================
// TEST SUITE: hsb_halfspace::normalize() — Phase-1
// =============================================================================

TEST_SUITE("hsb_normalizer — hsb_halfspace::normalize (Phase 1)") {

	TEST_CASE("1D: divide by absolute value of highest-indexed coeff") {
		auto h  = make_raw_hs({4.0}, -8.0);
		auto hn = h.normalize();
		// divisor = |4| = 4: w=[1], b=-2
		CHECK(hn.w.size() == 1);
		CHECK(hn.w[0] == doctest::Approx(1.0));
		CHECK(hn.b   == doctest::Approx(-2.0));
	}

	TEST_CASE("1D: negative leading coefficient preserved in sign") {
		auto h  = make_raw_hs({-3.0}, 6.0);
		auto hn = h.normalize();
		// divisor = |-3| = 3: w=[-1], b=2
		CHECK(hn.w[0] == doctest::Approx(-1.0));
		CHECK(hn.b    == doctest::Approx(2.0));
	}

	TEST_CASE("2D: normalize divides by |w[highest non-zero]|") {
		// w=[4, 2], b=0 → highest non-zero index = 1, |w[1]|=2 → w=[2,1], b=0
		auto h  = make_raw_hs({4.0, 2.0}, 0.0);
		auto hn = h.normalize();
		REQUIRE(hn.w.size() == 2);
		CHECK(hn.w[0] == doctest::Approx(2.0));
		CHECK(hn.w[1] == doctest::Approx(1.0));
		CHECK(hn.b    == doctest::Approx(0.0));
	}

	TEST_CASE("2D: trailing zero coefficient — uses last non-zero") {
		// w=[0, -6], b=12 → highest non-zero index=1, |w[1]|=6 → w=[0,-1], b=2
		auto h  = make_raw_hs({0.0, -6.0}, 12.0);
		auto hn = h.normalize();
		REQUIRE(hn.w.size() == 2);
		CHECK(hn.w[0] == doctest::Approx(0.0));
		CHECK(hn.w[1] == doctest::Approx(-1.0));
		CHECK(hn.b    == doctest::Approx(2.0));
	}

	TEST_CASE("already-normalized halfspace is unchanged") {
		auto h  = make_raw_hs({1.0, -1.0}, 3.0);
		auto hn = h.normalize();
		// highest non-zero index=1, |w[1]|=1 → no change
		REQUIRE(hn.w.size() == 2);
		CHECK(hn.w[0] == doctest::Approx(1.0));
		CHECK(hn.w[1] == doctest::Approx(-1.0));
		CHECK(hn.b    == doctest::Approx(3.0));
	}

	TEST_CASE("zero vector is returned unchanged") {
		auto h  = make_raw_hs({0.0, 0.0}, 5.0);
		auto hn = h.normalize();
		REQUIRE(hn.w.size() == 2);
		CHECK(hn.w[0] == doctest::Approx(0.0));
		CHECK(hn.w[1] == doctest::Approx(0.0));
		CHECK(hn.b    == doctest::Approx(5.0));
	}

	TEST_CASE("is_strict() is preserved by normalization") {
		// strict halfspace (positive leading coeff)
		auto hs_strict = make_raw_hs({3.0}, -9.0);
		CHECK(hs_strict.is_strict() == hs_strict.normalize().is_strict());

		// non-strict halfspace (negative leading coeff)
		auto hs_nonstrict = make_raw_hs({-2.0}, 4.0);
		CHECK(hs_nonstrict.is_strict() == hs_nonstrict.normalize().is_strict());
	}

	TEST_CASE("normalize is idempotent on halfspaces") {
		auto h  = make_raw_hs({6.0, 3.0}, -9.0);
		auto n1 = h.normalize();
		auto n2 = n1.normalize();
		CHECK(n1 == n2);
	}

	TEST_CASE("two scaled halfspaces normalize to the same representation") {
		// 2*x[0] - 10 < 0  and  x[0] - 5 < 0 both represent {x[0] < 5}
		auto h1 = make_raw_hs({2.0}, -10.0).normalize();
		auto h2 = make_raw_hs({1.0}, -5.0).normalize();
		CHECK(h1 == h2);
	}

} // TEST_SUITE Phase-1

// =============================================================================
// TEST SUITE: hsb_halfspace::operator< — boundary ordering
// =============================================================================

TEST_SUITE("hsb_normalizer — hsb_halfspace::operator<") {

	TEST_CASE("smaller first coefficient is less") {
		auto h1 = make_raw_hs({1.0}, 0.0);
		auto h2 = make_raw_hs({2.0}, 0.0);
		CHECK(h1 < h2);
		CHECK(!(h2 < h1));
	}

	TEST_CASE("equal first coefficient: compare on bias") {
		auto h1 = make_raw_hs({1.0}, -3.0);
		auto h2 = make_raw_hs({1.0}, -1.0);
		CHECK(h1 < h2);
		CHECK(!(h2 < h1));
	}

	TEST_CASE("equal coefficients and bias: not less-than") {
		auto h1 = make_raw_hs({1.0, -2.0}, 5.0);
		auto h2 = make_raw_hs({1.0, -2.0}, 5.0);
		CHECK(!(h1 < h2));
		CHECK(!(h2 < h1));
	}

	TEST_CASE("2D: first differing component determines order") {
		auto h1 = make_raw_hs({1.0, 3.0}, 0.0);
		auto h2 = make_raw_hs({1.0, 5.0}, 0.0);
		CHECK(h1 < h2);
		CHECK(!(h2 < h1));
	}

	TEST_CASE("2D vs 1D with implicit zero padding") {
		auto h1 = make_raw_hs({0.0}, 5.0);    // 1D: [0], b=5
		auto h2 = make_raw_hs({0.0, 1.0}, 5.0); // 2D: [0,1], b=5 — bigger due to w[1]=1>0
		CHECK(h1 < h2);
		CHECK(!(h2 < h1));
	}

} // TEST_SUITE operator<

// =============================================================================
// TEST SUITE: normalize_hsb — trivial and constant inputs
// =============================================================================

TEST_SUITE("hsb_normalizer — normalize_hsb trivial inputs") {

	TEST_CASE("bottom → bottom") {
		CHECK(normalize_hsb(hsb::bottom()) == hsb::bottom());
	}

	TEST_CASE("top → top") {
		CHECK(normalize_hsb(hsb::top()) == hsb::top());
	}

	TEST_CASE("contradiction collapses to bottom") {
		// x[0] + 1 < 0  AND  -x[0] + 2 <= 0  =>  x[0] < -1 AND x[0] <= -2
		// These are compatible (x[0] < -2 region is non-empty... wait let me pick incompatible ones)
		// x[0] < 0 AND x[0] >= 0 — complement pair → infeasible
		auto pos = make_hs({1.0}, 0.0);   // x[0] < 0
		auto neg = ~pos;                   // x[0] >= 0
		CHECK(normalize_hsb(pos & neg) == hsb::bottom());
	}

	TEST_CASE("tautology (h | ~h) collapses to top") {
		auto h = make_hs({1.0}, -5.0);
		CHECK(normalize_hsb(h | ~h) == hsb::top());
	}

	TEST_CASE("2D tautology collapses to top") {
		auto h = make_hs({1.0, 1.0}, -7.0);
		CHECK(normalize_hsb(h | ~h) == hsb::top());
	}

} // TEST_SUITE trivial

// =============================================================================
// TEST SUITE: normalize_hsb — (N1) Semantic equivalence
// =============================================================================

TEST_SUITE("hsb_normalizer — (N1) semantic equivalence") {

	TEST_CASE("1D single halfspace: normalize preserves region") {
		auto h = make_hs({1.0}, -5.0);       // x[0] < 5
		auto n = normalize_hsb(h);
		CHECK(is_hsb_zero(n ^ h));            // symmetric difference == 0
	}

	TEST_CASE("1D interval: normalize preserves region") {
		// x[0] >= 0 AND x[0] < 3
		auto x = make_hs({-1.0}, 0.0) & make_hs({1.0}, -3.0);
		auto n = normalize_hsb(x);
		CHECK(is_hsb_zero(n ^ x));
	}

	TEST_CASE("2D conjunction: normalize preserves region") {
		auto x = make_hs({1.0, 0.0}, -4.0) & make_hs({0.0, 1.0}, -4.0);
		auto n = normalize_hsb(x);
		CHECK(is_hsb_zero(n ^ x));
	}

	TEST_CASE("2D disjunction: normalize preserves region") {
		auto a = make_hs({1.0, 0.0}, -2.0);
		auto b = make_hs({0.0, 1.0}, -2.0);
		auto x = a | b;
		auto n = normalize_hsb(x);
		CHECK(is_hsb_zero(n ^ x));
	}

	TEST_CASE("negation of 2D halfspace: normalize preserves region") {
		auto h = make_hs({1.0, 0.0}, -3.0);
		auto x = ~h;
		auto n = normalize_hsb(x);
		CHECK(is_hsb_zero(n ^ x));
	}

	TEST_CASE("De Morgan form: normalize preserves region") {
		auto a = make_hs({1.0}, -2.0);
		auto b = make_hs({-1.0}, -2.0);
		auto demorgan  = ~(~a & ~b);     // = a | b  (by De Morgan)
		auto direct    = a | b;
		CHECK(is_hsb_zero(normalize_hsb(demorgan) ^ direct));
	}

	TEST_CASE("3D conjunction: normalize preserves region") {
		auto x = make_hs({1.0, 0.0, 0.0}, -5.0)
		       & make_hs({0.0, 1.0, 0.0}, -5.0)
		       & make_hs({0.0, 0.0, 1.0}, -5.0);
		auto n = normalize_hsb(x);
		CHECK(is_hsb_zero(n ^ x));
	}

	TEST_CASE("scaled halfspace: semantic equivalence after normalize") {
		// 2*x[0] - 10 < 0  (same region as x[0] < 5)
		auto scaled = make_hs({2.0}, -10.0);
		auto unit   = make_hs({1.0}, -5.0);
		// The two formulas represent the same region, so normalize of the
		// scaled version is semantically equivalent to the unit version.
		CHECK(is_hsb_zero(normalize_hsb(scaled) ^ unit));
	}

} // TEST_SUITE semantic-equivalence

// =============================================================================
// TEST SUITE: normalize_hsb — (N2) Idempotency
// =============================================================================

TEST_SUITE("hsb_normalizer — (N2) idempotency") {

	TEST_CASE("1D single halfspace is idempotent") {
		auto h  = make_hs({1.0}, -5.0);
		auto n1 = normalize_hsb(h);
		auto n2 = normalize_hsb(n1);
		CHECK(n1 == n2);
	}

	TEST_CASE("1D interval is idempotent") {
		auto x  = make_hs({-1.0}, 0.0) & make_hs({1.0}, -3.0);
		auto n1 = normalize_hsb(x);
		auto n2 = normalize_hsb(n1);
		CHECK(n1 == n2);
	}

	TEST_CASE("2D conjunction is idempotent") {
		auto x  = make_hs({1.0, 0.0}, -4.0) & make_hs({0.0, 1.0}, -4.0);
		auto n1 = normalize_hsb(x);
		auto n2 = normalize_hsb(n1);
		CHECK(n1 == n2);
	}

	TEST_CASE("L-shape (2D) is idempotent") {
		// [0,2) x [0,1)  union  [0,1) x [1,2)
		auto p1 = make_hs({-1.0, 0.0}, 0.0) & make_hs({1.0, 0.0}, -2.0)
		        & make_hs({0.0, -1.0}, 0.0) & make_hs({0.0, 1.0}, -1.0);
		auto p2 = make_hs({-1.0, 0.0}, 0.0) & make_hs({1.0, 0.0}, -1.0)
		        & make_hs({0.0, -1.0}, 1.0) & make_hs({0.0, 1.0}, -2.0);
		auto x  = p1 | p2;
		auto n1 = normalize_hsb(x);
		auto n2 = normalize_hsb(n1);
		CHECK(n1 == n2);
	}

	TEST_CASE("bottom is idempotent") {
		CHECK(normalize_hsb(normalize_hsb(hsb::bottom())) == hsb::bottom());
	}

	TEST_CASE("top is idempotent") {
		CHECK(normalize_hsb(normalize_hsb(hsb::top())) == hsb::top());
	}

} // TEST_SUITE idempotency

// =============================================================================
// TEST SUITE: normalize_hsb — (N3) Canonical form
// =============================================================================

TEST_SUITE("hsb_normalizer — (N3) canonical form") {

	TEST_CASE("two scalings of the same halfspace produce == output") {
		// x[0] < 5  written as H_{[1],-5} and H_{[2],-10}
		auto h1 = make_hs({1.0}, -5.0);
		auto h2 = make_hs({2.0}, -10.0);
		CHECK(normalize_hsb(h1) == normalize_hsb(h2));
	}

	TEST_CASE("conjunction order does not change canonical form") {
		auto a = make_hs({-1.0, 0.0}, 0.0);  // x[0] >= 0
		auto b = make_hs({1.0, 0.0}, -3.0);  // x[0] < 3
		// (a & b) and (b & a) are syntactically different but equal formulas
		CHECK(normalize_hsb(a & b) == normalize_hsb(b & a));
	}

	TEST_CASE("disjunction order does not change canonical form") {
		auto a = make_hs({1.0, 0.0}, -2.0);
		auto b = make_hs({0.0, 1.0}, -3.0);
		CHECK(normalize_hsb(a | b) == normalize_hsb(b | a));
	}

	TEST_CASE("factored vs expanded form produce == output") {
		// a & (b | c)  ==  (a & b) | (a & c)  syntactically different
		auto a = make_hs({-1.0, 0.0}, 0.0);  // x[0] >= 0
		auto b = make_hs({1.0, 0.0}, -2.0);  // x[0] < 2
		auto c = make_hs({0.0, 1.0}, -3.0);  // x[1] < 3

		auto factored  = a & (b | c);
		auto expanded  = (a & b) | (a & c);
		CHECK(normalize_hsb(factored) == normalize_hsb(expanded));
	}

	TEST_CASE("L-shape: two syntactic orderings give same canonical output") {
		// [0,2)x[0,1) union [0,1)x[1,2)
		auto xs0_ge_0 = make_hs({-1.0, 0.0},  0.0);
		auto xs0_lt_1 = make_hs({ 1.0, 0.0}, -1.0);
		auto xs0_lt_2 = make_hs({ 1.0, 0.0}, -2.0);
		auto xs1_ge_0 = make_hs({ 0.0,-1.0},  0.0);
		auto xs1_lt_1 = make_hs({ 0.0, 1.0}, -1.0);
		auto xs1_ge_1 = make_hs({ 0.0,-1.0},  1.0);
		auto xs1_lt_2 = make_hs({ 0.0, 1.0}, -2.0);

		auto lshape_v1 = (xs0_ge_0 & xs0_lt_2 & xs1_ge_0 & xs1_lt_1)
		               | (xs0_ge_0 & xs0_lt_1 & xs1_ge_1 & xs1_lt_2);

		// Same region, reversed disjunct order and shuffled conjuncts
		auto lshape_v2 = (xs1_ge_1 & xs1_lt_2 & xs0_ge_0 & xs0_lt_1)
		               | (xs1_ge_0 & xs1_lt_1 & xs0_lt_2 & xs0_ge_0);

		CHECK(normalize_hsb(lshape_v1) == normalize_hsb(lshape_v2));
	}

	TEST_CASE("De Morgan rewrite gives same canonical form as original disjunction") {
		auto a = make_hs({ 1.0}, -4.0);
		auto b = make_hs({-1.0}, -4.0);
		auto disjunction  = a | b;
		auto demorgan     = ~(~a & ~b);   // logically equal
		CHECK(normalize_hsb(disjunction) == normalize_hsb(demorgan));
	}

	TEST_CASE("triple-negation collapses to same form as original") {
		auto h = make_hs({1.0}, -5.0);
		// ~~~h == ~h  (by ~~A = A applied twice: ~~~h = ~(~~h) = ~h)
		CHECK(normalize_hsb(~~~h) == normalize_hsb(~h));
	}

} // TEST_SUITE canonical-form

// =============================================================================
// TEST SUITE: normalize_hsb — (N4) Disjointness of output clauses
// =============================================================================

TEST_SUITE("hsb_normalizer — (N4) disjointness of output clauses") {

	TEST_CASE("1D two-clause output: clauses are disjoint") {
		// disjunction of two non-overlapping intervals
		auto x = make_hs({1.0}, 5.0) | make_hs({-1.0}, -10.0);
		// x[0] < -5  OR  x[0] >= 10
		auto n = normalize_hsb(x);
		std::vector<hsb> clauses;
		collect_clauses(n, clauses);
		for (size_t i = 0; i < clauses.size(); ++i)
			for (size_t j = i + 1; j < clauses.size(); ++j)
				CHECK(is_hsb_zero(clauses[i] & clauses[j]));
	}

	TEST_CASE("L-shape: two output clauses are disjoint") {
		auto xs0_ge_0 = make_hs({-1.0, 0.0},  0.0);
		auto xs0_lt_1 = make_hs({ 1.0, 0.0}, -1.0);
		auto xs0_lt_2 = make_hs({ 1.0, 0.0}, -2.0);
		auto xs1_ge_0 = make_hs({ 0.0,-1.0},  0.0);
		auto xs1_lt_1 = make_hs({ 0.0, 1.0}, -1.0);
		auto xs1_ge_1 = make_hs({ 0.0,-1.0},  1.0);
		auto xs1_lt_2 = make_hs({ 0.0, 1.0}, -2.0);

		auto lshape = (xs0_ge_0 & xs0_lt_2 & xs1_ge_0 & xs1_lt_1)
		            | (xs0_ge_0 & xs0_lt_1 & xs1_ge_1 & xs1_lt_2);

		auto n = normalize_hsb(lshape);
		std::vector<hsb> clauses;
		collect_clauses(n, clauses);
		REQUIRE(clauses.size() == 2);

		CHECK(is_hsb_zero(clauses[0] & clauses[1]));
	}

	TEST_CASE("2D three-clause disjunction: all pairs disjoint") {
		// three non-overlapping strips along x[1]
		auto s1 = make_hs({ 0.0,-1.0},  0.0) & make_hs({0.0, 1.0}, -1.0); // [0,1)
		auto s2 = make_hs({ 0.0,-1.0},  1.0) & make_hs({0.0, 1.0}, -2.0); // [1,2)
		auto s3 = make_hs({ 0.0,-1.0},  2.0) & make_hs({0.0, 1.0}, -3.0); // [2,3)
		auto x  = s1 | s2 | s3;

		auto n = normalize_hsb(x);
		std::vector<hsb> clauses;
		collect_clauses(n, clauses);
		for (size_t i = 0; i < clauses.size(); ++i)
			for (size_t j = i + 1; j < clauses.size(); ++j)
				CHECK(is_hsb_zero(clauses[i] & clauses[j]));
	}

} // TEST_SUITE disjointness

// =============================================================================
// TEST SUITE: normalize_hsb — output structure
// =============================================================================

TEST_SUITE("hsb_normalizer — output structure") {

	TEST_CASE("non-trivial output has no negation nodes") {
		auto h = make_hs({1.0}, -5.0);
		auto n = normalize_hsb(h);
		if (n.root->k != hsb::kind::bot && n.root->k != hsb::kind::top)
			CHECK(!has_negation(n.root));
	}

	TEST_CASE("2D conjunction output has no negation nodes") {
		auto x = make_hs({1.0, 0.0}, -4.0) & make_hs({0.0, 1.0}, -4.0);
		auto n = normalize_hsb(x);
		CHECK(!has_negation(n.root));
	}

	TEST_CASE("complement of halfspace output has no negation nodes") {
		auto h = make_hs({1.0, 0.0}, -3.0);
		auto n = normalize_hsb(~h);
		if (n.root->k != hsb::kind::bot && n.root->k != hsb::kind::top)
			CHECK(!has_negation(n.root));
	}

	TEST_CASE("L-shape output has exactly two clauses") {
		auto xs0_ge_0 = make_hs({-1.0, 0.0},  0.0);
		auto xs0_lt_1 = make_hs({ 1.0, 0.0}, -1.0);
		auto xs0_lt_2 = make_hs({ 1.0, 0.0}, -2.0);
		auto xs1_ge_0 = make_hs({ 0.0,-1.0},  0.0);
		auto xs1_lt_1 = make_hs({ 0.0, 1.0}, -1.0);
		auto xs1_ge_1 = make_hs({ 0.0,-1.0},  1.0);
		auto xs1_lt_2 = make_hs({ 0.0, 1.0}, -2.0);

		auto lshape = (xs0_ge_0 & xs0_lt_2 & xs1_ge_0 & xs1_lt_1)
		            | (xs0_ge_0 & xs0_lt_1 & xs1_ge_1 & xs1_lt_2);

		auto n = normalize_hsb(lshape);
		std::vector<hsb> clauses;
		collect_clauses(n, clauses);
		CHECK(clauses.size() == 2);
	}

	TEST_CASE("single halfspace normalizes to a single clause") {
		auto h = make_hs({1.0}, -5.0);
		auto n = normalize_hsb(h);
		std::vector<hsb> clauses;
		collect_clauses(n, clauses);
		CHECK(clauses.size() == 1);
	}

} // TEST_SUITE output-structure

// =============================================================================
// TEST SUITE: normalize_hsb — multi-dimensional
// =============================================================================

TEST_SUITE("hsb_normalizer — multi-dimensional inputs") {

	TEST_CASE("2D scaled halfspace normalizes canonically") {
		// 2*x[0] + 4*x[1] - 8 < 0  ==  x[0] + 2*x[1] - 4 < 0
		// Both represent the same open halfspace.
		auto h1 = make_hs({2.0, 4.0}, -8.0);
		auto h2 = make_hs({1.0, 2.0}, -4.0);
		CHECK(normalize_hsb(h1) == normalize_hsb(h2));
	}

	TEST_CASE("2D conjunction with scaled halfspaces normalizes canonically") {
		auto a1 = make_hs({2.0, 0.0}, -6.0);  // 2*x[0] < 6 ≡ x[0] < 3
		auto a2 = make_hs({1.0, 0.0}, -3.0);  // x[0] < 3
		auto b  = make_hs({0.0, 1.0}, -4.0);
		CHECK(normalize_hsb(a1 & b) == normalize_hsb(a2 & b));
	}

	TEST_CASE("3D box: normalize gives semantically equivalent region") {
		auto x = make_hs({ 1.0, 0.0, 0.0}, -5.0)
		       & make_hs({-1.0, 0.0, 0.0},  0.0)
		       & make_hs({ 0.0, 1.0, 0.0}, -5.0)
		       & make_hs({ 0.0,-1.0, 0.0},  0.0)
		       & make_hs({ 0.0, 0.0, 1.0}, -5.0)
		       & make_hs({ 0.0, 0.0,-1.0},  0.0);
		auto n = normalize_hsb(x);
		CHECK(!is_hsb_zero(n));
		CHECK(is_hsb_zero(n ^ x));
	}

	TEST_CASE("3D: idempotent") {
		auto x  = make_hs({1.0, 0.0, 0.0}, -5.0) & make_hs({0.0, 1.0, 0.0}, -3.0);
		auto n1 = normalize_hsb(x);
		auto n2 = normalize_hsb(n1);
		CHECK(n1 == n2);
	}

	TEST_CASE("2D: non-axis-aligned halfspace normalizes correctly") {
		// x[0] + x[1] < 5  (diagonal halfspace)
		auto h = make_hs({1.0, 1.0}, -5.0);
		auto n = normalize_hsb(h);
		CHECK(!is_hsb_zero(n));
		CHECK(is_hsb_zero(n ^ h));
	}

	TEST_CASE("2D: scaled diagonal halfspace normalizes to same as unit") {
		// 3*(x[0]+x[1]) < 15  ≡  x[0]+x[1] < 5
		auto h_scaled = make_hs({3.0, 3.0}, -15.0);
		auto h_unit   = make_hs({1.0, 1.0}, -5.0);
		CHECK(normalize_hsb(h_scaled) == normalize_hsb(h_unit));
	}

} // TEST_SUITE multi-dimensional
