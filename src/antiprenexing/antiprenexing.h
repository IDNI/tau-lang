// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file antiprenexing.h
 * @brief Anti-prenexing interfaces: pushing quantifiers inward and eliminating them.
 *
 * This file declares the anti-prenex conversion API. Template implementations
 * live in antiprenexing.tmpl.h, which -- unlike the usual convention in this
 * codebase -- is NOT included at the bottom of this header. The definitions
 * call back into normal_forms.tmpl.h helpers (normalize_atomic_formula_operators,
 * term_boole_decomposition, squeeze_absorb, atm_formula_order_for_quant_elim)
 * that are internal to normal_forms.tmpl.h and have no header declarations of
 * their own. So src/antiprenexing/ is not a standalone module here -- it is a
 * continuation of normal_forms.tmpl.h's internal scope, and antiprenexing.tmpl.h
 * is included by normal_forms.h *after* normal_forms.tmpl.h so those helpers
 * are already in scope, instead of inventing header declarations for them.
 *
 * Note: as a side effect of this move, these definitions now log under the
 * "anti_prenex" channel rather than their former "normal_forms" (slab C) and
 * "assign_and_reduce" (slab A) channels; all three are currently commented out
 * in LOG_ENABLED_CHANNELS (src/logging.h).
 */

#ifndef __IDNI__TAU__ANTIPRENEXING_H__
#define __IDNI__TAU__ANTIPRENEXING_H__

#include <functional>

#include "tau_tree.h"
#include "block_atom_profile.h"
#include "block_squeeze.h"
#include "boole_atom_analysis.h"

namespace idni::tau_lang {

/**
 * @brief Apply the anti-prenex transformation to a formula.
 *
 * Drives the full anti-prenex procedure: converts to NNF, identifies
 * quantifier blocks, and calls `anti_prenex_block` to push quantifiers into
 * the formula structure as deeply as possible.
 * @tparam node Tree node type.
 * @param formula Formula to anti-prenex.
 * @return Formula with quantifiers pushed in as far as possible.
 *
 * @par Example
 * @code{.cpp}
 * // The inner "ex o2[1],o1[1] o1[1]o2[1]=0" is always satisfiable (pick
 * // o1[1]=o2[1]=0), so the whole formula reduces to a tautology once the
 * // quantifier is pushed in and resolved (see
 * // tests/integration/test_integration-wff_normalization.cpp:131-136).
 * tref fm = get_nso_rr(
 *     "all o1[0], o2[0] !o1[0]o2[0] = 0 || o1[0]o2[0] = 0 && "
 *     "(ex o2[1], o1[1] o1[1]o2[1] = 0).").value().main->get();
 * tref res = anti_prenex<node_t>(fm);
 * CHECK( tau::get(res).equals_T() );
 * @endcode
 */
template <NodeType node>
tref anti_prenex(tref formula);

/**
 * @brief Process a single existentially quantified clause in the anti-prenex algorithm.
 *
 * Applies `ex_quantified_boole_decomposition` repeatedly until the quantifier is
 * eliminated or no further simplification is possible.
 * @tparam node Tree node type.
 * @param ex_clause An existentially quantified formula (a single clause).
 * @param[out] quant_eliminated Set to `true` if the quantifier was removed.
 * @return Simplified (possibly quantifier-free) formula.
 */
template <NodeType node>
tref treat_ex_quantified_clause(tref ex_clause, bool& quant_eliminated);

/**
 * @brief Resolve/eliminate quantifiers in a formula.
 *
 * Pushes quantifiers inward (via `push_quantifiers_in`), then applies Boole
 * decomposition to eliminate remaining existential quantifiers. Handles
 * bitvector formulas by delegating to the CVC5 solver when the formula is closed.
 * @tparam node Tree node type.
 * @param formula Formula containing quantifiers to eliminate.
 * @return Quantifier-free formula, or a formula with as few quantifiers as possible.
 */
template<NodeType node>
tref resolve_quantifiers(tref formula);

/**
 * @brief Skip predicate that skips nothing; suitable as `anti_prenex_block`'s
 * `skip` argument when no content should be deferred to blasting.
 * @tparam node Tree node type.
 * @param t tref (unused).
 * @return Always `false`.
 */
template <NodeType node>
bool no_skip(tref t);

// Note: no default argument for `skip` here -- function templates cannot
// gain a default argument in a later declaration once an earlier one (the
// forward declaration in heuristics/bv_predicate_blasting.h, included before
// this header via normal_forms_transformations.h) exists without one. The
// single-argument overload below plays the role of the default, calling
// through with is_tref_bv_type_family<node>.
template <NodeType node>
tref anti_prenex_block(tref formula, std::function<bool(tref)> skip);

template <NodeType node>
tref anti_prenex_block(tref formula);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ANTIPRENEXING_H__
