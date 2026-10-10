// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file expand.tmpl.h
 * @brief Template implementations for expand.h. expand.h says what each
 * procedure means; the comments here say how it is built.
 *
 * The pieces come from the files below: `members`, `formula_size`,
 * `fv_intersect`, `concat` and the shape predicates (foundations/dag.h) for
 * the lists and the keys, `prio_of` and `innermost` (foundations/ctx.h) for
 * the probe variable and the disjunct key, `term_of` and `node_test`
 * (foundations/terms.h) with `cof` (shared/cofactors.h) for the probe,
 * `atoms` and `subst_atom` (foundations/subst.h) for a tree's literal-wise
 * fold, the result joins (normalisers/joins.h) for every assembly, `simplify`
 * (normalisers/simplify.h) at the case edge, `check_time_limit`
 * (foundations/ctx.h) per case, and `push_block` — the hub,
 * `push/push_block.h` — for every case.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__PUSH__EXPAND_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__PUSH__EXPAND_TMPL_H__

#include <algorithm>
#include <cassert>
#include <utility>

namespace idni::tau_lang::anti_prenexing {

namespace expand_detail {

/// §6 `FOLD_DECIDED`'s probe: is the equation `atom` `F`? Its term's
/// cofactors on the innermost block variable have a nonzero constant
/// product, by the node test on it. `COF` memoises the product.
template <NodeType node>
bool probe(tref atom, const block& X, ctx<node>& c) {
	// `TERM_OF(atom)` holds no variable the atom lacks.
	if (!fv_meets<node>(atom, X)) return false;
	const tref f = term_of<node>(atom, regime::backed);
	const block vs = fv_intersect<node>(f, X);
	if (vs.empty()) return false;
	const cof_entry e = cof<node>(f, innermost<node>(vs, c));
	return e.usable && node_test<node>(e.p, node_question::nonzero)
		.is(node_question::nonzero);
}

/// `FOLD_DECIDED` on a negative tree, literal-wise: an atom the probe proves
/// `F` is set to `F`, so its literal `¬a` is `T` and folds up through the
/// hooks.
template <NodeType node>
tref fold_tree(tref t, const block& X, ctx<node>& c) {
	t = tree<node>::trim_right_sibling(t);
	// A copy: `atoms` hands out a table row, which a sweep during the
	// rewrites below may rebuild.
	const trefs as = atoms<node>(t);
	for (tref a : as)
		if (probe<node>(a, X, c)) t = subst_atom<node>(t, a, false);
	return t;
}

} // namespace expand_detail

// --- FOLD_DECIDED -----------------------------------------------------------------

template <NodeType node>
std::optional<trefs> fold_decided(const trefs& C, const block& X,
	ctx<node>& c)
{
	using namespace expand_detail;
	using tau = tree<node>;
	trefs out;
	out.reserve(C.size());
	// `term_of` is reached through the (¬)equation and tree branches alone:
	// it Debug-asserts on anything else. A unit, a reference, an order atom
	// and an ∨-node that is no tree are kept as they are.
	for (tref m : C) {
		if (is_child<node>(m, tau::bf_eq)) {
			if (probe<node>(m, X, c)) return std::nullopt;
		} else if (is_negated_equation<node>(m)) {
			if (probe<node>(atom_of<node>(m), X, c)) continue;
		} else if (is_negative_tree<node>(m)) {
			m = fold_tree<node>(m, X, c);
			if (tau::get(m).equals_T()) continue;
		}
		out.push_back(m);
	}
	return out;
}

// --- EXPAND -----------------------------------------------------------------------

template <NodeType node>
tref expand(tref psi, const block& X, ctx<node>& c) {
	using namespace expand_detail;
	using tau = tree<node>;
	DBG(assert(psi != nullptr && !X.empty());)
	DBG(assert(is_child<node>(psi, tau::wff_and));)
	trefs plain, S;
	for (tref m : members<node>(psi))
		(is_child<node>(m, tau::wff_or) ? S : plain).push_back(m);
	const std::optional<trefs> E = fold_decided<node>(plain, X, c);
	if (!E) return _F<node>();
	// The settle move pushes a clause without a disjunctive conjunct home,
	// so the floor never sees one.
	DBG(assert(!S.empty());)

	// THE DISJUNCT whose innermost variable is outermost, then the fewest
	// members, then content order.
	auto key = [&](tref d) {
		const block vs = fv_intersect<node>(d, X);
		// The wrapper strips X-free conjuncts: every `d` meets `X`.
		DBG(assert(!vs.empty());)
		return std::pair<int_t, size_t>{
			prio_of<node>(innermost<node>(vs, c), c),
			members<node>(d).size() };
	};
	size_t at = 0;
	std::pair<int_t, size_t> best = key(S[0]);
	for (size_t i = 1; i < S.size(); ++i)
		if (const auto k = key(S[i]); k < best
			|| (k == best && tau::subtree_less(S[i], S[at])))
				at = i, best = k;
	trefs rest;
	for (size_t i = 0; i < S.size(); ++i) if (i != at) rest.push_back(S[i]);

	// Cheap deciders first, stably: ties keep the chain's order.
	trefs ds = members<node>(S[at]);
	std::stable_sort(ds.begin(), ds.end(), [](tref a, tref b) {
		return formula_size<node>(a) < formula_size<node>(b); });
	join_builder<node, false> acc;
	for (tref d : ds) {
		// Work multiplies per case, so the clock is read here (§3).
		check_time_limit();
		const std::optional<trefs> dc =
			fold_decided<node>(members<node>(d), X, c);
		// A dead case: no copy.
		if (!dc) continue;
		// A member proven `T` makes `D` `T`, which subsumes every case.
		if (dc->empty())
			return push_block<node>(
				simplified_and_join<node>(concat(*E, rest)), X, c);
		// DEEP over `E` and the member alone: the other disjunctive
		// conjuncts are simplified at their own expansion.
		const tref cc = simplify<node>(
			simplified_and_join<node>(concat(*E, *dc)),
			regime::backed, c.P);
		if (tau::get(cc).equals_F()) continue;
		// The wrapper narrows `X` and strips what the case decided about
		// free variables; `T` decides the ∨-join (∃'s short-circuit).
		if (acc.insert(push_block<node>(
			simplified_and_join<node>(concat(trefs{ cc }, rest)), X, c)))
				break;
	}
	// Every case dead is the empty join, `F`.
	return acc.result();
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__PUSH__EXPAND_TMPL_H__
