// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for Algorithm B: P_σ binary encoding skeleton builder.
//
// Algorithm B extends Algorithm A (D_i + R_ρ) with ⌈log₂|T₂|⌉ input propositions
// encoding the T₂ = (pos_m, pos_x, rel_mx) type.  This is required for SOUNDNESS
// when the formula contains input-variable atoms: without P_σ the system cannot
// observe the input's T₁ type and the oracle may fail.

#include "test_init.h"
#include "omcat_types.h"
#include "algorithm_b_skeleton.h"
#include "test_tau_helpers.h"
#include "ltl_aba.h"

using namespace idni::tau_lang::omcat;
using namespace idni::tau_lang::alg_b;

// ── build_algorithm_b_skeleton unit tests ────────────────────────────────────

TEST_SUITE("[Algorithm B: skeleton builder]") {

	TEST_CASE("[ALG-B-01] n_pbits: T2_size=1 → 0, 2 → 1, 4 → 2, 5 → 3") {
		// T2_size=1: only one type, no bits needed.
		auto b1 = build_algorithm_b_skeleton(1, 1, 0, {{0,0,0}}, {0}, "true");
		CHECK(b1.n_pbits == 0);
		CHECK(b1.ins.empty());

		// T2_size=2: one bit.
		auto b2 = build_algorithm_b_skeleton(1, 2, 0, {{0,0,0},{1,0,0}}, {0,0}, "true");
		CHECK(b2.n_pbits == 1);
		CHECK(b2.ins.size() == 1u);
		CHECK(b2.ins[0] == "p_0");

		// T2_size=4: two bits.
		auto b4 = build_algorithm_b_skeleton(1, 4, 0,
			{{0,0,0},{1,0,0},{2,0,0},{3,0,0}}, {0,0,0,0}, "true");
		CHECK(b4.n_pbits == 2);

		// T2_size=5: three bits.
		auto b5 = build_algorithm_b_skeleton(1, 5, 0,
			{{0,0,0},{1,0,0},{2,0,0},{3,0,0},{4,0,0}},
			{0,0,0,0,0}, "true");
		CHECK(b5.n_pbits == 3);
	}

	TEST_CASE("[ALG-B-02] outs = r-bits then d-bits, ins = p-bits") {
		// T1_size=3 (n_rbits=2), T2_size=3 (n_pbits=2), K=2.
		std::vector<std::tuple<int,int,int>> fs = {{0,0,3},{1,1,3},{2,2,3}};
		std::vector<int> pm = {0, 1, 2};
		auto b = build_algorithm_b_skeleton(3, 3, 2, fs, pm, "d_0 & d_1");
		// outs: r_0, r_1 (n_rbits=2), d_0, d_1 (K=2)
		REQUIRE(b.outs.size() == 4u);
		CHECK(b.outs[0] == "r_0");
		CHECK(b.outs[1] == "r_1");
		CHECK(b.outs[2] == "d_0");
		CHECK(b.outs[3] == "d_1");
		// ins: p_0, p_1 (n_pbits=2)
		REQUIRE(b.ins.size() == 2u);
		CHECK(b.ins[0] == "p_0");
		CHECK(b.ins[1] == "p_1");
	}

	TEST_CASE("[ALG-B-03] P-validity clause in assume: G(!p_encode(overflow))") {
		// T2_size=3, n_pbits=2: overflow = σ=3 → G(!(p_0 & p_1))
		std::vector<std::tuple<int,int,int>> fs = {{0,0,0},{1,0,0},{2,0,0}};
		std::vector<int> pm = {0,0,0};
		auto b = build_algorithm_b_skeleton(1, 3, 0, fs, pm, "true");
		// Overflow σ=3: p_encode(3,2) = "p_0 & p_1" → G(!(p_0 & p_1))
		CHECK(b.formula.find("G(!(p_0 & p_1))") != std::string::npos);
	}

	TEST_CASE("[ALG-B-04] Ψ_I clause: G(r_encode(ρ) → X(∨ p_encode(σ's)))") {
		// T1_size=1 (one memory type, ρ=0), T2_size=2 (two input types),
		// both T2 types have pos_m=0 (memory matches ρ=0).
		std::vector<std::tuple<int,int,int>> fs = {{0,0,0},{1,0,0}};
		std::vector<int> pm = {0, 0};  // both T2 types have pos_m=0
		auto b = build_algorithm_b_skeleton(1, 2, 0, fs, pm, "true");
		// Ψ_I for ρ=0: G(r_encode(0,1) → X(p_encode(0,1) | p_encode(1,1)))
		// r_encode(0,1) = "!r_0"; p_encode(0,1) = "!p_0"; p_encode(1,1) = "p_0"
		// So: G((!r_0) -> X((!p_0) | (p_0)))  = G(!r_0 -> X(true)) [simplifiable but we check raw]
		CHECK(b.formula.find("-> X(") != std::string::npos);
	}

	TEST_CASE("[ALG-B-05] Φ_δ infeasibility clause present") {
		// T1_size=1, T2_size=1, K=1: only (σ=0, ρ=0, A=1) is feasible.
		// (σ=0, ρ=0, A=0) must be infeasible → G(!) clause.
		std::vector<std::tuple<int,int,int>> fs = {{0, 0, 1}};  // only D₀=1 feasible
		std::vector<int> pm = {0};
		auto b = build_algorithm_b_skeleton(1, 1, 1, fs, pm, "d_0");
		// Infeasible: (σ=0, ρ=0, A=0) → G(!(p_enc(0) & r_enc(0) & !d_0))
		CHECK(b.formula.find("G(!") != std::string::npos);
		CHECK(b.formula.find("d_0") != std::string::npos);
	}

	TEST_CASE("[ALG-B-06] phi* appears verbatim in guarantee") {
		std::vector<std::tuple<int,int,int>> fs = {{0,0,3}};
		std::vector<int> pm = {0};
		auto b = build_algorithm_b_skeleton(1, 1, 2, fs, pm, "d_0 U d_1");
		CHECK(b.formula.find("d_0 U d_1") != std::string::npos);
		CHECK(b.formula.find("q_") == std::string::npos);
	}

	TEST_CASE("[ALG-B-07] T2 enumeration: zero constants → 3 types (LT/EQ/GT in one interval)") {
		auto T2 = enumerate_qlt_T2({});
		// One interval, all three relations consistent within same interval.
		CHECK(T2.size() == 3u);
		// All types have pos_m=0, pos_x=0.
		for (const auto& t : T2) {
			CHECK(t.pos_m == 0);
			CHECK(t.pos_x == 0);
		}
	}

	TEST_CASE("[ALG-B-08] T2 consistent with T3: every T3 type maps to a T2 type") {
		// Verify that for zero constants, every T3 type has a corresponding T2 type.
		auto T2 = enumerate_qlt_T2({});
		auto T3 = enumerate_qlt_T3({});
		// Build T2 lookup.
		std::map<std::tuple<int,int,int>, int> t2_lookup;
		for (int s = 0; s < (int)T2.size(); ++s)
			t2_lookup[{T2[s].pos_m, T2[s].pos_x, (int)T2[s].rel}] = s;
		// Every T3 type should match some T2 type via (pos_m, pos_x, rel_mx).
		for (const auto& t3 : T3) {
			auto key = std::make_tuple(t3.pos_m, t3.pos_x, (int)t3.rel_mx);
			CHECK(t2_lookup.count(key));
		}
	}
}

// ── Integration tests (Algorithm B end-to-end) ───────────────────────────────

static tref spec(const char* s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

static bool alg_b_realizable(const char* s) {
	setenv("TAU_LTL_ALG", "B", 1);
	bdd_init<Bool>();
	tref fm = spec(s);
	bool result = (fm != nullptr) && is_tau_formula_sat<node_t>(fm);
	unsetenv("TAU_LTL_ALG");
	return result;
}

TEST_SUITE("[Algorithm B: integration]") {

	TEST_CASE("[ALG-B-10] G(o1 > {0}) REALIZABLE: output-only regression") {
		// System always picks y > 0. No input variables.
		CHECK(alg_b_realizable("G (o1[t]:qlt > {0}:qlt)."));
	}

	TEST_CASE("[ALG-B-11] G(o1 < {0}) REALIZABLE: output always negative") {
		CHECK(alg_b_realizable("G (o1[t]:qlt < {0}:qlt)."));
	}

	TEST_CASE("[ALG-B-12] GF(o1 > {0}) REALIZABLE: liveness regression") {
		CHECK(alg_b_realizable("G F (o1[t]:qlt > {0}:qlt)."));
	}

	TEST_CASE("[ALG-B-13] G(o1 > i1) REALIZABLE: input-variable, system tracks input type") {
		// System always picks y > x.  P_σ lets system observe x's T₁ type.
		CHECK(alg_b_realizable("G (o1[t]:qlt > i1[t]:qlt)."));
	}

	TEST_CASE("[ALG-B-14] G(o1 > i1[t-1]) REALIZABLE: output > previous input") {
		// System always picks y > prev_x.
		CHECK(alg_b_realizable("G (o1[t]:qlt > i1[t-1]:qlt)."));
	}

	TEST_CASE("[ALG-B-15] G(o1>i1) && G(o1<i1) UNREALIZABLE: o1>i1 and o1<i1 simultaneously") {
		// Requires output strictly greater than AND strictly less than input simultaneously.
		// No (o1, i1) pair satisfies both: UNSATISFIABLE in any context.
		CHECK_FALSE(alg_b_realizable(
			"G (o1[t]:qlt > i1[t]:qlt) && G (o1[t]:qlt < i1[t]:qlt)."));
	}

	TEST_CASE("[ALG-B-16] G(o1 > {1/2} && o1 < {0}) UNREALIZABLE: contradictory constants") {
		// Same infeasibility check as Algorithm A: y > 1/2 and y < 0 can't both hold.
		CHECK_FALSE(alg_b_realizable(
			"G (o1[t]:qlt > {1/2}:qlt) && G (o1[t]:qlt < {0}:qlt)."));
	}

	TEST_CASE("[ALG-B-17] TAU_LTL_ALG=A falls back to B when inputs are present") {
		setenv("TAU_LTL_ALG", "A", 1);
		bdd_init<Bool>();
		tref fm = spec("G (o1[t]:qlt > i1[t]:qlt).");
		bool result = (fm != nullptr) && is_tau_formula_sat<node_t>(fm);
		unsetenv("TAU_LTL_ALG");
		CHECK(result);
	}

	TEST_CASE("[ALG-B-18] TAU_LTL_ALG=D falls back to B when inputs are present") {
		setenv("TAU_LTL_ALG", "D", 1);
		bdd_init<Bool>();
		tref fm = spec("G (o1[t]:qlt > i1[t]:qlt).");
		bool result = (fm != nullptr) && is_tau_formula_sat<node_t>(fm);
		unsetenv("TAU_LTL_ALG");
		CHECK(result);
	}

	TEST_CASE("[ALG-B-19] input lookback still routes through P-bit encoding") {
		setenv("TAU_LTL_ALG", "A", 1);
		bdd_init<Bool>();
		tref fm = spec("G (o1[t]:qlt > i1[t-1]:qlt).");
		bool result = (fm != nullptr) && is_tau_formula_sat<node_t>(fm);
		unsetenv("TAU_LTL_ALG");
		CHECK(result);
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}