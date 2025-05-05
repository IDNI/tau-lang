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

// TEST_SUITE("wff_sometimes_always") {
	// TEST_CASE("push_in_1") {
	// 	const char* sample = "sometimes (x=0 && o1[t] = 0 && sometimes(x=0 && o1[t] = 0 && sometimes(x=0 && o1[t] = 0))).";
	// 	auto src = make_tau_source(sample);
	// 	auto formula = make_nso_rr_using_factory<sbf_ba>(src);
	// 	CHECK( formula.has_value() );
	// 	if (!formula.has_value()) return;
	// 	auto fm = formula.value().main;
	// 	std::set<tau<sbf_ba>> visited;
	// 	auto result = push_sometimes_always_in(fm, visited);
	// 	auto simp_res = result
	// 		| repeat_all<step<sbf_ba>, sbf_ba>(simplify_wff<sbf_ba>)
	// 		| reduce_wff<sbf_ba>;
	// 	std::stringstream ss; ss << simp_res;
	// 	CHECK((ss.str() == "x = 0 && (sometimes o1[t] = 0)" || ss.str() == "(sometimes o1[t] = 0) && x = 0"));
	// }

	// TEST_CASE("push_in_2") {
	// 	const char* sample = "sometimes T.";
	// 	auto src = make_tau_source(sample);
	// 	auto formula = make_nso_rr_using_factory<sbf_ba>(src);
	// 	CHECK( formula.has_value() );
	// 	if (!formula.has_value()) return;
	// 	auto fm = formula.value().main;
	// 	std::set<tau<sbf_ba>> visited;
	// 	auto result = push_sometimes_always_in(fm, visited);
	// 	auto simp_res = result
	// 		| repeat_all<step<sbf_ba>, sbf_ba>(simplify_wff<sbf_ba>)
	// 		| reduce_wff<sbf_ba>;
	// 	CHECK( (simp_res | tau_parser::wff_t).has_value() );
	// }

	// TEST_CASE("push_in_3") {
	// 	const char* sample = "sometimes (always o1[t] = 0 || (sometimes o1[t] = 0)) && (always o1[t] = 0).";
	// 	auto src = make_tau_source(sample);
	// 	auto formula = make_nso_rr_using_factory<sbf_ba>(src);
	// 	CHECK( formula.has_value() );
	// 	if (!formula.has_value()) return;
	// 	auto fm = formula.value().main;
	// 	std::set<tau<sbf_ba>> visited;
	// 	auto result = push_sometimes_always_in(fm, visited);
	// 	auto simp_res = result
	// 		| repeat_all<step<sbf_ba>, sbf_ba>(simplify_wff<sbf_ba>)
	// 		| reduce_wff<sbf_ba>;
	// 	std::stringstream ss; ss << simp_res ;
	// 	CHECK( ss.str() == "always o1[t] = 0" );
	// }

	// TEST_CASE("pull_out_1") {
	// 	const char* sample = "(sometimes T && x=0) || (sometimes T && x=0) || (sometimes x=0).";
	// 	auto src = make_tau_source(sample);
	// 	auto formula = make_nso_rr_using_factory<sbf_ba>(src);
	// 	CHECK( formula.has_value() );
	// 	if (!formula.has_value()) return;
	// 	auto fm = formula.value().main;
	// 	auto result = pull_sometimes_always_out(fm);
	// 	auto simp_res = result
	// 		| repeat_all<step<sbf_ba>, sbf_ba>(simplify_wff<sbf_ba>)
	// 		| reduce_wff<sbf_ba>;
	// 	std::stringstream ss; ss << simp_res;
	// 	CHECK(ss.str() == "sometimes x = 0");
	// }

	// TEST_CASE("pull_out_2") {
	// 	const char* sample = "(always x=0) && x=0 && (sometimes x=0 && x=0).";
	// 	auto src = make_tau_source(sample);
	// 	auto formula = make_nso_rr_using_factory<sbf_ba>(src);
	// 	CHECK( formula.has_value() );
	// 	if (!formula.has_value()) return;
	// 	auto fm = formula.value().main;
	// 	auto result = pull_sometimes_always_out(fm);
	// 	auto simp_res = result
	// 		| repeat_all<step<sbf_ba>, sbf_ba>(simplify_wff<sbf_ba>)
	// 		| reduce_wff<sbf_ba>;
	// 	std::stringstream ss; ss << simp_res;
	// 	CHECK(ss.str() == "x = 0");
	// }
// }