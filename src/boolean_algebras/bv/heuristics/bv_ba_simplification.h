/**
 * @file bv_ba_simplification.h
 * @brief BV Boolean-algebra simplification passes.
 *
 * Provides two simplification strategies for BV terms:
 * - `bv_ba_custom_simplification` — algebraic rewrites without calling cvc5.
 * - `bv_ba_cvc5_simplification`   — delegates to cvc5's simplifier.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BA_BV_SIMPLIFICATION_H__
#define __IDNI__TAU__BA_BV_SIMPLIFICATION_H__

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

/**
 * @brief Simplify BV term @p term using custom algebraic rules (no cvc5 call).
 *
 * Repeatedly groups runs of `+`/`-` and of `*` into blocks and folds their
 * constant operands together via `simplify_blocks`, iterating to a fixpoint
 * (cycle-detected via a visited-set, capped by the runtime option
 * `max_simplify_rounds`, 0 = unlimited). Additive blocks fold fully (Z/2^n is
 * a group under `+`); a `/` is never part of a block and stays an opaque
 * operand, since `bvudiv` neither inverts `bvmul` nor reassociates.
 *
 * @note Fallback only by contract: `simplify_bv_term` dispatches
 * @ref bv_ba_cvc5_simplification first and calls this one only when that
 * returns `nullptr`.
 *
 * @tparam node Tree node type.
 * @param term BV term to simplify.
 * @return The simplified term (never a decline); when the round cap is hit
 * the partial simplification comes with a warning; an error only when a
 * block cannot be rebuilt.
 *
 * @par Example
 * @code{.cpp}
 * // {5}:bv[8] - {2}:bv[8] + {2}:bv[8] simplifies to {5}:bv[8] (see
 * // src/boolean_algebras/bv/tests/test_integration-heuristics-bv_ba_custom_simplification.cpp).
 * auto pbf = parse_bf();
 * tref src = tau::get("{5}:bv[8] - {2}:bv[8] + {2}:bv[8]", pbf);
 * tref simplified = bv_ba_custom_simplification<node_t>(src).value_or(nullptr);
 * tref expected = tau::get("{5}:bv[8]", pbf);
 * CHECK( tau::get(simplified) == tau::get(expected) );
 * @endcode
 */
template <NodeType node>
result<tref> bv_ba_custom_simplification(tref term);

/**
 * @brief Simplify BV term @p term by invoking the cvc5 `simplify` procedure.
 *
 * Evaluates @p term into a cvc5 bitvector object, runs cvc5's own
 * simplifier on it (`normalize_bv`), and translates the result back into a
 * tau tree via `cvc5_tree_to_tau_tree`.
 *
 * @tparam node Tree node type.
 * @param term BV term to simplify.
 * @return The simplified term; a decline (no value, no error) when the
 * back-translation cannot produce a term, which `simplify_bv_term` relies on
 * to fall back to @ref bv_ba_custom_simplification; an error when
 * `bv_eval_node` fails on @p term.
 *
 * @par Example
 * @code{.cpp}
 * // {1}:bv[8] + {2}:bv[8] + {3}:bv[8] simplifies to {6}:bv[8] (see
 * // src/boolean_algebras/bv/tests/test_integration-heuristics-bv_ba_cvc5_simplification.cpp).
 * auto pbf = parse_bf();
 * tref src = tau::get("{1}:bv[8] + {2}:bv[8] + {3}:bv[8]", pbf);
 * tref simplified = bv_ba_cvc5_simplification<node_t>(src).value_or(nullptr);
 * tref expected = tau::get("{6}:bv[8]", pbf);
 * CHECK( tau::get(simplified) == tau::get(expected) );
 * @endcode
 */
template <NodeType node>
result<tref> bv_ba_cvc5_simplification(tref term);

} // namespace idni::tau_lang

#include "bv_ba_custom_simplification.tmpl.h"
#include "bv_ba_cvc5_simplification.tmpl.h"

#endif // __IDNI__TAU__BA_BV_SIMPLIFICATION_H__