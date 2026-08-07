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
// For collect_used_ref_variables: treat_ex_quantified_clause needs to know
// whether the variable it is about to eliminate is entangled with an
// unresolved predicate reference. Self-contained (tau_tree/union_find/ba_types
// only), so it introduces no cycle with the block headers below.
#include "ref_variables_resolver.h"
#include "block_atom_profile.h"
#include "block_squeeze.h"
#include "boole_atom_analysis.h"

namespace idni::tau_lang {

// Step formulas normalized by the interpreter pay the block pipeline's
// expansion: on recurrent shapes `anti_prenex_block` can hand its own
// fallback a formula orders of magnitude larger than the block it was given,
// and that formula is what does not come back. Routing each quantifier block
// to the legacy path instead keeps the pieces small, which matters because
// the Boole decomposition is exponential in the atom count. Applies ONLY
// while the interpreter normalizes, so one-off queries keep the stock
// pipeline and its blasting-residue protection. Off by default; enable via
// `api::set_run_block_bailout` or TAU_RUN_BLOCK_BAILOUT.
inline bool run_block_bailout = false;
inline thread_local int interpreter_normalization_depth = 0;
struct interpreter_normalization_scope {
	interpreter_normalization_scope() { ++interpreter_normalization_depth; }
	~interpreter_normalization_scope() { --interpreter_normalization_depth; }
};


/**
 * @brief Apply the legacy, per-quantifier anti-prenex transformation.
 *
 * Walks @p formula bottom-up and, at every existentially quantified
 * subformula, alternates `push_existential_quantifier_one` (distribute the
 * binder over the connective below it) with `ex_quantified_boole_decomposition`
 * (split on one atomic formula) until the binder can no longer be moved or
 * split; a clause is then handed to `treat_ex_quantified_clause`, which may
 * remove it outright. Universal quantifiers are dualized by the surrounding
 * NNF machinery, not here.
 *
 * This is *not* the block-based algorithm. `anti_prenex_block` collects maximal
 * same-kind quantifier blocks and applies the chapter-5 fast paths to each, and
 * that is what the normalizer pipeline calls; `anti_prenex` is the fallback
 * `resolve_ex_block` reaches for when a block survives its own elimination.
 * Neither `anti_prenex` nor `resolve_quantifiers` accepts a `skip` predicate,
 * so content a caller reserved for blasting or the solver is protected on this
 * path only by the internal checks of the eliminators themselves.
 *
 * Inputs pass through `syntactic_formula_simplification` (whose last step is
 * `to_nnf`) before each elimination step, so a negated equation reaches the
 * eliminators as `bf_neq` rather than as `wff_neg` over `bf_eq`.
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
 * @brief Eliminate the binder of a single existentially quantified clause.
 *
 * Applies Corollary 2.3 of the Taba book once (it does not loop, and it never
 * calls `ex_quantified_boole_decomposition`): the clause's conjuncts are split
 * into those mentioning the bound variable and those that do not, the latter
 * being lifted out; a substitution-based witness is tried first
 * (`ex_subs_based_elimination`); bitvector content goes to the solver or to
 * predicate blasting; otherwise the positive equations are squeezed into a
 * single term `f` (`squeeze_positives`) and the binder removed via
 * `f[x<-0] & f[x<-1] = 0`, conjoined with the surviving disequations. The
 * repetition lives in `anti_prenex`'s own loop, not here.
 *
 * A conjunct that mentions the bound variable but still holds a quantifier or
 * an unresolved `wff_ref` blocks elimination: its equations are not top-level
 * conjuncts of this clause, so squeezing them would drop the inner binder and
 * leak its variable free.
 *
 * @pre The clause must be in NNF, i.e. a negated equation appears as `bf_neq`
 * and never as `wff_neg` over `bf_eq`. `squeeze_positives` selects `bf_eq`
 * atoms with `select_top`, which descends through `wff_neg`, so a `!(f = 0)`
 * conjunct would be squeezed as if it were positive and its negation silently
 * dropped. Note this is the *opposite* of `push_ex_block_into_clause`'s
 * precondition, which requires no `bf_neq` at all.
 * @tparam node Tree node type.
 * @param ex_clause An existentially quantified formula (a single clause).
 * @param[out] quant_eliminated Set to `true` if the quantifier was removed.
 * @return Simplified (possibly quantifier-free) formula.
 */
template <NodeType node>
tref treat_ex_quantified_clause(tref ex_clause, bool& quant_eliminated);

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
 * `push_quantifiers_in` and `anti_prenex` / `anti_prenex_block`.
 * @tparam node Tree node type.
 * @param formula Formula containing quantifiers to resolve.
 * @return Formula with bitvector-typed scopes decided or blasted where
 * possible; every other quantifier is preserved.
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
tref anti_prenex_block(tref formula, const std::function<bool(tref)>& skip);

template <NodeType node>
tref anti_prenex_block(tref formula);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ANTIPRENEXING_H__
