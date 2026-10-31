// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file hsb_options.h
 * @brief The runtime options of the hsb algebra.
 *
 * The caps live in hsb.h, where normalization reads them. This
 * header adds @ref hsb_option_set, which the pack declares, and its
 * `bind_options()` calls @ref hsb_bind_options.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_OPTIONS_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_OPTIONS_H__

#include <cstddef>

#include "utility/options.h"
#include "option_codecs.h"
#include "tau_diagnostics.h"
#include "tau_tree.h"
#include "boolean_algebras/hsb/hsb.h"

namespace idni::tau_lang {

/// The options of hsb. Each name carries the type prefix, so the command
/// line, the REPL and the environment use one name.
inline const option_set hsb_option_set{ {
	{ "hsb-max-dnf-clauses", "hsb", std::size_t{ hsb_max_dnf_clauses },
		"cap the DNF clauses of one hsb normalization or split; past it "
		"normalization answers UNKNOWN (0 = unlimited)" },
} };

/// Binds each option of @ref hsb_option_set to its field. A cap changes
/// which constants fold, so a write empties the tree caches.
template <NodeType node>
result<void> hsb_bind_options(options_repository& repo) {
	result<void> r;
	const option_hook hook = ba_option_hook([] {
		tree<node>::clear_caches(); });
	TAU_TRY_VOID(repo.bind("hsb-max-dnf-clauses", hsb_max_dnf_clauses, hook));
	return r;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_OPTIONS_H__
