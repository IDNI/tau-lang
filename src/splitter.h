// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file splitter.h
 * @brief Splitter functions for Tau satisfiability solving.
 *
 * Provides `tau_splitter` and `tau_bad_splitter`, which compute a splitter of
 * a normalized Tau formula: a satisfiable formula strictly implying it and
 * not equivalent to it. Internal helpers reside in `splitter.tmpl.h`.
 */

#ifndef __IDNI__TAU__SPLITTER_H__
#define __IDNI__TAU__SPLITTER_H__

#include "splitter_types.h"
#include "satisfiability.h"

namespace idni::tau_lang {

/**
 * @brief Return a "bad" splitter for @p fm.
 *
 * Conjoins `c != 0`, for a fresh uninterpreted tau constant `c`, to the left
 * disjunct of the first `wff_or` found in post-order, or to @p fm as a whole
 * when it has no disjunction. The result implies @p fm but is only trivially
 * smaller. Assumes @p fm is fully normalized by the normalizer.
 * @tparam BAs Boolean-algebra type pack.
 * @param fm Formula to produce a bad splitter for (defaults to `T`).
 * @return The bad splitter; never `nullptr`.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tref tau_bad_splitter(tref fm = tree<node<BAs...>>::_T());

/**
 * @brief Compute a splitter for a normalized DNF Tau formula.
 *
 * Assumes @p fm is normalized in DNF. A non-temporal @p fm is split by its
 * equalities, inequalities, coefficients or disjuncts; a temporal one clause
 * by clause through its `always`/`sometimes` parts, then by dropping a clause
 * the others do not imply. Every candidate is checked with the satisfiability
 * and equivalence procedures, so the cost is that of several solver calls.
 * @tparam BAs Boolean-algebra type pack.
 * @param fm Formula normalized in DNF.
 * @param st Splitter type selector; `splitter_type::bad` asks directly for
 *        a bad splitter of a non-temporal @p fm.
 * @return Result holding the splitter formula; a value is never `nullptr`.
 *         Falls back to a bad splitter (tau_bad_splitter) when no real
 *         splitter is found. A failed child report travels in the returned
 *         result.
 */
template <typename... BAs>
requires BAsPack<BAs...>
result<tref> tau_splitter(tref fm, splitter_type st);

} // namespace idni::tau_lang

#include "splitter.tmpl.h"

#endif //__IDNI__TAU__SPLITTER_H__
