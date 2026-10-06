/**
 * @file simplify_options.h
 * @brief Runtime parameter bounding `bv_ba_custom_simplification`.
 *
 * Deliberately dependency-free, like `heuristics/preprocess_placement.h`: the
 * algorithm (`boolean_algebras/bv/heuristics/bv_ba_custom_simplification.tmpl.h`)
 * is only reachable through `boolean_algebras/bv/bv_ba.h`, which is outside
 * `api.h`'s include chain, while the api setter
 * (`api<node>::set_max_simplify_rounds`) must see the
 * knob's declaration wherever `api.h` is included -- including in a pack
 * without bv, where that plugin header does not exist at all. Keeping the
 * knob here, algebra-neutral, lets both sides include it without dragging
 * cvc5 or the bv solver along.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__SIMPLIFY_OPTIONS_H__
#define __IDNI__TAU__SIMPLIFY_OPTIONS_H__

#include <cstddef>
#include "env_limits.h"

namespace idni::tau_lang {

/// Cap on `bv_ba_custom_simplification`'s rewrite-to-fixpoint rounds; 0 =
/// unlimited (the default). Oscillating cycles are caught by a visited set
/// regardless; only an ever-growing rewrite is unbounded when unlimited.
/// Runtime parameter by policy: set via `--max-simplify-rounds`, REPL
/// `simplifyrounds`, or `api<node>::set_max_simplify_rounds`. Like `preprocessing`,
/// NOT thread-safe: the tau library assumes single-threaded access.
/// Environment fallback `TAU_MAX_SIMPLIFY_ROUNDS`.
inline env_limit<size_t> max_simplify_rounds{ "TAU_MAX_SIMPLIFY_ROUNDS", 0 };

} // namespace idni::tau_lang

#endif // __IDNI__TAU__SIMPLIFY_OPTIONS_H__
