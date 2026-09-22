// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file junctions.tmpl.h
 * @brief Template implementations for junctions.h. junctions.h says what each
 * procedure means; the comments here say how it is built.
 *
 * The pieces come from the files below: `members`, `formula_size` and
 * `fv_meets` (foundations/dag.h) for the member lists and the orders,
 * `incidence` (shared/parts.h) for the narrowing, the result joins
 * (normalisers/joins.h) for every assembly, `eliminate_block`
 * (eliminate/eliminate_block.h) for the sub-block and the consistency
 * question, `try_fast_paths` (push/fast_paths.h) for 2a and 2b,
 * `try_witness` and `try_case_witness` (witness/witness.h) for the two pin
 * matches, `subst_var` (foundations/subst.h) with `simplify`
 * (normalisers/simplify.h) for a branch's copy, and `rewrap`
 * (foundations/prims.h) for the floor.
 *
 * `push_block` — the hub, `push/push_block.h` — is what every re-entry goes
 * through, so each of them narrows `X` and strips the conjuncts the narrowed
 * block no longer touches on its own.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__PUSH__JUNCTIONS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__PUSH__JUNCTIONS_TMPL_H__

#include <algorithm>
#include <cassert>
#include <optional>
#include <utility>

namespace idni::tau_lang::anti_prenexing {

namespace junctions_detail {

/// `X ∖ ys`, KEEPING `X`'s order: the smaller block a step hands on once it
/// has taken variables out of `X`. Two short lists, so a linear scan.
template <NodeType node>
block without(const block& X, const block& ys) {
	using tau = tree<node>;
	block out = X;
	std::erase_if(out, [&ys](tref v) {
		return std::any_of(ys.begin(), ys.end(),
			[v](tref y) { return tau::subtree_equals(v, y); }); });
	return out;
}

} // namespace junctions_detail

// --- 2d ---------------------------------------------------------------------------

template <NodeType node>
tref push_over_disjunction(tref phi, const block& X, ctx<node>& c) {
	DBG(assert(phi != nullptr && !X.empty());)
	DBG(assert(is_child<node>(phi, tree<node>::wff_or));)
	trefs ds = members<node>(phi);
	// SMALLEST FIRST, stably: `|d|` is the cost prediction, and ties keep
	// the member view's order, which is content order.
	std::stable_sort(ds.begin(), ds.end(), [](tref a, tref b) {
		return formula_size<node>(a) < formula_size<node>(b); });
	join_builder<node, false> acc;
	for (tref d : ds)
		// The wrapper narrows `X` to the disjunct itself. A `T` decides
		// the join, and the disjuncts behind it are never pushed
		// (invariant 7).
		if (acc.insert(push_block<node>(d, X, c))) break;
	return acc.result();
}

// --- the conjunction's ladder -------------------------------------------------------

template <NodeType node>
tref push_over_conjunction(tref psi, const block& X, ctx<node>& c) {
	using tau = tree<node>;
	using namespace junctions_detail;
	DBG(assert(psi != nullptr && !X.empty());)
	DBG(assert(is_child<node>(psi, tau::wff_and));)
	// THE CONJUNCT LIST, taken once: every sub-formula below is a join of
	// members of it, and every re-entry is a `push_block`, which takes its
	// own view of what it is given.
	const trefs cs = members<node>(psi);

	// 1. SCOPE NARROWING, one incidence pass for both moves.
	const incidence_result<node> inc = incidence<node>(cs, X);
	if (inc.parts.size() > 1) {
		// THE SPLIT: no block variable holds two parts together, so
		// each part is pushed under its own variables and the results
		// are conjoined. `p.vars` is `X ∩ FV(⋀p)` in `X`'s order.
		join_builder<node, true> acc;
		for (const part<node>& p : inc.parts)
			if (acc.insert(push_block<node>(
				simplified_and_join<node>(p.conjuncts),
				p.vars, c))) break;   // eager F (invariant 7)
		return acc.result();
	}
	if (!inc.settled.empty()) {
		// THE SETTLE MOVE. `A` is the conjuncts the settled variables
		// touch — plain conjuncts and negative trees, by the way
		// `settled` is read off — and `Xs ∩ FV(B) = ∅` for the rest,
		// so the sub-block `∃Xs.A` is an equivalence.
		trefs A, others;
		for (tref m : cs)
			(fv_meets<node>(m, inc.settled) ? A : others)
				.push_back(m);
		// A SUB-BLOCK of the component, eliminated under the
		// component's live order. Under `keep_functional` its emission
		// hides the variables of `X ∖ Xs` it carries (§7 `DISCHARGE`,
		// §1 the leaf hazard), and they ride on as a dependent
		// conjunct.
		const tref r = eliminate_block<node>(
			simplified_and_join<node>(A), inc.settled, c);
		// Eager F (invariant 7): no sibling can make an unsatisfiable
		// sub-block satisfiable.
		if (tau::get(r).equals_F()) return _F<node>();
		// Everything settled: the block is pushed home.
		if (inc.settled.size() == X.size()) return r;
		trefs rest{ r };
		rest.insert(rest.end(), others.begin(), others.end());
		return push_block<node>(simplified_and_join<node>(rest),
			without<node>(X, inc.settled), c);
	}

	// 2. THE FAST PATHS, on the narrowed `ψ`: the wrapper stripped the
	// X-free conjuncts, so 2b's cross product carries nothing it need not.
	if (const std::optional<tref> r = try_fast_paths<node>(psi, X, c))
		return *r;

	// 3. THE CONSISTENCY CHECK, before anything multiplies (invariant 8).
	// The squeeze of the positives, discharged per component, folds to `F`
	// exactly when some component's meet of them is a nonzero constant —
	// joint over every positive and every block variable of that
	// component. Otherwise such a body is multiplied into cases that each
	// die at their own leaf. PURE positives only: anything else would
	// freeze or reach the solver. The elimination narrows `X` to `⋀P`
	// itself, so 2a and this ask `elim_memo` the same question.
	trefs P;
	for (tref m : cs)
		if (is_child<node>(m, tau::bf_eq)
			&& is_pure_equation<node>(m, c.type)) P.push_back(m);
	if (!P.empty() && tau::get(eliminate_block<node>(
		simplified_and_join<node>(P), X, c)).equals_F())
			return _F<node>();

	// 4. THE WITNESS STEP: one pin scan per block variable, no copies. The
	// pin deletes its binder and the residual it leaves is free of it, so
	// the re-entry is over a strictly smaller block.
	for (tref x : X)
		if (const std::optional<tref> r = try_witness<node>(x, psi, c))
			return push_block<node>(*r, without<node>(X, { x }), c);

	// 5. THE CASE WITNESS, LAZY EMISSION: 2d's race over §3's rewrite
	// `∃x.(D ∧ R) = ⋁ᵢ (dᵢ ∧ R)[x ← tᵢ]` without materialising the
	// distribution. One branch at a time is substituted, SIMPLIFIED
	// (invariant 6) and pushed, and a branch that decides `T` leaves every
	// later copy unbuilt.
	for (tref x : X) {
		std::optional<case_witness<node>> cw =
			try_case_witness<node>(x, psi, c);
		if (!cw) continue;
		std::stable_sort(cw->branches.begin(), cw->branches.end(),
			[](const std::pair<tref, tref>& a,
				const std::pair<tref, tref>& b) {
					return formula_size<node>(a.first)
						< formula_size<node>(b.first); });
		const block rest_of_X = without<node>(X, { x });
		join_builder<node, false> acc;
		for (const auto& [d, t] : cw->branches) {
			// The branch stands as it is, its pin included: under
			// `[x ← tᵢ]` the pin becomes the branch's residual
			// (§3), and the copy is simplified where it is built.
			const tref b = simplify<node>(subst_var<node>(
				simplified_and_join<node>({ d, cw->rest }),
				x, t, c.order), c.order);
			if (acc.insert(push_block<node>(b, rest_of_X, c)))
				break;
		}
		return acc.result();
	}

	// 6. THE FLOOR: a conjunction none of the steps above takes re-wraps
	// (invariant 3).
	return rewrap<node>(psi, X);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__PUSH__JUNCTIONS_TMPL_H__
