// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("Normalizer") {
	TEST_CASE("1") {
		const char* sample = "all a,b,c,d a'c|b'd = 0 <-> a & b' & d | a' & c | b' & c' & d = 0.";
		CHECK( normalize_and_check(sample, tau::wff_t) );
	}
	TEST_CASE("2") {
		const char* sample = "all x all y ex z (x != y) -> (x < z && z < y).";
		CHECK( normalize_and_check(sample, tau::wff_f) );
	}
	TEST_CASE("3") {
		const char* sample = "all x all y ex z (x < y) -> (x < z && z < y).";
		CHECK( normalize_and_check(sample, tau::wff_t) );
	}
	TEST_CASE("4") {
		const char* sample = "all a all b all c all d all e all f (ax ^ bx' != cy ^ d'y' || ax ^ bx' = ey ^ fy') <-> (ax ^ bx' = ey ^ fy' || ax ^ bx' != cy ^ d'y').";
		CHECK( normalize_and_check(sample, tau::wff_t) );
	}
	TEST_CASE("5") {
		const char* sample = "all a all b all c all d all e all f (ax ^ bx' != cy ^ d'y' || ax ^ bx' = ey ^ fy') <-> (ax ^ bx' = ey ^ fy' || ax ^ bx' = cy ^ d'y').";
		CHECK( normalize_and_check(sample, tau::wff_f) );
	}
	TEST_CASE("6" * doctest::skip()) {
		// REVIEW (HIGH): Bool-type bf_lt variables are not yet supported by
		// resolve_quantifiers/resolve_quantifiers2 — this hangs indefinitely.
		// The formula should normalize to F once comparison handling is added.
		const char* sample = "all x ex y all z ex w all u ex v ((x<y && y<z) || (z<w && w<u)|| (u<v && v<x)).";
		CHECK( normalize_and_check(sample, tau::wff_f) );
	}
	TEST_CASE("7") {
		const char* sample = "ex t [t > 3].";
		CHECK( normalize_and_check(sample, tau::wff_always) );
	}
	TEST_CASE("8") {
		const char* sample = "{ !i5[t] = <:x> || o5[t] = <:y> } : tau = u[0].";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = normalize_non_temp<node_t>(fm);
		CHECK(tau::get(res).to_str() == "u[0]:tau = { always i5[t]:tau != <:x> || o5[t]:tau = <:y> }:tau");
	}
	// Block-driver tests: exercise anti_prenex_block through normalize
	TEST_CASE("ex_block_both_zero") {
		// ∃x ∃y. (x = 0 && y = 0) → T (pick x = 0, y = 0)
		CHECK( normalize_and_check("ex x ex y x = 0 && y = 0.", tau::wff_t) );
	}
	TEST_CASE("ex_block_contradiction") {
		// ∃x. (x = 0 && x != 0) → F (unsatisfiable)
		CHECK( normalize_and_check("ex x x = 0 && x != 0.", tau::wff_f) );
	}
	TEST_CASE("all_ex_witness") {
		// ∀x ∃y. (xy = 0 && x'y = 0) → T (y = 0 is always a witness)
		CHECK( normalize_and_check("all x ex y (xy = 0 && x'y = 0).", tau::wff_t) );
	}
	TEST_CASE("all_complement_tautology") {
		// ∀x. x'x = 0 → T (complement law, tautology)
		CHECK( normalize_and_check("all x x'x = 0.", tau::wff_t) );
	}
	TEST_CASE("all_nonzero_false") {
		// ∀x. x != 0 → F (x = 0 is a counterexample)
		CHECK( normalize_and_check("all x x != 0.", tau::wff_f) );
	}
	TEST_CASE("ex_quantifier_eliminated") {
		// ∃x. x|y = 0 → y = 0 (x is eliminated; result has no ex-quantifier)
		tref fm = get_nso_rr("ex x x|y = 0.").value().main->get();
		tref res = normalize_non_temp<node_t>(fm);
		CHECK(!tau::get(res).find_top(is<node_t, tau::wff_ex>));
	}
}

TEST_SUITE("syntactic_path_simplification") {
	TEST_CASE("1") {
		const char* sample = "x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z | (j & l) | k | (j & l)'";
		tref fm = get_bf_nso_rr("", sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>::on(fm);
		CHECK((tau::get(res).to_str() == "1"));
	}
	TEST_CASE("2") {
		const char* sample = "x = 0 && (z != 0 || (y = 0 && (k = 0 || x != 0))) && x = 0 || x = 0 && y = 0 || z = 0 && (z != 0 || k = 0) && z = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>::on(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"x = 0 && (z != 0 || k = 0 && y = 0) || x = 0 && y = 0 || z = 0 && k = 0",
			"x = 0 && (z != 0 || y = 0 && k = 0) || y = 0 && x = 0 || z = 0 && k = 0",
			"x = 0 && (z != 0 || y = 0 && k = 0) || x = 0 && y = 0 || z = 0 && k = 0",
		}) );
	}
	TEST_CASE("3") {
		const char* sample = "x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z";
		tref fm = get_bf_nso_rr("", sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>::on(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"x&(z'|ky)|yx|zk",
			"x&(z'|yk)|xy|zk",
			"x&(z'|yk)|yx|zk",
			"x&(z'|ky)|xy|zk",
		}) );
	}
	TEST_CASE("4") {
		const char* sample = "x = 0 && (z != 0 || (y = 0 && (k = 0 || x != 0))) && x = 0 || x = 0 && y = 0 || z = 0 && (z != 0 || k = 0) && z = 0 || (j = 0 && l = 0) || k = 0 || !(j = 0 && l = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>::on(fm);
		CHECK((tau::get(res).to_str() == "T"));
	}
	TEST_CASE("4_5") {
		const char* sample = "(ex x x = 0) && (ex x x != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>::on(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"(ex b1 b1 != 0) && (ex b1 b1 = 0)",
			"(ex b1 b1 = 0) && (ex b1 b1 != 0)",
		}) );
	}
}

TEST_SUITE("simplify_using_equality") {
	TEST_CASE("1") {
		const char* sample = "xy|zx = 0 && xy = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"yx|xz = 0",
			"xy|xz = 0",
			"yx|zx = 0",
			"xy|zx = 0",
		}) );
	}
	TEST_CASE("2") {
		const char* sample = "(o1[1]' = 0 && s = 0 && o1[1] = 0 && y|y'w != 0 && y != 0 && w != 0 && z != 0 && o1[0]' = 0 || o1[0]o1[1]'|o1[0]' = 0 && s = 0 && (s = 0 && o1[1] = 0 && y|y'w != 0 && y != 0 && w != 0 && z != 0 || z|z's != 0 && s != 0 && y|y'w != 0 && w != 0 && (z != 0 || y = 0 || o1[1]' = 0) && (y != 0 || z = 0) || z|z's != 0 && y|y'w != 0 && y != 0 && w != 0 && (s != 0 || o1[1] = 0) && (z != 0 || o1[1]' = 0)) && o1[0]' != 0) && v != 0 && x != 0 && o1[0] != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("3") {
		const char* sample = "(w != 0 || o1[0]'&x = 0) && x&o1[0]'|o2[0] = 0 && x'|o1[0] != 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("5") {
		const char* sample = "(w != 0 || o1[0]'&x = 0) && x&o1[0]'|o2[0] = 0 && x'|o1[0] != 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("6") {
		const char* sample = "(ex x x = 0) && (ex x x != 0) && ad != 0 && z = 0 && ad = z.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("7") {
		const char* sample = "xy = 0 && vw = 0 && (yw|xy|vw = 0 && xv|yw|xy|vw = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"yx = 0 && vw = 0 && wy = 0 && vx = 0",
			"xy = 0 && vw = 0 && yw = 0 && xv = 0",
			"yx = 0 && vw = 0 && yw = 0 && vx = 0",
			"yx = 0 && wv = 0 && yw = 0 && xv = 0",
			"yx = 0 && wv = 0 && yw = 0 && vx = 0",
			"xy = 0 && vw = 0 && wy = 0 && vx = 0",
			"yx = 0 && wv = 0 && wy = 0 && vx = 0",
			"xy = 0 && wv = 0 && wy = 0 && xv = 0",
		}) );
	}
	TEST_CASE("8") {
		const char* sample = "xyk|x'yk:bv[16] < 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k < 1");
	}
	TEST_CASE("9") {
		const char* sample = "xyk|x'yk:bv[16] !< 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k' = 0");
	}
	TEST_CASE("10") {
		const char* sample = "xyk|x'yk:bv[16] > 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && 0 < k");
	}
	TEST_CASE("11") {
		const char* sample = "xyk|x'yk:bv[16] !> 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k = 0");
	}
	TEST_CASE("12") {
		const char* sample = "xyk|x'yk:bv[16] <= 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k = 0");
	}
	TEST_CASE("13") {
		const char* sample = "xyk|x'yk:bv[16] !<= 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k !<= 0");
	}
	TEST_CASE("14") {
		const char* sample = "xyk|x'yk:bv[16] >= 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k' = 0");
	}
	TEST_CASE("15") {
		const char* sample = "xyk|x'yk:bv[16] !>= 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && 1 !<= k");
	}
}

TEST_SUITE("squeeze_absorb") {
	TEST_CASE("1") {
		const char* sample = "(((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm, tau::build_variable("x", 1));
		CHECK(tau::get(res).to_str() == "xy = 0 && (xyz&(xy)' != 0 || w = 0 || xyz&(xyz&(xy)')'|xw|xy = 0 && f(x))");
	}
	TEST_CASE("2") {
		const char* sample = "(((xy) = 0 || f(x)) && (xy)' = 0) || w = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm, tau::build_variable("x", 1));
		CHECK(tau::get(res).to_str() == "w = 0 || (xy)' = 0 && f(x)");
	}
	TEST_CASE("3") {
		const char* sample = "(((xy) = 0 || f(x)) && (xy)' = 0) || w = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "w = 0 || (xy)' = 0 && f(x)");
	}
	TEST_CASE("4") {
		const char* sample = "(((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "xy = 0 && (xyz&(xy)' != 0 || w = 0 || xyz&(xyz&(xy)')'|xw|xy = 0 && f(x))");
	}
	TEST_CASE("5") {
		const char* sample = "xy = 0 && vw = 0 && (yw = 0 && xv = 0 || k = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "xy = 0 && vw = 0 && (k = 0 || yw = 0 && xv = 0)");
	}
	TEST_CASE("6") {
		const char* sample = "fx != 0 || (g = 0 && fxy = 0 && fxw != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "fx != 0 || g = 0 && fxy&(fx)' = 0 && (fxw|fx)&(fxy&(fx)')' != 0");
	}
	TEST_CASE("7") {
		const char* sample = "fx = 0 && (g != 0 || fxy != 0 || fxw = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "fx = 0 && (g != 0 || fxy&(fx)' != 0 || (fxw|fx)&(fxy&(fx)')' = 0)");
	}
	TEST_CASE("8") {
		const char* sample = "o1[4] = o1[3] && o2[0] != 0 && (o1[3] != o1[1] || o2[2] != 0 || o2[1] = 0) && (o2[1] != 0 || o2[2] = 0) && (o2[2] = 0 || o2[3] = 0 && o1[4] != o1[2]).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "o2[0]:tau != 0 && o1[4]:tau = o1[3]:tau && (o2[2]:tau != 0 || o2[1]:tau = 0 || o1[3]:tau != o1[1]:tau) && (o2[1]:tau != 0 || o2[2]:tau = 0) && (o2[2]:tau = 0 || o2[3]:tau = 0 && o1[4]:tau != o1[2]:tau)");
	}
}

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
			strings{"wy = 0", "yw = 0"}) );
	}
	TEST_CASE("subs_elim: ex x (x=w) → T") {
		// After substitution the body reduces to T.
		CHECK( normalize_and_check("ex x (x = w).", tau::wff_t) );
	}
}

TEST_SUITE("boole_normal_form") {
	TEST_CASE("1") {
		const char* sample = "ab|ax|bx' != 0 || a = 0 && b = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = boole_normal_form<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"ba'x|b'ax' = 0 || b&(a|x')|b'ax != 0",
			"x'b'a|xba' = 0 || b&(x'|a)|xb'a != 0",
			"b'ax'|ba'x = 0 || b&(a|x')|b'ax != 0",
			"b'x'a|bxa' = 0 || b&(x'|a)|b'xa != 0",
			"a'xb|ax'b' = 0 || a&(x|b)|a'x'b != 0",
		}) );
	}
	TEST_CASE("2") {
		const char* sample = "f(0, 0)f(0, 1) = 0 && f(1, 1)f(1, 0) = 0 && f(1, 0)f(1, 1)|f(0, 1)f(0, 0) != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = boole_normal_form<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	// Inequality reasoning is not supported yet
	// TEST_CASE("3") {
	// 	const char* sample = "(x<y && y<z && x>z).";
	// 	tref fm = get_nso_rr(sample).value().main->get();
	// 	tref res = boole_normal_form<node_t>(fm);
	// 	std::cout << "res: " << tau::get(res) << "\n";
	// 	CHECK(true);
	// }
	TEST_CASE("4") {
		const char* sample = "xy = 0 && (abx' | by'a) != 0 && ab = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = boole_normal_form<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("5") {
		const char* sample = "!(xy = 0 && (abx' | by'a) != 0 && ab = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = boole_normal_form<node_t>(fm);
		CHECK(tau::get(res).equals_T());
	}
}
