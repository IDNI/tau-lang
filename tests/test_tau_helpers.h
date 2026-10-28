// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TESTS__TEST_TAU_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_TAU_HELPERS_H__

// helper types and functions for tau with just Bool BA as tree<node<Bool>>
// tau_pack.h includes the header of every configured BA, so none is named here.
#include "tau_pack.h"
#define bas_pack TAU_PACK_FULL_BAS
#include "test_helpers.h"

#include <unordered_set>

namespace idni::tau_lang {

// Sweeps the tree store; the Tau-BDD store is never swept and holds raw Tau
// trefs, so its live references are kept.
inline void do_gc() {
	std::unordered_set<tref> keep;
	tau_term_bdd<node_t>::collect_live_refs(keep);
	tau::gc(keep);
}

// Fixture: runs do_gc() before the case.
struct gc_fixture { gc_fixture() { do_gc(); } };

inline tref normalize_test_tau(const char* sample) {
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return nullptr;
	auto r = normalizer<node_t>(nso_rr.value());
	return r.has_value() ? r.value() : nullptr;
}

inline std::pair<tref, tref> get_nso_rr_tau_splitter(const char *sample,
	splitter_type st, bool do_normalize = false)
{
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return std::make_pair(nullptr, nullptr);
	tref fm = nso_rr.value().main->get();
	if (do_normalize) {
		auto r = normalizer<node_t>(fm);
		fm = r.has_value() ? r.value() : nullptr;
	}
	auto s = tau_splitter<bas_pack>(fm, st);
	REQUIRE( s.has_value() );
	return std::make_pair(fm, s.value());
}

// The main formula of spec @p s; a spec that does not parse fails the test.
inline tref spec_or_fail(const char* s) {
	auto nso = get_nso_rr<node_t>(tau::get(s).value_or(nullptr));
	REQUIRE( nso.has_value() );
	tref fm = nso.value().main->get();
	REQUIRE( fm );
	return fm;
}

// is_tau_formula_sat's verdict; an undecided verdict fails the test instead
// of reading as false.
inline bool sat_or_fail(tref fm) {
	REQUIRE( fm );
	auto r = is_tau_formula_sat<node_t>(fm);
	REQUIRE( r.has_value() );
	return r.value();
}

inline bool sat_or_fail(const char* s) { return sat_or_fail(spec_or_fail(s)); }

// is_ltl_aba_realizable's verdict; an undecided verdict fails the test
// instead of reading as false.
inline bool realizable_or_fail(tref fm) {
	REQUIRE( fm );
	auto r = is_ltl_aba_realizable<node_t>(fm, 0, false);
	REQUIRE( r.has_value() );
	return r.value();
}

inline bool realizable_or_fail(const char* s) {
	return realizable_or_fail(spec_or_fail(s));
}

#ifdef TAU_PACK_HAS_BA_BV

/**
 * @brief A bitvector constant node of @p bitwidth bits holding @p value.
 *
 * Lives here rather than on tree<node>: core never builds a bv constant of a
 * given width, only the blasting-predicate tests do.
 */
template <NodeType node>
static tref bv_constant(size_t bitwidth, size_t value) {
	return tree<node>::get_ba_constant(
		make_bitvector_value(bitwidth, value), bv_type_id<node>(static_cast<unsigned short>(bitwidth)));
}

// Bitvector type-id shorthands. These live here rather than in core: nothing
// in src/ reads them, only the type-inference integration tests. Each id is
// computed at its first call: building a type tree needs the parser tables,
// which a variable initialized before main may not find built yet.
/** @brief Cached type id for bitvector-8 under node @p node. */
template<NodeType node>
static size_t bv8_type_id() {
	static const size_t id = ba_types<node>::id(bv_type<node>(8));
	return id;
}
/** @brief Cached type id for bitvector-16 under node @p node. */
template<NodeType node>
static size_t bv16_type_id() {
	static const size_t id = ba_types<node>::id(bv_type<node>(16));
	return id;
}
/** @brief Cached type id for bitvector-32 under node @p node. */
template<NodeType node>
static size_t bv32_type_id() {
	static const size_t id = ba_types<node>::id(bv_type<node>(32));
	return id;
}

#endif // TAU_PACK_HAS_BA_BV

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__TEST_TAU_HELPERS_H__
