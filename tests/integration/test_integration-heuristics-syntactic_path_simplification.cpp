// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "heuristics/syntactic_path_simplification.h"

TEST_SUITE("syntactic_path_simplification") {
	TEST_CASE("1") {
		const char* sample = "x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z | (j & l) | k | (j & l)'";
		tref fm = get_bf_nso_rr("", sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK((tau::get(res).to_str() == "1"));
	}
	TEST_CASE("2") {
		const char* sample = "x = 0 && (z != 0 || (y = 0 && (k = 0 || x != 0))) && x = 0 || x = 0 && y = 0 || z = 0 && (z != 0 || k = 0) && z = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"x = 0 && (z != 0 || k = 0 && y = 0) || x = 0 && y = 0 || z = 0 && k = 0",
			"x = 0 && (z != 0 || y = 0 && k = 0) || y = 0 && x = 0 || z = 0 && k = 0",
			"x = 0 && (z != 0 || y = 0 && k = 0) || x = 0 && y = 0 || z = 0 && k = 0",
		}) );
	}
	TEST_CASE("3") {
		const char* sample = "x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z";
		tref fm = get_bf_nso_rr("", sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
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
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK((tau::get(res).to_str() == "T"));
	}
	TEST_CASE("4_5") {
		const char* sample = "(ex x x = 0) && (ex x x != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"(ex b1 b1 != 0) && (ex b1 b1 = 0)",
			"(ex b1 b1 = 0) && (ex b1 b1 != 0)",
		}) );
	}
}
