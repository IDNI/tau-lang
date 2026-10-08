// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file tau_ba_options.h
 * @brief The option set of the tau algebra and the binding of each option to
 * its field.
 *
 * tau_descriptor.tmpl.h includes this file. Its `declared_options()` gives
 * @ref tau_ba_option_set, which the pack declares, and its `bind_options()`
 * calls @ref tau_ba_bind_options.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_BA_OPTIONS_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_BA_OPTIONS_H__

#include <cstddef>

#include "utility/options.h"
#include "option_codecs.h"
#include "tau_diagnostics.h"
#include "tau_tree.h"
#include "boolean_algebras/tau/tau_ba.h"

namespace idni::tau_lang {

/// The options of the tau algebra. Each name carries the prefix `ba` (see
/// @ref ba_option_prefix).
inline const option_set tau_ba_option_set{ {
	{ "ba-component-factoring", "tau", bool{ ba_component_factoring },
		"decide a Tau-BA constant one variable-disjoint component at a "
		"time" },
	{ "ba-decision-pins", "tau", std::size_t{ ba_decision_pins },
		"cap the decided Tau-BA rows whose key tree is kept alive "
		"across the sweep (0 = none)" },
} };

/// Binds each option of @ref tau_ba_option_set to its field.
template <NodeType node>
result<void> tau_ba_bind_options(options_repository& repo) {
	result<void> r;
	// The factoring decides which verdicts the tree caches hold.
	TAU_TRY_VOID(repo.bind("ba-component-factoring", ba_component_factoring,
		ba_option_hook([] { tree<node>::clear_caches(); })));
	TAU_TRY_VOID(repo.bind("ba-decision-pins", ba_decision_pins,
		ba_option_hook()));
	return r;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_BA_OPTIONS_H__
