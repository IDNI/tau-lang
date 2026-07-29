// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file antiprenexing.h
 * @brief Anti-prenexing interfaces: pushing quantifiers inward and eliminating them.
 *
 * This file declares the anti-prenex conversion API. Template implementations
 * live in antiprenexing.tmpl.h, which -- unlike the usual convention in this
 * codebase -- is NOT included at the bottom of this header. The definitions
 * call back into normal_forms.tmpl.h helpers, one of which
 * (atm_formula_order_for_quant_elim) has a deduced return type and so cannot
 * be forward-declared. antiprenexing.tmpl.h is therefore included by
 * normal_forms.h *after* normal_forms.tmpl.h.
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
