// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("cpp operators") {

	TEST_CASE("Ohad's example: ex X ( { ex X ( X = {ex Y (Y = 0).}). } = 0).") {
		const char* sample = "ex X ( { ex X ( X = { ( ex Y Y = 0).}). } = 0).";
		CHECK( normalize_and_check(sample, tau::wff_f) );
	}
}

TEST_SUITE("allowing unresolved rr's in normalization") {

	TEST_CASE("f[0](x).") {
		const char* sample = "f[0](x).";
		CHECK( normalize_and_check(sample, "f[0](x)") );
	}

	TEST_CASE("ex x f[0](x).") {
		const char* sample = "ex x f[0](x).";
		CHECK( normalize_and_check(sample, "ex b1 f[0](b1)") );
	}

	TEST_CASE("ex x f[0](x) && x = 0.") {
		// anti_prenex_block performs substitution-based elimination:
		// ex x (f[0](x) && x = 0) → f[0](0)  (x must be 0, so substitute)
		const char* sample = "ex x f[0](x) && x = 0.";
		CHECK( normalize_and_check(sample, "f[0](0)") );
	}

	TEST_CASE("ex x f[0](x) && x != 0.") {
		const char* sample = "ex x f[0](x) && x != 0.";
		CHECK( normalize_and_check(sample, strings{
			"ex b1 f[0](b1) && b1 != 0",
			"ex b1 b1 != 0 && f[0](b1)",
		}) );
	}
}

TEST_SUITE("tau_ba dispatcher helpers") {
	TEST_CASE("pack/unpack roundtrip preserves main formula") {
		using dispatcher = base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
		auto nso = get_nso_rr("x = 0.");
		REQUIRE(nso.has_value());
		tref fm = nso->main->get();
		auto packed = dispatcher::pack_tau_ba(fm);
		CHECK(std::holds_alternative<tau_ba<bv, sbf_ba>>(packed));
		tref unpacked = dispatcher::unpack_tau_ba(packed);
		REQUIRE(unpacked != nullptr);
		CHECK(tau::get(unpacked) == tau::get(fm));
	}

	TEST_CASE("is_closed accepts io-only free variables") {
		auto nso = get_nso_rr("i1[t] = o1[t].");
		REQUIRE(nso.has_value());
		CHECK(is_tau_closed(tau_ba<bv, sbf_ba>(nso->main->get())));
	}

	TEST_CASE("is_closed rejects ordinary free variables") {
		auto nso = get_nso_rr("x = 0.");
		REQUIRE(nso.has_value());
		CHECK(!is_tau_closed(tau_ba<bv, sbf_ba>(nso->main->get())));
	}
}
