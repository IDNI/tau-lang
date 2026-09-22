// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file fast_paths.tmpl.h
 * @brief Template implementations for fast_paths.h. fast_paths.h says what
 * each procedure means; the comments here say how it is built.
 *
 * The pieces come from the files below: `fv_meets` and the shape predicates
 * (foundations/dag.h) for the classification, `is_pure_equation`, `term_of`,
 * `simplify_term` and `simplify_atom` (foundations/terms.h) for the literals
 * and the terms, the result joins (normalisers/joins.h) for every assembly,
 * and `eliminate_block` with `ex_distributes_over_negatives`
 * (eliminate/eliminate_block.h) for the clauses and for 2a's licence.
 *
 * THE THREE WALKS, and what the library gives them (tree.h,
 * tree_traversals_pre_order.tmpl.h):
 * - `visit_unique(visit, visit_subtree, up)`: `visit` is called on a node
 *   `visit_subtree` admits and on no other, and its `false` PRUNES — the
 *   node's children are not visited. Equal nodes are visited once.
 * - `search_unique` is the same walk except that `visit`'s `false`
 *   TERMINATES it: a stop, and no way to hold a subtree back on its own. The
 *   census needs both, so it prunes with `visit_unique` and carries its
 *   abort in a flag that makes every later `visit` prune at once, classifying
 *   nothing.
 * - `apply_unique_pure(f, visit_subtree, up)`: a node the predicate turns
 *   away gets neither `f` nor `up` and comes back verbatim, and `up` does
 *   run on a node `f` changed into something the predicate then rejects.
 *   Its memo covers the subtrees the walk ENTERED, so `f` still runs per
 *   occurrence of a node it changes into a leaf.
 * - A node that is not a `wff` WRAPPER — the `wff_and` / `wff_or` operator
 *   node under one — has no free-variable set of its own
 *   (`get_free_vars` answers off a `bf` or `wff` node alone,
 *   tau_tree_extractors.tmpl.h), so it would read as X-free. Every predicate
 *   here therefore classifies the wrapper and steps through the operator
 *   node, as `is_negative_tree`'s walk does (foundations/dag.h).
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__PUSH__FAST_PATHS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__PUSH__FAST_PATHS_TMPL_H__

#include <cassert>
#include <utility>

namespace idni::tau_lang::anti_prenexing {

namespace fast_paths_detail {

/// The ∧/∨ SKELETON a walk of this file stays on: a wrapper and the two
/// connective nodes under one, and nothing else — so no walk enters a term,
/// a unit's body or a reference's arguments.
template <NodeType node>
bool skeleton(tref n) {
	using tau = tree<node>;
	const tau& t = tau::get(n);
	return t.is(tau::wff) || t.is(tau::wff_and) || t.is(tau::wff_or);
}

/// An ∧-node or an ∨-node (§1), on a wrapper.
template <NodeType node>
bool is_junction(tref n) {
	using tau = tree<node>;
	return is_child<node>(n, tau::wff_and) || is_child<node>(n, tau::wff_or);
}

} // namespace fast_paths_detail

// --- the census -------------------------------------------------------------------

template <NodeType node>
census sign_census(tref phi, const block& X, size_t p) {
	using tau = tree<node>;
	using namespace fast_paths_detail;
	DBG(assert(phi != nullptr);)
	census k;
	bool refuted = false;
	// One classification per node, and the counts decide after each one
	// whether the walk still has a question to answer.
	auto classify = [&](tref n) {
		if (refuted) return false;
		if (!tau::get(n).is(tau::wff)) return true;
		bool descend = false;
		if (!fv_meets<node>(n, X)) ++k.free;
		else if (is_junction<node>(n)) descend = true;
		else if (is_child<node>(n, tau::bf_eq)) ++k.pos;
		else if (is_negated_equation<node>(n)) ++k.neg;
		else ++k.other;
		refuted = k.other > 0
			|| (k.pos > p && (k.neg > 0 || k.free > 0));
		return descend && !refuted;
	};
	auto up = [](tref) {};
	pre_order<node>(phi).visit_unique(classify, skeleton<node>, up);
	return k;
}

// --- 2a and 2b --------------------------------------------------------------------

template <NodeType node>
std::optional<tref> try_fast_paths(tref phi, const block& X, ctx<node>& c) {
	using tau = tree<node>;
	DBG(assert(phi != nullptr && !X.empty() && fv_meets<node>(phi, X));)
	// The top-level split, taken on the member view once: `P` the pure
	// positive equations, `others` what the distribution rebuilds. A
	// formula that is no conjunction has an empty `P` and is its own rest.
	trefs P, others;
	const bool conj = is_child<node>(phi, tau::wff_and);
	if (conj) for (tref m : members<node>(phi))
		((is_child<node>(m, tau::bf_eq)
			&& is_pure_equation<node>(m, c.type)) ? P : others)
				.push_back(m);
	const census k = sign_census<node>(phi, X, P.size());
	if (k.other > 0) return std::nullopt;
	// 2a. Every positive leaf is one of the top-level literals, so it is a
	// conjunct of every DNF clause: with `f` the squeeze of `P`,
	// `∃X(P ∧ ⋀ᵢNᵢ ∧ C)` is `C ∧ ∀_X f = 0 ∧ ⋀ᵢ ∃_X f′gᵢ ≠ 0`
	// (cor:qelim-main, then the per-leaf identity), which refactors back
	// through the ∧/∨ skeleton. The `∀_X f = 0` half is emitted ONCE at
	// the top, however many leaves carry `P`: an X-free clause needs it and
	// every other clause implies it. This is why a top-level positive —
	// what a decomposition's positive arm and an exclusive case leave
	// behind — does not refute the path.
	if (k.pos == P.size() && k.neg > 0) {
		if (!ex_distributes_over_negatives<node>(c.type))
			return std::nullopt;
		const tref rest = conj ? simplified_and_join<node>(others) : phi;
		// The elimination narrows `X` to `⋀P` itself, so its key is the
		// one the consistency check builds for the same positives (§6).
		return simplified_and_join<node>({
			eliminate_block<node>(
				simplified_and_join<node>(P), X, c),
			distribute_to_atoms<node>(rest, X, c, P) });
	}
	// 2b. `f₁ = 0 ∧ f₂ = 0 ≡ f₁ ∪ f₂ = 0` (prop:squeeze-pos) and ∧ over ∨,
	// on terms, in any BA — reaching a one-atom clause with no expansion.
	// An X-free part refutes it: the term-level squeeze has no way to carry
	// a formula through the cross product.
	if (k.free == 0 && k.neg == 0 && k.pos > 0) {
		// Guard first (invariant 7): a `T` among the eliminations
		// stops the rest of the list from being eliminated at all.
		join_builder<node, false> acc;
		for (tref t : squeeze_positives<node>(phi, c.order)) {
			// An atom built here is simplified as it is built
			// (invariant 6).
			const tref a = simplify_atom<node>(
				build_bf_eq_0<node>(t), c.order);
			if (acc.insert(eliminate_block<node>(a, X, c))) break;
		}
		return acc.result();
	}
	return std::nullopt;
}

// --- the distribution to the atoms --------------------------------------------------

template <NodeType node>
tref distribute_to_atoms(tref phi, const block& X, ctx<node>& c,
	const trefs& P)
{
	using tau = tree<node>;
	using namespace fast_paths_detail;
	DBG(assert(phi != nullptr);)
	// The literal's clause, with the top-level positives riding in it. The
	// `¬` goes INSIDE the elimination: replacing the atom under a kept `¬`
	// would compute `¬(∀_X f = 0)` instead of `∃_X f ≠ 0`.
	auto down = [&](tref n) -> tref {
		if (!tau::get(n).is(tau::wff) || is_junction<node>(n)) return n;
		DBG(assert(is_child<node>(n, tau::bf_eq)
			|| is_negated_equation<node>(n));)
		trefs clause = P;
		clause.push_back(n);
		return eliminate_block<node>(
			simplified_and_join<node>(clause), X, c);
	};
	// An X-free subtree is never entered, so it comes back verbatim and
	// carries no binder of its own. What `down` hands back is X-free —
	// resolved, or a re-wrapped clause binding `X` — so the predicate does
	// not enter that either.
	auto enter = [&](tref n) {
		if (!tau::get(n).is(tau::wff)) return skeleton<node>(n);
		return fv_meets<node>(n, X);
	};
	// A junction is re-emitted through its join, so a literal that decided
	// folds through the skeleton.
	auto up = [](tref r) -> tref {
		if (is_child<node>(r, tau::wff_and))
			return simplified_and_join<node>(members<node>(r));
		if (is_child<node>(r, tau::wff_or))
			return simplified_or_join<node>(members<node>(r));
		return r;
	};
	// The walk's memo holds the sub-skeletons it entered, so a shared one
	// is rebuilt once; a literal is not one of them, and a second
	// occurrence of it is a hit in `elim_memo` (§7), which keys exactly
	// this clause.
	return pre_order<node>(phi).apply_unique_pure(down, enter, up);
}

// --- the squeeze ---------------------------------------------------------------------

template <NodeType node>
trefs squeeze_positives(tref phi, const var_order<node>& order) {
	using tau = tree<node>;
	using namespace fast_paths_detail;
	DBG(assert(phi != nullptr);)
	// One list per junction, filled in POST ORDER, so a junction reads
	// lists its children left behind and a shared sub-skeleton is squeezed
	// once. A LEAF has no list: its one term is read where it is used.
	subtree_unordered_map<node, trefs> lists;
	auto list_of = [&](tref n) -> trefs {
		if (const auto it = lists.find(n); it != lists.end())
			return it->second;
		DBG(assert(is_child<node>(n, tau::bf_eq));)
		return trefs{ term_of<node>(n, order) };
	};
	// Over the operator node's OWN children, not the flattened member view:
	// a chain's spine then holds partial products its parent goes on with,
	// and no node computes a product twice.
	auto up = [&](tref n) {
		const tau& t = tau::get(n);
		if (!t.is(tau::wff) || !is_junction<node>(n)) return;
		const bool conj = is_child<node>(n, tau::wff_and);
		trefs out;
		bool opened = false;
		for (tref m : tau::get(t.first()).children()) {
			trefs l = list_of(m);
			if (!conj) out.insert(out.end(), l.begin(), l.end());
			else if (!opened) out = std::move(l);
			else {
				// `f₁ = 0 ∧ f₂ = 0` squeezes to `f₁ ∪ f₂ = 0`;
				// the union is RAW, the hooks folding what they
				// fold, and the sweep comes once at the end.
				trefs product;
				product.reserve(out.size() * l.size());
				for (tref a : out) for (tref b : l)
					product.push_back(
						build_bf_or<node>(a, b));
				out = std::move(product);
			}
			opened = true;
		}
		lists.emplace(n, std::move(out));
	};
	auto classify = [&](tref n) {
		if (!tau::get(n).is(tau::wff)) return true;
		if (is_junction<node>(n)) return true;
		// 2b's precondition: the leaves are positive equations.
		DBG(assert(is_child<node>(n, tau::bf_eq));)
		return false;
	};
	pre_order<node>(phi).visit_unique(classify, skeleton<node>, up);
	trefs out = list_of(phi);
	for (tref& t : out) t = simplify_term<node>(t, order);
	return out;
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__PUSH__FAST_PATHS_TMPL_H__
