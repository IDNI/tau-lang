// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Q40-XV2: cross-validation of tau's sbf data oracle against cvc5.
//
// The sbf BA represents simple Boolean functions via BDDs.  An sbf element
// is a Boolean function of a fixed variable set V.  For G-only, output-only
// LTL formulas the realizability question reduces to: does there exist an
// sbf output that satisfies every atomic equality/inequality in the spec?
// That in turn reduces to Boolean reasoning on V.
//
// We translate each sbf atom `o = expr` into a Boolean equality and ask
// cvc5 whether the conjunction is Boolean-satisfiable via an auxiliary
// truth-table encoding over explicit constants (one per V-assignment).
// This avoids UF+FORALL which cvc5 handles poorly.

#include "test_init.h"
#include "test_tau_helpers.h"
#include <cvc5/cvc5.h>

using namespace idni::tau_lang;
using namespace cvc5;

static bool realizable(const char* s) {
	auto nso = get_nso_rr<node_t>(tau::get(s));
	if (!nso.has_value()) return false;
	tref fm = nso.value().main->get();
	if (!fm) return false;
	return is_tau_formula_sat<node_t>(fm);
}

// Enumerate all 2^3 valuations of (X, Y, Z) as truth-table rows.
// For each row, evaluate a caller-supplied Boolean expression E(X,Y,Z) and
// equate it to an output bit out[row].  The "output function" is the
// 8-bit truth table over rows; ∃output with given constraints is SAT iff
// the conjunction of constraints on out[0..7] has a solution, which cvc5
// decides as pure Boolean SAT (no quantifiers, no UF).
struct TruthTableSolver {
	TermManager tm;
	Sort bs;
	std::vector<Term> out_bits; // out[row] for row = 0..7

	TruthTableSolver() : bs(tm.getBooleanSort()) {
		for (int i = 0; i < 8; ++i)
			out_bits.push_back(tm.mkConst(bs, "o_r" + std::to_string(i)));
	}
	// Build the constant Boolean corresponding to the V-expression at this row.
	static bool eval(int row, const std::vector<int>& term_tt) {
		return term_tt[row] != 0;
	}
	// Helper: returns the 8-row truth table of a Boolean expression over V.
	// fn(x,y,z) is passed as a lambda.
	template<class F>
	std::vector<int> tt(F fn) {
		std::vector<int> v(8);
		for (int r = 0; r < 8; ++r) {
			bool x = (r>>0)&1, y = (r>>1)&1, z = (r>>2)&1;
			v[r] = fn(x, y, z) ? 1 : 0;
		}
		return v;
	}
	// Constraint: output(V) == expr truth-table.
	Term output_equals(const std::vector<int>& tt) {
		std::vector<Term> cs;
		for (int r = 0; r < 8; ++r) {
			Term rhs = tt[r] ? tm.mkBoolean(true) : tm.mkBoolean(false);
			cs.push_back(tm.mkTerm(Kind::EQUAL, {out_bits[r], rhs}));
		}
		if (cs.size() == 1) return cs[0];
		return tm.mkTerm(Kind::AND, cs);
	}
	bool check(Term constraint) {
		Solver s(tm);
		s.assertFormula(constraint);
		return s.checkSat().isSat();
	}
};

TEST_SUITE("sbf oracle cross-validation (tau vs cvc5 Boolean)") {

	TEST_CASE("SBF-01: G(o = {X & Y}) — REAL/SAT") {
		bool tau_r = realizable("G (o1[t]:sbf = {X & Y}:sbf).");
		TruthTableSolver tts;
		auto tt_xy = tts.tt([](bool x, bool y, bool){ return x && y; });
		CHECK(tau_r == true);
		CHECK(tts.check(tts.output_equals(tt_xy)) == true);
	}

	TEST_CASE("SBF-02: G(o = {X & Y} && o = {X | Y}) — UNREAL/UNSAT") {
		bool tau_r = realizable(
			"G ((o1[t]:sbf = {X & Y}:sbf) && (o1[t]:sbf = {X | Y}:sbf)).");
		TruthTableSolver tts;
		auto tt_and = tts.tt([](bool x, bool y, bool){ return x && y; });
		auto tt_or  = tts.tt([](bool x, bool y, bool){ return x || y; });
		Term c = tts.tm.mkTerm(Kind::AND, {
			tts.output_equals(tt_and), tts.output_equals(tt_or)});
		CHECK(tau_r == false);
		CHECK(tts.check(c) == false);
	}

	TEST_CASE("SBF-03: G(o = {X | (Y & Z)}) — REAL/SAT") {
		bool tau_r = realizable("G (o1[t]:sbf = {X | (Y & Z)}:sbf).");
		TruthTableSolver tts;
		auto tt_e = tts.tt([](bool x, bool y, bool z){ return x || (y && z); });
		CHECK(tau_r == true);
		CHECK(tts.check(tts.output_equals(tt_e)) == true);
	}

	TEST_CASE("SBF-04: G((o={X&Y}) || (o={X|Y})) — REAL/SAT") {
		bool tau_r = realizable(
			"G ((o1[t]:sbf = {X & Y}:sbf) || (o1[t]:sbf = {X | Y}:sbf)).");
		TruthTableSolver tts;
		auto tt_and = tts.tt([](bool x, bool y, bool){ return x && y; });
		auto tt_or  = tts.tt([](bool x, bool y, bool){ return x || y; });
		Term c = tts.tm.mkTerm(Kind::OR, {
			tts.output_equals(tt_and), tts.output_equals(tt_or)});
		CHECK(tau_r == true);
		CHECK(tts.check(c) == true);
	}

	TEST_CASE("SBF-05: G((o={X&Y}) && (o={X&Y})) — REAL/SAT (same fn)") {
		// Trivial identity: same constraint twice.
		bool tau_r = realizable(
			"G ((o1[t]:sbf = {X & Y}:sbf) && (o1[t]:sbf = {X & Y}:sbf)).");
		TruthTableSolver tts;
		auto tt_and = tts.tt([](bool x, bool y, bool){ return x && y; });
		CHECK(tau_r == true);
		CHECK(tts.check(tts.output_equals(tt_and)) == true);
	}

	TEST_CASE("SBF-06: G(o != {X & Y}) — REAL/SAT (negation)") {
		// System picks any sbf ≠ X&Y; 2^8 - 1 possibilities exist.
		bool tau_r = realizable("G (!(o1[t]:sbf = {X & Y}:sbf)).");
		TruthTableSolver tts;
		auto tt_and = tts.tt([](bool x, bool y, bool){ return x && y; });
		// Constraint: NOT (output == X&Y truth-table).
		Term eq = tts.output_equals(tt_and);
		Term neq = tts.tm.mkTerm(Kind::NOT, {eq});
		CHECK(tau_r == true);
		CHECK(tts.check(neq) == true);
	}
}
