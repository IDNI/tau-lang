// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file simplify_using_equality.h
 * @brief Equality-aware formula simplification heuristic.
 *
 * Traverses a formula in pre-order and, at each `bf_eq` node, records the
 * equality in a union-find structure (stacked per disjunction branch). Later
 * occurrences of terms known to be equal are replaced by their canonical
 * representative, potentially simplifying the whole formula to `T` or `F`.
 */

#ifndef __IDNI__TAU__SIMPLIFY_USING_EQUALITY_H__
#define __IDNI__TAU__SIMPLIFY_USING_EQUALITY_H__

namespace idni::tau_lang {

/**
 * @brief Simplify @p fm by propagating equalities found in conjunctions.
 *
 * Converts @p fm to NNF, then traverses it in pre-order. At each `bf_eq`
 * conjunct, the equality is registered in a scoped union-find; later
 * sub-terms equal to a known representative are replaced by it. Each
 * disjunction branch gets its own scope so that assumptions from one
 * branch never leak into another.
 *
 * @tparam node Tree node type.
 * @param fm Formula to simplify.
 * Sub-formulas under a quantifier, a temporal quantifier, or an implication,
 * equivalence, xor or conditional are not entered.
 *
 * @return Simplified formula; `T`/`F` when a branch is trivially true/false.
 * An `internal_error` when the union-find scopes do not balance.
 *
 * @par Example
 * @code{.cpp}
 * // "xy|zx = 0 && xy = 0": registering xy = 0 lets the other clause absorb
 * // it (see case "1" of the simplify_using_equality suite in
 * // tests/integration/test_integration-heuristics-simplify_using_equality.cpp).
 * tref fm = get_nso_rr("xy|zx = 0 && xy = 0.").value().main->get();
 * auto res = simplify_using_equality<node_t>(fm);
 * // res.value() matches "yx|zx = 0" up to and/or operand order
 * // (matches_wff_mod_and_or_any_of)
 * @endcode
 */
template <NodeType node>
result<tref> simplify_using_equality(tref fm);

} // namespace idni::tau_lang

#include "simplify_using_equality.tmpl.h"

#endif // __IDNI__TAU__SIMPLIFY_USING_EQUALITY_H__
