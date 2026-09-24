// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file parts.h
 * @brief §6 `PARTS` and `INCIDENCE`: the split of a conjunct list into the
 * connected components of its BLOCK variables, the same split with the
 * variables that are already settled read off along the way, and the parts a
 * predicate takes whole.
 *
 * TWO CONJUNCTS ARE CONNECTED when they share a variable of the block `X`.
 * Everything else they share — a free variable outside `X`, a constant, a
 * reference — connects nothing: the split exists so that a quantifier over
 * `X` can be pushed into each part separately, and only an `X` variable
 * holds two conjuncts under one quantifier (invariant 2). The relation is
 * closed transitively, so a part is a connected component, not a pair.
 *
 * A CONJUNCT TOUCHING NO VARIABLE OF `X` IS LEGAL and becomes a part of its
 * own with empty `vars`, merging with nothing, so that the parts still
 * partition the input. `INCIDENCE` flags nothing for such a conjunct: it
 * holds no block variable to flag.
 *
 * DETERMINISTIC AND CONTENT-DERIVED, like every order in the module (§1): the
 * parts come in the order of their FIRST conjunct, a part's conjuncts in
 * input order, and a part's variables in `X`'s order. Nothing depends on how
 * the union-find happened to root its classes.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__SHARED__PARTS_H__
#define __IDNI__TAU__ANTI_PRENEX__SHARED__PARTS_H__

#include <vector>

#include "../foundations/fwd.h"
#include "../foundations/dag.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §6 one PART: the conjuncts of one connected component together with
 * the block variables they hold.
 *
 * `vars` is `X ∩ FV(⋀conjuncts)` in `X`'s order — the sub-block the caller
 * quantifies over this part — and `conjuncts` is a sublist of the input, in
 * input order. The parts of one call partition the input list. `vars` is
 * EMPTY for the part of a conjunct that touches no variable of `X`, which is
 * a part of one conjunct.
 */
template <NodeType node>
struct part {
	trefs conjuncts;
	block vars;
};

/**
 * @brief §6 `PARTS(conjuncts, X)`: the conjuncts grouped into the connected
 * components of block-variable co-occurrence.
 *
 * ONE PASS over cached free-variable sets — one intersection per conjunct, no
 * descent into any of them (§1) — and near-linear in the input.
 */
template <NodeType node>
std::vector<part<node>> parts(const trefs& conjuncts, const block& X);

/**
 * @brief The two sides of `partition_parts`: the parts the predicate takes,
 * and the rest.
 *
 * `hit` and `rest` are sublists of the input in input order, `hit_vars` and
 * `rest_vars` sublists of the block in the block's order. No block variable
 * of one side occurs on the other.
 */
template <NodeType node>
struct parts_partition {
	trefs hit;
	block hit_vars;
	trefs rest;
	block rest_vars;
};

/**
 * @brief §6 `PARTS` closed under a predicate: every part holding a conjunct
 * @p hit holds of is taken WHOLE, and the conjuncts and `X` split into the
 * taken side and the rest.
 *
 * Parts share no block variable, so `∃X(H ∧ R) ≡ ∃X_H.H ∧ ∃X_R.R`. Both
 * sides keep input order and `X`'s order. A block variable lands on the side
 * of its part; one that no conjunct holds lands on the rest's.
 *
 * @param hit a predicate `bool(tref conjunct)`, asked of a conjunct at most
 *        once
 */
template <NodeType node, typename Hit>
parts_partition<node> partition_parts(const trefs& conjuncts, const block& X,
	Hit&& hit);

/**
 * @brief §6 `INCIDENCE`'s result: the parts, plus `Xs`, the block variables
 * that are SETTLED.
 *
 * `settled` is `X`'s order, and holds every variable of `X` that no
 * disjunctive conjunct touches — a negative tree excepted, which the leaf
 * methods take whole (§1) and which therefore settles nothing open. A
 * variable outside every conjunct's `X`-intersection is settled too: nothing
 * in the list keeps it open.
 */
template <NodeType node>
struct incidence_result {
	std::vector<part<node>> parts;
	block settled;
};

/**
 * @brief §6 `INCIDENCE(conjuncts, X)`: `PARTS` plus ONE kind test per
 * conjunct.
 *
 * A variable is FLAGGED when it occurs in a conjunct that is an ∨-node and is
 * not a negative tree (one cached test, dag.h), because such a conjunct still
 * has to be expanded or decomposed before the variable is done. `settled` is
 * what is left of `X`. The parts are exactly `parts(conjuncts, X)`, computed
 * in the same pass.
 */
template <NodeType node>
incidence_result<node> incidence(const trefs& conjuncts, const block& X);

} // namespace idni::tau_lang::anti_prenexing

#include "parts.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__SHARED__PARTS_H__
