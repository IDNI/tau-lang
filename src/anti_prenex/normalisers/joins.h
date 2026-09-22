// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file joins.h
 * @brief §3 "The result joins": `SIMPLIFIED_AND_JOIN` and
 * `SIMPLIFIED_OR_JOIN`, the one way the algorithm assembles
 * already-simplified formulas into a conjunction or a disjunction.
 *
 * INCREMENTAL (invariant 7): a deciding operand stops the later ones from
 * being built at all, so the builder takes one operand at a time and answers
 * `decided()` in between; the n-ary functions are loops over it.
 *
 * SHALLOW (§3): every operand is already simplified (invariant 6), so only
 * the members' TOP-LEVEL interaction is new — the six insert rules and the
 * one absorption pass. A member is never descended into, and no equality
 * propagation happens between members; that belongs to `SIMPLIFY`.
 *
 * AC-CANONICAL: insertion order drives the short-circuits, but the assembled
 * node is a function of the member SET — flattening gives associativity, the
 * content-order emission of `canonical_and`/`canonical_or` (dag.h) gives
 * commutativity — so equal member sets build equal nodes and equal memo keys.
 *
 * The construction hooks fold `T`/`F`, `X ∧ X` and `X ∧ ¬X`; the insert rules
 * are those same facts one step earlier, where they can still stop an operand
 * from being built. The absorption pass has no counterpart in the codebase:
 * the hooks are binary and local and the path sweep
 * (`heuristics/syntactic_path_simplification`) works down a path, so neither
 * sees `d ∧ (d ∨ e)` as one top-level member interaction.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__NORMALISERS__JOINS_H__
#define __IDNI__TAU__ANTI_PRENEX__NORMALISERS__JOINS_H__

#include "../foundations/fwd.h"
#include "../foundations/options.h"
#include "../foundations/dag.h"

namespace idni::tau_lang::anti_prenexing {

/**
 * @brief §3 the result joins as one incremental builder: `conj = true` is
 * `SIMPLIFIED_AND_JOIN`, `conj = false` is `SIMPLIFIED_OR_JOIN`, the dual
 * with `T` and `F` swapped.
 *
 * The members are kept in insertion order, which is the order the insert
 * rules see; the emission order is the content order of §1.
 *
 * Every operand is a `wff` node. A member taken out of a chain carries its
 * right sibling, so the member set is keyed by CONTENT
 * (`subtree_unordered_set` ignores the right sibling in both its hash and its
 * equality) and `canonical_and`/`canonical_or` trims what it emits.
 */
template <NodeType node, bool conj>
struct join_builder {
	/**
	 * @brief The six insert rules of §3, in the ∧ form (the ∨ form is the
	 * dual): `T` is skipped; `F` decides; a same-connective operand
	 * contributes its members, flattened; an operand that is already a
	 * member is skipped; a LITERAL whose complement is a member decides;
	 * anything else is added.
	 *
	 * @return `true` when the join is DECIDED (`F` for ∧, `T` for ∨).
	 * Later inserts are no-ops; invariant 7 asks the caller not to build
	 * the operand at all.
	 */
	bool insert(tref r);

	/// `true` once a deciding operand has been inserted.
	bool decided() const;

	/**
	 * @brief The absorption pass, then the raw canonical constructor
	 * (`canonical_and`/`canonical_or`, dag.h): the surviving members in
	 * content order, deduplicated, folded through the hooked binary
	 * builder.
	 *
	 * A decided join is the deciding constant (`F` for ∧, `T` for ∨); an
	 * empty join is the neutral one (`T` for ∧, `F` for ∨); one member is
	 * that member. Nothing is consumed: two calls answer alike.
	 */
	tref result();

private:
	/// One operand that is not the join's own connective: rules 1, 2, 4, 5
	/// and 6. Returns `decided()`.
	bool insert_member(tref m);

	trefs ms;                             ///< the members, insertion order
	subtree_unordered_set<node> present;  ///< the same members, by content
	bool is_decided = false;
};

/**
 * @brief §3 `SIMPLIFIED_AND_JOIN(r₁, …, rₙ)`: the n-ary form, a loop over
 * `join_builder<node, true>` that stops at the first deciding operand. An
 * empty join is `T`.
 *
 * Its operands are already built, so invariant 7's "never built" is beyond
 * it: a caller whose operands are recursions drives the builder itself.
 */
template <NodeType node>
tref simplified_and_join(const trefs& rs);

/// §3 `SIMPLIFIED_OR_JOIN(r₁, …, rₙ)`: the dual. An empty join is `F`.
template <NodeType node>
tref simplified_or_join(const trefs& rs);

} // namespace idni::tau_lang::anti_prenexing

#include "joins.tmpl.h"

#endif // __IDNI__TAU__ANTI_PRENEX__NORMALISERS__JOINS_H__
