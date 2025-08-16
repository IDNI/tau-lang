// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TESTS__TEST_TAU_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_TAU_HELPERS_H__

// helper types and functions for tau with just Bool BA as tree<node<Bool>>
#define bas_pack tau_ba<bv, sbf_ba>, bv, sbf_ba
#include "test_helpers.h"

namespace idni::tau_lang {

std::pair<tref, tref> get_nso_rr_tau_splitter(const char *sample,
	splitter_type st, bool do_normalize = false)
{
	auto nso_rr = get_nso_rr(sample);
	if (!nso_rr.has_value()) return std::make_pair(nullptr, nullptr);
	tref fm = nso_rr.value().main->get();
	if (do_normalize) fm = normalizer<node_t>(fm);
	tref s = tau_splitter<bas_pack>(fm, st);
	return std::make_pair(fm, s);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__TEST_TAU_HELPERS_H__
