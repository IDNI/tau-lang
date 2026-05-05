// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Literature LTL synthesis benchmarks (Q40-TB1).
//
// Classical LTL synthesis patterns from the synthesis literature, scaled to
// tau-lang's capabilities (2-3 variables, propositional skeleton).  These are
// named for recognizability — each corresponds to a pattern widely cited in
// Pnueli & Rosner / Kupferman & Vardi / LTLsynt competition entries.
//
// All use `:sbf` propositional variables (two-valued Boolean) to avoid the
// atomless-BA semantic gap that complicates tau-type cross-validation.
// Variables use the io_var naming: iN = input, oN = output.
//
// Skip with TAU_LTL_SKIP_BENCHMARKS=1 for fast test runs.

#include "test_init.h"
#include "test_tau_helpers.h"
#include <cstdlib>

using namespace idni::tau_lang;

static bool skip_benchmarks() {
	const char* s = std::getenv("TAU_LTL_SKIP_BENCHMARKS");
	return s && *s && std::string(s) != "0";
}

static bool realizable(const char* s) {
	auto nso = get_nso_rr<node_t>(tau::get(s));
	if (!nso.has_value()) return false;
	tref fm = nso.value().main->get();
	if (!fm) return false;
	return is_tau_formula_sat<node_t>(fm);
}

TEST_SUITE("LTL literature benchmarks") {

	TEST_CASE("LB-01 request-grant: G(i1 -> F o1)") {
		// Classical pattern: every request is eventually granted.
		// Simple strategy: always grant (o1 := 1).  REALIZABLE.
		if (skip_benchmarks()) return;
		CHECK(realizable(
			"G ((i1[t] = 1) -> F ((o1[t] = 1)))."
		));
	}

	TEST_CASE("LB-02 safety echo: G(o1 = i1)") {
		// System must echo input — REALIZABLE (strategy: copy i1 to o1).
		if (skip_benchmarks()) return;
		CHECK(realizable(
			"G ((o1[t] = 1) <-> (i1[t] = 1))."
		));
	}

	TEST_CASE("LB-03 reactive response: G(i1 -> X o1)") {
		// Next-step response — tau expresses X via time index [t+1] but that is
		// a forward reference; use lookback instead (equivalent form).
		// G(i1[t-1] -> o1[t]) : whenever input was high last step, output now.
		// REALIZABLE: strategy copies previous input to current output.
		if (skip_benchmarks()) return;
		CHECK(realizable(
			"G ((i1[t-1] = 1) -> (o1[t] = 1))."
		));
	}

	TEST_CASE("LB-04 always-grant impossibility: G o1 && F !o1") {
		// Classic contradiction — cannot always o1=1 AND eventually o1=0.
		// UNREALIZABLE.
		if (skip_benchmarks()) return;
		CHECK_FALSE(realizable(
			"(G ((o1[t] = 1))) && (F ((o1[t] = 0)))."
		));
	}

	TEST_CASE("LB-05 fairness (G-wrapped): G(i1 -> F o1)") {
		// Tau requires the top-level to be temporal-scoped, so the classical
		// `(GF i) -> (GF o)` is expressed as the stronger request-grant
		// pattern wrapped in G.  REALIZABLE: always grant.
		if (skip_benchmarks()) return;
		CHECK(realizable(
			"G ((i1[t] = 1) -> (F ((o1[t] = 1))))."
		));
	}

	TEST_CASE("LB-06 mutual exclusion: G!(o1 && o2)") {
		// Two-process mutex: never both outputs high.  Strategy: always o1=0,o2=0.
		// REALIZABLE.
		if (skip_benchmarks()) return;
		CHECK(realizable(
			"G (!((o1[t] = 1) && (o2[t] = 1)))."
		));
	}

	TEST_CASE("LB-07 arbiter: G(i1 -> F o1) && G(i2 -> F o2) && G!(o1 && o2)") {
		// Two-client arbiter with mutex.  Each client served eventually;
		// outputs never simultaneously granted.
		// REALIZABLE via round-robin.
		if (skip_benchmarks()) return;
		CHECK(realizable(
			"(G ((i1[t] = 1) -> F ((o1[t] = 1))))"
			" && (G ((i2[t] = 1) -> F ((o2[t] = 1))))"
			" && (G (!((o1[t] = 1) && (o2[t] = 1))))."
		));
	}

	TEST_CASE("LB-08 weak-until: (o1 = 1) W (i1 = 1)") {
		// Weak until: o1 = 1 until i1 happens, or o1 = 1 forever if i1 never does.
		// REALIZABLE: strategy outputs 1 forever regardless of i1.
		// (Strong U would be UNREAL — env can withhold i1 = 1 indefinitely.)
		if (skip_benchmarks()) return;
		CHECK(realizable(
			"((o1[t] = 1) W (i1[t] = 1))."
		));
	}

	TEST_CASE("LB-09 persistence: F G o1") {
		// Eventually always o1 = 1.  REALIZABLE: set o1=1 from t=0 onwards.
		if (skip_benchmarks()) return;
		CHECK(realizable(
			"F (G ((o1[t] = 1)))."
		));
	}

	TEST_CASE("LB-10 priority arbiter: G((i1 -> o1) && ((o1) -> (i1 || i2)))") {
		// Priority arbiter: client 1 always served when requesting; o1 only
		// fires when some request is active.  REALIZABLE via o1 := i1.
		if (skip_benchmarks()) return;
		CHECK(realizable(
			"G (((i1[t] = 1) -> (o1[t] = 1))"
			"  && ((o1[t] = 1) -> ((i1[t] = 1) || (i2[t] = 1))))."
		));
	}
}
