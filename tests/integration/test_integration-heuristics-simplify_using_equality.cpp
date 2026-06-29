// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "heuristics/simplify_using_equality.h"

TEST_SUITE("simplify_using_equality") {
	TEST_CASE("1") {
		const char* sample = "xy|zx = 0 && xy = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
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
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("3") {
		const char* sample = "(w != 0 || o1[0]'&x = 0) && x&o1[0]'|o2[0] = 0 && x'|o1[0] != 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("5") {
		const char* sample = "(w != 0 || o1[0]'&x = 0) && x&o1[0]'|o2[0] = 0 && x'|o1[0] != 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("6") {
		const char* sample = "(ex x x = 0) && (ex x x != 0) && ad != 0 && z = 0 && ad = z.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	TEST_CASE("7") {
		const char* sample = "xy = 0 && vw = 0 && (yw|xy|vw = 0 && xv|yw|xy|vw = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
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
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k < 1");
	}
	TEST_CASE("9") {
		const char* sample = "xyk|x'yk:bv[16] !< 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k' = 0");
	}
	TEST_CASE("10") {
		const char* sample = "xyk|x'yk:bv[16] > 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && 0 < k");
	}
	TEST_CASE("11") {
		const char* sample = "xyk|x'yk:bv[16] !> 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k = 0");
	}
	TEST_CASE("12") {
		const char* sample = "xyk|x'yk:bv[16] <= 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k = 0");
	}
	TEST_CASE("13") {
		const char* sample = "xyk|x'yk:bv[16] !<= 0 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k !<= 0");
	}
	TEST_CASE("14") {
		const char* sample = "xyk|x'yk:bv[16] >= 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && k' = 0");
	}
	TEST_CASE("15") {
		const char* sample = "xyk|x'yk:bv[16] !>= 1 && y = 1.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = simplify_using_equality<node_t>(fm);
		CHECK(tau::get(res).to_str() == "y' = 0 && 1 !<= k");
	}
}
