// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file parts.h
 * @brief §6 `PARTS` and `INCIDENCE`: the split of a conjunct list into the
 * connected components of its BLOCK variables, and the same split with the
 * variables that are already settled read off along the way.
 *
 * TWO CONJUNCTS ARE CONNECTED when they share a variable of the block `X`.
 * Everything else they share — a free variable outside `X`, a constant, a
 * reference — connects nothing: the split exists so that a quantifier over
 * `X` can be pushed into each part separately, and only an `X` variable
 * holds two conjuncts under one quantifier (invariant 2). The relation is
 * closed transitively, so a part is a connected component, not a pair.
 *
 * The consumers in §7 are `FREEZE_OPAQUE_COMPONENTS`, which freezes a whole
 * part around an opaque conjunct, and `SQUEEZE`, which squeezes the positives
 * of one part into one equation; §6 `PUSH_OVER_CONJUNCTION` and the finite
 * method's second tier follow in later layers.
 *
 * A CONJUNCT TOUCHING NO VARIABLE OF `X` IS LEGAL and becomes a part of its
 * own with empty `vars`, merging with nothing, so that the parts still
 * partition the input. `SQUEEZE` is where one arises: the zero form of a
 * positive can strip its last block variable (`x + b = x + c` gives
 * `b + c = 0`), and `DISCHARGE` over the empty block then emits that atom
 * unchanged (§7). `INCIDENCE` flags nothing for such a conjunct: it holds no
 * block variable to flag.
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
