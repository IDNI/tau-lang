// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Cross-validation of tau's qlt data oracle against cvc5 LRA.
//
// The qlt BA implements DLO (dense linear order) interval arithmetic entirely
// in C++ without cvc5.  This test independently verifies it by:
//
//   1. Constructing a simple qlt LTL formula (G-only, output-only).
//   2. Running tau synthesis → REALIZABLE iff ∃o. phi(o).
//   3. Running cvc5 LRA → SAT iff ∃x:Real. phi(x).
//   4. Asserting both agree.
//
// For G-only output-only formulas, REALIZABLE = system can always output a
// constant satisfying phi = ∃o. phi(o) = cvc5 LRA satisfiability.

#include "test_init.h"
#include "test_tau_helpers.h"
#include <cvc5/cvc5.h>

using namespace idni::tau_lang;
using namespace cvc5;

// ── helpers ───────────────────────────────────────────────────────────────────

static bool realizable(const char* s) {
	auto nso = get_nso_rr<node_t>(tau::get(s));
	if (!nso.has_value()) return false;
	tref fm = nso.value().main->get();
	if (!fm) return false;
	return is_tau_formula_sat<node_t>(fm);
}

// Ask cvc5 LRA: ∃x:Real. phi(x), where phi is built by the caller.
// Returns true iff SAT.
struct LraSolver {
	TermManager tm;
	Sort real_sort;
	Term x, y; // free variables "o1", "o2"

	LraSolver()
		: real_sort(tm.getRealSort()),
		  x(tm.mkVar(real_sort, "o1")),
		  y(tm.mkVar(real_sort, "o2")) {}

	// Rational constant p/q.
	Term rat(long long p, long long q = 1) {
		return tm.mkReal(p, q);
	}

	// Check ∃x. phi(x) (single var).
	bool exists_sat(Term phi) {
		Solver s(tm);
		s.setLogic("LRA");
		Term vlist = tm.mkTerm(Kind::VARIABLE_LIST, {x});
		Term query  = tm.mkTerm(Kind::EXISTS, {vlist, phi});
		s.assertFormula(query);
		return s.checkSat().isSat();
	}

	// Check ∃x ∃y. phi(x, y).
	bool exists_sat2(Term phi) {
		Solver s(tm);
		s.setLogic("LRA");
		Term vlist = tm.mkTerm(Kind::VARIABLE_LIST, {x, y});
		Term query  = tm.mkTerm(Kind::EXISTS, {vlist, phi});
		s.assertFormula(query);
		return s.checkSat().isSat();
	}
};

// ── tests ─────────────────────────────────────────────────────────────────────

TEST_SUITE("qlt oracle cross-validation (tau vs cvc5 LRA)") {

	TEST_CASE("QO-01: open interval (1/4, 3/4) — SAT") {
		// tau: G((o1 > 1/4) && (o1 < 3/4)) → REALIZABLE (x = 1/2)
		// cvc5: ∃x. x > 1/4 ∧ x < 3/4 → SAT
		bool tau_r = realizable(
			"G ((o1[t]:qlt > {1/4}:qlt) && (o1[t]:qlt < {3/4}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(1, 4)}),
			lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(3, 4)})
		});
		bool cvc5_r = lra.exists_sat(phi);
		CHECK(tau_r == true);
		CHECK(cvc5_r == true);
		CHECK(tau_r == cvc5_r);
	}

	TEST_CASE("QO-02: inverted interval (3/4, 1/4) — UNSAT") {
		// tau: G((o1 > 3/4) && (o1 < 1/4)) → UNREALIZABLE (empty interval)
		// cvc5: ∃x. x > 3/4 ∧ x < 1/4 → UNSAT
		bool tau_r = realizable(
			"G ((o1[t]:qlt > {3/4}:qlt) && (o1[t]:qlt < {1/4}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(3, 4)}),
			lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(1, 4)})
		});
		bool cvc5_r = lra.exists_sat(phi);
		CHECK(tau_r == false);
		CHECK(cvc5_r == false);
		CHECK(tau_r == cvc5_r);
	}

	TEST_CASE("QO-03: singleton equality {1/3} — SAT") {
		bool tau_r = realizable("G ((o1[t]:qlt = {1/3}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::EQUAL, {lra.x, lra.rat(1, 3)});
		bool cvc5_r = lra.exists_sat(phi);
		CHECK(tau_r == true);
		CHECK(cvc5_r == true);
		CHECK(tau_r == cvc5_r);
	}

	TEST_CASE("QO-04: contradictory equalities {1/3} = {2/3} — UNSAT") {
		bool tau_r = realizable(
			"G ((o1[t]:qlt = {1/3}:qlt) && (o1[t]:qlt = {2/3}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::EQUAL, {lra.x, lra.rat(1, 3)}),
			lra.tm.mkTerm(Kind::EQUAL, {lra.x, lra.rat(2, 3)})
		});
		bool cvc5_r = lra.exists_sat(phi);
		CHECK(tau_r == false);
		CHECK(cvc5_r == false);
		CHECK(tau_r == cvc5_r);
	}

	TEST_CASE("QO-05: strictly positive half-line — SAT") {
		bool tau_r = realizable("G ((o1[t]:qlt > {0}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(0)});
		bool cvc5_r = lra.exists_sat(phi);
		CHECK(tau_r == true);
		CHECK(cvc5_r == true);
		CHECK(tau_r == cvc5_r);
	}

	TEST_CASE("QO-06: intersection of nested intervals — SAT") {
		// (0,1) ∩ (1/2,3/4) = (1/2,3/4) — non-empty
		bool tau_r = realizable(
			"G ((o1[t]:qlt > {0}:qlt) && (o1[t]:qlt < {1}:qlt)"
			" && (o1[t]:qlt > {1/2}:qlt) && (o1[t]:qlt < {3/4}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(0)}),
			lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(1)}),
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(1, 2)}),
			lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(3, 4)})
		});
		bool cvc5_r = lra.exists_sat(phi);
		CHECK(tau_r == true);
		CHECK(cvc5_r == true);
		CHECK(tau_r == cvc5_r);
	}

	TEST_CASE("QO-07: point exclusion makes interval empty — UNSAT") {
		// (1/3,2/3) AND o1 ≠ 1/2: still non-empty (SAT)
		// (1/2,1/2): open interval — empty (UNSAT)
		bool tau_r = realizable(
			"G ((o1[t]:qlt > {1/2}:qlt) && (o1[t]:qlt < {1/2}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(1, 2)}),
			lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(1, 2)})
		});
		bool cvc5_r = lra.exists_sat(phi);
		CHECK(tau_r == false);
		CHECK(cvc5_r == false);
		CHECK(tau_r == cvc5_r);
	}

	TEST_CASE("QO-08: negative rational constants — SAT") {
		// ∃x. x > -1 ∧ x < 0 (open interval (-1,0))
		bool tau_r = realizable(
			"G ((o1[t]:qlt > {-1}:qlt) && (o1[t]:qlt < {0}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(-1)}),
			lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(0)})
		});
		bool cvc5_r = lra.exists_sat(phi);
		CHECK(tau_r == true);
		CHECK(cvc5_r == true);
		CHECK(tau_r == cvc5_r);
	}

	TEST_CASE("QO-09: tightly bounded — SAT") {
		// ∃x. x > 99/100 ∧ x < 101/100 → SAT (x = 1)
		bool tau_r = realizable(
			"G ((o1[t]:qlt > {99/100}:qlt) && (o1[t]:qlt < {101/100}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(99, 100)}),
			lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(101, 100)})
		});
		bool cvc5_r = lra.exists_sat(phi);
		CHECK(tau_r == true);
		CHECK(cvc5_r == true);
		CHECK(tau_r == cvc5_r);
	}

	TEST_CASE("QO-10: tightly bounded — UNSAT") {
		// ∃x. x > 1 ∧ x < 1 → UNSAT (empty open interval)
		bool tau_r = realizable(
			"G ((o1[t]:qlt > {1}:qlt) && (o1[t]:qlt < {1}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(1)}),
			lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(1)})
		});
		bool cvc5_r = lra.exists_sat(phi);
		CHECK(tau_r == false);
		CHECK(cvc5_r == false);
		CHECK(tau_r == cvc5_r);
	}

	TEST_CASE("QO-11: union of intervals (x<1/4 || x>3/4) — SAT") {
		bool tau_r = realizable(
			"G ((o1[t]:qlt < {1/4}:qlt) || (o1[t]:qlt > {3/4}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::OR, {
			lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(1, 4)}),
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(3, 4)})
		});
		CHECK(tau_r == true);
		CHECK(lra.exists_sat(phi) == true);
	}

	TEST_CASE("QO-12: non-strict comparisons (x>=1/2 && x<=1/2) — SAT") {
		// Closed singleton {1/2}: satisfiable (x=1/2).
		bool tau_r = realizable(
			"G ((o1[t]:qlt >= {1/2}:qlt) && (o1[t]:qlt <= {1/2}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GEQ, {lra.x, lra.rat(1, 2)}),
			lra.tm.mkTerm(Kind::LEQ, {lra.x, lra.rat(1, 2)})
		});
		CHECK(tau_r == true);
		CHECK(lra.exists_sat(phi) == true);
	}

	TEST_CASE("QO-13: non-strict contradictory (x>=1 && x<=0) — UNSAT") {
		bool tau_r = realizable(
			"G ((o1[t]:qlt >= {1}:qlt) && (o1[t]:qlt <= {0}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GEQ, {lra.x, lra.rat(1)}),
			lra.tm.mkTerm(Kind::LEQ, {lra.x, lra.rat(0)})
		});
		CHECK(tau_r == false);
		CHECK(lra.exists_sat(phi) == false);
	}

	TEST_CASE("QO-14: two-variable independent bounds — SAT") {
		// ∃o1 ∃o2. o1 > 1/3 ∧ o2 < 2/3 : independent intervals.
		bool tau_r = realizable(
			"G ((o1[t]:qlt > {1/3}:qlt) && (o2[t]:qlt < {2/3}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(1, 3)}),
			lra.tm.mkTerm(Kind::LT, {lra.y, lra.rat(2, 3)})
		});
		CHECK(tau_r == true);
		CHECK(lra.exists_sat2(phi) == true);
	}

	TEST_CASE("QO-15: two-variable relation (o1 < o2) with bounds — SAT") {
		// ∃o1 ∃o2. o1 > 0 ∧ o2 > 1/2 ∧ o1 < 1/3 ∧ o2 < 1 — disjoint intervals
		// with o1's interval < o2's interval, so o1 < o2 is satisfiable.
		bool tau_r = realizable(
			"G ((o1[t]:qlt > {0}:qlt) && (o1[t]:qlt < {1/3}:qlt)"
			" && (o2[t]:qlt > {1/2}:qlt) && (o2[t]:qlt < {1}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(0)}),
			lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(1, 3)}),
			lra.tm.mkTerm(Kind::GT, {lra.y, lra.rat(1, 2)}),
			lra.tm.mkTerm(Kind::LT, {lra.y, lra.rat(1)})
		});
		CHECK(tau_r == true);
		CHECK(lra.exists_sat2(phi) == true);
	}

	TEST_CASE("QO-16: tautology (x<1 || x>=1) — SAT") {
		// Trivial tautology over Q.
		bool tau_r = realizable(
			"G ((o1[t]:qlt < {1}:qlt) || (o1[t]:qlt >= {1}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::OR, {
			lra.tm.mkTerm(Kind::LT,  {lra.x, lra.rat(1)}),
			lra.tm.mkTerm(Kind::GEQ, {lra.x, lra.rat(1)})
		});
		CHECK(tau_r == true);
		CHECK(lra.exists_sat(phi) == true);
	}

	TEST_CASE("QO-17: tight closed interval [1/4, 3/4] — SAT") {
		bool tau_r = realizable(
			"G ((o1[t]:qlt >= {1/4}:qlt) && (o1[t]:qlt <= {3/4}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GEQ, {lra.x, lra.rat(1, 4)}),
			lra.tm.mkTerm(Kind::LEQ, {lra.x, lra.rat(3, 4)})
		});
		CHECK(tau_r == true);
		CHECK(lra.exists_sat(phi) == true);
	}

	TEST_CASE("QO-18: large rational bounds — SAT") {
		// ∃x. x > 1234/5678 ∧ x < 9012/345.
		bool tau_r = realizable(
			"G ((o1[t]:qlt > {1234/5678}:qlt)"
			" && (o1[t]:qlt < {9012/345}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(1234, 5678)}),
			lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(9012, 345)})
		});
		CHECK(tau_r == true);
		CHECK(lra.exists_sat(phi) == true);
	}

	TEST_CASE("QO-19: three-way union with gap — SAT") {
		// ∃x. x < -1 ∨ (x > 0 ∧ x < 1) ∨ x > 2 — three disjoint intervals.
		bool tau_r = realizable(
			"G ((o1[t]:qlt < {-1}:qlt)"
			" || ((o1[t]:qlt > {0}:qlt) && (o1[t]:qlt < {1}:qlt))"
			" || (o1[t]:qlt > {2}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::OR, {
			lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(-1)}),
			lra.tm.mkTerm(Kind::AND, {
				lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(0)}),
				lra.tm.mkTerm(Kind::LT, {lra.x, lra.rat(1)})
			}),
			lra.tm.mkTerm(Kind::GT, {lra.x, lra.rat(2)})
		});
		CHECK(tau_r == true);
		CHECK(lra.exists_sat(phi) == true);
	}

	TEST_CASE("QO-20: conjunction excluding endpoints — UNSAT") {
		// ∃x. x > 1/2 ∧ x < 1/2 ∧ x >= 0 — still empty.
		bool tau_r = realizable(
			"G ((o1[t]:qlt > {1/2}:qlt)"
			" && (o1[t]:qlt < {1/2}:qlt)"
			" && (o1[t]:qlt >= {0}:qlt)).");
		LraSolver lra;
		auto phi = lra.tm.mkTerm(Kind::AND, {
			lra.tm.mkTerm(Kind::GT,  {lra.x, lra.rat(1, 2)}),
			lra.tm.mkTerm(Kind::LT,  {lra.x, lra.rat(1, 2)}),
			lra.tm.mkTerm(Kind::GEQ, {lra.x, lra.rat(0)})
		});
		CHECK(tau_r == false);
		CHECK(lra.exists_sat(phi) == false);
	}
}
