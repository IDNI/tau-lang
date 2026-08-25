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
#include "eliminability.h"
#include "block_atom_profile.h"
#include "block_squeeze.h"
#include "boole_atom_analysis.h"

namespace idni::tau_lang {

/**
 * @brief The anti-prenex pipeline: push every quantifier as far inward as
 * possible, eliminating what can be eliminated on the way.
 *
 * NNF + syntactic simplification, substitution-based elimination, canonical
 * operator normalization, then maximal-block elimination
 * (`process_quantifier_blocks`), with quantifier ids canonicalised at entry
 * and exit. The two-argument overload takes an `eliminability` analysis
 * marking content this pass must not Boole-decompose (bitvector content
 * headed for the solver or blasting, reference-entangled variables); the
 * one-argument overload uses `eliminability<node>::bv_only()`.
 *
 * Until 2026-08-04 this name belonged to a step-based, per-quantifier
 * algorithm, and the block pipeline lived at `anti_prenex_block`; the legacy
 * algorithm was deleted once both full suites passed without it, and the
 * pipeline took the name. The block *core* -- the 5..8-argument recursion --
 * still goes by `anti_prenex_block`.
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

/** @brief The pipeline with an explicit eliminability analysis; see above. */
// Note: no default argument for `el` here -- function templates cannot
// gain a default argument in a later declaration once an earlier one (the
// forward declaration in heuristics/bv_predicate_blasting.h, included before
// this header via normal_forms_transformations.h) exists without one. The
// one-argument overload above plays the role of the default, calling through
// with eliminability<node>::bv_only().
template <NodeType node>
tref anti_prenex(tref formula, const eliminability<node>& el);

/**
 * @brief Decide or blast bitvector-typed quantifier scopes.
 *
 * Visits every quantified subformula whose bound variable is bitvector-typed
 * and tries, in order: the cvc5 solver on a closed scope; the solver on an
 * *open* scope by closing its free variables the two opposite ways (`all Y s`
 * sat means `s` is valid, `ex Y s` unsat means `s` is unsatisfiable); then
 * predicate blasting. A scope the solver cannot decide -- it answered
 * `unknown`, or the translation failed -- is left exactly as it was, since
 * "cannot decide" is not "false".
 *
 * Quantifiers over any other type are left untouched: this function neither
 * pushes quantifiers inward nor performs Boole decomposition. Those are
 * `anti_prenex`'s job.
 * @tparam node Tree node type.
 * @param formula Formula containing quantifiers to resolve.
 * @return Formula with bitvector-typed scopes decided or blasted where
 * possible; every other quantifier is preserved.
 */
template<NodeType node>
tref resolve_quantifiers(tref formula);

/**
 * @brief Last-resort complete elimination, one quantifier at a time, by
 * Boole/Shannon expansion (`eliminate_block_over_clause`'s squeeze, handed a
 * singleton block).
 *
 * `process_quantifier_block`'s pipeline above can give up with quantifiers
 * still standing: its own pivot selection for Boole-decomposition splitting
 * only ever picks a NON-negated atom, so a variable occurring solely in `!=`
 * atoms starves it of a pivot and it re-wraps the block instead of resolving
 * it. This distributes each remaining quantifier over its scope's
 * disjunction (`ex v (A|B) = (ex v A)|(ex v B)`) and eliminates `v` from
 * every resulting OR-free clause with the same squeeze a whole block uses --
 * which is sound for ANY Boolean algebra, atomless or atomic, not just
 * `bool`, and itself declines (keeping the binder) exactly where that is not
 * the case. A quantifier whose scope still holds a temporal operator (the
 * NZ-1 shape `resolve_quantifiers`' caller documents as genuinely
 * undecidable) is left exactly as found, before any of that is attempted.
 * @tparam node Tree node type.
 * @param formula Formula to eliminate remaining quantifiers from.
 * @return `formula` with every temporal-free quantifier this squeeze can
 * discharge eliminated; anything else survives quantified.
 */
template<NodeType node>
tref complete_quantifier_elimination(tref formula);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ANTIPRENEXING_H__
