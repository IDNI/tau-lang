// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file leaf_clause.h
 * @brief Eliminating a quantifier block over a single dependent clause.
 *
 * Merges what `push_ex_block_into_clause` and `treat_ex_quantified_clause`
 * used to do separately: lift the conjuncts no block variable touches,
 * partition the rest by eliminability component, try a substitution witness
 * per live variable, hand bitvector content to the solver or to blasting, and
 * otherwise squeeze the positive equations into a single term and drop the
 * binder.
 *
 * Split out of `antiprenexing.tmpl.h` because that file's block core is
 * already large; merging the clause logic into it would make both unreadable.
 *
 * Like the rest of `src/antiprenexing/`, this header does NOT include its own
 * `.tmpl.h` -- see the comment at `normal_forms.h`'s include block for why the
 * order there is load-bearing.
 */

#ifndef __IDNI__TAU__ANTIPRENEXING_LEAF_CLAUSE_H__
#define __IDNI__TAU__ANTIPRENEXING_LEAF_CLAUSE_H__

#include <functional>

#include "tau_tree.h"
#include "eliminability.h"

namespace idni::tau_lang {

// Forward declaration, same reason and same shape as the one in
// heuristics/bv_predicate_blasting.h: the blast-and-re-enter branch of
// eliminate_block_over_clause calls back into the block driver, whose
// definition lands later in normal_forms.h's include order. No default
// argument here -- a default cannot be redeclared, and that call site always
// passes an explicit analysis.
template<NodeType node>
tref anti_prenex(tref formula, const eliminability<node>& el);

/**
 * @brief Eliminate @p block over the single clause @p clause.
 *
 * @pre @p clause is a conjunction with no top-level disjunction: the block
 * core splits disjunctions before reaching here.
 *
 * Preconditions on atom spelling are established **locally**, not inherited
 * from the caller: the block squeeze path normalises to `!(= 0)` first, the
 * single-variable squeeze path runs `to_nnf` first. They genuinely differ --
 * see the file comment in `leaf_clause.tmpl.h` before touching either.
 *
 * @tparam node Tree node type.
 * @param clause Clause to eliminate over.
 * @param block Block variables, outermost first.
 * @param elim Verdicts for @p block against @p clause's conjuncts.
 * @param order BDD variable order for the block.
 * @return Clause with the eliminable part of the block removed and the frozen
 * part re-wrapped around only its own conjuncts.
 */
template <NodeType node>
tref eliminate_block_over_clause(tref clause, const trefs& block,
	const block_eliminability<node>& elim,
	const typename term_handle<node>::order& order);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ANTIPRENEXING_LEAF_CLAUSE_H__
