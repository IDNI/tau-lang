// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("Tau_splitter_upper_tests") {
	TEST_CASE("Tau_splitter_1") {
		const char *sample = "xyz = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_2") {
		const char *sample = "xyz != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_3") {
		const char *sample = "x = 0 && w != 0 || yz = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_4") {
		const char *sample = "x = 0 && w != 0 || y|z != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}
	TEST_CASE("Tau_splitter_5") {
		const char *sample = "(always ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (sometimes o1[t] != 0 && o1[t] != 1) && (sometimes o1[t] = 0).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}
	TEST_CASE("Tau_splitter_6") {
		const char *sample = "(always ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (sometimes o1[t] != 0 && o1[t] != 1) && (sometimes o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}
	TEST_CASE("Tau_splitter_7") {
		const char *sample = "(sometimes o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}
	TEST_CASE("Tau_splitter_8") {
		const char *sample = "(always o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_9") {
		const char *sample = "(always o1[t] = i1[t]).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_10") {
		const char *sample = "(always o1[0] = 0) && (sometimes o1[0]|o1[1] != 0).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}
	TEST_CASE("Tau_splitter_11") {
		const char *sample = "xy = 0 && x != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s ));
	}
	TEST_CASE("Tau_splitter_12") {
		const char *sample = "{ab|cd = 0} & y = 0 && {ab|cd = 0} & y < {ab|cd = 0}.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}
	TEST_CASE("Tau_splitter_13") {
		const char *sample = "{ab|cd = 0} & y != 0 && { ab|cd = 0 } : tau ({ ab|cd = 0 } : tau y)' != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}
	TEST_CASE("Tau_splitter_14") {
		const char *sample = "x = 0 && y != 0 || z = 0 && k = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		// std::cout << "splitter: " << tau::get(s) << "\n";
		CHECK(is_splitter<bas_pack>(fm, s));
	}
}

// For now we only support upper splitters
/*TEST_SUITE("Tau_splitter_middle_tests") {
	TEST_CASE("Tau_splitter_1") {
		const char *sample = "xyz = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_2") {
		const char *sample = "xyz != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_3") {
		const char *sample = "x = 0 && w != 0 || yz = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_4") {
		const char *sample = "x = 0 && w != 0 || y|z != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}
#ifndef DEBUG
	TEST_CASE("Tau_splitter_5") {
		const char *sample = "(always ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (sometimes o1[t] != 0 && o1[t] != 1) && (sometimes o1[t] = 0).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_6") {
		const char *sample = "(always ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (sometimes o1[t] != 0 && o1[t] != 1) && (sometimes o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}
#endif
	TEST_CASE("Tau_splitter_7") {
		const char *sample = "(sometimes o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}
	TEST_CASE("Tau_splitter_8") {
		const char *sample = "(always o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::middle);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}
}

TEST_SUITE("Tau_splitter_lower_tests") {
	TEST_CASE("Tau_splitter_1") {
		const char *sample = "xyz = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::lower);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_2") {
		const char *sample = "xyz != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::lower);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_3") {
		const char *sample = "x = 0 && w != 0 || yz = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::lower);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_4") {
		const char *sample = "x = 0 && w != 0 || y|z != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::lower);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	//TODO: re-enable once performance allows it
	TEST_CASE("Tau_splitter_5") {
		const char *sample = "(always ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (sometimes o1[t] != 0 && o1[t] != 1) && (sometimes o1[t] = 0).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::lower);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}

	TEST_CASE("Tau_splitter_6") {
		const char *sample = "(always ([t>0] -> (o1[t] = 0 || o1[t] = 1))) && (sometimes o1[t] != 0 && o1[t] != 1) && (sometimes o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::lower);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}#1#

	TEST_CASE("Tau_splitter_7") {
		const char *sample = "(sometimes o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::lower);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}
	TEST_CASE("Tau_splitter_8") {
		const char *sample = "(always o1[t] = 1).";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::lower);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s, fm));
	}
}*/

TEST_SUITE("Tau_splitter_coeff") {

	TEST_CASE("Tau_splitter_coeff1") {
		bdd_init<Bool>();
		const char *sample = "{x3}:sbf & {y}:sbf w != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper, true);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_coeff2") {
		bdd_init<Bool>();
		const char *sample = "({x}:sbf|{y}:sbf) w != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper, true);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_coeff3") {
		bdd_init<Bool>();
		const char *sample = "{x}:sbf f = 0 && {y}:sbf g = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper, true);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}
}

TEST_SUITE("Tau_splitter_uniter_const") {
	TEST_CASE("Tau_splitter_tau_uniter_const") {
		bdd_init<Bool>();
		const char *src = "<:split1> = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(src, splitter_type::upper, true);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		auto s_str = tau::get(s).to_str();
		CHECK(s_str == "<:split1> = 0 && <:split2> != 0");
	}
}
