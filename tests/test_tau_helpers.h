// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TESTS__TEST_TAU_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_TAU_HELPERS_H__

// helper types and functions for tau with just Bool BA as tree<node<Bool>>
// tau_pack.h includes the header of every configured BA, so none is named here.
#include "tau_pack.h"
#define bas_pack TAU_PACK_FULL_BAS
#include "test_helpers.h"

namespace idni::tau_lang {

inline tref normalize_test_tau(const char* sample) {
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return nullptr;
	return normalizer<node_t>(nso_rr.value());
}

inline std::pair<tref, tref> get_nso_rr_tau_splitter(const char *sample,
	splitter_type st, bool do_normalize = false)
{
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return std::make_pair(nullptr, nullptr);
	tref fm = nso_rr.value().main->get();
	if (do_normalize) fm = normalizer<node_t>(fm);
	tref s = tau_splitter<bas_pack>(fm, st);
	return std::make_pair(fm, s);
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
		make_bitvector_value(bitwidth, value), bv_type_id<node>(bitwidth));
}

// Bitvector type-id shorthands. These live here rather than in core: nothing
// in src/ reads them, only the type-inference integration tests.
/** @brief Cached type id for bitvector-8 under node @p node. */
template<NodeType node>
static size_t bv8_type_id = ba_types<node>::id(bv_type<node>(8));
/** @brief Cached type id for bitvector-16 under node @p node. */
template<NodeType node>
static size_t bv16_type_id = ba_types<node>::id(bv_type<node>(16));
/** @brief Cached type id for bitvector-32 under node @p node. */
template<NodeType node>
static size_t bv32_type_id = ba_types<node>::id(bv_type<node>(32));

#endif // TAU_PACK_HAS_BA_BV

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__TEST_TAU_HELPERS_H__
