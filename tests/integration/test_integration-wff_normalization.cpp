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
		const char* sample = "all a all b all c all d all e all f (ax + bx' != cy + d'y' || ax + bx' = ey + fy') <-> (ax + bx' = ey + fy' || ax + bx' != cy + d'y').";
		CHECK( normalize_and_check(sample, tau::wff_t) );
	}
	TEST_CASE("5") {
		const char* sample = "all a all b all c all d all e all f (ax + bx' != cy + d'y' || ax + bx' = ey + fy') <-> (ax + bx' = ey + fy' || ax + bx' = cy + d'y').";
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
		const char* sample = "{ !i5[t] = x || o5[t] = y } : tau = u[0].";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = normalize_non_temp<node_t>(fm);
		CHECK(tau::get(res).to_str() == "{ always i5[t] != x || o5[t] = y } : tau = u[0]");
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
		CHECK(tau::get(res).to_str() == "x = 0 && (z != 0 || y = 0 && k = 0) || y = 0 && x = 0 || z = 0 && k = 0");
	}
	TEST_CASE("3") {
		const char* sample = "x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z";
		tref fm = get_bf_nso_rr("", sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>::on(fm);
		CHECK((tau::get(res).to_str() == "x(z'|yk)|xy|zk"));
	}
	TEST_CASE("4") {
		const char* sample = "x = 0 && (z != 0 || (y = 0 && (k = 0 || x != 0))) && x = 0 || x = 0 && y = 0 || z = 0 && (z != 0 || k = 0) && z = 0 || (j = 0 && l = 0) || k = 0 || !(j = 0 && l = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>::on(fm);
		CHECK((tau::get(res).to_str() == "T"));
	}
	TEST_CASE("4_5") {
		const char* sample = "(ex x x != 0) && (ex x x = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>::on(fm);
		CHECK(tau::get(res) == tau::get(fm));
	}
}

TEST_SUITE("simplify_using_equality") {
	TEST_CASE("1") {
		const char* sample = "x&(y|z) = 0 && xy = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "xy = 0 && xz = 0");
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
	TEST_CASE("4") {
		const char* sample = "zyx = 0 && x&(v|y&(t|z|s|r)|g) = w.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		CHECK(tau::get(res).to_str() == "xyz = 0 && x(g|v|y(s|t|r)) = w");
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
}

TEST_SUITE("squeeze_absorb") {
	TEST_CASE("1") {
		const char* sample = "ex x (((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb_down<node_t>(fm, tau::build_variable("x", 1));
		std::cout << "res: " << tau::get(res) << "\n";
		CHECK(tau::get(res).to_str() == "ex x xy = 0 && (x(yz|w|y) = 0 && f(x) || w = 0)");
	}
	TEST_CASE("2") {
		const char* sample = "(((xy) = 0 || f(x)) && (xy)' = 0) || w = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb_down<node_t>(fm, tau::build_variable("x", 1));
		CHECK(tau::get(res).to_str() == "(xy)' = 0 && f(x) || w = 0");
	}
}

// This suite could be used once boole_normal_form is more advanced
// TEST_SUITE("boole_normal_form") {
// 	TEST_CASE("1") {
// 		const char* sample = "(x<y && y<z && x>z) || (z<w && w<u).";
// 		tref fm = get_nso_rr(sample).value().main->get();
// 		tref res = boole_normal_form<node_t>(fm);
// 		std::cout << tau::get(res);
// 		CHECK(true);
// 	}
// 	TEST_CASE("2") {
// 		const char* sample = "xy = 0 && (abx' | by'a)|(xy) != 0.";
// 		tref fm = get_nso_rr(sample).value().main->get();
// 		tref res = boole_normal_form<node_t>(fm);
// 		std::cout << tau::get(res);
// 		CHECK(true);
// 	}
// }

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
		const char* sample = "all y !({ adc|a'dc|b = 0 } : tau y != 0 ) || { ab|cd = 0 } : tau y != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK(tau::get(res).equals_T());
	}
	TEST_CASE("4") {
		const char* sample = "{always a&(b|dc)|a'dc = 0}'&{always b|dc = 0} != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
}
