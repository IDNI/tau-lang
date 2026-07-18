// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// BA-21: base_ba_dispatcher's is_closed/pack_tau_ba/unpack_tau_ba
// (src/base_ba_dispatcher.h, src/base_ba_dispatcher.tmpl.h) were untested --
// tests/test_Bool_helpers.h defines its own test-only dispatcher
// specializations (base_ba_dispatcher<bv, Bool> and
// base_ba_dispatcher<bv, sbf_ba, Bool>) rather than exercising the real
// production specialization. This file targets the real production
// specialization base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba> defined
// in base_ba_dispatcher.tmpl.h, which is the one actually used to embed
// tau_ba as a BA alternative alongside bv and sbf_ba.

#include "test_init.h"
#include "test_tau_helpers.h"

namespace {
// This file exercises base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>,
// a different (smaller) node type than the ambient `node_t`/`tau` that
// test_tau_helpers.h defines for the rest of the suite (the 7-BA
// tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb> pack). `tref`s are only
// valid within the tree pool of the node type they were created in, so
// every formula fed to `dispatcher` below must be built via `small_tau`,
// not the ambient `tau`.
using small_node = idni::tau_lang::node<
	idni::tau_lang::tau_ba<idni::tau_lang::bv, idni::tau_lang::sbf_ba>,
	idni::tau_lang::bv, idni::tau_lang::sbf_ba>;
using small_tau = idni::tau_lang::tree<small_node>;
using dispatcher = idni::tau_lang::base_ba_dispatcher<
	idni::tau_lang::tau_ba<idni::tau_lang::bv, idni::tau_lang::sbf_ba>,
	idni::tau_lang::bv, idni::tau_lang::sbf_ba>;
using variant_t = std::variant<
	idni::tau_lang::tau_ba<idni::tau_lang::bv, idni::tau_lang::sbf_ba>,
	idni::tau_lang::bv, idni::tau_lang::sbf_ba>;

std::optional<rr<small_node>> small_get_nso_rr(const char* sample) {
	tref spec = small_tau::get(sample);
	if (spec == nullptr) return {};
	return idni::tau_lang::get_nso_rr<small_node>(spec);
}
} // namespace

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("base_ba_dispatcher pack_tau_ba/unpack_tau_ba round-trip") {

	TEST_CASE("pack_tau_ba wraps a wff tref into the tau_ba alternative") {
		tref t_wff = small_tau::_T();
		variant_t packed = dispatcher::pack_tau_ba(t_wff);
		REQUIRE( std::holds_alternative<tau_ba<bv, sbf_ba>>(packed) );
		CHECK( std::get<tau_ba<bv, sbf_ba>>(packed).is_one() );
	}

	TEST_CASE("unpack_tau_ba round-trips the wrapped wff for the tau_ba alternative") {
		tref t_wff = small_tau::_T();
		variant_t packed = dispatcher::pack_tau_ba(t_wff);
		tref unpacked = dispatcher::unpack_tau_ba(packed);
		REQUIRE(unpacked != nullptr);
		CHECK( small_tau::subtree_equals(unpacked, t_wff) );
	}

	TEST_CASE("pack/unpack round-trip preserves an arbitrary closed wff") {
		auto nso_rr = small_get_nso_rr("xyz = 0.");
		REQUIRE(nso_rr.has_value());
		tref main_fm = nso_rr.value().main->get();
		variant_t packed = dispatcher::pack_tau_ba(main_fm);
		REQUIRE( std::holds_alternative<tau_ba<bv, sbf_ba>>(packed) );
		tref unpacked = dispatcher::unpack_tau_ba(packed);
		CHECK( small_tau::subtree_equals(unpacked, main_fm) );
	}

	TEST_CASE("unpack_tau_ba returns nullptr for non-tau_ba alternatives") {
		auto parsed = parse_sbf<tau_ba<bv, sbf_ba>, bv, sbf_ba>("1");
		REQUIRE(parsed.has_value());
		const variant_t& v = parsed.value().first;
		REQUIRE( std::holds_alternative<sbf_ba>(v) );
		CHECK( dispatcher::unpack_tau_ba(v) == nullptr );
	}
}

TEST_SUITE("base_ba_dispatcher::is_closed") {

	TEST_CASE("bv and sbf_ba alternatives are always closed") {
		auto parsed = parse_sbf<tau_ba<bv, sbf_ba>, bv, sbf_ba>("1");
		REQUIRE(parsed.has_value());
		CHECK( dispatcher::is_closed(parsed.value().first) );
	}

	TEST_CASE("tau_ba alternative delegates to is_tau_closed: T is closed") {
		variant_t packed = dispatcher::pack_tau_ba(small_tau::_T());
		CHECK( dispatcher::is_closed(packed) );
	}

	TEST_CASE("tau_ba alternative delegates to is_tau_closed: i/o-only spec is closed") {
		auto nso_rr = small_get_nso_rr("(always o1[t] = i1[t]).");
		REQUIRE(nso_rr.has_value());
		tau_ba<bv, sbf_ba> spec(nso_rr.value().rec_relations,
					 nso_rr.value().main);
		variant_t packed{ spec };
		CHECK( dispatcher::is_closed(packed) );
	}

	TEST_CASE("tau_ba alternative delegates to is_tau_closed: plain free var is not closed") {
		auto nso_rr = small_get_nso_rr("xyz = 0.");
		REQUIRE(nso_rr.has_value());
		tau_ba<bv, sbf_ba> spec(nso_rr.value().rec_relations,
					 nso_rr.value().main);
		variant_t packed{ spec };
		CHECK_FALSE( dispatcher::is_closed(packed) );
	}
}
