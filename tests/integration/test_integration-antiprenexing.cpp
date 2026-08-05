// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("anti_prenex") {
	TEST_CASE("1") {
		const char* sample = "ex v { o1[t]o2[t] = 0 } : tau ({ o1[t]o2[t] = 0 } : tau v)' = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK(tau::get(res).equals_T());
	}
	TEST_CASE("2") {
		const char* sample = "all o1[0], o2[0] !o1[0]o2[0] = 0 || o1[0]o2[0] = 0 && (ex o2[1], o1[1] o1[1]o2[1] = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK(tau::get(res).equals_T());
	}
	TEST_CASE("3") {
		const char* sample = "all y !({ <:a><:d><:c>|<:a>'<:d><:c>|<:b> = 0 } : tau y != 0 ) || { <:a><:b>|<:c><:d> = 0 } : tau y != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK(tau::get(res).equals_T());
	}
	TEST_CASE("4") {
		const char* sample = "{!(always <:a>&(<:b>|<:d><:c>)|<:a>'<:d><:c> = 0)}&{always <:b>|<:d><:c> = 0} != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	// The next three cases check that squeeze/absorb runs on the scoped
	// formula of each quantifier inside inner_quant (B4): the redundant
	// xyz = 0 conjunct must be absorbed into xy = 0 before elimination,
	// so it cannot survive in the kept clause around the unresolved f
	TEST_CASE("b4 squeeze_absorb below ex") {
		const char* sample = "ex x (((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"w = 0 || (ex b1 b1 w = 0 && b1 y = 0 && f(b1))",
			"w = 0 || (ex b1 b1 y = 0 && b1 w = 0 && f(b1))",
			"(ex b1 b1 w = 0 && b1 y = 0 && f(b1)) || w = 0",
			"(ex b1 b1 y = 0 && b1 w = 0 && f(b1)) || w = 0",
		}) );
	}
	TEST_CASE("b4 squeeze_absorb below all") {
		const char* sample = "all x !((((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0)).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"w != 0 && (all b1 b1 w != 0 || b1 y != 0 || !f(b1))",
			"w != 0 && (all b1 b1 y != 0 || b1 w != 0 || !f(b1))",
			"(all b1 b1 w != 0 || b1 y != 0 || !f(b1)) && w != 0",
			"(all b1 b1 y != 0 || b1 w != 0 || !f(b1)) && w != 0",
		}) );
	}
	TEST_CASE("b4 squeeze_absorb below all, fully eliminated") {
		// equivalence guard: same scope under a plain all resolves
		// completely, with and without the squeeze
		const char* sample = "all x (((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"y = 0 && w = 0",
			"w = 0 && y = 0",
		}) );
	}

	// Test to see the blow up caused by quantified free function symbols
	// In particular conversion to Boole normal form causes blow up
	// TEST_CASE("5") {
	// 	const char* sample = "all x ex y (f1(x,y)=0 && g1(x,y)!=0 && h1(x,y)!=0) || (f2(x,y)=0 && g2(x,y)!=0 && h2(x,y)!=0).";
	// 	tref fm = get_nso_rr(sample).value().main->get();
	// 	tref res = anti_prenex<node_t>(fm);
	// 	std::cout << "res: " << tau::get(res) << "\n";
	// 	res = boole_normal_form<node_t>(res);
	// 	std::cout << "res: " << tau::get(res) << "\n";
	// 	CHECK(true);
	// }
}

TEST_SUITE("AntiPrenexBlockPipeline") {
	// These tests exercise anti_prenex_block through the full normalize
	// pipeline (normalize_non_temp). Single-level quantifiers:
	TEST_CASE("ex_all_single: ex x all y xy=0 → T") {
		// ∃x. ∀y. xy=0 : pick x=0, then 0·y=0 for all y
		CHECK( normalize_and_check("ex x all y xy = 0.", tau::wff_t) );
	}
	TEST_CASE("all_ex_single: all x ex y xy=0 → T") {
		// ∀x. ∃y. xy=0 : pick y=0, then x·0=0 for any x
		CHECK( normalize_and_check("all x ex y xy = 0.", tau::wff_t) );
	}
	TEST_CASE("ex_all_neg: ex x all y xy!=0 → F") {
		// ∃x. ∀y. xy≠0 : for any x, pick y=0 → x·0=0, contradiction
		CHECK( normalize_and_check("ex x all y xy != 0.", tau::wff_f) );
	}
	TEST_CASE("all_ex_neg: all x ex y xy!=0 → F") {
		// ∀x. ∃y. xy≠0 : for x=0, need y s.t. 0≠0, impossible
		CHECK( normalize_and_check("all x ex y xy != 0.", tau::wff_f) );
	}
	// Two-level quantifier alternation:
	TEST_CASE("all_ex_all: all x ex y all z xyz=0 → T") {
		// pick y=0: x·0·z=0 for all x,z
		CHECK( normalize_and_check("all x ex y all z xyz = 0.", tau::wff_t) );
	}
	TEST_CASE("ex_all_ex: ex x all y ex z xyz!=0 → F") {
		// for x=0: 0·y·z=0 for all y,z, so can never be ≠0
		CHECK( normalize_and_check("ex x all y ex z xyz != 0.", tau::wff_f) );
	}
	// Mixed with disjunction/conjunction (exercises B11/B12/B13):
	TEST_CASE("ex_all_disjunction: ex x all y (xy=0 || x'y=0) → T") {
		// ∃x=0: 0·y=0 for all y ✓ (disjunct 1 satisfied)
		CHECK( normalize_and_check("ex x all y (xy = 0 || x'y = 0).", tau::wff_t) );
	}
	TEST_CASE("all_ex_conjunction: all x ex y (xy=0 && x'y=0) → T") {
		// pick y=0: x·0=0 and x'·0=0 for any x ✓
		CHECK( normalize_and_check("all x ex y (xy = 0 && x'y = 0).", tau::wff_t) );
	}
	// subs_elim path: ex x (x=t && phi(x)) → phi(t)
	TEST_CASE("subs_elim: ex x (xy=0 && x=w) → wy=0") {
		// Step 2 of anti_prenex_block substitutes x:=w giving wy=0.
		CHECK( normalize_and_check("ex x (xy = 0 && x = w).",
			strings{"yw = 0", "wy = 0"}) );
	}
	TEST_CASE("subs_elim: ex x (x=w) → T") {
		// After substitution the body reduces to T.
		CHECK( normalize_and_check("ex x (x = w).", tau::wff_t) );
	}
	// trivial_skolem path: an eligible atom under wff_or is unreachable by
	// subs_elim (which bails on any wff_or in scope), so only the
	// trivial_skolem wiring in process_quantifier_block can remove x here.
	TEST_CASE("trivial_skolem: ex x (x=w || z=0) → T") {
		CHECK( normalize_and_check("ex x (x = w || z = 0).", tau::wff_t) );
	}
	// Regression tests: a BV-typed atom unrelated to the (non-BV) block
	// variable must not force the whole block to fall back to anti_prenex,
	// nor be dropped/mishandled when it does get pulled into the block's
	// Boole-decomposition candidate set.
	TEST_CASE("bv atom alongside block var: ex x ((xy=0||Z=3) && xw=0) -> T") {
		// x = 0 makes both xy = 0 and xw = 0 true regardless of y, w, Z:
		// tautology, independently of how the unrelated Z:bv[8] atom is
		// handled.
		CHECK( normalize_and_check(
			"ex x ((xy = 0 || Z:bv[8] = { 3 }:bv[8]) && xw = 0).",
			tau::wff_t) );
	}
	TEST_CASE("bv atom survives block elimination: contradicts outer Z != 3") {
		// The BV constraint Z = 3 pulled inside the block must still
		// propagate out intact; ANDed with Z != 3 outside, the whole
		// formula is unsatisfiable.
		CHECK( normalize_and_check(
			"(ex x (xy = 0 && xw = 0 && Z:bv[8] = { 3 }:bv[8])) "
			"&& Z:bv[8] != { 3 }:bv[8].",
			tau::wff_f) );
	}
}
