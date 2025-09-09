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
		const char* sample = "xy = 0 && (abx' | by'a) != 0 <-> xy = 0 && ab != 0.";
		CHECK( normalize_and_check(sample, tau::wff_t) );
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
	//TODO: goes to unit tests
	TEST_CASE("5") {
		const char* sample = "(a=0||b=0)&&x=0&&y=0&&(p=0||f=0)&&(t=0||q=0).";
		tref fm = get_nso_rr(sample).value().main->get();
		for (tref path : expression_paths<node_t>(fm)) {
			std::cout << "path: " << tau::get(path) << "\n";
			trefs paths = get_cnf_wff_clauses<node_t>(path);
			assert(paths.size() == 5);
			for (tref p : paths) assert(tau::get(p).is(tau::wff));
		}
		CHECK(true);
	}
	TEST_CASE("6") {
		const char* sample = "x&(v|y&(t|z|s|r)|g)";
		tref fm = get_bf_nso_rr("", sample).value().main->get();
		for (tref path : expression_paths<node_t>(fm)) {
			std::cout << "path: " << tau::get(path) << "\n";
			trefs paths = get_cnf_bf_clauses<node_t>(path);
			for (tref p : paths) assert(tau::get(p).is(tau::bf));
		}
		CHECK(true);
	}
	TEST_CASE("7") {
		const char* sample = " (sometimes (a=0||b=0)&&x=0&&y=0&&(p=0||f=0)&&(t=0||q=0)) && (always x = 0) || (always y = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		for (tref path : expression_paths<node_t>(fm)) {
			std::cout << "path: " << tau::get(path) << "\n";
			trefs paths = get_cnf_wff_clauses<node_t>(path);
			for (tref p : paths) assert(tau::get(p).is(tau::wff));
		}
		CHECK(true);
	}
}

TEST_SUITE("simplify_using_equality") {
	TEST_CASE("1") {
		const char* sample = "x&(y|z) = 0 && xy = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		std::cout << "res: " << tau::get(res) << "\n";
		CHECK(true);
	}
	TEST_CASE("2") {
		const char* sample = "ex z (o1[1]' = 0 && s = 0 && o1[1] = 0 && y|y'w != 0 && y != 0 && w != 0 && z != 0 && o1[0]' = 0 || o1[0]o1[1]'|o1[0]' = 0 && s = 0 && (s = 0 && o1[1] = 0 && y|y'w != 0 && y != 0 && w != 0 && z != 0 || z|z's != 0 && s != 0 && y|y'w != 0 && w != 0 && (z != 0 || y = 0 || o1[1]' = 0) && (y != 0 || z = 0) || z|z's != 0 && y|y'w != 0 && y != 0 && w != 0 && (s != 0 || o1[1] = 0) && (z != 0 || o1[1]' = 0)) && o1[0]' != 0) && v != 0 && x != 0 && o1[0] != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		std::cout << "res: " << tau::get(res) << "\n";
		CHECK(true);
	}
	TEST_CASE("3") {
		const char* sample = "(w != 0 || o1[0]'&x = 0) && x&o1[0]'|o2[0] = 0 && x'|o1[0] != 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		std::cout << "res: " << tau::get(res) << "\n";
		CHECK(true);
	}
	TEST_CASE("4") {
		const char* sample = "zyx = 0 && x&(v|y&(t|z|s|r)|g) = w.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		std::cout << "res: " << tau::get(res) << "\n";
		CHECK(true);
	}
	TEST_CASE("5") {
		const char* sample = "(w != 0 || o1[0]'&x = 0) && x&o1[0]'|o2[0] = 0 && x'|o1[0] != 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>::on(fm);
		std::cout << "res: " << tau::get(res) << "\n";
		CHECK(true);
	}
}

// TEST_SUITE("term_boole_normal_form") {
// 	TEST_CASE("1") {
// 		const char* sample = "x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z";
// 		tref fm = get_bf_nso_rr("", sample).value().main->get();
// 		tref res = syntactic_path_simplification<node_t>::on(fm);
// 		CHECK(true);
// 	}
// }

TEST_SUITE("squeeze_absorb") {
	TEST_CASE("1") {
		const char* sample = "ex x (((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb_down<node_t>(fm, tau::build_bf_variable("x", 1));
		std::cout << "res: " << tau::get(res) << "\n";
		CHECK(true);
	}
	TEST_CASE("2") {
		const char* sample = "(((xy) = 0 || f(x)) && (xy)' = 0) || w = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = squeeze_absorb_down<node_t>(fm, tau::build_bf_variable("x", 1));
		std::cout << "res: " << tau::get(res) << "\n";
		CHECK(true);
	}
}

TEST_SUITE("boole_normal_form") {
	TEST_CASE("1") {
		const char* sample = "(x<y && y<z && x>z) || (z<w && w<u).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = boole_normal_form<node_t>(fm);
		std::cout << "res: " << tau::get(res) << "\n";
		CHECK(true);
	}
	TEST_CASE("2") {
		const char* sample = "xy = 0 && (abx' | by'a)|(xy) != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = boole_normal_form<node_t>(fm);
		std::cout << "res: " << tau::get(res) << "\n";
		CHECK(true);
	}
}

TEST_SUITE("anti_prenex") {
	TEST_CASE("1") {
		const char* sample = "all a,b,c,d a'c|b'd = 0 <-> a & b' & d | a' & c | b' & c' & d = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		anti_prenex<node_t>(fm);
		CHECK(true);
	}
	TEST_CASE("2") {
		const char* sample = "all x all y ex z (x != y) -> (x < z && z < y).";
		tref fm = get_nso_rr(sample).value().main->get();
		anti_prenex<node_t>(fm);
		CHECK(true);
	}
	TEST_CASE("3") {
		const char* sample = "all x all y ex z (x < y) -> (x < z && z < y).";
		tref fm = get_nso_rr(sample).value().main->get();
		anti_prenex<node_t>(fm);
		CHECK(true);
	}
	TEST_CASE("4") {
		const char* sample = "all a all b all c all d all e all f (ax + bx' != cy + d'y' || ax + bx' = ey + fy') <-> (ax + bx' = ey + fy' || ax + bx' != cy + d'y').";
		tref fm = get_nso_rr(sample).value().main->get();
		anti_prenex<node_t>(fm);
		CHECK(true);
	}
	TEST_CASE("5") {
		const char* sample = "all a all b all c all d all e all f (ax + bx' != cy + d'y' || ax + bx' = ey + fy') <-> (ax + bx' = ey + fy' || ax + bx' = cy + d'y').";
		tref fm = get_nso_rr(sample).value().main->get();
		anti_prenex<node_t>(fm);
		CHECK(true);
	}
	TEST_CASE("6") {
		const char* sample = "all x ex y all z ex w all u ex v ((x<y && y<z) || (z<w && w<u)|| (u<v && v<x)).";
		tref fm = get_nso_rr(sample).value().main->get();
		anti_prenex<node_t>(fm);
		CHECK(true);
	}
	TEST_CASE("7") {
		const char* sample = "xy = 0 && (abx' | by'a) != 0 <-> xy = 0 && ab != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		boole_normal_form<node_t>(fm);
		CHECK(true);
	}
}