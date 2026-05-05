// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for omcat::enumerate_qlt_T1 and friends — foundation for
// Algorithm D (direct parity-game construction).

#include "test_init.h"
#include "omcat_types.h"
#include "omcat_constants.h"

using namespace idni::tau_lang;
using omcat::QltType1;
using omcat::Rat;
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
		auto ts = enumerate_qlt_T1({Rat(0, 1)});
		CHECK(ts.size() == 3u);
		// positions: 0=(-∞,0), 1={0}, 2=(0,+∞)
		CHECK(ts[0].is_interval());
		CHECK(ts[1].is_point());
		CHECK(ts[2].is_interval());
	}

	TEST_CASE("two constants (0, 1): |T_1| = 5") {
		auto ts = enumerate_qlt_T1({Rat(0, 1), Rat(1, 1)});
		CHECK(ts.size() == 5u);
		CHECK(ts[0].is_interval()); // (-∞, 0)
		CHECK(ts[1].is_point());    // {0}
		CHECK(ts[2].is_interval()); // (0, 1)
		CHECK(ts[3].is_point());    // {1}
		CHECK(ts[4].is_interval()); // (1, +∞)
	}

	TEST_CASE("dedup and sort constants") {
		auto ts = enumerate_qlt_T1(
			{Rat(1, 1), Rat(0, 1), Rat(1, 1)});
		CHECK(ts.size() == 5u); // still 2 distinct constants ⇒ 5 types
	}

	TEST_CASE("realize: point type returns the constant") {
		auto ts = enumerate_qlt_T1({Rat(3, 7)});
		auto w = ts[1].realize();
		CHECK(cmp(w, Rat(3, 7)) == 0);
	}

	TEST_CASE("realize: interval type returns value in the interval") {
		auto ts = enumerate_qlt_T1({Rat(0, 1), Rat(1, 1)});
		auto w = ts[2].realize(); // (0, 1)
		CHECK(cmp(w, Rat(0, 1)) > 0);
		CHECK(cmp(w, Rat(1, 1)) < 0);
	}

	TEST_CASE("realize: (-∞, c) returns value below c") {
		auto ts = enumerate_qlt_T1({Rat(5, 1)});
		auto w = ts[0].realize();
		CHECK(cmp(w, Rat(5, 1)) < 0);
	}

	TEST_CASE("realize: (c, +∞) returns value above c") {
		auto ts = enumerate_qlt_T1({Rat(5, 1)});
		auto w = ts[2].realize();
		CHECK(cmp(w, Rat(5, 1)) > 0);
	}

	TEST_CASE("qlt_type_of inverts realize") {
		std::vector<Rat> cs = {
			Rat(-1, 2), Rat(0, 1),
			Rat(3, 4)
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
		auto t2 = omcat::enumerate_qlt_T2({omcat::Rat(0, 1)});
		CHECK(t2.size() == 13u);
	}

	TEST_CASE("T_2 restriction") {
		auto t2 = omcat::enumerate_qlt_T2({omcat::Rat(0, 1)});
		for (const auto& s : t2) {
			CHECK(s.restrict_m().index() == s.pos_m);
			CHECK(s.restrict_x().index() == s.pos_x);
		}
	}

	TEST_CASE("Pre: unconstrained, W=T_1 ⇒ Pre = T_1") {
		auto T1 = omcat::enumerate_qlt_T1({omcat::Rat(0, 1)});
		auto T2 = omcat::enumerate_qlt_T2({omcat::Rat(0, 1)});
		std::vector<int> W;
		for (const auto& t : T1) W.push_back(t.pos);
		auto P = omcat::Pre_over_T1_any(T1, T2, W);
		CHECK(P.size() == T1.size());
	}

	TEST_CASE("Pre: W=∅ ⇒ Pre=∅") {
		auto T1 = omcat::enumerate_qlt_T1({omcat::Rat(0, 1)});
		auto T2 = omcat::enumerate_qlt_T2({omcat::Rat(0, 1)});
		auto P = omcat::Pre_over_T1_any(T1, T2, {});
		CHECK(P.empty());
	}

	TEST_CASE("νX. Pre(X) on full T_1 = T_1 (G true)") {
		auto T1 = omcat::enumerate_qlt_T1({omcat::Rat(0, 1)});
		auto T2 = omcat::enumerate_qlt_T2({omcat::Rat(0, 1)});
		auto W = omcat::nu_fixpoint(T1, [&](const std::vector<int>& X){
			return omcat::Pre_over_T1_any(T1, T2, X);
		});
		CHECK(W.size() == T1.size());
	}

	TEST_CASE("μX. Pre(X) on empty = ∅ (unreachable)") {
		auto T1 = omcat::enumerate_qlt_T1({omcat::Rat(0, 1)});
		auto T2 = omcat::enumerate_qlt_T2({omcat::Rat(0, 1)});
		auto W = omcat::mu_fixpoint(T1, [&](const std::vector<int>& X){
			return omcat::Pre_over_T1_any(T1, T2, X);
		});
		CHECK(W.empty());
	}

	TEST_CASE("reachable_from: unrestricted reaches all T_1") {
		auto T1 = omcat::enumerate_qlt_T1({omcat::Rat(0, 1)});
		auto T2 = omcat::enumerate_qlt_T2({omcat::Rat(0, 1)});
		auto next_all = [&](const omcat::QltType2&) {
			std::vector<int> all;
			for (const auto& t : T1) all.push_back(t.pos);
			return all;
		};
		auto R = omcat::reachable_from(0, T1, T2, next_all);
		CHECK(R.size() == T1.size());
	}

	TEST_CASE("Pre_over_T1 with admissible_next restricted to {0}") {
		// System can only produce next-memory-type 0 regardless of input.
		// So Pre(W) = T_1 if 0 ∈ W, else ∅.
		auto T1 = omcat::enumerate_qlt_T1({omcat::Rat(0, 1)});
		auto T2 = omcat::enumerate_qlt_T2({omcat::Rat(0, 1)});
		auto only_zero = [&](const omcat::QltType2&) {
			return std::vector<int>{0};
		};
		{
			auto P = omcat::Pre_over_T1<decltype(only_zero)>(
				T1, T2, {0}, only_zero);
			CHECK(P.size() == T1.size());
		}
		{
			auto P = omcat::Pre_over_T1<decltype(only_zero)>(
				T1, T2, {1, 2}, only_zero);
			CHECK(P.empty());
		}
	}

	TEST_CASE("ν fixpoint of Pre_only_zero = T_1 iff 0 reachable") {
		// G(memory-type = 0):  νX. Pre_{only_0}(X).
		// Since from any b we can reach 0, and 0 ∈ {0}, starting X = T_1
		// gives a stable X = T_1.  So the winning set is T_1.
		auto T1 = omcat::enumerate_qlt_T1({omcat::Rat(0, 1)});
		auto T2 = omcat::enumerate_qlt_T2({omcat::Rat(0, 1)});
		auto only_zero = [&](const omcat::QltType2&) {
			return std::vector<int>{0};
		};
		auto W = omcat::nu_fixpoint(T1, [&](const std::vector<int>& X){
			return omcat::Pre_over_T1<decltype(only_zero)>(T1, T2, X, only_zero);
		});
		CHECK(W.size() == T1.size());
	}

	TEST_CASE("μ fixpoint of Pre_only_zero converges to T_1 eventually-forced") {
		// F(memory-type = 0):  μX. B_0 ∪ Pre(X), where B_0 = {b : next can be 0}.
		// Under our only_zero transition, from ANY b we can force next = 0,
		// so B_0 = T_1, so μ fixpoint converges to T_1 immediately.
		auto T1 = omcat::enumerate_qlt_T1({omcat::Rat(0, 1)});
		auto T2 = omcat::enumerate_qlt_T2({omcat::Rat(0, 1)});
		auto only_zero = [&](const omcat::QltType2&) {
			return std::vector<int>{0};
		};
		// The "reach {0}" problem: μX. Pre_only_zero({0}) ∪ Pre(X).
		auto W = omcat::mu_fixpoint(T1, [&](const std::vector<int>& X){
			// B_0 ∪ Pre(X) — here we just compute Pre({0}) ∪ Pre(X).
			std::vector<int> one_step = omcat::Pre_over_T1<decltype(only_zero)>(
				T1, T2, {0}, only_zero);
			std::vector<int> via_X = omcat::Pre_over_T1_any(T1, T2, X);
			// Union.
			std::vector<int> u = one_step;
			for (int v : via_X) {
				if (std::find(u.begin(), u.end(), v) == u.end())
					u.push_back(v);
			}
			return u;
		});
		// Since from any b, next = 0 is reachable in ONE step, the fixpoint
		// already stabilises at B_0 = T_1.
		CHECK(W.size() == T1.size());
	}

	TEST_CASE("reachable_from: restricted to even positions stays even") {
		auto T1 = omcat::enumerate_qlt_T1({omcat::Rat(0, 1), omcat::Rat(1, 1)});
		auto T2 = omcat::enumerate_qlt_T2({omcat::Rat(0, 1), omcat::Rat(1, 1)});
		auto next_even = [&](const omcat::QltType2&) {
			std::vector<int> evens;
			for (const auto& t : T1) if ((t.pos & 1) == 0) evens.push_back(t.pos);
			return evens;
		};
		auto R = omcat::reachable_from(0, T1, T2, next_even);
		for (int r : R) CHECK((r & 1) == 0);
	}

	TEST_CASE("ordering predicates") {
		auto ts = enumerate_qlt_T1({Rat(0, 1), Rat(1, 1)});
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

#include "omcat_oracle_cache.h"

TEST_SUITE("omcat: oracle cache") {
	TEST_CASE("get miss, put, get hit") {
		omcat::OracleCache c;
		auto k = omcat::make_oracle_key({-1, 0, 1});
		CHECK(c.get(k) == -1);
		c.put(k, 42);
		CHECK(c.get(k) == 42);
		CHECK(c.size() == 1u);
	}
	TEST_CASE("get_or_compute only computes on miss") {
		omcat::OracleCache c;
		auto k = omcat::make_oracle_key({1, 2, 3});
		int calls = 0;
		auto compute = [&]() { ++calls; return 99; };
		CHECK(c.get_or_compute(k, compute) == 99);
		CHECK(c.get_or_compute(k, compute) == 99);
		CHECK(c.get_or_compute(k, compute) == 99);
		CHECK(calls == 1);
	}
	TEST_CASE("distinct atomic-query vectors give distinct keys") {
		auto k1 = omcat::make_oracle_key({-1});
		auto k2 = omcat::make_oracle_key({+1});
		auto k3 = omcat::make_oracle_key({-1, 0});
		CHECK(!(k1 == k2));
		CHECK(!(k1 == k3));
	}
	TEST_CASE("clear empties the cache") {
		omcat::OracleCache c;
		c.put(omcat::make_oracle_key({0}), 1);
		CHECK(c.size() == 1u);
		c.clear();
		CHECK(c.size() == 0u);
	}
}

TEST_SUITE("omcat: parse_rat_literal") {
	TEST_CASE("integer") {
		auto r = omcat::parse_rat_literal("42");
		CHECK(omcat::cmp(r, omcat::Rat(42, 1)) == 0);
	}
	TEST_CASE("fraction") {
		auto r = omcat::parse_rat_literal("3/7");
		CHECK(omcat::cmp(r, omcat::Rat(3, 7)) == 0);
	}
	TEST_CASE("negative fraction") {
		auto r = omcat::parse_rat_literal("-1/2");
		CHECK(omcat::cmp(r, omcat::Rat(-1, 2)) == 0);
	}
	TEST_CASE("decimal") {
		auto r = omcat::parse_rat_literal("0.25");
		CHECK(omcat::cmp(r, omcat::Rat(1, 4)) == 0);
	}
	TEST_CASE("decimal 0.45") {
		auto r = omcat::parse_rat_literal("0.45");
		CHECK(omcat::cmp(r, omcat::Rat(9, 20)) == 0);
	}
	TEST_CASE("invalid returns {0, 0}") {
		auto r = omcat::parse_rat_literal("not-a-rational");
		CHECK(r.q == 0);
	}
}
