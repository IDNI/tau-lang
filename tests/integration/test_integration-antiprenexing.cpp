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
	// The next three cases originally pinned the legacy algorithm's B4
	// squeeze/absorb; since its deletion they pin the block pipeline on the
	// same inputs. The expected shapes changed with the switch -- each new
	// shape was checked equivalent to its input by hand (the conservative
	// are_nso_equivalent cannot decide reference-carrying formulas) before
	// being added here, per the redesign's acceptance rule: expectations may
	// be rewritten, semantics may not. The old shapes are kept in the lists
	// deliberately -- they are equivalent too, and a future simplification
	// improvement may legitimately return to them.
	TEST_CASE("b4 squeeze_absorb below ex") {
		const char* sample = "ex x (((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			// block pipeline, 2026-08-04 (Debug's matcher requires the
			// canonical shape FIRST): carries a redundant second
			// disjunct (its two conjuncts force w = 0 and w != 0, so
			// it is F) and an unabsorbed b1 yz != 0 literal (dead
			// under b1 y = 0); verified equivalent by hand.
			"(ex b1 b1 w = 0 && b1 y = 0 && (b1 yz != 0 || w = 0 || f(b1))) "
			"|| (ex b1 b1 y = 0 && b1 w != 0 && (b1 yz != 0 || w = 0))",
			// pre-deletion shapes, equivalent; a future simplification
			// improvement may legitimately return to them.
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
			// block pipeline, 2026-08-04 (canonical shape first): the
			// dual of the ex case above -- the second conjunct is
			// identically T, and the first folds to the old shape
			// (b1 = 0 forces w != 0); verified equivalent by hand.
			"(all b1 b1 w != 0 || b1 y != 0 || b1 yz = 0 && w != 0 && !f(b1)) "
			"&& (all b1 b1 y != 0 || b1 w = 0 || b1 yz = 0 && w != 0)",
			// pre-deletion shapes, equivalent.
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
			// block pipeline, 2026-08-04 (canonical shape first):
			// under y = 0 the kept universal reduces to
			// w = 0 && (all b1 f(b1)), whose disjunction with w = 0
			// is w = 0 -- so this is y = 0 && w = 0 in a bulkier
			// spelling; verified equivalent by hand.
			"y = 0 && ((all b1 b1 yz != 0 || b1 w = 0 && f(b1)) || w = 0)",
			// pre-deletion shapes, equivalent.
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
			strings{"wy = 0", "yw = 0"}) );
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
