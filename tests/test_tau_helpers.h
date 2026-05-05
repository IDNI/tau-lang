// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TESTS__TEST_TAU_HELPERS_H__
#define __IDNI__TAU__TESTS__TEST_TAU_HELPERS_H__

// helper types and functions for tau with just Bool BA as tree<node<Bool>>
#define bas_pack tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>, qint, qlt, nlang_ba, bv, sbf_ba, hsb
#include "boolean_algebras/qint.h"
#include "boolean_algebras/qlt.h"
#include "boolean_algebras/nlang_ba.h"
#include "boolean_algebras/hsb.h"
#include "test_helpers.h"
#include <cstdlib>

namespace idni::tau_lang {

// Register the same exit-time cleanup that src/main.cpp installs for
// the tau CLI: ba_constants<node_t>::cleanup() must run before cvc5's
// TermManager destructor, otherwise the static teardown order leaves
// ba_constants holding dangling cvc5::Term references and the process
// SEGFAULTs after the doctest summary prints.  Without this hook the
// tests pass all assertions but exit non-zero, which ctest reports as
// SEGFAULT — masking the actual test outcomes and inverting the
// pass/fail signal in CI.
//
// Registered here rather than in test_helpers.h because only the
// qlt/cvc5-bearing bas_pack benefits — Bool/sbf_ba-only tests instead
// see a double-free when cleanup() races their bdd-pool teardown.
namespace test_tau_init_detail {
	struct _CleanupRegistrar {
		// __attribute__((used)) so LTO does not DCE the registration.
		__attribute__((used)) _CleanupRegistrar() {
			std::atexit([]() { ba_constants<node_t>::cleanup(); });
		}
	};
	inline _CleanupRegistrar _ba_constants_cleanup_registrar;
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

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__TEST_TAU_HELPERS_H__
