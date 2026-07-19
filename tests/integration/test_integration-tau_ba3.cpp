// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

namespace {
// tau_ba<bv, sbf_ba> is a different (smaller) node type than the ambient
// `node_t`/`tau` that test_tau_helpers.h defines for the rest of the suite
// (the 6-BA tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb> pack). `tref`s are
// only valid within the tree pool of the node type they were created in, so
// formulas fed to tau_ba<bv, sbf_ba> must be built via `small_tau`, not the
// ambient `tau` (see tests/unit/test_base_ba_dispatcher.cpp).
using small_node = idni::tau_lang::node<
	idni::tau_lang::tau_ba<idni::tau_lang::bv, idni::tau_lang::sbf_ba>,
	idni::tau_lang::bv, idni::tau_lang::sbf_ba>;
using small_tau = idni::tau_lang::tree<small_node>;

std::optional<rr<small_node>> small_get_nso_rr(const char* sample) {
	tref spec = small_tau::get(sample);
	if (spec == nullptr) return {};
	return idni::tau_lang::get_nso_rr<small_node>(spec);
}
} // namespace

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
			"ex b1 b1 != 0 && f[0](b1)",
			"ex b1 f[0](b1) && b1 != 0",
		}) );
	}
}

TEST_SUITE("tau_ba dispatcher helpers") {
	TEST_CASE("pack/unpack roundtrip preserves main formula") {
		using dispatcher = base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
		auto nso = small_get_nso_rr("x = 0.");
		REQUIRE(nso.has_value());
		tref fm = nso->main->get();
		auto packed = dispatcher::pack_tau_ba(fm);
		CHECK(std::holds_alternative<tau_ba<bv, sbf_ba>>(packed));
		tref unpacked = dispatcher::unpack_tau_ba(packed);
		REQUIRE(unpacked != nullptr);
		CHECK(small_tau::get(unpacked) == small_tau::get(fm));
	}

	TEST_CASE("is_closed accepts io-only free variables") {
		auto nso = small_get_nso_rr("i1[t] = o1[t].");
		REQUIRE(nso.has_value());
		CHECK(is_tau_closed(tau_ba<bv, sbf_ba>(nso->main->get())));
	}

	TEST_CASE("is_closed rejects ordinary free variables") {
		auto nso = small_get_nso_rr("x = 0.");
		REQUIRE(nso.has_value());
		CHECK(!is_tau_closed(tau_ba<bv, sbf_ba>(nso->main->get())));
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
