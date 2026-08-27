/**
 * @file tau_ba_factoring.h
 * @brief Option and declarations for support-component factoring of the
 * Tau-BA constant tests.
 *
 * Shared by `tau_ba.tmpl.h` (definitions) and `satisfiability.tmpl.h`
 * (`simp_tau_unsat_valid` decides its per-path tests through these helpers),
 * so neither side needs to forward-declare the other's functions or rely on a
 * particular translation-unit layout.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU_BA_FACTORING_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU_BA_FACTORING_H__

#include <cstdlib>

#include "tau_tree.h"

namespace idni::tau_lang {

/// Opt-in for support-component factoring of the Tau-BA constant/valid
/// tests (`is_zero`/`is_one`; see the note in tau_ba.tmpl.h). Off by
/// default; enabled via `api::set_ba_component_factoring(true)` or the
/// environment variable TAU_BA_COMPONENT_FACTORING (a value of "0"
/// disables).
inline bool ba_component_factoring = false;

inline bool ba_component_factoring_enabled() {
	static const bool env = [] {
		const char* v = std::getenv("TAU_BA_COMPONENT_FACTORING");
		return v && *v && !(v[0] == '0' && v[1] == '\0');
	}();
	return ba_component_factoring || env;
}

// Split a formula into its support-component units; -1 = not applicable.
template <typename node>
int factored_tau_units(tref fm, trefs& units);

// Component-wise satisfiability; -1 = not applicable (fall back), 0 = unsat,
// 1 = sat.
template <typename node>
int factored_tau_sat(tref fm);

// Unit-wise validity (distributes over conjunction unconditionally);
// -1 = not applicable, 0 = not valid, 1 = valid.
template <typename node>
int factored_tau_valid(tref fm);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU_BA_FACTORING_H__
