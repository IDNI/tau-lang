// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd initialization") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("Tau_splitter_tau_coeff") {
	TEST_CASE("Tau_splitter_tau_coeff1") {
		const char *sample = "{o1[t]o2[t] = 0.} v != 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(sample, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		CHECK(is_splitter<bas_pack>(fm, s));
	}

	TEST_CASE("Tau_splitter_tau_coeff2") {
		const char *src = "{o1[t]|o2[t] = 0.}&v = 0.";
		auto [fm, s] = get_nso_rr_tau_splitter(src, splitter_type::upper);
		CHECK(fm != nullptr);
		CHECK(s != nullptr);
		auto s_str = tau::get(s).to_str();
		CHECK(is_splitter<bas_pack>(fm, s));
	}
}
