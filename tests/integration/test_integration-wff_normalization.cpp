// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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
	TEST_CASE("6") {
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
		CHECK(tau::get(res).to_str() == "{ always i5[t]:tau != <:x> || o5[t]:tau = <:y> }:tau = u[0]:tau");
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
		CHECK(tau::get(res).to_str() == "x = 0 && (z != 0 || y = 0 && k = 0) || x = 0 && y = 0 || z = 0 && k = 0");
	}
	TEST_CASE("3") {
		const char* sample = "x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z";
		tref fm = get_bf_nso_rr("", sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>::on(fm);
		CHECK((tau::get(res).to_str() == "x(z'|yk)|yx|zk"));
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
		CHECK(tau::get(res).to_str() == "(ex b1 b1 = 0) && (ex b1 b1 != 0)");
	}
}

TEST_SUITE("simplify_using_equality") {
	TEST_CASE("1") {
		const char* sample = "xy|zx = 0 && xy = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "yx = 0 && zx = 0");
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
		CHECK(tau::get(res).to_str() == "yx = 0 && yw|vw = 0 && vx = 0");
	}
	TEST_CASE("8") {
		const char* sample = "xyk|x'yk:bv < 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k < 1");
	}
	TEST_CASE("9") {
		const char* sample = "xyk|x'yk:bv !< 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k' = 0");
	}
	TEST_CASE("10") {
		const char* sample = "xyk|x'yk:bv > 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && 0 < k");
	}
	TEST_CASE("11") {
		const char* sample = "xyk|x'yk:bv !> 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k = 0");
	}
	TEST_CASE("12") {
		const char* sample = "xyk|x'yk:bv <= 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k = 0");
	}
	TEST_CASE("13") {
		const char* sample = "xyk|x'yk:bv !<= 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k !<= 0");
	}
	TEST_CASE("14") {
		const char* sample = "xyk|x'yk:bv >= 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k' = 0");
	}
	TEST_CASE("15") {
		const char* sample = "xyk|x'yk:bv !>= 1 && y = 1.";
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
		CHECK(tau::get(res).to_str() == "xy = 0 && (xyz(xy)' != 0 || w = 0 || xyz(xyz(xy)')'|xw|xy = 0 && f(x))");
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
		CHECK(tau::get(res).to_str() == "xy = 0 && (xyz(xy)' != 0 || w = 0 || xyz(xyz(xy)')'|xw|xy = 0 && f(x))");
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
		CHECK(tau::get(res).to_str() == "fx != 0 || g = 0 && fxy(fx)' = 0 && (fxw|fx)(fxy(fx)')' != 0");
	}
	TEST_CASE("7") {
		const char* sample = "fx = 0 && (g != 0 || fxy != 0 || fxw = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb<node_t>(fm);
		CHECK(tau::get(res).to_str() == "fx = 0 && (g != 0 || fxy(fx)' != 0 || (fxw|fx)(fxy(fx)')' = 0)");
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
		const char* sample = "{always <:a>&(<:b>|<:d><:c>)|<:a>'<:d><:c> = 0}'&{always <:b>|<:d><:c> = 0} != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK(tau::get(res).equals_F());
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

TEST_SUITE("boole_normal_form") {
	TEST_CASE("1") {
		const char* sample = "ab|ax|bx' != 0 || a = 0 && b = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = boole_normal_form<node_t>(fm);
		CHECK(tau::get(res).to_str() == "ba'x|b'ax' = 0 || b(a|x')|b'ax != 0");
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
