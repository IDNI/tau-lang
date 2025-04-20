// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentiTd cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "boolean_algebras/tau_ba.h"

#include "test_integration_helpers_depreciating.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		idni::tau_lang::bdd_init<Bool>();
	}
}

TEST_SUITE("Tau_splitter_upper_tests") {
	TEST_CASE("Tau_splitter_1") {
		const char *sample = "xyz = 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		CHECK(is_splitter(fm, s));
	}

	TEST_CASE("Tau_splitter_2") {
		const char *sample = "xyz != 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		CHECK(is_splitter(fm, s));
	}

	TEST_CASE("Tau_splitter_3") {
		const char *sample = "x = 0 && w != 0 || yz = 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		CHECK(is_splitter(fm, s));
	}

	TEST_CASE("Tau_splitter_4") {
		const char *sample = "x = 0 && w != 0 || y|z != 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		CHECK(is_splitter(fm, s));
	}
#ifndef DEBUG
	TEST_CASE("Tau_splitter_5") {
		const char *sample = "(always ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (sometimes o1[t] != 0 && o1[t] != 1) && (sometimes o1[t] = 0).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		CHECK(is_splitter(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_6") {
		const char *sample = "(always ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (sometimes o1[t] != 0 && o1[t] != 1) && (sometimes o1[t] = 1).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		CHECK(is_splitter(fm, s, fm));
	}
#endif
	TEST_CASE("Tau_splitter_7") {
		const char *sample = "(sometimes o1[t] = 1).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		CHECK(is_splitter(fm, s, fm));
	}
	TEST_CASE("Tau_splitter_8") {
		const char *sample = "(always o1[t] = 1).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		CHECK(is_splitter(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_9") {
		const char *sample = "(always o1[t] = i1[t]).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		CHECK(is_splitter(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_10") {
		const char *sample = "(always o1[0] = 0) && (sometimes o1[0]|o1[1] != 0).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		CHECK(is_splitter(fm, s, fm));
	}
}

TEST_SUITE("Tau_splitter_middle_tests") {
	TEST_CASE("Tau_splitter_1") {
		const char *sample = "xyz = 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::middle);
		CHECK(is_splitter(fm, s));
	}

	TEST_CASE("Tau_splitter_2") {
		const char *sample = "xyz != 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::middle);
		CHECK(is_splitter(fm, s));
	}

	TEST_CASE("Tau_splitter_3") {
		const char *sample = "x = 0 && w != 0 || yz = 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::middle);
		CHECK(is_splitter(fm, s));
	}

	TEST_CASE("Tau_splitter_4") {
		const char *sample = "x = 0 && w != 0 || y|z != 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::middle);
		CHECK(is_splitter(fm, s));
	}
#ifndef DEBUG
	TEST_CASE("Tau_splitter_5") {
		const char *sample = "(always ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (sometimes o1[t] != 0 && o1[t] != 1) && (sometimes o1[t] = 0).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::middle);
		CHECK(is_splitter(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_6") {
		const char *sample = "(always ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (sometimes o1[t] != 0 && o1[t] != 1) && (sometimes o1[t] = 1).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::middle);
		CHECK(is_splitter(fm, s, fm));
	}
#endif
	TEST_CASE("Tau_splitter_7") {
		const char *sample = "(sometimes o1[t] = 1).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::middle);
		CHECK(is_splitter(fm, s, fm));
	}
	TEST_CASE("Tau_splitter_8") {
		const char *sample = "(always o1[t] = 1).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::middle);
		CHECK(is_splitter(fm, s, fm));
	}
}

TEST_SUITE("Tau_splitter_lower_tests") {
	TEST_CASE("Tau_splitter_1") {
		const char *sample = "xyz = 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::lower);
		CHECK(is_splitter(fm, s));
	}

	TEST_CASE("Tau_splitter_2") {
		const char *sample = "xyz != 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::lower);
		CHECK(is_splitter(fm, s));
	}

	TEST_CASE("Tau_splitter_3") {
		const char *sample = "x = 0 && w != 0 || yz = 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::lower);
		CHECK(is_splitter(fm, s));
	}

	TEST_CASE("Tau_splitter_4") {
		const char *sample = "x = 0 && w != 0 || y|z != 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::lower);
		CHECK(is_splitter(fm, s));
	}

	//TODO: re-enable once performance allows it
	/*TEST_CASE("Tau_splitter_5") {
		const char *sample = "(always ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (sometimes o1[t] != 0 && o1[t] != 1) && (sometimes o1[t] = 0).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::lower);
		CHECK(is_splitter(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_6") {
		const char *sample = "(always ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (sometimes o1[t] != 0 && o1[t] != 1) && (sometimes o1[t] = 1).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::lower);
		CHECK(is_splitter(fm, s, fm));
	}*/

	TEST_CASE("Tau_splitter_7") {
		const char *sample = "(sometimes o1[t] = 1).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::lower);
		CHECK(is_splitter(fm, s, fm));
	}
	TEST_CASE("Tau_splitter_8") {
		const char *sample = "(always o1[t] = 1).";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::lower);
		CHECK(is_splitter(fm, s, fm));
	}
}

TEST_SUITE("Tau_splitter_coeff") {

	TEST_CASE("Tau_splitter_coeff1") {
		const char *sample = "{x3}:sbf & {y}:sbf w != 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		fm = normalizer(fm);
		auto s = tau_splitter(fm, splitter_type::upper);
		stringstream ss; ss << s;
		CHECK(ss.str() == "{ x3 y } : sbf ({ x3 y } : sbf w)' = 0" );
	}

	TEST_CASE("Tau_splitter_coeff2") {
		const char *sample = "({x}:sbf|{y}:sbf) w != 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		fm = normalizer(fm);
		auto s = tau_splitter(fm, splitter_type::upper);
		stringstream ss; ss << s;
		CHECK((ss.str() == "{ x | x' y } : sbf ({ x | x' y } : sbf w)' = 0" || ss.str() == "{ x y' | y } : sbf ({ x y' | y } : sbf w)' = 0"));
	}

	TEST_CASE("Tau_splitter_coeff3") {
		const char *sample = "{x}:sbf f = 0 && {y}:sbf g = 0.";
		auto src = make_tau_source(sample);
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		fm = normalizer(fm);
		auto s = tau_splitter(fm, splitter_type::upper);
		stringstream ss; ss << s;
		CHECK(ss.str() == "f{ x } : sbf = 0 && g{ y } : sbf = 0 && f{ x' } : sbf = 0");
	}
}

TEST_SUITE("Tau_splitter_uniter_const") {
	TEST_CASE("Tau_splitter_tau_uniter_const") {
		const char *src = "<:split1> = 0.";
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		stringstream ss; ss << s;
		CHECK(ss.str() == "<:split1> = 0 && <:split2> != 0");
	}
}
