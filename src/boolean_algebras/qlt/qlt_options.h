// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_options.h
 * @brief The option set of the qlt algebra and the binding of each option to
 * its field.
 *
 * qlt_descriptor.tmpl.h includes this file. Its `declared_options()` gives
 * @ref qlt_option_set, which the pack declares, and its `bind_options()`
 * calls @ref qlt_bind_options.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_OPTIONS_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_OPTIONS_H__

#include <cstddef>

#include "utility/options.h"
#include "option_codecs.h"
#include "tau_diagnostics.h"
#include "tau_tree.h"
#include "boolean_algebras/qlt/qlt.h"

namespace idni::tau_lang {

/// The options of qlt. Each name carries the type prefix, so the command
/// line, the REPL and the environment use one name.
inline const option_set qlt_option_set{ {
	{ "qlt-t3-cap", "qlt", std::size_t{ qlt_t3_encoding_cap },
		"cap the data atoms the qlt T3 synthesis encodings accept before "
		"the ABA-oracle path decides instead (at most 30; 0 = 30)" },
	{ "qlt-const-output-max", "qlt", std::size_t{ qlt_const_output_max },
		"cap the constant-output assignments the fast path in front of "
		"Algorithm B enumerates (0 = unlimited)" },
	{ "qlt-cells-budget", "qlt", std::size_t{ qlt_cells_budget },
		"cap the formula instances one qlt decision by cells evaluates "
		"before it is left open (0 = unlimited)" },
	{ "qlt-cells-max-params", "qlt", std::size_t{ qlt_cells_max_params },
		"cap the other free variables, and the named endpoints, a qlt "
		"decision by cells ranges over (0 = unlimited)" },
} };

/// Binds each option of @ref qlt_option_set to its field. A cap changes
/// which verdicts qlt reaches, so a write empties the tree caches.
template <NodeType node>
result<void> qlt_bind_options(options_repository& repo) {
	result<void> r;
	const option_hook hook = ba_option_hook([] {
		tree<node>::clear_caches(); });
	TAU_TRY_VOID(repo.bind("qlt-t3-cap", qlt_t3_encoding_cap, hook));
	TAU_TRY_VOID(repo.bind("qlt-const-output-max", qlt_const_output_max,
		hook));
	TAU_TRY_VOID(repo.bind("qlt-cells-budget", qlt_cells_budget, hook));
	TAU_TRY_VOID(repo.bind("qlt-cells-max-params", qlt_cells_max_params,
		hook));
	return r;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_OPTIONS_H__
