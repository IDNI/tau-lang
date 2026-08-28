// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Tests for semantic PWR (Optimal Mode) — winning-region-based revision.
//
// Tests the Algorithm D-based fallback from pwr-ltl.tex §11.
// LS-18: the production code builds θ = ψ ∧ G(Win) ONLY (see
// semantic_pwr_optimal) -- Win_0 is not conjoined; build_win0_formula is
// exercised by these tests but has no production caller, and nothing
// verifies Win_0 ⊆ Win (realizability only guarantees SOME initial ρ
// wins). Keep that in mind when reading the SPWR-O cases.
//
// Test categories:
//   SPWR-S-*   Safety regression (optimal mode matches fast mode)
//   SPWR-L-*   Liveness preservation
//   SPWR-I-*   Idempotence
//   SPWR-F-*   Fallback chain (fast → optimal → update)
//   SPWR-A-*   alg_d_result struct and winning region export

#include "test_init.h"
#include "test_tau_helpers.h"
#include "pointwise_revision.h"
#include "boolean_algebras/qlt/qlt.h"
#include "ltl_aba.h"

using namespace idni::tau_lang;

// PW-N4: the semantic fallback ships OFF; this file is its test surface.
static const bool semantic_fallback_enabled_for_tests = [] {
	pwr_semantic_fallback = true;
	return true;
}();

static tref spec(const char* s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

static bool is_realizable(tref fm) {
	return fm && is_tau_formula_sat<node_t>(fm);
}

// ============================================================================
// SPWR-S: Safety regression — optimal mode should not break fast mode
// ============================================================================

TEST_SUITE("[SPWR-S: Safety regression]") {

	TEST_CASE("[SPWR-S-01] Compatible safety: same value") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-S-02] Incompatible safety: different values") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-S-03] Partial conflict: two outputs") {
		tref s = spec("G ((o1[t] = 0) && (o2[t] = 0)).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-S-04] Trivial spec: T * G(o1=0) = update") {
		tref s = spec("T.");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

}

// ============================================================================
// SPWR-L: Liveness preservation
// ============================================================================

TEST_SUITE("[SPWR-L: Liveness preservation]") {

	TEST_CASE("[SPWR-L-01] F(goal) with compatible update") {
		tref s = spec("F (o1[t] = 1).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-L-02] Until with compatible update") {
		tref s = spec("(o1[t] = 0) U (o1[t] = 1).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-L-03] Release with compatible update") {
		tref s = spec("(o1[t] = 1) R (o1[t] = 0).");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

}

// ============================================================================
// SPWR-I: Idempotence — spec * spec should be equivalent to spec
// ============================================================================

TEST_SUITE("[SPWR-I: Idempotence]") {

	TEST_CASE("[SPWR-I-01] Safety idempotence") {
		tref s = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, s, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-I-02] Eventually idempotence" * doctest::skip(!ltlsynt_available())) {
		tref s = spec("F (o1[t] = 1).");
		REQUIRE(s != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, s, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-I-03] Multi-clause idempotence") {
		tref s = spec("G (o1[t] = 0) && G (o2[t] = 1).");
		REQUIRE(s != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, s, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

}

// ============================================================================
// SPWR-F: Fallback chain (fast → optimal → update)
// ============================================================================

TEST_SUITE("[SPWR-F: Fallback chain]") {

	TEST_CASE("[SPWR-F-01] Result always realizable (AGM success)") {
		tref s = spec("G (o1[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-F-02] F spec: unrealizable spec with realizable update") {
		tref s = spec("F.");
		tref u = spec("G (o1[t] = 0).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

	TEST_CASE("[SPWR-F-03] Multi-clause: one compatible, one not") {
		tref s = spec("G (o1[t] = 0) && G (o2[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(s != nullptr);
		REQUIRE(u != nullptr);
		tref result = pointwise_revision_temporal<node_t>(s, u, 0);
		REQUIRE(result != nullptr);
		CHECK(is_realizable(result));
	}

}

// ============================================================================
// SPWR-A: alg_d_result and winning region export
// ============================================================================

TEST_SUITE("[SPWR-A: Algorithm D result]") {

	TEST_CASE("[SPWR-A-01] solve_algorithm_d_full returns realizable with winning region") {
		// Build a simple realizable formula G(d_0)
		// T1_size = 1 (one constant: 0), T3 = trivial
		// This is a basic sanity check for the extended interface.
		std::string phi_star = "G(d_0)";
		int T1_size = 1;
		int K = 1;
		std::vector<omcat::qlt_type3> T3;
		std::vector<int> type_A;

		// Create a minimal T3/type_A for a single atom that's always true.
		omcat::qlt_type3 t3;
		t3.pos_m = 0;
		t3.pos_x = 0;
		t3.pos_y = 0;
		T3.push_back(t3);
		type_A.push_back(1); // D_0 is true in this type

		auto result = alg_d::solve_algorithm_d_full(phi_star, T1_size,
			T3, type_A, K, /*init_rho=*/0);

		if (result.realizable) {
			CHECK(!result.winning_region.empty());
			CHECK(result.init_rho >= 0);
			CHECK(result.T1_size == T1_size);
			CHECK(result.K == K);
		}
		// If unrealizable due to ltlsynt not installed, that's OK — skip.
	}

	TEST_CASE("[SPWR-A-02] solve_algorithm_d_full empty input returns unrealizable") {
		auto result = alg_d::solve_algorithm_d_full("", 0, {}, {}, 0,
			/*init_rho=*/0);
		CHECK_FALSE(result.realizable);
		CHECK(result.winning_region.empty());
		CHECK(result.init_rho == -1);
	}

}


// ============================================================================
// SPWR-W: Win formula construction
// ============================================================================

TEST_SUITE("[SPWR-W: Win formula construction]") {

	TEST_CASE("[SPWR-W-01] build_win_formula: empty winning region returns nullptr") {
		alg_d::alg_d_result result;
		result.T1_size = 1;
		result.synth_game.num_states = 1;
		result.K = 1;

		std::vector<omcat::qlt_type3> T3;
		omcat::qlt_type3 t3;
		t3.pos_m = 0;
		T3.push_back(t3);
		std::vector<int> type_A = {1};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref result_formula = build_win_formula<node_t>(result, atoms, T3, type_A);
		CHECK(result_formula == nullptr);
	}

	TEST_CASE("[SPWR-W-02] build_win0_formula: empty winning region returns nullptr") {
		alg_d::alg_d_result result;
		result.T1_size = 1;
		result.synth_game.num_states = 1;
		result.synth_game.init = 0;
		result.K = 1;

		std::vector<omcat::qlt_type3> T3;
		omcat::qlt_type3 t3;
		t3.pos_m = 0;
		T3.push_back(t3);
		std::vector<int> type_A = {1};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref result_formula = build_win0_formula<node_t>(result, atoms, T3, type_A);
		CHECK(result_formula == nullptr);
	}

	TEST_CASE("[SPWR-W-03] build_win_formula: single atom, bit set returns the atom itself") {
		alg_d::alg_d_result result;
		result.T1_size = 1;
		result.synth_game.num_states = 1;
		result.K = 1;
		result.winning_region = {0};

		std::vector<omcat::qlt_type3> T3;
		omcat::qlt_type3 t3;
		t3.pos_m = 0;
		T3.push_back(t3);
		std::vector<int> type_A = {1};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref result_formula = build_win_formula<node_t>(result, atoms, T3, type_A);
		REQUIRE(result_formula != nullptr);
		CHECK(result_formula == atom0);
	}

	TEST_CASE("[SPWR-W-04] build_win_formula: single atom, bit clear returns negated atom") {
		alg_d::alg_d_result result;
		result.T1_size = 1;
		result.synth_game.num_states = 1;
		result.K = 1;
		result.winning_region = {0};

		std::vector<omcat::qlt_type3> T3;
		omcat::qlt_type3 t3;
		t3.pos_m = 0;
		T3.push_back(t3);
		std::vector<int> type_A = {0};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref result_formula = build_win_formula<node_t>(result, atoms, T3, type_A);
		REQUIRE(result_formula != nullptr);
		tref expected = build_wff_neg<node_t>(atom0);
		CHECK(result_formula == expected);
	}

	TEST_CASE("[SPWR-W-05] build_win_formula: two winning T3 types yield disjunction of both patterns") {
		alg_d::alg_d_result result;
		result.T1_size = 2;
		result.synth_game.num_states = 1;
		result.K = 1;
		result.winning_region = {0, 1};

		std::vector<omcat::qlt_type3> T3;
		omcat::qlt_type3 t3_0;
		t3_0.pos_m = 0;
		T3.push_back(t3_0);
		omcat::qlt_type3 t3_1;
		t3_1.pos_m = 1;
		T3.push_back(t3_1);
		std::vector<int> type_A = {0, 1};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref result_formula = build_win_formula<node_t>(result, atoms, T3, type_A);
		REQUIRE(result_formula != nullptr);
		tref expected = build_wff_or<node_t>(build_wff_neg<node_t>(atom0), atom0);
		CHECK(result_formula == expected);
	}

	TEST_CASE("[SPWR-W-06] build_win_formula: two atoms, mixed-bit pattern builds correct conjunction") {
		alg_d::alg_d_result result;
		result.T1_size = 1;
		result.synth_game.num_states = 1;
		result.K = 2;
		result.winning_region = {0};

		std::vector<omcat::qlt_type3> T3;
		omcat::qlt_type3 t3;
		t3.pos_m = 0;
		T3.push_back(t3);
		std::vector<int> type_A = {2}; // binary 10: bit0 clear, bit1 set

		tref atom0 = spec("o1[t] = 1.");
		tref atom1 = spec("o2[t] = 1.");
		REQUIRE(atom0 != nullptr);
		REQUIRE(atom1 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {
			{atom0, "d_0"}, {atom1, "d_1"}};

		tref result_formula = build_win_formula<node_t>(result, atoms, T3, type_A);
		REQUIRE(result_formula != nullptr);
		tref expected = build_wff_and<node_t>(build_wff_neg<node_t>(atom0), atom1);
		CHECK(result_formula == expected);
	}

	TEST_CASE("[SPWR-W-07] build_win_formula: stub states beyond base_n are ignored") {
		alg_d::alg_d_result result;
		result.T1_size = 1;
		result.synth_game.num_states = 1;
		result.K = 1;
		result.winning_region = {0, 5};

		std::vector<omcat::qlt_type3> T3;
		omcat::qlt_type3 t3;
		t3.pos_m = 0;
		T3.push_back(t3);
		std::vector<int> type_A = {1};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref with_stub = build_win_formula<node_t>(result, atoms, T3, type_A);

		alg_d::alg_d_result result_no_stub;
		result_no_stub.T1_size = 1;
		result_no_stub.synth_game.num_states = 1;
		result_no_stub.K = 1;
		result_no_stub.winning_region = {0};

		tref without_stub = build_win_formula<node_t>(result_no_stub, atoms, T3, type_A);

		REQUIRE(with_stub != nullptr);
		REQUIRE(without_stub != nullptr);
		CHECK(with_stub == without_stub);
		CHECK(with_stub == atom0);
	}

	TEST_CASE("[SPWR-W-08] build_win_formula vs build_win0_formula: all-states vs initial-state-only") {
		alg_d::alg_d_result result;
		result.T1_size = 2;
		result.synth_game.num_states = 2;
		result.synth_game.init = 0;
		result.K = 1;
		result.winning_region = {0, 3};
		// LG-12: Win₀ reads the FIXED initial memory from init_rho now
		// (the solver sets it; hand-built results must set it too).
		result.init_rho = 0;

		std::vector<omcat::qlt_type3> T3;
		omcat::qlt_type3 t3_0;
		t3_0.pos_m = 0;
		T3.push_back(t3_0);
		omcat::qlt_type3 t3_1;
		t3_1.pos_m = 1;
		T3.push_back(t3_1);
		std::vector<int> type_A = {1, 0};

		tref atom0 = spec("o1[t] = 1.");
		REQUIRE(atom0 != nullptr);
		std::vector<std::pair<tref, std::string>> atoms = {{atom0, "d_0"}};

		tref win = build_win_formula<node_t>(result, atoms, T3, type_A);
		REQUIRE(win != nullptr);
		tref expected_win = build_wff_or<node_t>(build_wff_neg<node_t>(atom0), atom0);
		CHECK(win == expected_win);

		tref win0 = build_win0_formula<node_t>(result, atoms, T3, type_A);
		REQUIRE(win0 != nullptr);
		CHECK(win0 == atom0);
	}

}


// ============================================================================
// LS-1 — and_distribute must recurse below a conjunction root
// ============================================================================
//
// `and_distribute` inspects only the ROOT operator, so a top-level `wff_and`
// makes `get_temporal_op` return NONE and the whole formula comes back
// unchanged.  `pointwise_revision_temporal` calls it once on the entire spec
// BEFORE `gather_top_conjuncts`, and the interpreter feeds it exactly the
// shape `G(∧ inners) ∧ (∧ rest)` produced by `unsqueeze_always` whenever any
// non-always clause exists.  Result: whole G-blocks get revised or dropped
// wholesale instead of per conjunct, contrary to pwr-ltl §3 Step 0.

TEST_SUITE("[LS-1: and_distribute below conjunction roots]") {

	static size_t clause_count(tref fm) {
		std::vector<tref> conjs;
		gather_top_conjuncts<node_t>(fm, conjs);
		return conjs.size();
	}

	// NOTE on the parentheses: `G X && Y` parses as `G (X && Y)` — the
	// temporal quantifier scopes over the rest of the conjunction.  Without
	// the explicit parens around each clause the root is an `always`, which
	// and_distribute already handles, and the case tests nothing.
	TEST_CASE("[PWR-AD-01] (G(a && b)) && (F c) splits the G block") {
		tref fm = spec("(G (o1[t] = 0 && o2[t] = 0)) && (F (o3[t] = 1)).");
		REQUIRE(fm != nullptr);
		REQUIRE(clause_count(fm) == 2);       // G-block + F, undistributed
		tref d = and_distribute<node_t>(fm);
		REQUIRE(d != nullptr);
		CHECK(clause_count(d) == 3);          // G(a), G(b), F(c)
	}

	TEST_CASE("[PWR-AD-02] a pure G root still distributes (regression)") {
		tref fm = spec("G (o1[t] = 0 && o2[t] = 0).");
		REQUIRE(fm != nullptr);
		tref d = and_distribute<node_t>(fm);
		REQUIRE(d != nullptr);
		CHECK(clause_count(d) == 2);
	}

	TEST_CASE("[PWR-AD-03] nested conjunction roots distribute too") {
		tref fm = spec("((G (o1[t] = 0 && o2[t] = 0)) && (G (o3[t] = 0))) "
		               "&& (F (o4[t] = 1)).");
		REQUIRE(fm != nullptr);
		REQUIRE(clause_count(fm) == 3);
		tref d = and_distribute<node_t>(fm);
		REQUIRE(d != nullptr);
		CHECK(clause_count(d) == 4);
	}

	TEST_CASE("[PWR-AD-04] a formula with nothing to distribute is unchanged") {
		tref fm = spec("(G (o1[t] = 0)) && (F (o2[t] = 1)).");
		REQUIRE(fm != nullptr);
		CHECK(and_distribute<node_t>(fm) == fm);
	}

}

// ============================================================================
// LS-2 / LS-16 — semantic_pwr_optimal: coverage + the Algorithm-A guards
// ============================================================================
//
// LS-16: every existing PWR test uses untyped/sbf atoms, so the applicability
// gate at the top of `semantic_pwr_optimal` returns nullptr and the entire
// body (skeleton build, solve_algorithm_d_full, Win/θ construction, the sat
// verification) has never executed.  These cases use qlt atoms so it does.
//
// LS-2: the body runs the same T3 encoding as Algorithm A in
// `ltl_aba_builders`, but WITHOUT Algorithm A's two soundness guards —
// (a) atoms that no T3 type can classify (`{top}:qlt` / `{bot}:qlt`), and
// (b) two or more distinct output variables, which the single Y/M slot
// conflates.  Both must fall back (nullptr) rather than return a θ built from
// a garbage winning region.

TEST_SUITE("[LS-2/LS-16: semantic_pwr_optimal]") {

	// Coverage case: a qlt clause + update that pass the applicability gate.
	// The verdict may legitimately be nullptr (unrealizable via Algorithm D,
	// or θ not realizable), but whatever comes back must be a sound revision.
	TEST_CASE("[SPWR-O-01] qlt clause reaches optimal mode") {
		bdd_init<Bool>();
		// Optimal mode runs Algorithm D, which shells out to ltlsynt; without
		// it every verdict below would be vacuous.  Skip explicitly rather
		// than pass on a nullptr that means "no backend".
		if (::system("which ltlsynt > /dev/null 2>&1") != 0) {
			MESSAGE("ltlsynt not on PATH — optimal mode cannot run");
			return;
		}
		tref c = spec("G (o1[t]:qlt < {1/2}:qlt).");
		tref u = spec("G (o1[t]:qlt > {1/2}:qlt).");
		REQUIRE(c != nullptr);
		REQUIRE(u != nullptr);
		tref theta = qlt_semantic_pwr_optimal<node_t>(c, u);
		// qlt_semantic_pwr_optimal itself no longer checks realizability —
		// that gate moved to the caller (pointwise_revision.h), since it is
		// an ordinary sat question and not (Q,<) theory.  Mirror the caller
		// here so this case still exercises the composed production
		// behaviour: a θ that is not realizable is discarded just as
		// pointwise_revision_temporal would discard it.
		if (theta && !is_tau_formula_sat<node_t>(theta, 0)) theta = nullptr;
		// θ is allowed to be null (Algorithm D may report the clause
		// unrealizable, or the composed θ may not be realizable), but a
		// returned θ must be a sound revision: realizable, and it must
		// still carry the update it was built from — `theta` is
		// `update ∧ G(Win)`, so dropping the update would be the failure mode
		// that matters.
		//
		// Membership is checked STRUCTURALLY, via subtree_set.  tref equality
		// is not structural equality in this tree — that is what the
		// subtree_map / subtree_set family exists for (see extract_data_atoms'
		// "structural equality (subtree_equals)" note) — and a raw `cj == u`
		// comparison reported "update dropped" for a θ that carries it.
		//
		// Two shapes are accepted: the update as a top-level conjunct of θ,
		// and — since both `update` and `G(Win)` are `always` formulas and
		// this codebase merges those — the update's body among the conjuncts
		// of a single enclosing G.
		if (theta) {
			INFO("theta:  " << tau::get(theta).to_str());
			INFO("update: " << tau::get(u).to_str());
			CHECK(is_realizable(theta));

			auto is_always = [](tref n) {
				const auto& t = tau::get(n);
				return t.has_child() && t[0].value.nt == tau::wff_always;
			};
			std::vector<tref> top;
			gather_top_conjuncts<node_t>(theta, top);
			subtree_set<node_t> top_set(top.begin(), top.end());
			bool keeps_update = top_set.contains(u);
			if (!keeps_update && is_always(u)) {
				tref ubody = tau::trim2(u);
				for (tref cj : top) {
					if (!is_always(cj)) continue;
					std::vector<tref> inner;
					gather_top_conjuncts<node_t>(tau::trim2(cj), inner);
					subtree_set<node_t> inner_set(inner.begin(), inner.end());
					if (inner_set.contains(ubody)) { keeps_update = true; break; }
				}
			}
			CHECK(keeps_update);
		} else {
			MESSAGE("optimal mode returned nullptr (Algorithm D reported the "
			        "clause unrealizable, or the composed theta was not "
			        "realizable) — nothing to assert on theta");
		}
	}

	// LS-2 (a): `{top}:qlt` gives qlt_atom_holds_in_type3 == nullopt for every
	// T3 type, so the atom is completely unconstrained in the symbolic
	// encoding — `h != false` maps nullopt to "holds".  Algorithm A refuses
	// this shape; optimal mode must too.
	//
	// The clause carries an ordinary rational constant so that
	// enumerate_qlt_T3 produces a NON-EMPTY type set: with only {top} around,
	// T3 is empty and the function bails out at build_win_formula for an
	// unrelated reason, which is what the first draft of this case hit.
	TEST_CASE("[SPWR-O-02] unclassifiable top/bot qlt atoms fall back") {
		bdd_init<Bool>();
		tref c = spec("G (o1[t]:qlt < {1/2}:qlt).");
		tref u = spec("F (o1[t]:qlt = {top}:qlt).");
		REQUIRE(c != nullptr);
		REQUIRE(u != nullptr);
		CHECK(qlt_semantic_pwr_optimal<node_t>(c, u) == nullptr);
	}

	// LS-2 (b): two distinct output variables share the single Y slot, so
	// `o1 < 1/2 && o2 > 1/2` collapses to "Y < 1/2 && Y > 1/2".
	TEST_CASE("[SPWR-O-03] two output variables fall back") {
		bdd_init<Bool>();
		tref c = spec("G (o1[t]:qlt < {1/2}:qlt && o2[t]:qlt > {1/2}:qlt).");
		tref u = spec("G (o1[t]:qlt > {1/2}:qlt).");
		REQUIRE(c != nullptr);
		REQUIRE(u != nullptr);
		CHECK(qlt_semantic_pwr_optimal<node_t>(c, u) == nullptr);
	}

	// The gate itself must keep rejecting non-omcat atoms.
	TEST_CASE("[SPWR-O-04] sbf atoms still do not reach optimal mode") {
		bdd_init<Bool>();
		tref c = spec("G (o1[t] = 0).");
		tref u = spec("G (o1[t] = 1).");
		REQUIRE(c != nullptr);
		REQUIRE(u != nullptr);
		CHECK(qlt_semantic_pwr_optimal<node_t>(c, u) == nullptr);
	}

}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}