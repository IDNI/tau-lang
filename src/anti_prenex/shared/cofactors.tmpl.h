// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file cofactors.tmpl.h
 * @brief Template implementations for cofactors.h. cofactors.h says what each
 * function means; the comments here say how it is built.
 *
 * The pieces come from the foundations: `cofactor`, `quantify_over`,
 * `simplify_term`, `simplify_atom` (terms.h), the library's chain
 * constructor `build_functional_quantifiers` (tau_bdd.h), the memo wrapper
 * `memoised` (ctx.h), and the pin test `pin_from_cofactors` (simplify.h).
 * Nothing is computed here that one of them already computes.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__SHARED__COFACTORS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__SHARED__COFACTORS_TMPL_H__

#include <cassert>

namespace idni::tau_lang::anti_prenexing {

template <NodeType node>
cof_entry cof(tref f, tref x, ctx<node>& c) {
	// `x` free in `f`: `free_in` (simplify.tmpl.h) is the binary search
	// over `get_free_vars`' sorted set, the cheapest form of the test.
	DBG(assert(detail::free_in<node>(f, x) && "cof: x is not free in f");)
	return memoised<node, table::cof_memo>({ f, x }, [&] {
		// CHILD SELECTION, then `SIMPLIFY_TERM` (§6): `cofactor` is
		// canonical over the decision variables but leaves the leaves
		// alone, and the pin test compares terms syntactically.
		const tref f0 = simplify_term<node>(
			cofactor<node>(f, x, false, c.order), c.order);
		const tref f1 = simplify_term<node>(
			cofactor<node>(f, x, true, c.order), c.order);
		return pin_from_cofactors<node>(f0, f1, x, c.order);
	});
}

template <NodeType node>
tref discharge(tref term, bool negative, const block& X, ctx<node>& c) {
	using tbdd = tau_term_bdd<node>;
	// `∃X. h ≠ 0` is `∃_X h ≠ 0`, `∃X. f = 0` is `∀_X f = 0` (§7).
	const quantifier<node> kind = negative ? tbdd::ex : tbdd::all;
	tref q;
	if (!c.keep_functional) {
		// ONE BDD quantification under the live order, whatever |X| is.
		q = quantify_over<node>(kind, term, X, c.order);
	} else {
		// KEPT INSTEAD OF DISCHARGED: the canonical chain over `term`,
		// `X` outermost first as the constructor takes it. The
		// constructor leaves a body holding a stored BDD alone, so what
		// comes out keeps that BDD until the component's close, where
		// `resolve_functional_quantifiers_bdd` (terms.h) quantifies it
		// once. Block variables such a body still carries are HIDDEN in
		// a leaf (§1 LEAF HAZARD): `cof` on its term finds `usable`
		// false. Nothing slides and nothing is reordered (§7).
		typename tbdd::quants qs;
		qs.reserve(X.size());
		for (tref x : X) qs.emplace_back(x, kind);
		q = tbdd::build_functional_quantifiers(qs, term);
	}
	// INVARIANT 4: a negative atom is `¬(h = 0)`, never `h ≠ 0`.
	// `simplify_atom` folds through the one `¬`.
	const tref atom = build_bf_eq_0<node>(q);
	return simplify_atom<node>(
		negative ? build_wff_neg<node>(atom) : atom, c.order);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__SHARED__COFACTORS_TMPL_H__
