// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

TEST_SUITE("normal forms: snf for wff") {

	TEST_CASE("simple case: T") {
		const char* sample = "T.";
		CHECK( get_nso_rr_and_check(sample, tau::wff_t) );
	}

	TEST_CASE("simple case: F") {
		const char* sample = "F.";
		CHECK( get_nso_rr_and_check(sample, tau::wff_f) );
	}

	TEST_CASE("simple case: X = 0") {
		const char* sample = "X = 0.";
		auto nso_rr = get_nso_rr(sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		tref result = snf_wff<node_t>(nso_rr.value().main->get());
		CHECK( nso_rr.value().main->get() == result );
	}

	TEST_CASE("quantifiers: always X = 0") {
		const char* sample = "always X = 0.";
		auto nso_rr = get_nso_rr(sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		tref result = snf_wff<node_t>(nso_rr.value().main->get());
		CHECK( nso_rr.value().main->get() == result );
	}

	TEST_CASE("quantifiers: sometimes o1[t] = 0") {
		const char* sample = "sometimes o1[t] = 0.";
		auto nso_rr = get_nso_rr(sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		tref result = snf_wff<node_t>(nso_rr.value().main->get());
		CHECK( nso_rr.value().main->get() == result );
	}

	TEST_CASE("quantifiers: all X X = 0") {
		const char* sample = "all X X = 0.";
		auto nso_rr = get_nso_rr(sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		tref result = snf_wff<node_t>(nso_rr.value().main->get());
		CHECK( nso_rr.value().main->get() == result );
	}

	TEST_CASE("quantifiers: ex X X = 0") {
		const char* sample = "ex X X = 0.";
		auto nso_rr = get_nso_rr(sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		tref result = snf_wff<node_t>(nso_rr.value().main->get());
		CHECK( nso_rr.value().main->get() == result );
	}

	TEST_CASE("rec. relations: f[0](X)") {
		const char* sample = "f[0](X).";
		auto nso_rr = get_nso_rr(sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		tref result = snf_wff<node_t>(nso_rr.value().main->get());
		CHECK( nso_rr.value().main->get() == result );
	}

	// TODO (MEDIUM) fix this test
	/*TEST_CASE("simple case: {sbf: a} x = 0 && {sbf:a}' x = 0") {
		const char* sample = "{sbf: a} x = 0 && {sbf:a}' x = 0.";
		auto nso_rr = get_nso_rr(sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		tref result = snf_wff<node_t>(nso_rr.value().main->get());
		CHECK( tau::get(result).to_str() == "x = 0" );
	}*/

	// TODO (HIGH) fix this test
	TEST_CASE("simple case: xy = 0 && x = 0") {
		const char* sample = "xy = 0 && x = 0.";
		auto nso_rr = get_nso_rr(sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		tref result = snf_wff<node_t>(nso_rr.value().main->get());
		CHECK( tau::get(result).to_str() == "x = 0" );
	}

	// TODO (HIGH) fix this test
	TEST_CASE("simple case: xy != 0 && x != 0") {
		const char* sample = "xy != 0 && x != 0.";
		auto nso_rr = get_nso_rr(sample);
		CHECK( nso_rr.has_value() );
		if (!nso_rr.has_value()) return;
		tref result = snf_wff<node_t>(nso_rr.value().main->get());
		CHECK( tau::get(result).to_str() == "xy != 0" );
	}
}
