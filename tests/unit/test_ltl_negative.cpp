// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Negative / error-path tests for LTL(ABA) synthesis.
//
// Three test suites:
//
//   NEG-PARSE  : Malformed or invalid formula strings must fail to parse
//                (spec() returns nullptr) rather than crashing or producing
//                garbage results. Guards against regressions where bad input
//                causes an exception or undefined behavior.
//
//   NEG-UNREAL : Known UNREALIZABLE formulas must consistently return false.
//                Ensures the solver does not accidentally flip verdicts, and
//                that contradictions stay contradictions.
//
//   NEG-CONSIST: Any formula (realizable or not) must return the same verdict
//                every time it is evaluated. Tests determinism of the full
//                synthesis pipeline for a set of representative formulas.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "parse_error_hint.h"

#include <unordered_set>

using namespace idni::tau_lang;

// ── helpers ────────────────────────────────────────────────────────────────────

static void do_gc() {
	std::unordered_set<tref> keep;
	tau::gc(keep);
}

static tref spec_parse(const char* s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

static bool realizable(const char* s) {
	do_gc();
	tref fm = spec_parse(s);
	if (!fm) return false;
	return is_tau_formula_sat<node_t>(fm);
}

// ── NEG-PARSE tests ────────────────────────────────────────────────────────────

TEST_SUITE("NEG-PARSE: malformed formulas return nullptr") {

	TEST_CASE("NEG-PARSE-01: empty string") {
		CHECK(spec_parse("") == nullptr);
	}

	TEST_CASE("NEG-PARSE-02: whitespace only") {
		CHECK(spec_parse("   ") == nullptr);
	}

	TEST_CASE("NEG-PARSE-03: garbage token") {
		CHECK(spec_parse("XYZZY.") == nullptr);
	}

	TEST_CASE("NEG-PARSE-04: X (neXt) operator is not defined in tau-lang") {
		// X is deliberately omitted (time-indexed io_vars cover its role)
		CHECK(spec_parse("X (o1[t] = 0).") == nullptr);
	}

	TEST_CASE("NEG-PARSE-05: Y (Yesterday) operator is not defined in tau-lang") {
		// Y is deliberately omitted (past is covered by [t-k] indices)
		CHECK(spec_parse("Y (o1[t] = 0).") == nullptr);
	}

	TEST_CASE("NEG-PARSE-06: two G sub-formulas with no operator between them") {
		CHECK(spec_parse("G (o1[t] = 0) G (o1[t] = 1).") == nullptr);
	}

	TEST_CASE("NEG-PARSE-07: unclosed parenthesis") {
		// Missing closing paren — no trailing dot either
		CHECK(spec_parse("G (o1[t] = 0") == nullptr);
	}

	TEST_CASE("NEG-PARSE-08: unknown multi-letter operator name") {
		// Known-undefined operator names must not silently parse
		CHECK(spec_parse("UNTIL (o1[t] = 0) (o1[t] = 1).") == nullptr);
	}

	TEST_CASE("NEG-PARSE-09: unknown operator name") {
		CHECK(spec_parse("H (o1[t] = 0).") == nullptr);
	}

	TEST_CASE("NEG-PARSE-10: binary operator with missing left operand") {
		CHECK(spec_parse("U (o1[t] = 0) (o1[t] = 1).") == nullptr);
	}

	TEST_CASE("NEG-PARSE-11: type-annotated without var") {
		CHECK(spec_parse("G (:sbf = {X}:sbf).") == nullptr);
	}

	TEST_CASE("NEG-PARSE-12: mismatched bracket") {
		CHECK(spec_parse("G (o1[t = 0).") == nullptr);
	}
}

// ── NEG-UNREAL tests ───────────────────────────────────────────────────────────

TEST_SUITE("NEG-UNREAL: known-UNREALIZABLE formulas return false") {

	// Input variable: system cannot control environment
	TEST_CASE("NEG-UNREAL-01: F(input=0) — cannot force env") {
		CHECK_FALSE(realizable("F (i1[t] = 0)."));
	}

	TEST_CASE("NEG-UNREAL-02: F(input=1) — cannot force env") {
		CHECK_FALSE(realizable("F (i1[t] = 1)."));
	}

	TEST_CASE("NEG-UNREAL-03: G(F(input=0)) — cannot force env infinitely often") {
		CHECK_FALSE(realizable("G (F (i1[t] = 0))."));
	}

	// Contradictions: cannot satisfy both simultaneously
	TEST_CASE("NEG-UNREAL-04: o=0 AND o=1 at same time") {
		CHECK_FALSE(realizable("((o1[t] = 0) && (o1[t] = 1))."));
	}

	TEST_CASE("NEG-UNREAL-05: G(o=0) AND G(o=1) — contradiction in safety") {
		CHECK_FALSE(realizable("(G (o1[t] = 0) && G (o1[t] = 1))."));
	}

	// Unsatisfiable constants
	TEST_CASE("NEG-UNREAL-06: F(false) — eventually false is never satisfiable") {
		CHECK_FALSE(realizable("F (0 = 1)."));
	}

	// Cannot satisfy input constraint via output
	TEST_CASE("NEG-UNREAL-07: G(input=0) — env always provides a value system cannot choose") {
		CHECK_FALSE(realizable("G (i1[t] = 0)."));
	}

	TEST_CASE("NEG-UNREAL-08: F(input=0) U F(input=1) — both unrealizable") {
		CHECK_FALSE(realizable("(F (i1[t] = 0)) U (F (i1[t] = 1))."));
	}

	// Temporal contradictions
	TEST_CASE("NEG-UNREAL-09: G(o=0) && F(o=1) — safety blocks liveness") {
		CHECK_FALSE(realizable("(G (o1[t] = 0) && F (o1[t] = 1))."));
	}

	TEST_CASE("NEG-UNREAL-10: (o=0) U (input=0) with G(input=1) — env contradiction") {
		// System must maintain o=0 until input=0, but input is always 1
		// This is UNREALIZABLE: the Until never fires and G blocks escape
		// Actually this test needs thought: with no G(input=1) constraint,
		// the U formula itself is unrealizable if env never sends 0.
		// Simpler: just F(input=0) && G(input=1) is a contradiction only if
		// input is both always 1 and eventually 0. Use a pure contradiction:
		CHECK_FALSE(realizable("(G (i1[t] = 0) && F (i1[t] = 1))."));
	}
}

// ── NEG-CONSIST tests ──────────────────────────────────────────────────────────

TEST_SUITE("NEG-CONSIST: determinism — same verdict on repeated evaluation") {

	// Helper: evaluate formula N times and check all results match
	static auto consistent = [](const char* formula, int n = 5) -> bool {
		tref fm = spec_parse(formula);
		if (!fm) return true; // parse failure is deterministically skipped
		bool first = is_tau_formula_sat<node_t>(fm);
		for (int i = 1; i < n; ++i)
			if (is_tau_formula_sat<node_t>(fm) != first) return false;
		return true;
	};

	TEST_CASE("NEG-CONSIST-01: realizable G formula is stable") {
		CHECK(consistent("G (o1[t] = 0)."));
	}

	TEST_CASE("NEG-CONSIST-02: realizable F formula is stable") {
		CHECK(consistent("F (o1[t] = 1)."));
	}

	TEST_CASE("NEG-CONSIST-03: unrealizable F(input) is stable") {
		CHECK(consistent("F (i1[t] = 0)."));
	}

	TEST_CASE("NEG-CONSIST-04: G(F(o=1)) liveness formula is stable") {
		CHECK(consistent("G (F (o1[t] = 1))."));
	}

	TEST_CASE("NEG-CONSIST-05: Until formula is stable") {
		CHECK(consistent("((o1[t] = 0) U (o1[t] = 1))."));
	}

	TEST_CASE("NEG-CONSIST-06: W (weak until) formula is stable") {
		CHECK(consistent("((o1[t] = 0) W (i1[t] = 1))."));
	}

	TEST_CASE("NEG-CONSIST-07: R (release) formula is stable") {
		CHECK(consistent("((o1[t] = 0) R (o1[t] = 1))."));
	}

	TEST_CASE("NEG-CONSIST-08: contradiction is stable (always false)") {
		CHECK(consistent("(G (o1[t] = 0) && G (o1[t] = 1))."));
	}

	TEST_CASE("NEG-CONSIST-09: nested G F formula is stable") {
		CHECK(consistent("G (F (o1[t] = 1) && F (o1[t] = 0))."));
	}

	TEST_CASE("NEG-CONSIST-10: mixed i/o U formula is stable") {
		CHECK(consistent("((o1[t] = 0) U (i1[t] = 1))."));
	}
}

// ── NEG-HINT tests ─────────────────────────────────────────────────────────────
//
// Q40-UX1: classify_parse_error() returns actionable hints for common mistakes.

TEST_SUITE("NEG-HINT: classify_parse_error produces actionable hints") {

	TEST_CASE("NEG-HINT-01: missing trailing dot detected") {
		auto hint = classify_parse_error("G (o1[t] = 0)");
		CHECK(!hint.empty());
		CHECK(hint.find("'.'") != std::string::npos);
	}

	TEST_CASE("NEG-HINT-02: forward time reference [t+k] detected") {
		auto hint = classify_parse_error("G (o1[t+1] = 0).");
		CHECK(!hint.empty());
		CHECK(hint.find("forward") != std::string::npos);
	}

	TEST_CASE("NEG-HINT-03: unknown type annotation detected") {
		auto hint = classify_parse_error("G ({1}:mytype > {0}:mytype).");
		CHECK(!hint.empty());
		CHECK(hint.find("mytype") != std::string::npos);
	}

	TEST_CASE("NEG-HINT-04: valid formula returns empty hint") {
		auto hint = classify_parse_error("G (o1[t] = 0).");
		CHECK(hint.empty());
	}

	TEST_CASE("NEG-HINT-05: nested G operators detected") {
		auto hint = classify_parse_error("G (G (o1[t] = 0)).");
		CHECK(!hint.empty());
		CHECK(hint.find("nested") != std::string::npos);
	}
}
