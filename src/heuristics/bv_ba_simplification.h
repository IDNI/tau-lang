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
 * Repeatedly groups runs of `+`/`-`/`*`/`/` into blocks and folds their
 * constant operands together via `simplify_blocks`, iterating to a fixpoint
 * (cycle-detected via a visited-set, capped at 1,000,000 rounds).
 *
 * @tparam node Tree node type (despite the `BAs...` pack shown in this
 * declaration, the template actually instantiated -- see
 * bv_ba_custom_simplification.tmpl.h -- takes a single `NodeType node`; call
 * it as `bv_ba_custom_simplification<node_t>(term)`).
 * @param term BV term to simplify.
 * @return The simplified term.
 *
 * @par Example
 * @code{.cpp}
 * // {5}:bv[8] - {2}:bv[8] + {2}:bv[8] simplifies to {5}:bv[8] (see
 * // tests/integration/test_integration-heuristics-bv_ba_custom_simplification.cpp:62-69).
 * auto pbf = parse_bf();
 * tref src = tau::get("{5}:bv[8] - {2}:bv[8] + {2}:bv[8]", pbf);
 * tref simplified = bv_ba_custom_simplification<node_t>(src);
 * tref expected = tau::get("{5}:bv[8]", pbf);
 * CHECK( tau::get(simplified) == tau::get(expected) );
 * @endcode
 */
template <typename ... BAs> requires BAsPack<BAs...>
tref bv_ba_custom_simplification(tref term);

/**
 * @brief Simplify BV term @p term by invoking the cvc5 `simplify` procedure.
 *
 * Evaluates @p term into a cvc5 bitvector object, runs cvc5's own
 * simplifier on it, and translates the result back into a tau tree via
 * `cvc5_tree_to_tau_tree`.
 *
 * @tparam node Tree node type (see the note on @ref bv_ba_custom_simplification
 * about the `BAs...` pack shown here vs. the actual `NodeType node`
 * instantiation; call it as `bv_ba_cvc5_simplification<node_t>(term)`).
 * @param term BV term to simplify.
 * @return The simplified term.
 *
 * @par Example
 * @code{.cpp}
 * // {1}:bv[8] + {2}:bv[8] + {3}:bv[8] simplifies to {6}:bv[8] (see
 * // tests/integration/test_integration-heuristics-bv_ba_cvc5_simplification.cpp:37-45).
 * auto pbf = parse_bf();
 * tref src = tau::get("{1}:bv[8] + {2}:bv[8] + {3}:bv[8]", pbf);
 * tref simplified = bv_ba_cvc5_simplification<node_t>(src);
 * tref expected = tau::get("{6}:bv[8]", pbf);
 * CHECK( tau::get(simplified) == tau::get(expected) );
 * @endcode
 */
template<typename ... BAs> requires BAsPack<BAs...>
tref bv_ba_cvc5_simplification(tref term);

} // namespace idni::tau_lang

#include "bv_ba_custom_simplification.tmpl.h"
#include "bv_ba_cvc5_simplification.tmpl.h"

#endif // __IDNI__TAU__BA_BV_SIMPLIFICATION_H__