/**
 * @file bv_simplify_options.h
 * @brief Runtime parameter bounding `bv_ba_custom_simplification`.
 *
 * Deliberately dependency-free, like `heuristics/blast_placement.h`: the
 * algorithm (heuristics/bv_ba_custom_simplification.tmpl.h) is only reachable
 * through `boolean_algebras/bv_ba.h`, which is outside `api.h`'s include
 * chain, while the api setter (`api::set_max_simplify_rounds`) must see the
 * knob's declaration wherever `api.h` is included. Keeping the knob here lets
 * both sides include it without dragging cvc5 or the bv solver along.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BV_SIMPLIFY_OPTIONS_H__
#define __IDNI__TAU__BV_SIMPLIFY_OPTIONS_H__

#include <cstddef>

namespace idni::tau_lang {

/// Cap on `bv_ba_custom_simplification`'s rewrite-to-fixpoint rounds; 0 =
/// unlimited (the default). Oscillating cycles are caught by a visited set
/// regardless; only an ever-growing rewrite is unbounded when unlimited.
/// Runtime parameter by policy: set via `--max-simplify-rounds`, REPL
/// `simplifyrounds`, or `api::set_max_simplify_rounds`. Like `bv_blasting`,
/// NOT thread-safe: the tau library assumes single-threaded access.
inline size_t max_simplify_rounds = 0;

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BV_SIMPLIFY_OPTIONS_H__
