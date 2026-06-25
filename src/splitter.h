// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file splitter.h
 * @brief Splitter functions for Tau satisfiability solving.
 *
 * Provides `tau_splitter`, `tau_bad_splitter`, and the `split_sym` tag that
 * decompose a normalized Tau formula into an atomic "splitter" — a Boolean
 * function used by the satisfiability solver to guide case analysis.
 * Internal helpers reside in `splitter.tmpl.h`.
 */

#ifndef __IDNI__TAU__SPLITTER_H__
#define __IDNI__TAU__SPLITTER_H__

#include "splitter_types.h"
#include "satisfiability.h"

namespace idni::tau_lang {

/**
 * @brief Identifies whether a conjunction or disjunction drives a split.
 */
enum class split_sym {
	conjunction, ///< Split along a conjunction.
	disjunction  ///< Split along a disjunction.
};

/**
 * @brief Return a "bad" splitter (the `T` constant) for @p fm.
 *
 * Assumes @p fm is fully normalized by the normalizer.
 * @tparam BAs Boolean-algebra type pack.
 * @param fm Formula to produce a bad splitter for (default: `T`).
 * @return `T` constant node as a bad splitter.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tref tau_bad_splitter(tref fm = tree<node<BAs...>>::_T());

/**
 * @brief Compute a splitter for a normalized DNF Tau formula.
 *
 * Assumes @p fm is normalized in DNF.
 * @tparam BAs Boolean-algebra type pack.
 * @param fm Formula normalized in DNF.
 * @param st Splitter type selector.
 * @return Splitter formula, or `nullptr` if none found.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tref tau_splitter(tref fm, splitter_type st);

} // namespace idni::tau_lang

#include "splitter.tmpl.h"

#endif //__IDNI__TAU__SPLITTER_H__
