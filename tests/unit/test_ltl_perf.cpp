// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Performance regression tests for LTL(ABA) synthesis.
//
// Each test measures wall-clock time for a formula known to complete quickly.
// A test fails if synthesis exceeds its time bound.
//
// Skip in normal CI by setting TAU_LTL_SKIP_PERF=1.
// Run explicitly in release CI: ctest -R test_ltl_perf (no env override needed).
//
// Time bounds are conservative (5× typical observed latency on a 4-core machine).
// "fast" formulas: < 5s. "medium" formulas: < 15s.

#include "test_init.h"
#include "test_tau_helpers.h"

#include <chrono>
#include <cstdlib>
#include <string>

using namespace idni::tau_lang;
using clock_t_ = std::chrono::steady_clock;

// Returns elapsed seconds for one synthesis call.
static double timed_realizable(const char* s, bool* result = nullptr) {
	auto t0 = clock_t_::now();
	auto nso = get_nso_rr<node_t>(tau::get(s));
	bool r = false;
	if (nso.has_value()) {
		tref fm = nso.value().main->get();
		if (fm) r = is_tau_formula_sat<node_t>(fm);
	}
	auto t1 = clock_t_::now();
	if (result) *result = r;
	return std::chrono::duration<double>(t1 - t0).count();
}

// Check whether perf tests should be skipped (set TAU_LTL_SKIP_PERF=1 in CI).
static bool skip_perf() {
	const char* v = std::getenv("TAU_LTL_SKIP_PERF");
	return v && v[0] == '1';
}

TEST_SUITE("LTL perf (performance regression)") {

	// ── fast formulas (< 5s each) ──────────────────────────────────────────────

	TEST_CASE("PERF-F01: G(o=0) constant output — < 5s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable("G (o1[t] = 0).", &r);
		MESSAGE("G(o=0): " << sec << "s, result=" << r);
		CHECK(r == true);
		CHECK(sec < 5.0);
	}

	TEST_CASE("PERF-F02: F(o=1) simple liveness — < 5s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable("F (o1[t] = 1).", &r);
		MESSAGE("F(o=1): " << sec << "s, result=" << r);
		CHECK(r == true);
		CHECK(sec < 5.0);
	}

	TEST_CASE("PERF-F03: (o=0) U (o=1) simple until — < 5s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable("((o1[t] = 0)) U ((o1[t] = 1)).", &r);
		MESSAGE("(o=0) U (o=1): " << sec << "s, result=" << r);
		CHECK(r == true);
		CHECK(sec < 5.0);
	}

	TEST_CASE("PERF-F04: G(F(o=1)) recurring liveness — < 5s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable("G (F (o1[t] = 1)).", &r);
		MESSAGE("G(F(o=1)): " << sec << "s, result=" << r);
		CHECK(r == true);
		CHECK(sec < 5.0);
	}

	TEST_CASE("PERF-F05: G(i=1 -> o=1) echo — < 5s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable("G ((i1[t] = 1) -> (o1[t] = 1)).", &r);
		MESSAGE("G(i=1->o=1): " << sec << "s, result=" << r);
		CHECK(r == true);
		CHECK(sec < 5.0);
	}

	TEST_CASE("PERF-F06: G(o=0) && F(o=1) contradiction — < 5s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable("(G (o1[t] = 0)) && (F (o1[t] = 1)).", &r);
		MESSAGE("G(o=0)&&F(o=1): " << sec << "s, result=" << r);
		CHECK(r == false);
		CHECK(sec < 5.0);
	}

	TEST_CASE("PERF-F07: (o=1) R (o=0) release — < 5s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable("((o1[t] = 1)) R ((o1[t] = 0)).", &r);
		MESSAGE("(o=1) R (o=0): " << sec << "s, result=" << r);
		CHECK(r == true);
		CHECK(sec < 5.0);
	}

	TEST_CASE("PERF-F08: (o=0) W (o=1) weak until — < 5s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable("((o1[t] = 0)) W ((o1[t] = 1)).", &r);
		MESSAGE("(o=0) W (o=1): " << sec << "s, result=" << r);
		CHECK(r == true);
		CHECK(sec < 5.0);
	}

	// ── medium formulas (< 15s each) ──────────────────────────────────────────

	TEST_CASE("PERF-M01: G(F(o=i)) repeated match — < 15s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable("G (F ((o1[t] = 1) && (i1[t] = 1))).", &r);
		MESSAGE("G(F(o=1&&i=1)): " << sec << "s, result=" << r);
		// UNREALIZABLE: system cannot force i=1
		CHECK(r == false);
		CHECK(sec < 15.0);
	}

	TEST_CASE("PERF-M02: G(F(o=0)) && G(F(o=1)) oscillation — < 15s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable("(G (F (o1[t] = 0))) && (G (F (o1[t] = 1))).", &r);
		MESSAGE("G(F(o=0))&&G(F(o=1)): " << sec << "s, result=" << r);
		CHECK(r == true);
		CHECK(sec < 15.0);
	}

	TEST_CASE("PERF-M03: G(i=1 -> F(o=1)) response — < 15s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable("G ((i1[t] = 1) -> (F (o1[t] = 1))).", &r);
		MESSAGE("G(i=1->F(o=1)): " << sec << "s, result=" << r);
		CHECK(r == true);
		CHECK(sec < 15.0);
	}

	TEST_CASE("PERF-M04: nested U formula — < 15s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable(
			"(((o1[t] = 0)) U ((o1[t] = 1))) || (G (o1[t] = 0)).", &r);
		MESSAGE("(o=0 U o=1)||G(o=0): " << sec << "s, result=" << r);
		CHECK(r == true);
		CHECK(sec < 15.0);
	}

	TEST_CASE("PERF-M05: G(F(o=0)) with input constraint — < 15s") {
		if (skip_perf()) { MESSAGE("TAU_LTL_SKIP_PERF=1 — skipped"); return; }
		bool r;
		double sec = timed_realizable(
			"G ((i1[t] = 0) -> (F (o1[t] = 0))).", &r);
		MESSAGE("G(i=0->F(o=0)): " << sec << "s, result=" << r);
		CHECK(r == true);
		CHECK(sec < 15.0);
	}
}
