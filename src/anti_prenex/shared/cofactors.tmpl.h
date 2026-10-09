// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file cofactors.tmpl.h
 * @brief Template implementations for cofactors.h. cofactors.h says what each
 * function means; the comments here say how it is built.
 *
 * The pieces come from the foundations: `cofactor`, `quantify_over`,
 * `simplify_term`, `simplify_atom` (terms.h), the library's chain
 * constructor `build_functional_quantifiers` and the slide `build_bdd` runs
 * on a chain (tau_bdd.h), the memo wrapper `memoised` (ctx.h), and the pin
 * test `pin_from_cofactors` (simplify.h). Nothing is computed here that one
 * of them already computes.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__SHARED__COFACTORS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__SHARED__COFACTORS_TMPL_H__

#include <algorithm>
#include <cassert>

namespace idni::tau_lang::anti_prenexing {

namespace cofactors_detail {

/// X's levels are the lowest of @p term: every decision variable outside `X`
/// sits above every variable of `X` the term decides on — an absent one has
/// no level. One walk over the BDD's nodes, tracking the topmost of `X` and
/// the lowest of the rest.
template <NodeType node>
bool lowest_levels(tref term, const block& X) {
	using tau = tree<node>;
	using tbdd = tau_term_bdd<node>;
	using thandle = term_handle<node>;
	if (X.empty() || !thandle::is_bdd_backed(term)) return true;
	trefs xs;
	xs.reserve(X.size());
	for (tref x : X) xs.push_back(tau::trim_right_sibling(x));
	std::sort(xs.begin(), xs.end(), tau::subtree_less);
	tref top_x = nullptr, low_other = nullptr;
	auto levels = [&](typename tbdd::ref n, bool is_leaf) {
		if (is_leaf) return true;
		const tref v = tbdd::get_var(n);
		if (!std::binary_search(xs.begin(), xs.end(), v, tau::subtree_less)) {
			if (!low_other || tbdd::less_then(low_other, v)) low_other = v;
		} else if (!top_x || tbdd::less_then(v, top_x)) top_x = v;
		return true;
	};
	tbdd::visit_nodes(thandle::convert_to_handle(term).get(), levels);
	return !top_x || !low_other || tbdd::less_then(low_other, top_x);
}

} // namespace cofactors_detail

template <NodeType node>
cof_entry cof(tref f, tref x) {
	// `x` free in `f`: `free_in` (simplify.tmpl.h) is the binary search
	// over `get_free_vars`' sorted set, the cheapest form of the test.
	DBG(assert(detail::free_in<node>(f, x) && "cof: x is not free in f");)
	return memoised<node, table::cof_memo>({ f, x }, [&] {
		// CHILD SELECTION, then `SIMPLIFY_TERM` (§6): `cofactor` is
		// canonical over the decision variables but leaves the leaves
		// alone, and the pin test compares terms syntactically.
		const tref f0 = simplify_term<node>(cofactor<node>(f, x, false),
			regime::backed);
		const tref f1 = simplify_term<node>(cofactor<node>(f, x, true),
			regime::backed);
		cof_entry e = pin_from_cofactors<node>(f0, f1, x, regime::backed);
		// A non-pin's residual, left unbuilt there, is still part of the
		// record this table promises for every usable entry.
		if (e.usable && !e.pin)
			e.p = simplify_term<node>(build_bf_and<node>(f0, f1),
				regime::backed);
		return e;
	});
}

template <NodeType node>
tref discharge(tref term, bool negative, const block& X, ctx<node>& c) {
	using tau = tree<node>;
	using tbdd = tau_term_bdd<node>;
	using thandle = term_handle<node>;
	// `∃X. h ≠ 0` is `∃_X h ≠ 0`, `∃X. f = 0` is `∀_X f = 0` (§7).
	const quantifier<node> kind = negative ? tbdd::ex : tbdd::all;
	tref q = nullptr;
	if (c.keep_functional
		&& cofactors_detail::lowest_levels<node>(term, X))
	{
		// KEPT: the canonical chain over `term`, `X` outermost first as
		// the constructor takes it, asked about once as built.
		typename tbdd::quants qs;
		qs.reserve(X.size());
		for (tref x : X) qs.emplace_back(x, kind);
		const tref ch = tbdd::build_functional_quantifiers(qs, term);
		const tau& tch = tau::get(ch);
		if (!tch.child_is(tau::bf_fall) && !tch.child_is(tau::bf_fex))
			q = ch;
		else if (c.kf(ch)) {
			// Building the chain slides it to X's levels: the
			// variables above stay decision structure, each piece
			// over X's levels a leaf or, closed, its constant.
			const auto slid = tbdd::build_bdd(ch);
			DBG(auto outside_X = [&X](typename tbdd::ref n,
				bool is_leaf)
			{
				return is_leaf || std::ranges::none_of(X,
					[v = tbdd::get_var(n)](tref x) {
						return tau::subtree_equals(
							tau::trim_right_sibling(x), v);
					});
			};)
			DBG(assert(tbdd::visit_nodes(slid, outside_X));)
			q = thandle::convert_to_tau_node_or_term(thandle(slid),
				find_ba_type<node>(term));
		}
	}
	// ONE BDD quantification, whatever |X| is: keep mode off, a sub-block
	// with block variables pending below its levels, or a chain `kf` does
	// not keep.
	if (q == nullptr) q = quantify_over<node>(kind, term, X);
	// INVARIANT 4: a negative atom is `¬(h = 0)`, never `h ≠ 0`.
	// `simplify_atom` folds through the one `¬`.
	const tref atom = build_bf_eq_0<node>(q);
	return simplify_atom<node>(
		negative ? build_wff_neg<node>(atom) : atom, regime::backed);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__SHARED__COFACTORS_TMPL_H__
