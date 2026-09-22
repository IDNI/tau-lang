// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file parts.tmpl.h
 * @brief Template implementations for parts.h. parts.h says what each
 * function means; the comments here say how it is built.
 *
 * THE UNION-FIND IS `union_find_with_sets` (src/union_find_with_sets.h),
 * keyed through `subtree_unordered_map`, so two content-equal variable nodes
 * are one element however they were spelled, with a path-compressed `find`.
 * Its comparator picks the root of a merged class and is `subtree_less`, the
 * content order (§1), so the class roots are content-derived. It is held BY
 * REFERENCE inside the union-find, so the lambda outlives every call made on
 * it.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__SHARED__PARTS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__SHARED__PARTS_TMPL_H__

#include <vector>

#include "../../union_find_with_sets.h"

namespace idni::tau_lang::anti_prenexing {

namespace detail {

/**
 * @brief The shared body of `PARTS` and `INCIDENCE` (§6): the grouping, with
 * each conjunct's block variables handed back in @p vars for the caller that
 * needs them a second time.
 *
 * Three linear walks. FIRST, per conjunct, `X ∩ FV(c)` (dag.h — an
 * intersection of cached sets, never a descent) and its variables merged into
 * one class, which is what "co-occur in a conjunct" means. SECOND, the
 * conjuncts in input order, each appended to the part of its class root, a
 * new part opening when that root is seen first — so the parts come in the
 * order of their first conjunct. THIRD, `X` in ITS OWN order, each variable
 * appended to the part of its class, which is how a part's `vars` end up in
 * `X`'s order without a sort; a variable of `X` that no conjunct holds is in
 * no class and belongs to no part. That third walk is `X` FILTERED by class
 * membership, one `find` per variable — the union-find's `get_set` hands a
 * class over directly, but in its ring's order, which would have to be sorted
 * back into `X`'s.
 */
template <NodeType node>
std::vector<part<node>> group_parts(const trefs& conjuncts, const block& X,
	std::vector<block>& vars)
{
	using tau = tree<node>;
	auto root_less = [](tref l, tref r) { return tau::subtree_less(l, r); };
	union_find_with_sets<decltype(root_less), node> uf(root_less);
	vars.clear();
	vars.reserve(conjuncts.size());
	for (tref c : conjuncts) {
		vars.push_back(fv_intersect<node>(c, X));
		const block& vs = vars.back();
		// A conjunct touching no variable of `X` joins no class and
		// becomes a part of its own below.
		if (vs.empty()) continue;
		uf.insert(vs[0]);
		for (size_t i = 1; i < vs.size(); ++i) uf.merge(vs[0], vs[i]);
	}
	std::vector<part<node>> ps;
	subtree_unordered_map<node, size_t> of_root;
	for (size_t i = 0; i < conjuncts.size(); ++i) {
		// A conjunct free of `X` is a part of its own over no
		// variable, merging with nothing, so that the parts still
		// partition the input.
		if (vars[i].empty()) {
			ps.push_back(part<node>{ trefs{ conjuncts[i] }, {} });
			continue;
		}
		const tref root = uf.find(vars[i][0]);
		auto [it, fresh] = of_root.try_emplace(root, ps.size());
		if (fresh) ps.push_back(part<node>{});
		ps[it->second].conjuncts.push_back(conjuncts[i]);
	}
	for (tref x : X) {
		if (!uf.contains(x)) continue;
		// Trimmed, like `fv_intersect`'s own result: a block variable
		// read off a binder carries the body as its right sibling.
		ps[of_root.at(uf.find(x))].vars.push_back(
			tau::trim_right_sibling(x));
	}
	return ps;
}

} // namespace detail

template <NodeType node>
std::vector<part<node>> parts(const trefs& conjuncts, const block& X) {
	std::vector<block> vars;
	return detail::group_parts<node>(conjuncts, X, vars);
}

template <NodeType node>
incidence_result<node> incidence(const trefs& conjuncts, const block& X) {
	using tau = tree<node>;
	std::vector<block> vars;
	incidence_result<node> r{ detail::group_parts<node>(conjuncts, X, vars),
		{} };
	// The kind test, once per conjunct, on the intersections the grouping
	// already took: a disjunctive conjunct that is no negative tree flags
	// every block variable it holds.
	subtree_unordered_set<node> flagged;
	for (size_t i = 0; i < conjuncts.size(); ++i) {
		if (!is_child<node>(conjuncts[i], tau::wff_or)
			|| is_negative_tree<node>(conjuncts[i])) continue;
		for (tref x : vars[i]) flagged.insert(x);
	}
	for (tref x : X)
		if (!flagged.contains(x))
			r.settled.push_back(tau::trim_right_sibling(x));
	return r;
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__SHARED__PARTS_TMPL_H__
