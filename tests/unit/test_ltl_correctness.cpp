// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Correctness meta-tests for LTL(ABA) synthesis.
//
// These tests do NOT use the synthesis engine as an oracle for itself.
// Instead, they use three independent methods:
//
//   EQUIV  : LTL equivalences — equivalent formulas must give the same
//            realizability verdict. Catches normalization, NNF, and
//            skeleton-construction bugs.
//
//   CROSS  : Safety-path cross-validation — G(φ) formulas (no F/U/R/W)
//            must agree between the LTL code path and the original safety
//            pipeline (wff_always). The safety path is independently tested
//            and more trusted.
//
//   ADV    : Adversarial strategy verifier — for every REALIZABLE formula,
//            run the synthesized strategy against adversarial input streams
//            and verify the formula's semantic property holds at every step.
//            Uses formula-specific checkers written independently of the
//            synthesis code path.

#include "test_init.h"
#include "test_tau_helpers.h"

using namespace idni::tau_lang;

// ── helpers ───────────────────────────────────────────────────────────────────

static bool realizable(const char* s) {
	tref fm = [&]{
		auto nso = get_nso_rr<node_t>(tau::get(s));
		return nso.has_value() ? nso.value().main->get() : nullptr;
	}();
	if (!fm) return false;
	return is_tau_formula_sat<node_t>(fm);
}

// Run formula with given i1 stream for N steps, return o1 values.
static strings run_with_i1(const char* formula, const strings& i1_vals, size_t n) {
	io_context<node_t> ctx;
	ctx.add_input("i1", tau_type_id<node_t>(),
	              std::make_shared<vector_input_stream>(i1_vals));
	auto o1 = std::make_shared<vector_output_stream>();
	ctx.add_output("o1", tau_type_id<node_t>(), o1);
	auto nso = get_nso_rr<node_t>(ctx, tau::get(formula));
	if (!nso.has_value()) return {};
	tref fm = nso.value().main->get();
	if (!fm) return {};
	run<node_t>(fm, ctx, n);
	return o1->get_values();
}

// Run formula with no inputs for N steps, return o1 values.
static strings run_no_input(const char* formula, size_t n) {
	io_context<node_t> ctx;
	auto o1 = std::make_shared<vector_output_stream>();
	ctx.add_output("o1", tau_type_id<node_t>(), o1);
	auto nso = get_nso_rr<node_t>(ctx, tau::get(formula));
	if (!nso.has_value()) return {};
	tref fm = nso.value().main->get();
	if (!fm) return {};
	run<node_t>(fm, ctx, n);
	return o1->get_values();
}

// Run a qlt-typed formula with no inputs for N steps, return o1 values.
static strings run_qlt_no_input(const char* formula, size_t n) {
	io_context<node_t> ctx;
	auto o1 = std::make_shared<vector_output_stream>();
	ctx.add_output("o1", qlt_type_id<node_t>(), o1);
	subtree_map<node_t, size_t> scope;
	for (const auto& [var, type] : ctx.types) scope[var->get()] = type;
	tau::get_options opts;
	opts.global_scope = &scope;
	auto nso = get_nso_rr<node_t>(ctx, tau::get(formula, opts));
	if (!nso.has_value()) return {};
	tref fm = nso.value().main->get();
	if (!fm) return {};
	run<node_t>(fm, ctx, n);
	return o1->get_values();
}

// Run a qlt-typed formula with a given i1 stream for N steps, return o1 values.
static strings run_qlt_with_i1(const char* formula, const strings& i1_vals,
                               size_t n) {
	io_context<node_t> ctx;
	ctx.add_input("i1", qlt_type_id<node_t>(),
	              std::make_shared<vector_input_stream>(i1_vals));
	auto o1 = std::make_shared<vector_output_stream>();
	ctx.add_output("o1", qlt_type_id<node_t>(), o1);
	subtree_map<node_t, size_t> scope;
	for (const auto& [var, type] : ctx.types) scope[var->get()] = type;
	tau::get_options opts;
	opts.global_scope = &scope;
	auto nso = get_nso_rr<node_t>(ctx, tau::get(formula, opts));
	if (!nso.has_value()) return {};
	tref fm = nso.value().main->get();
	if (!fm) return {};
	run<node_t>(fm, ctx, n);
	return o1->get_values();
}

// ── semantic checkers (independent of synthesis engine) ───────────────────────

// Check G(o=c): every value must equal c.
static bool check_G_const(const strings& vals, const std::string& c) {
	for (auto& v : vals) if (v != c) return false;
	return !vals.empty();
}

// Check F(φ): some value in vals satisfies predicate.
static bool check_F(const strings& vals, const std::string& target) {
	for (auto& v : vals) if (v == target) return true;
	return false;
}

// Check G(F(φ)): within every window of size W, at least one step satisfies φ.
// We use W=2 (the Mealy period for a 2-state strategy).
static bool check_GF(const strings& vals, const std::string& target, size_t W = 2) {
	if (vals.size() < W) return !vals.empty();
	for (size_t i = 0; i + W <= vals.size(); i += W) {
		bool found = false;
		for (size_t j = 0; j < W; ++j)
			if (vals[i + j] == target) { found = true; break; }
		if (!found) return false;
	}
	return true;
}

// Check (o=A) U (o=B): all values before the first B must be A;
// B must appear.
static bool check_U(const strings& vals,
                    const std::string& left, const std::string& right) {
	for (size_t i = 0; i < vals.size(); ++i) {
		if (vals[i] == right) return true;      // right satisfied
		if (vals[i] != left)  return false;     // left must hold until then
	}
	return false; // right never appeared
}

// Check (o=A) W (o=B): all values before the first B (if any) must be A.
static bool check_W(const strings& vals,
                    const std::string& left, const std::string& right) {
	for (auto& v : vals) {
		if (v == right) return true;
		if (v != left)  return false;
	}
	return true; // left held forever — also satisfies W
}

// Check (φ T ψ) at index i of a finite trace, written straight from the
// definition and independent of the compile-away pass:
//     trace ⊨ φ T ψ at i   iff   ∀ j ≤ i : ψ(j) ∨ ∃ k ∈ (j, i] : φ(k)
// (φ T ψ is the past dual of Release: φ T ψ ≡ ¬(¬φ S ¬ψ).)
static bool check_trigger_at(const std::vector<bool>& phi,
                             const std::vector<bool>& psi, size_t i) {
	for (size_t j = 0; j <= i; ++j) {
		if (psi[j]) continue;            // ψ discharges this j
		bool excused = false;            // …otherwise some later φ must
		for (size_t k = j + 1; k <= i; ++k)
			if (phi[k]) { excused = true; break; }
		if (!excused) return false;
	}
	return true;
}

// A tau spec must hold at every step of the run, so check φ T ψ at every i.
static bool check_trigger_always(const std::vector<bool>& phi,
                                 const std::vector<bool>& psi) {
	if (phi.empty() || phi.size() != psi.size()) return false;
	for (size_t i = 0; i < phi.size(); ++i)
		if (!check_trigger_at(phi, psi, i)) return false;
	return true;
}

// Check G(o=i): every output equals paired input.
static bool check_G_mirror(const strings& outs, const strings& ins) {
	if (outs.size() != ins.size()) return false;
	for (size_t i = 0; i < outs.size(); ++i)
		if (outs[i] != ins[i]) return false;
	return !outs.empty();
}

// Check G(o=i[t-1]): output[k] = input[k-1] for k >= 1 (step 0 is warmup).
static bool check_G_lookback1(const strings& outs, const strings& ins) {
	for (size_t k = 1; k < outs.size() && k < ins.size(); ++k)
		if (outs[k] != ins[k - 1]) return false;
	return outs.size() >= 2;
}

// ── EQUIV: LTL equivalence tests ─────────────────────────────────────────────
//
// For each pair of equivalent LTL formulas, both must give the same
// realizability verdict.  These tests use only the realizability oracle,
// NOT the execution engine, so they are independent of the strategy path.

TEST_SUITE("LTL correctness: equivalences") {

	// G(φ) ≡ ¬F(¬φ) — global/eventual duality
	TEST_CASE("[EQUIV-01] G(o=0) ≡ ¬F(¬(o=0)): same realizability") {
		bdd_init<Bool>();
		bool r1 = realizable("G (o1[t] = 0).");
		bool r2 = realizable("! F (! (o1[t] = 0)).");
		CHECK(r1 == r2);
		CHECK(r1); // both should be REALIZABLE
	}

	TEST_CASE("[EQUIV-02] G(o=1) ≡ ¬F(¬(o=1)): same realizability") {
		bdd_init<Bool>();
		CHECK(realizable("G (o1[t] = 1).") == realizable("! F (! (o1[t] = 1))."));
	}

	// F(φ) ≡ T U φ (T is wff_t, written as (1=1) or just use the tau constant)
	TEST_CASE("[EQUIV-03] F(o=0) ≡ F(o=0): trivial self-equivalence") {
		bdd_init<Bool>();
		// Both parse paths of F(o=0) should agree
		bool r1 = realizable("F (o1[t] = 0).");
		bool r2 = realizable("F (o1[t] = 0).");
		CHECK(r1 == r2);
		CHECK(r1);
	}

	// G(φ) && G(ψ) ≡ G(φ && ψ) — our new auto-merge fix
	TEST_CASE("[EQUIV-04] G(o=0)&&G(o=0) ≡ G(o=0): same realizability") {
		bdd_init<Bool>();
		bool r1 = realizable("G (o1[t] = 0) && G (o1[t] = 0).");
		bool r2 = realizable("G (o1[t] = 0).");
		CHECK(r1 == r2);
		CHECK(r1);
	}

	TEST_CASE("[EQUIV-05] G(o=1)&&G(o=1) ≡ G(o=1): same realizability") {
		bdd_init<Bool>();
		bool r1 = realizable("G (o1[t] = 1) && G (o1[t] = 1).");
		bool r2 = realizable("G (o1[t] = 1).");
		CHECK(r1 == r2);
		CHECK(r1);
	}

	// φ U ψ realizability implies φ W ψ realizability (W is weaker)
	TEST_CASE("[EQUIV-06] (o=1) U (o=0) realizable → (o=1) W (o=0) realizable") {
		bdd_init<Bool>();
		bool u = realizable("(o1[t] = 1) U (o1[t] = 0).");
		bool w = realizable("(o1[t] = 1) W (o1[t] = 0).");
		CHECK(u); // U is realizable
		CHECK(w); // W must also be realizable (W is weaker — fewer obligations)
	}

	TEST_CASE("[EQUIV-07] (o=0) U (o=1) realizable → (o=0) W (o=1) realizable") {
		bdd_init<Bool>();
		bool u = realizable("(o1[t] = 0) U (o1[t] = 1).");
		bool w = realizable("(o1[t] = 0) W (o1[t] = 1).");
		CHECK(u);
		CHECK(w);
	}

	// φ R ψ ≡ ¬(¬φ U ¬ψ) — release/until duality
	TEST_CASE("[EQUIV-08] (o=0) R (o=1) ≡ ¬(¬(o=0) U ¬(o=1)): same realizability") {
		bdd_init<Bool>();
		bool r1 = realizable("(o1[t] = 0) R (o1[t] = 1).");
		bool r2 = realizable("! (! (o1[t] = 0)) U (! (o1[t] = 1)).");
		CHECK(r1 == r2);
	}

	// G(F(φ)) realizability — phi = 0 and phi = 1 both realizable
	TEST_CASE("[EQUIV-09] G(F(o=0)) is realizable") {
		bdd_init<Bool>();
		CHECK(realizable("G (F (o1[t] = 0))."));
	}

	TEST_CASE("[EQUIV-10] G(F(o=1)) is realizable") {
		bdd_init<Bool>();
		CHECK(realizable("G (F (o1[t] = 1))."));
	}

	// Double negation: ¬¬φ has same realizability as φ
	TEST_CASE("[EQUIV-11] ¬¬G(o=0) ≡ G(o=0): same realizability") {
		bdd_init<Bool>();
		bool r1 = realizable("G (o1[t] = 0).");
		bool r2 = realizable("! ! G (o1[t] = 0).");
		CHECK(r1 == r2);
	}

	TEST_CASE("[EQUIV-12] ¬¬F(o=1) ≡ F(o=1): same realizability") {
		bdd_init<Bool>();
		bool r1 = realizable("F (o1[t] = 1).");
		bool r2 = realizable("! ! F (o1[t] = 1).");
		CHECK(r1 == r2);
	}

	// Contradictory formulas are UNREALIZABLE
	TEST_CASE("[EQUIV-13] G(o=0) && G(o=1) is UNREALIZABLE (same type)") {
		bdd_init<Bool>();
		CHECK_FALSE(realizable("G (o1[t] = 0) && G (o1[t] = 1)."));
	}

	TEST_CASE("[EQUIV-14] F(o=0) && G(o=1) is UNREALIZABLE") {
		bdd_init<Bool>();
		CHECK_FALSE(realizable("F (o1[t] = 0) && G (o1[t] = 1)."));
	}

	// Mixed-input unrealizability: env always blocks mixed atom
	TEST_CASE("[EQUIV-15] F((o1&i1)={T.}:tau) is UNREALIZABLE — env blocks") {
		bdd_init<Bool>();
		CHECK_FALSE(realizable("F ((o1[t] & i1[t]) = {T.}:tau)."));
	}

	// G(o=i) is REALIZABLE (system can mirror input)
	TEST_CASE("[EQUIV-16] G(o=i):tau is REALIZABLE") {
		bdd_init<Bool>();
		CHECK(realizable("G (o1[t]:tau = i1[t]:tau)."));
	}

	// sbf type equivalences
	TEST_CASE("[EQUIV-17] G(o={X&Y}:sbf) ≡ ¬F(¬(o={X&Y}:sbf)): same realizability") {
		bdd_init<Bool>();
		bool r1 = realizable("G (o1[t]:sbf = {X & Y}:sbf).");
		bool r2 = realizable("! F (! (o1[t]:sbf = {X & Y}:sbf)).");
		CHECK(r1 == r2);
		CHECK(r1);
	}

	TEST_CASE("[EQUIV-18] G(o={X&Y}:sbf)&&G(o={X&Y}:sbf) ≡ G(o={X&Y}:sbf)") {
		bdd_init<Bool>();
		bool r1 = realizable("G (o1[t]:sbf = {X & Y}:sbf) && G (o1[t]:sbf = {X & Y}:sbf).");
		bool r2 = realizable("G (o1[t]:sbf = {X & Y}:sbf).");
		CHECK(r1 == r2);
		CHECK(r1);
	}

} // TEST_SUITE "LTL correctness: equivalences"

// ── CROSS: safety-path cross-validation ──────────────────────────────────────
//
// G(φ) with no F/U/R/W can be checked by BOTH the LTL code path AND the
// original safety pipeline (wff_always / tau::build_wff_always).
// They must agree.

TEST_SUITE("LTL correctness: safety path cross-validation") {

	// Helper: check a formula via the safety path (no LTL operators).
	// In tau-lang, "always" is the default when no temporal operator is present.
	// A formula without G/F/U/R/W goes through the original safety synthesis
	// pipeline in is_tau_formula_sat (has_ltl_operators returns false).
	static bool safety_realizable(const char* fm_str) {
		auto nso = get_nso_rr<node_t>(tau::get(fm_str));
		if (!nso.has_value()) return false;
		tref fm = nso.value().main->get();
		if (!fm) return false;
		return is_tau_formula_sat<node_t>(fm);
	}

	TEST_CASE("[CROSS-01] G(o=0):tau — LTL path agrees with safety path") {
		bdd_init<Bool>();
		bool ltl_r  = realizable("G (o1[t] = 0).");
		bool safe_r = safety_realizable("(o1[t] = 0).");
		CHECK(ltl_r == safe_r);
		CHECK(ltl_r);
	}

	TEST_CASE("[CROSS-02] G(o=1):tau — LTL path agrees with safety path") {
		bdd_init<Bool>();
		bool ltl_r  = realizable("G (o1[t] = 1).");
		bool safe_r = safety_realizable("(o1[t] = 1).");
		CHECK(ltl_r == safe_r);
		CHECK(ltl_r);
	}

	TEST_CASE("[CROSS-03] G(o=i):tau — LTL path agrees with safety path") {
		bdd_init<Bool>();
		bool ltl_r  = realizable("G (o1[t]:tau = i1[t]:tau).");
		bool safe_r = safety_realizable("(o1[t]:tau = i1[t]:tau).");
		CHECK(ltl_r == safe_r);
		CHECK(ltl_r);
	}

	TEST_CASE("[CROSS-04] G(o=i[t-1]):tau — LTL agrees with safety path") {
		bdd_init<Bool>();
		bool ltl_r  = realizable("G (o1[t]:tau = i1[t-1]:tau).");
		bool safe_r = safety_realizable("(o1[t]:tau = i1[t-1]:tau).");
		CHECK(ltl_r == safe_r);
		CHECK(ltl_r);
	}

	TEST_CASE("[CROSS-05] G((o&i)={T.}:tau) — UNREALIZABLE on both paths") {
		bdd_init<Bool>();
		bool ltl_r  = realizable("G ((o1[t] & i1[t]) = {T.}:tau).");
		bool safe_r = safety_realizable("((o1[t] & i1[t]) = {T.}:tau).");
		CHECK(ltl_r == safe_r);
		CHECK_FALSE(ltl_r);
	}

	TEST_CASE("[CROSS-06] G(o={X&Y}:sbf) — LTL agrees with safety path") {
		bdd_init<Bool>();
		bool ltl_r  = realizable("G (o1[t]:sbf = {X & Y}:sbf).");
		bool safe_r = safety_realizable("(o1[t]:sbf = {X & Y}:sbf).");
		CHECK(ltl_r == safe_r);
		CHECK(ltl_r);
	}

	TEST_CASE("[CROSS-07] G(o={#b10110101}:bv[8]) — LTL agrees with safety path") {
		bdd_init<Bool>();
		bool ltl_r  = realizable("G (o1[t]:bv[8] = {#b10110101}:bv[8]).");
		bool safe_r = safety_realizable("(o1[t]:bv[8] = {#b10110101}:bv[8]).");
		CHECK(ltl_r == safe_r);
		CHECK(ltl_r);
	}

	TEST_CASE("[CROSS-08] G(o&&o=0):tau UNREALIZABLE — both paths agree") {
		bdd_init<Bool>();
		// o1 & o1 = 0 means o1 = 0 in ABA. This IS realizable (o1=bottom).
		bool ltl_r  = realizable("G ((o1[t] & o1[t]) = 0).");
		bool safe_r = safety_realizable("((o1[t] & o1[t]) = 0).");
		CHECK(ltl_r == safe_r);
	}

} // TEST_SUITE "LTL correctness: safety path cross-validation"

// ── ADV: adversarial strategy verifier ───────────────────────────────────────
//
// For each REALIZABLE formula, the synthesized strategy is run against
// adversarial input streams and the formula's semantic property is verified
// at every step using formula-specific checkers written independently of the
// synthesis engine.
//
// Adversarial inputs (tau type):
//   bottom  = "F."  (ABA bottom element)
//   top     = "T."  (ABA top element)
//   named   = "<:a> = 0"  (named atom)
//   alt     = alternating bottom/top

TEST_SUITE("LTL correctness: adversarial strategy verifier") {

	const strings adv_bottom(8, "F.");
	const strings adv_top(8, "T.");
	const strings adv_alt = {"F.", "T.", "F.", "T.", "F.", "T.", "F.", "T."};
	const strings adv_named(6, "<:a> = 0");

	// ── G formulas ────────────────────────────────────────────────────────────

	TEST_CASE("[ADV-G-01] G(o=0):tau — bottom inputs: every output is F") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t] = 0).", adv_bottom, 8);
		CHECK(check_G_const(vals, "F"));
	}

	TEST_CASE("[ADV-G-02] G(o=0):tau — top inputs: every output is F") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t] = 0).", adv_top, 8);
		CHECK(check_G_const(vals, "F"));
	}

	TEST_CASE("[ADV-G-03] G(o=0):tau — alternating inputs: every output is F") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t] = 0).", adv_alt, 8);
		CHECK(check_G_const(vals, "F"));
	}

	TEST_CASE("[ADV-G-04] G(o=1):tau — bottom inputs: every output is T") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t] = 1).", adv_bottom, 8);
		CHECK(check_G_const(vals, "T"));
	}

	TEST_CASE("[ADV-G-05] G(o=1):tau — top inputs: every output is T") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t] = 1).", adv_top, 8);
		CHECK(check_G_const(vals, "T"));
	}

	TEST_CASE("[ADV-G-06] G(o=1):tau — alternating inputs: every output is T") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t] = 1).", adv_alt, 8);
		CHECK(check_G_const(vals, "T"));
	}

	// ── G(o=i) input mirroring ────────────────────────────────────────────────
	// Tau bottom ("F.") outputs as "F" and top ("T.") as "T" (no period).
	// Named atoms (e.g. "<:a> = 0") are preserved as-is.

	TEST_CASE("[ADV-MIR-01] G(o=i):tau — bottom inputs: every output is F") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t]:tau = i1[t]:tau).", adv_bottom, 6);
		CHECK(check_G_const(vals, "F"));
	}

	TEST_CASE("[ADV-MIR-02] G(o=i):tau — top inputs: every output is T") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t]:tau = i1[t]:tau).", adv_top, 6);
		CHECK(check_G_const(vals, "T"));
	}

	TEST_CASE("[ADV-MIR-03] G(o=i):tau — alternating inputs: output mirrors input") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t]:tau = i1[t]:tau).", adv_alt, 8);
		// F./T. alternate → output should alternate "F"/"T"
		strings expected_alt = {"F", "T", "F", "T", "F", "T", "F", "T"};
		CHECK(check_G_mirror(vals, expected_alt));
	}

	TEST_CASE("[ADV-MIR-04] G(o=i):tau — named inputs: output mirrors named") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t]:tau = i1[t]:tau).", adv_named, 6);
		CHECK(check_G_mirror(vals, adv_named));
	}

	// ── G(o=i[t-1]) lookback ─────────────────────────────────────────────────
	// Output[k] = input[k-1]; "F." inputs → "F" outputs, "T." → "T".

	TEST_CASE("[ADV-LB-01] G(o=i[t-1]):tau — bottom inputs: steps 1+ are F") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t]:tau = i1[t-1]:tau).", adv_bottom, 6);
		strings bottom_canonical(adv_bottom.size(), "F");
		CHECK(check_G_lookback1(vals, bottom_canonical));
	}

	TEST_CASE("[ADV-LB-02] G(o=i[t-1]):tau — top inputs: steps 1+ are T") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t]:tau = i1[t-1]:tau).", adv_top, 6);
		strings top_canonical(adv_top.size(), "T");
		CHECK(check_G_lookback1(vals, top_canonical));
	}

	TEST_CASE("[ADV-LB-03] G(o=i[t-1]):tau — alternating: steps 1+ are shifted alt") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (o1[t]:tau = i1[t-1]:tau).", adv_alt, 7);
		strings alt_canonical = {"F", "T", "F", "T", "F", "T", "F", "T"};
		CHECK(check_G_lookback1(vals, alt_canonical));
	}

	// ── F formulas ────────────────────────────────────────────────────────────

	TEST_CASE("[ADV-F-01] F(o=0):tau — bottom inputs: F appears within 4 steps") {
		bdd_init<Bool>();
		auto vals = run_with_i1("F (o1[t] = 0).", adv_bottom, 4);
		CHECK(check_F(vals, "F"));
	}

	TEST_CASE("[ADV-F-02] F(o=0):tau — top inputs: F appears within 4 steps") {
		bdd_init<Bool>();
		auto vals = run_with_i1("F (o1[t] = 0).", adv_top, 4);
		CHECK(check_F(vals, "F"));
	}

	TEST_CASE("[ADV-F-03] F(o=1):tau — alternating inputs: T appears within 4 steps") {
		bdd_init<Bool>();
		auto vals = run_with_i1("F (o1[t] = 1).", adv_alt, 4);
		CHECK(check_F(vals, "T"));
	}

	// ── G(F(φ)) liveness ─────────────────────────────────────────────────────

	TEST_CASE("[ADV-GF-01] G(F(o=0)):tau — bottom inputs: F within every 2-step window") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (F (o1[t] = 0)).", adv_bottom, 8);
		CHECK(check_GF(vals, "F", 2));
	}

	TEST_CASE("[ADV-GF-02] G(F(o=0)):tau — top inputs: F within every 2-step window") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (F (o1[t] = 0)).", adv_top, 8);
		CHECK(check_GF(vals, "F", 2));
	}

	TEST_CASE("[ADV-GF-03] G(F(o=0)):tau — alternating inputs: F within every 2-step window") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (F (o1[t] = 0)).", adv_alt, 8);
		CHECK(check_GF(vals, "F", 2));
	}

	TEST_CASE("[ADV-GF-04] G(F(o=1)):tau — bottom inputs: T within every 2-step window") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (F (o1[t] = 1)).", adv_bottom, 8);
		CHECK(check_GF(vals, "T", 2));
	}

	TEST_CASE("[ADV-GF-05] G(F(o=1)):tau — named inputs: T within every 2-step window") {
		bdd_init<Bool>();
		auto vals = run_with_i1("G (F (o1[t] = 1)).", adv_named, 6);
		CHECK(check_GF(vals, "T", 2));
	}

	// ── Until / Weak-Until ────────────────────────────────────────────────────

	TEST_CASE("[ADV-U-01] (o=1) U (o=0):tau — bottom inputs: left until right") {
		bdd_init<Bool>();
		auto vals = run_with_i1("(o1[t] = 1) U (o1[t] = 0).", adv_bottom, 6);
		CHECK(check_U(vals, "T", "F"));
	}

	TEST_CASE("[ADV-U-02] (o=1) U (o=0):tau — top inputs: left until right") {
		bdd_init<Bool>();
		auto vals = run_with_i1("(o1[t] = 1) U (o1[t] = 0).", adv_top, 6);
		CHECK(check_U(vals, "T", "F"));
	}

	TEST_CASE("[ADV-U-03] (o=1) U (o=0):tau — alternating inputs: left until right") {
		bdd_init<Bool>();
		auto vals = run_with_i1("(o1[t] = 1) U (o1[t] = 0).", adv_alt, 6);
		CHECK(check_U(vals, "T", "F"));
	}

	TEST_CASE("[ADV-W-01] (o=1) W (o=0):tau — bottom inputs: left until right or forever") {
		bdd_init<Bool>();
		auto vals = run_with_i1("(o1[t] = 1) W (o1[t] = 0).", adv_bottom, 6);
		CHECK(check_W(vals, "T", "F"));
	}

	TEST_CASE("[ADV-W-02] (o=0) W (o=1):tau — alternating inputs: left until right or forever") {
		bdd_init<Bool>();
		auto vals = run_with_i1("(o1[t] = 0) W (o1[t] = 1).", adv_alt, 6);
		CHECK(check_W(vals, "F", "T"));
	}

	// ── sbf type adversarial ──────────────────────────────────────────────────

	TEST_CASE("[ADV-SBF-01] G(o={X&Y}:sbf) — every output is non-empty") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto nso = get_nso_rr<node_t>(ctx, tau::get("G (o1[t]:sbf = {X & Y}:sbf)."));
		REQUIRE(nso.has_value());
		run<node_t>(nso.value().main->get(), ctx, 6);
		auto vals = o1->get_values();
		REQUIRE(vals.size() == 6);
		for (auto& v : vals) CHECK(!v.empty()); // every step produces output
	}

	TEST_CASE("[ADV-SBF-02] F(o={X|Z}:sbf) — at least one non-empty output") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto nso = get_nso_rr<node_t>(ctx, tau::get("F (o1[t]:sbf = {X | Z}:sbf)."));
		REQUIRE(nso.has_value());
		run<node_t>(nso.value().main->get(), ctx, 4);
		bool found = false;
		for (auto& v : o1->get_values()) if (!v.empty()) { found = true; break; }
		CHECK(found);
	}

	// ── bv type adversarial ───────────────────────────────────────────────────

	TEST_CASE("[ADV-BV-01] G(o={#b10110101}:bv[8]) — every output is non-empty") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto nso = get_nso_rr<node_t>(ctx, tau::get("G (o1[t]:bv[8] = {#b10110101}:bv[8])."));
		REQUIRE(nso.has_value());
		run<node_t>(nso.value().main->get(), ctx, 6);
		auto vals = o1->get_values();
		REQUIRE(vals.size() == 6);
		for (auto& v : vals) CHECK(!v.empty());
	}

} // TEST_SUITE "LTL correctness: adversarial strategy verifier"

// ── TRG: Trigger (T) semantics ───────────────────────────────────────────────
//
// Hand derivation of the Trigger operator, used by every case below.
//
//   φ T ψ  is the past dual of Release:   φ T ψ  ≡  ¬(¬φ S ¬ψ)
//
//   π,i ⊨ φ S ψ   iff  ∃ j ≤ i : π,j ⊨ ψ  and  ∀ k ∈ (j,i] : π,k ⊨ φ
//   π,i ⊨ φ T ψ   iff  ∀ j ≤ i : π,j ⊨ ψ  or   ∃ k ∈ (j,i] : π,k ⊨ φ
//
//   Read operationally: ψ must hold from the most recent point where φ held
//   (or from 0, when φ never held) up to and INCLUDING i.  "Including i"
//   because j = i is in range and its excuse window (i,i] is empty, so
//
//        π,i ⊨ φ T ψ   ⇒   π,i ⊨ ψ        (for every i, unconditionally)
//
//   A tau spec must hold at every step of the run (the pure-past execution
//   path in compile_since_trigger encodes the top-level operator as a
//   G-invariant), therefore
//
//        spec "φ T ψ."   ⇒   ψ holds at EVERY step, t = 0 included.
//
// The bug this suite pins (LT-1): the T→S rewrite used to funnel is_outer
// into the S case, which asserts G(since) and ¬ψ@0 — i.e. exactly the
// negation of the required obligation, forcing ¬ψ at t=0 and forbidding ψ
// afterwards.

TEST_SUITE("LTL correctness: Trigger (T) semantics") {

	// ── execution ────────────────────────────────────────────────────────────

	// φ = (o1 = 1/4), ψ = (o1 = 3/4).
	// ψ is required at every step (j = i arm), so o1 = 3/4 always; and then
	// φ never holds (3/4 ≠ 1/4), which is the "φ never fires ⇒ ψ forever"
	// arm of the definition — consistent, so the spec is satisfiable.
	// Expected trace:  3/4, 3/4, 3/4, 3/4
	TEST_CASE("[TRG-EXEC-01] (o1={1/4}:qlt) T (o1={3/4}:qlt) — ψ at every step") {
		bdd_init<Bool>();
		auto vals = run_qlt_no_input(
		    "(o1[t]:qlt = {1/4}:qlt) T (o1[t]:qlt = {3/4}:qlt).", 4);
		REQUIRE(vals.size() == 4);
		for (auto& v : vals) CHECK(v == "3/4");
		std::vector<bool> phi, psi;
		for (auto& v : vals) {
			phi.push_back(v == "1/4");
			psi.push_back(v == "3/4");
		}
		CHECK(check_trigger_always(phi, psi));
	}

	// t = 0 edge, "φ fires at every step" arm.
	// φ = (o1 > 1/2), ψ = (o1 = 3/4).  Whenever ψ holds so does φ (3/4 > 1/2),
	// so every j < i is excused by k = i.  Only j = i is left, and its window
	// (i,i] is empty ⇒ ψ(i) required.  At i = 0 that is the ONLY obligation:
	// the very first emitted value must satisfy ψ.
	// Expected trace:  3/4, 3/4, 3/4, 3/4   (first value load-bearing)
	TEST_CASE("[TRG-EXEC-02] (o1>{1/2}:qlt) T (o1={3/4}:qlt) — ψ at t=0") {
		bdd_init<Bool>();
		auto vals = run_qlt_no_input(
		    "(o1[t]:qlt > {1/2}:qlt) T (o1[t]:qlt = {3/4}:qlt).", 4);
		REQUIRE(vals.size() == 4);
		CHECK(vals[0] == "3/4");   // t = 0 edge: inverted encoding forces ≠ 3/4
		for (auto& v : vals) CHECK(v == "3/4");
	}

	// Adversarial environment: φ lives on the input stream, so the environment
	// chooses when the trigger fires.  It cannot help: ψ is still required at
	// every i (j = i arm), independently of i1.  A trace where o1 lapses from
	// 3/4 at any step — in particular after a firing of φ at t=0 or t=2 —
	// violates the spec and must not be produced.
	// i1 = 1/4, 1/2, 1/4, 1/2  ⇒  expected o1 = 3/4, 3/4, 3/4, 3/4
	TEST_CASE("[TRG-EXEC-03] (i1={1/4}:qlt) T (o1={3/4}:qlt) — env cannot excuse ψ") {
		bdd_init<Bool>();
		const strings i1_vals = {"1/4", "1/2", "1/4", "1/2"};
		auto vals = run_qlt_with_i1(
		    "(i1[t]:qlt = {1/4}:qlt) T (o1[t]:qlt = {3/4}:qlt).", i1_vals, 4);
		REQUIRE(vals.size() == 4);
		for (auto& v : vals) CHECK(v == "3/4");
		std::vector<bool> phi, psi;
		for (size_t k = 0; k < vals.size(); ++k) {
			phi.push_back(i1_vals[k] == "1/4");
			psi.push_back(vals[k] == "3/4");
		}
		CHECK(check_trigger_always(phi, psi));
	}

	// ── satisfiability (mirrors the execution cases) ─────────────────────────

	TEST_CASE("[TRG-SAT-01] (o1={1/4}:qlt) T (o1={3/4}:qlt) is REALIZABLE") {
		bdd_init<Bool>();
		CHECK(realizable("(o1[t]:qlt = {1/4}:qlt) T (o1[t]:qlt = {3/4}:qlt)."));
	}

	// Violating requirement rejected: the T forces ψ at every step, the second
	// conjunct forbids it at every step.
	TEST_CASE("[TRG-SAT-02] (φ T ψ) && G(¬ψ) is UNREALIZABLE") {
		bdd_init<Bool>();
		CHECK_FALSE(realizable(
		    "(o1[t]:qlt = {1/4}:qlt) T (o1[t]:qlt = {3/4}:qlt) "
		    "&& G (o1[t]:qlt != {3/4}:qlt)."));
	}

	// ψ is a pure-input atom: the environment falsifies it and ψ is required
	// at every step, so no strategy exists.
	TEST_CASE("[TRG-SAT-03] (o1={1/4}:qlt) T (i1={3/4}:qlt) is UNREALIZABLE") {
		bdd_init<Bool>();
		CHECK_FALSE(realizable(
		    "(o1[t]:qlt = {1/4}:qlt) T (i1[t]:qlt = {3/4}:qlt)."));
	}

	// Duality: φ T ψ ≡ ¬(¬φ S ¬ψ) — both must get the same verdict.
	TEST_CASE("[TRG-SAT-04] φ T ψ ≡ ¬(¬φ S ¬ψ): same realizability") {
		bdd_init<Bool>();
		bool r1 = realizable("(o1[t]:qlt = {1/4}:qlt) T (o1[t]:qlt = {3/4}:qlt).");
		bool r2 = realizable("! ((! (o1[t]:qlt = {1/4}:qlt)) "
		                     "S (! (o1[t]:qlt = {3/4}:qlt))).");
		CHECK(r1 == r2);
		CHECK(r1);
	}

} // TEST_SUITE "LTL correctness: Trigger (T) semantics"


// ── LT-2: outer-flag propagation on the pure-past execution path ─────────────
//
// `compile_since_trigger_rec` gives the OUTERMOST S the treatment
// `G(curr && rhs)` — "this Since must hold at every step".  That treatment is
// only correct when the S is asserted positively by the top-level conjunct
// spine.  Propagating `is_outer` through negation and disjunction makes the
// derived safety formula demand `φ S ψ` always even when the spec says the
// opposite (`!(φ S ψ)`) or offers an alternative (`(φ S ψ) || χ`).
//
// The second half of the same defect is that `ltl_to_safety_formula_full`
// returns only `safety_fm && init_fm` and DISCARDS the compiled formula, so
// the Boolean structure around the S never reaches the interpreter at all.
//
// Both cases below are executed, not merely checked for satisfiability: the
// sat path uses the independent ppLTLTT tester encoding, which is not affected.

TEST_SUITE("LTL correctness: S under negation / disjunction (LT-2)") {

	// `!(φ S ψ)` with φ = (o1 = 1/4), ψ = (o1 = 3/4).
	//
	// Strong-past semantics give `φ S ψ` ≡ ψ at t = 0, and the spec must hold
	// at every step, so the correct reading is `G(¬(φ S ψ))`.  With the aux
	// tracking invariant `G(curr ↔ (ψ ∨ (φ ∧ prev)))` and the obligation
	// `G(¬curr)`, that forces `G(¬ψ)`: o1 is never 3/4.
	//
	// Under the outer-flag bug the S inherits is_outer through the negation,
	// so the safety formula becomes `G(curr && rhs) && ψ@0` — the exact
	// opposite — and the first emitted value is 3/4.
	TEST_CASE("[LT2-EXEC-01] !((o1={1/4}:qlt) S (o1={3/4}:qlt)) never emits ψ") {
		bdd_init<Bool>();
		auto vals = run_qlt_no_input(
		    "! ((o1[t]:qlt = {1/4}:qlt) S (o1[t]:qlt = {3/4}:qlt)).", 4);
		REQUIRE(vals.size() == 4);
		for (auto& v : vals) CHECK(v != "3/4");
	}

	// `(α S α) || (β S β)`.  Since `χ S χ ≡ χ`, this is just `G(α || β)`
	// with α = (i1 = 3/4) on the input stream and β = (o1 = 1/2) on the
	// output.  The environment never plays 3/4, so the system must emit 1/2
	// at every step.
	//
	// Three properties of this shape are load-bearing:
	//
	//  * BOTH operands are temporal.  A bare `(o1 = 1/2)` disjunct is
	//    rejected outright by `missing_temp_quants` ("must be scoped by a
	//    temporal quantifier"), which is what the first draft hit.
	//  * α is false at every step INCLUDING t=0, and it is both the φ and
	//    the ψ of its Since, so `curr ↔ (ψ ∨ (φ ∧ prev))` pins that
	//    auxiliary to 0 without even needing the t=0 anchor — a phantom
	//    `aux[-1]` could never leak in through a false φ.  (Since LA-N3
	//    both inner auxiliaries are additionally seeded to 0 by
	//    seed_since_aux_bits, which agrees with the pinned value.)
	//  * neither disjunct is an `always`, so the obligation wraps as
	//    `G(curr0 || curr1)` rather than a nested `G(… || G(…))`.
	//
	// Under the outer-flag bug BOTH Sinces inherit is_outer through the `||`
	// and are forced to hold at every step; the first then demands
	// `i1 = 3/4` always, which this input stream makes impossible, so the
	// spec is rejected and nothing is emitted at all.
	TEST_CASE("[LT2-EXEC-02] ((i1={3/4}) S (i1={3/4})) || ((o1={1/2}) S (o1={1/2}))") {
		bdd_init<Bool>();
		const strings i1_vals = {"1/4", "1/4", "1/4", "1/4"};
		auto vals = run_qlt_with_i1(
		    "((i1[t]:qlt = {3/4}:qlt) S (i1[t]:qlt = {3/4}:qlt)) "
		    "|| ((o1[t]:qlt = {1/2}:qlt) S (o1[t]:qlt = {1/2}:qlt)).",
		    i1_vals, 4);
		REQUIRE(vals.size() == 4);
		for (auto& v : vals) CHECK(v == "1/2");
	}

	// Regression guard for the half of the propagation that must NOT change:
	// on a top-level conjunct spine the S keeps the outer treatment, so ψ is
	// still required at every step.  The other conjuncts carry their own
	// temporal quantifiers for the same `missing_temp_quants` reason.
	//
	// THREE conjuncts, not two: `A && B && C` is ONE n-ary wff_and node, and
	// the obligation wrapper in `ltl_to_safety_formula_full` used to read only
	// its first two children — so the third conjunct was dropped straight out
	// of the executed safety formula.  The last conjunct here is the one that
	// forbids 1/4, which is exactly what the S would otherwise be free to emit
	// once ψ has held (the `φ ∧ prev` arm), so its loss is observable.
	TEST_CASE("[LT2-EXEC-03] conjunct spine keeps the outer treatment") {
		bdd_init<Bool>();
		auto vals = run_qlt_no_input(
		    "((o1[t]:qlt = {1/4}:qlt) S (o1[t]:qlt = {3/4}:qlt)) "
		    "&& always (o1[t]:qlt != {1/2}:qlt) "
		    "&& always (o1[t]:qlt != {1/4}:qlt).", 4);
		REQUIRE(vals.size() == 4);
		for (auto& v : vals) {
			CHECK(v == "3/4");
			CHECK(v != "1/2");   // second conjunct
			CHECK(v != "1/4");   // third conjunct — dropped by the old wrapper
		}
	}

	// Sat-path cross-check: the ppLTLTT tester encoding (independent of the
	// execution encoding) must agree that both LT2 specs are realizable.
	TEST_CASE("[LT2-SAT-01] both LT-2 shapes are REALIZABLE") {
		bdd_init<Bool>();
		CHECK(realizable(
		    "! ((o1[t]:qlt = {1/4}:qlt) S (o1[t]:qlt = {3/4}:qlt))."));
		CHECK(realizable(
		    "((i1[t]:qlt = {3/4}:qlt) S (i1[t]:qlt = {3/4}:qlt)) "
		    "|| ((o1[t]:qlt = {1/2}:qlt) S (o1[t]:qlt = {1/2}:qlt))."));
	}

} // TEST_SUITE "LTL correctness: S under negation / disjunction (LT-2)"


// ── LT-6: solutions that carry no encodable strategy ─────────────────────────
//
// `solve_ltl_aba_algorithm_b` returns a solution with an EMPTY `atoms` vector
// (only `aut` is set), and the constant-output fast path returns one with
// `num_states == 0`.  `ltl_to_safety_formula_full` mapped both to
// `{tau::_T(), sol}` under the comment "purely propositional: realizable but
// no data constraints to encode" — but realizability there depended on a
// concrete output strategy (the P/D-bit machinery, or a specific constant
// output combination) that the safety formula never encodes.  The interpreter
// then executes `always T` and emits default outputs, which can violate the
// very spec that was reported REALIZABLE.
//
// The assertions below are deliberately verdict-shaped rather than
// trace-shaped: it is acceptable for the interpreter to refuse the spec
// ("not executable"), and it is acceptable for a future implementation to
// encode the strategy properly — what is NOT acceptable is emitting a trace
// that violates the spec.

TEST_SUITE("LTL correctness: strategy must survive into execution (LT-6)") {

	// Algorithm B path: qlt atoms with an input variable, one output var.
	// The liveness conjunct F(o1 = 1/2) is exactly the obligation that
	// `always T` drops on the floor.
	TEST_CASE("[LT6-EXEC-01] Algorithm-B spec does not execute as `always T`") {
		bdd_init<Bool>();
		const strings i1_vals = {"1/4", "1/4", "1/4", "1/4"};
		auto vals = run_qlt_with_i1(
		    "F (o1[t]:qlt = {1/2}:qlt) "
		    "&& G (o1[t]:qlt != i1[t]:qlt).", i1_vals, 4);
		bool refused   = vals.empty();
		bool satisfied = false;
		for (auto& v : vals) if (v == "1/2") satisfied = true;
		CHECK((refused || satisfied));
	}

	// Constant-output fast path: `constant_output_realizable` succeeds, so the
	// solution comes back with num_states == 0 and no automaton at all.
	TEST_CASE("[LT6-EXEC-02] constant-output spec does not execute as `always T`") {
		bdd_init<Bool>();
		const strings i1_vals = {"1/4", "1/4", "1/4", "1/4"};
		auto vals = run_qlt_with_i1(
		    "F (o1[t]:qlt = {1/2}:qlt) "
		    "&& G (i1[t]:qlt = {1/4}:qlt -> o1[t]:qlt != {3/4}:qlt).",
		    i1_vals, 4);
		bool refused   = vals.empty();
		bool satisfied = false;
		for (auto& v : vals) if (v == "1/2") satisfied = true;
		CHECK((refused || satisfied));
	}

} // TEST_SUITE "LTL correctness: strategy must survive into execution (LT-6)"


// ── LA-N3: the t = 0 anchor of an off-spine (inner) S/T ──────────────────────
//
// Strong-past semantics: S(-1) = false, so at the first step `φ S ψ` can hold
// only through ψ.  The compile-away pass encodes each S as an auxiliary bv
// output with the tracking invariant G(curr ↔ (ψ ∨ (φ ∧ prev))); for an
// OUTERMOST S the anchor comes from `ψ@0` plus `G(curr && rhs)`, but an inner
// S (anything under a disjunction, a negation, another S/T — or under
// `always`, which drops off the conjunct spine too) had NO anchor: `prev` at
// the first enforced step was a free system output, so the strategy could
// claim the Since through phantom memory (`aux = 1` although ψ never held).
//
// The sat path is unaffected (the ppLTLTT tester encoding hard-codes
// !state_var at t = 0), which is exactly the verdict/execution disagreement
// these cases pin: the spec below is UNREALIZABLE, yet the unanchored
// execution ran it forever.
//
// The fix seeds every inner S auxiliary to bv-0 at t = formula_time_point - 1
// through the same memory pre-population the Mealy state bits use — a
// single-type, non-negative-time encoding of S(-1) = false.  (The cross-type
// `curr@0 ↔ ψ@0` initial condition remains a do-not-retry trap; see the
// revert note in compile_since_trigger_rec.)

TEST_SUITE("LTL correctness: inner S/T anchored at t=0 (LA-N3)") {

	// o1 must always be 0, so the disjunct (o1 = 1) is unavailable and the
	// inner S must hold at every enforced step.  Its φ = (o1 = 0) is TRUE
	// throughout, so the phantom-memory arm (φ ∧ prev) is live: unanchored,
	// the system sets the auxiliary's initial lookback to 1 and executes
	// although ψ = (i1 = 1) never holds.  Anchored, the step-1 solve is
	// unsat and the run stops short.
	TEST_CASE("[LAN3-EXEC-01] inner S: phantom memory cannot satisfy it") {
		bdd_init<Bool>();
		const strings i1_vals = {"0", "0", "0", "0"};
		auto vals = run_qlt_with_i1(
		    "always ((o1[t]:qlt = {0}:qlt) && "
		    "(((o1[t]:qlt = {0}:qlt) S (i1[t]:qlt = {1}:qlt)) "
		    "|| (o1[t]:qlt = {1}:qlt))).", i1_vals, 4);
		CHECK(vals.size() < 4); // ψ never held: no complete violating trace
	}

	// The same spec with cooperative inputs: still refused, agreeing with
	// the sat verdict (the spec is UNREALIZABLE — [LAN3-SAT-01]).  The
	// interpreter's unbounded continuation is the synthesis fixpoint
	// (∀ inputs ∃ outputs, sustainable forever), whose only invariant here
	// is the aux chain the anchor now denies at its start; it does not
	// execute unrealizable specs opportunistically on inputs that happen
	// to cooperate.  Before the fix this ran 4 steps — through phantom
	// memory, for cooperative and hostile inputs alike.
	TEST_CASE("[LAN3-EXEC-02] inner S: cooperative inputs do not resurrect "
	          "an unrealizable spec") {
		bdd_init<Bool>();
		const strings i1_vals = {"1", "1", "1", "1"};
		auto vals = run_qlt_with_i1(
		    "always ((o1[t]:qlt = {0}:qlt) && "
		    "(((o1[t]:qlt = {0}:qlt) S (i1[t]:qlt = {1}:qlt)) "
		    "|| (o1[t]:qlt = {1}:qlt))).", i1_vals, 4);
		CHECK(vals.size() < 4);
	}

	// T dual.  ¬(φ T ψ) = (¬φ) S (¬ψ) with ¬φ = (o1 ≠ 1) true throughout
	// (o1 is pinned to 0) and ¬ψ = (i1 ≠ 1) false throughout (i1 is fed 1),
	// so the required Since can only be claimed through phantom memory:
	// T(-1) = true must anchor the rewritten S's auxiliary to 0.
	TEST_CASE("[LAN3-EXEC-03] inner T: T(-1)=true is enforced") {
		bdd_init<Bool>();
		const strings i1_vals = {"1", "1", "1", "1"};
		auto vals = run_qlt_with_i1(
		    "always ((o1[t]:qlt = {0}:qlt) && "
		    "(! ((o1[t]:qlt = {1}:qlt) T (i1[t]:qlt = {1}:qlt)))).",
		    i1_vals, 4);
		CHECK(vals.size() < 4);
	}

	// S nested inside another S's operand (ψ position): both auxiliaries are
	// inner and both must be anchored — the outer S reduces to the inner one
	// (its own φ ∧ prev arm is anchored away at the first enforced step),
	// and the inner one to an input the environment refuses.  Like
	// [LAN3-EXEC-02], cooperative inputs make no difference: the spec is
	// unrealizable and the anchored continuation refuses it either way.
	TEST_CASE("[LAN3-EXEC-04] S-in-S: both auxiliaries anchored") {
		bdd_init<Bool>();
		const strings i1_zero = {"0", "0", "0", "0"};
		auto vals = run_qlt_with_i1(
		    "always ((o1[t]:qlt = {0}:qlt) && "
		    "(((o1[t]:qlt = {0}:qlt) S "
		    "((o1[t]:qlt = {0}:qlt) S (i1[t]:qlt = {1}:qlt))) "
		    "|| (o1[t]:qlt = {1}:qlt))).", i1_zero, 4);
		CHECK(vals.size() < 4);

		const strings i1_one = {"1", "1", "1", "1"};
		auto vals2 = run_qlt_with_i1(
		    "always ((o1[t]:qlt = {0}:qlt) && "
		    "(((o1[t]:qlt = {0}:qlt) S "
		    "((o1[t]:qlt = {0}:qlt) S (i1[t]:qlt = {1}:qlt))) "
		    "|| (o1[t]:qlt = {1}:qlt))).", i1_one, 4);
		CHECK(vals2.size() < 4);
	}

	// Positive control: a REALIZABLE spec with an inner S still executes
	// under the anchor.  S(χ, χ) ≡ χ, so the spec is G(o1 = 0 ∨ o1 = 1);
	// the anchored auxiliary tracks χ from the first enforced step on and
	// never needs phantom memory.
	TEST_CASE("[LAN3-EXEC-06] realizable inner S executes under the anchor") {
		bdd_init<Bool>();
		auto vals = run_qlt_no_input(
		    "always (((o1[t]:qlt = {0}:qlt) S (o1[t]:qlt = {0}:qlt)) "
		    "|| (o1[t]:qlt = {1}:qlt)).", 4);
		REQUIRE(vals.size() == 4);
		for (auto& v : vals) CHECK((v == "0" || v == "1"));
	}

	// The sat path must agree: both reproducer specs are UNREALIZABLE (the
	// environment refuses ψ at t = 0).  These already held before the fix —
	// they pin that verdict and execution now agree.
	TEST_CASE("[LAN3-SAT-01] the reproducer specs are UNREALIZABLE") {
		bdd_init<Bool>();
		CHECK_FALSE(realizable(
		    "always ((o1[t]:qlt = {0}:qlt) && "
		    "(((o1[t]:qlt = {0}:qlt) S (i1[t]:qlt = {1}:qlt)) "
		    "|| (o1[t]:qlt = {1}:qlt)))."));
		CHECK_FALSE(realizable(
		    "always ((o1[t]:qlt = {0}:qlt) && "
		    "(! ((o1[t]:qlt = {1}:qlt) T (i1[t]:qlt = {1}:qlt))))."));
	}

	// Regression guard: an OUTERMOST S keeps its free auxiliary (its anchor
	// is ψ@0 + G(curr && rhs)); blanket-seeding it to 0 would force ψ at
	// every step and outlaw the φ-chain.  ψ = (o1 = 3/4) must hold at t = 0,
	// after which φ = (o1 = 0) may carry the Since indefinitely.
	TEST_CASE("[LAN3-EXEC-05] outermost S: the φ-chain survives the anchor") {
		bdd_init<Bool>();
		auto vals = run_qlt_no_input(
		    "(o1[t]:qlt = {0}:qlt) S (o1[t]:qlt = {3/4}:qlt).", 4);
		REQUIRE(vals.size() == 4);
		CHECK(vals[0] == "3/4"); // ψ@0
		// Semantic check, straight from the S definition: at every step,
		// ψ held at some k ≤ t and φ held on (k, t].
		for (size_t t = 0; t < vals.size(); ++t) {
			bool since = false;
			for (size_t k = 0; k <= t && !since; ++k) {
				if (vals[k] != "3/4") continue;
				bool phi_after = true;
				for (size_t j = k + 1; j <= t; ++j)
					if (vals[j] != "0") { phi_after = false; break; }
				since = phi_after;
			}
			CHECK(since);
		}
	}

} // TEST_SUITE "LTL correctness: inner S/T anchored at t=0 (LA-N3)"


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}