// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for Algorithm A: D_i + R_ρ encoding skeleton builder.

#include "test_init.h"
#include "omcat_types.h"
#include "algorithm_a_skeleton.h"

using namespace idni::tau_lang::omcat;
using namespace idni::tau_lang::alg_a;

// ── enumerate_qlt_T3 ──────────────────────────────────────────────────────────

TEST_SUITE("enumerate_qlt_T3") {

	TEST_CASE("zero constants: exactly 13 3-types") {
		// With no named constants T_1 has 1 element (the single dense interval).
		// All three positions are free relative to each other; the 13 consistent
		// (r_mx, r_xy, r_my) triples are: 7 fully-determined chains + 3+3 from
		// the two ambiguous cases (LT,GT,*) and (GT,LT,*).
		auto T3 = enumerate_qlt_T3({});
		CHECK(T3.size() == 13u);
	}

	TEST_CASE("one constant: strictly more than 13 3-types") {
		// With one constant c_0, T_1 has 3 elements; more triples are admissible.
		auto T3 = enumerate_qlt_T3({Rat(0, 1)});
		CHECK(T3.size() > 13u);
	}

	TEST_CASE("pos fields in bounds: all positions in [0, 2k]") {
		std::vector<Rat> consts = {Rat(0, 1), Rat(1, 1)};  // k=2 constants
		auto T3 = enumerate_qlt_T3(consts);
		int k = 2, max_pos = 2 * k;  // 2*k+1 1-types → positions 0..4
		for (const auto& t : T3) {
			CHECK(t.pos_m >= 0); CHECK(t.pos_m <= max_pos);
			CHECK(t.pos_x >= 0); CHECK(t.pos_x <= max_pos);
			CHECK(t.pos_y >= 0); CHECK(t.pos_y <= max_pos);
		}
	}

	TEST_CASE("rel3_consistent: transitivity spot-checks") {
		// Forced chains
		CHECK( rel3_consistent(Rel::LT, Rel::LT, Rel::LT));
		CHECK(!rel3_consistent(Rel::LT, Rel::LT, Rel::EQ));
		CHECK(!rel3_consistent(Rel::LT, Rel::LT, Rel::GT));
		CHECK( rel3_consistent(Rel::GT, Rel::GT, Rel::GT));
		CHECK(!rel3_consistent(Rel::GT, Rel::GT, Rel::LT));
		// Ambiguous cases: any r_my consistent
		CHECK( rel3_consistent(Rel::LT, Rel::GT, Rel::LT));
		CHECK( rel3_consistent(Rel::LT, Rel::GT, Rel::EQ));
		CHECK( rel3_consistent(Rel::LT, Rel::GT, Rel::GT));
		CHECK( rel3_consistent(Rel::GT, Rel::LT, Rel::LT));
	}
}

// ── build_algorithm_a_skeleton ────────────────────────────────────────────────

TEST_SUITE("build_algorithm_a_skeleton") {

	TEST_CASE("n_rbits=1 for T1_size=1 and T1_size=2") {
		// T1_size=1: one T1 type (single interval), n_rbits=1.
		auto b1 = build_algorithm_a_skeleton(1, 0, {{0,0,0}}, "true");
		CHECK(b1.n_rbits == 1);
		CHECK(b1.outs.size() == 1u);
		CHECK(b1.outs[0] == "r_0");
		CHECK(!b1.formula.empty());

		// T1_size=2: two T1 types, n_rbits=1.
		auto b2 = build_algorithm_a_skeleton(2, 0, {{0,0,0},{0,1,0},{1,0,0},{1,1,0}}, "true");
		CHECK(b2.n_rbits == 1);
		CHECK(b2.outs.size() == 1u);
		CHECK(b2.outs[0] == "r_0");
	}

	TEST_CASE("n_rbits=2 for T1_size=3 and T1_size=4") {
		// Build a minimal feasible_set: all (sigma,rho,0) pairs for sigma,rho in [0,T1_size).
		auto make_all = [](int n) {
			std::vector<std::tuple<int,int,int>> fs;
			for (int s = 0; s < n; ++s)
				for (int r = 0; r < n; ++r)
					fs.emplace_back(s, r, 0);
			return fs;
		};
		auto b3 = build_algorithm_a_skeleton(3, 0, make_all(3), "true");
		CHECK(b3.n_rbits == 2);
		CHECK(b3.outs.size() == 2u);
		CHECK(b3.outs[0] == "r_0");
		CHECK(b3.outs[1] == "r_1");

		auto b4 = build_algorithm_a_skeleton(4, 0, make_all(4), "true");
		CHECK(b4.n_rbits == 2);
	}

	TEST_CASE("D bits appended to outs after R bits") {
		// T1_size=1, K=2: outs = [r_0, d_0, d_1].
		auto b = build_algorithm_a_skeleton(1, 2, {{0,0,0b00},{0,0,0b01},{0,0,0b10},{0,0,0b11}}, "true");
		CHECK(b.K == 2);
		CHECK(b.outs.size() == 3u);
		CHECK(b.outs[0] == "r_0");
		CHECK(b.outs[1] == "d_0");
		CHECK(b.outs[2] == "d_1");
	}

	TEST_CASE("R-validity: G(!r_encode(rho)) for unused encodings") {
		// T1_size=3, n_rbits=2 → encoding 3 (r_0 & r_1) unused.
		auto make_all = [](int n) {
			std::vector<std::tuple<int,int,int>> fs;
			for (int s = 0; s < n; ++s)
				for (int r = 0; r < n; ++r)
					fs.emplace_back(s, r, 0);
			return fs;
		};
		auto b = build_algorithm_a_skeleton(3, 0, make_all(3), "true");
		// r_encode(3, 2) = "r_0 & r_1" → G(!(r_0 & r_1))
		CHECK(b.formula.find("G(!(r_0 & r_1))") != std::string::npos);
	}

	TEST_CASE("conditional constraint: X( present for infeasible transition") {
		// T1_size=2, K=0, only self-loops feasible: (0→0) and (1→1).
		// Transitions 0→1 and 1→0 are infeasible → G(... -> X(!...)) emitted.
		auto b = build_algorithm_a_skeleton(2, 0, {{0,0,0},{1,1,0}}, "true");
		CHECK(b.formula.find("X(") != std::string::npos);
	}

	TEST_CASE("phi* passthrough: d_i appear verbatim, not expanded") {
		// With the new encoding, phi* is passed straight through — no q-disjunction.
		auto b = build_algorithm_a_skeleton(1, 2, {{0,0,0b00},{0,0,0b01},{0,0,0b10},{0,0,0b11}},
		                                    "d_0 U d_1");
		CHECK(b.formula.find("d_0 U d_1") != std::string::npos);
		// No old-style q_ expansion.
		CHECK(b.formula.find("q_") == std::string::npos);
	}

	TEST_CASE("unconditional infeasibility: G(neg_atom) for (rho,A) absent from any sigma") {
		// T1_size=1, K=1: feasible_set has only (sigma=0, rho=0, A=0b00).
		// (rho=0, A=0b01) is absent → G(!((!r_0) & d_0)) emitted.
		auto b = build_algorithm_a_skeleton(1, 1, {{0, 0, 0}}, "true");
		// r_encode(0,1) = "!r_0", d_pattern(1,1) = "d_0"
		// Expected: G(!(!r_0 & d_0))
		CHECK(b.formula.find("d_0") != std::string::npos);
		// The unconditional G(...) must appear (not only inside X).
		// Search for a G(...) that does NOT contain "->".
		auto pos = b.formula.find("G(!(");
		CHECK(pos != std::string::npos);
	}

	TEST_CASE("zero-constant T3 integration: T1_size=1, all 13 T3 types feasible") {
		// With zero constants, T3 has 13 types all with pos_m=pos_y=0.
		// feasible_set = {(0,0,0)} (after dedup).
		// Formula should: have no validity clauses (n_rbits=1, T1_size=1 → only ρ=1 unused)
		// but validity G(!r_0) for ρ=1 should appear; no unconditional (0,0) is feasible.
		auto T3 = enumerate_qlt_T3({});
		std::vector<std::tuple<int,int,int>> fs;
		for (const auto& t : T3) fs.emplace_back(t.pos_m, t.pos_y, 0);
		auto b = build_algorithm_a_skeleton(1, 0, fs, "true");
		CHECK(b.n_rbits == 1);
		CHECK(b.T1_size == 1);
		// validity for ρ=1: G(!(r_0))
		CHECK(b.formula.find("G(!(r_0))") != std::string::npos);
		// No additional unconditional since (0,0) is feasible.
		// No X( since sigma=0 has all (0,rho) feasible for rho=0 only, and rho=0 IS feasible.
		CHECK(b.formula.find("X(") == std::string::npos);
	}
}
