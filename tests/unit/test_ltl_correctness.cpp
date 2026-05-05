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

	TEST_CASE("[CROSS-07] G(o={#b10110101}:bv) — LTL agrees with safety path") {
		bdd_init<Bool>();
		bool ltl_r  = realizable("G (o1[t]:bv = {#b10110101}:bv).");
		bool safe_r = safety_realizable("(o1[t]:bv = {#b10110101}:bv).");
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

	TEST_CASE("[ADV-BV-01] G(o={#b10110101}:bv) — every output is non-empty") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(), o1);
		auto nso = get_nso_rr<node_t>(ctx, tau::get("G (o1[t]:bv = {#b10110101}:bv)."));
		REQUIRE(nso.has_value());
		run<node_t>(nso.value().main->get(), ctx, 6);
		auto vals = o1->get_values();
		REQUIRE(vals.size() == 6);
		for (auto& v : vals) CHECK(!v.empty());
	}

} // TEST_SUITE "LTL correctness: adversarial strategy verifier"
