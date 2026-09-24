// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file decompose.tmpl.h
 * @brief Template implementations for decompose.h. decompose.h says what each
 * procedure means; the comments here say how it is built.
 *
 * The pieces come from the files below: `members`, `formula_size`,
 * `fv_intersect`, `without`, `complement_of` and the literal shapes
 * (foundations/dag.h) for the census, the licence and the arms, `has_atom`
 * and `subst_atom` (foundations/subst.h) for the touched region and its
 * cofactors, `term_of` (foundations/terms.h) with `cof_pin`
 * (witness/witness.h) for the pin and `subst_var` (foundations/subst.h) for
 * its arm, `fold_decided` with `EXPAND`'s rank and list helpers
 * (push/expand.h) for the probe, the innermost-first order and the arms'
 * operands, the result joins (normalisers/joins.h) for every assembly,
 * `simplify` (normalisers/simplify.h) at the arm edge, and `push_block` — the
 * hub, `push/push_block.h` — for every arm.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__PUSH__DECOMPOSE_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__PUSH__DECOMPOSE_TMPL_H__

#include <algorithm>
#include <array>
#include <cassert>
#include <utility>
#include <vector>

namespace idni::tau_lang::anti_prenexing {

namespace decompose_detail {

/// §6 the census's signed coverage of one atom at member tops.
struct coverage {
	size_t pos = 0, neg = 0;
};

/// §6 the number of census candidates the licence tries.
inline constexpr size_t census_top = 3;

} // namespace decompose_detail

// --- TRY_DECOMPOSE ------------------------------------------------------------------

template <NodeType node>
std::optional<tref> try_decompose(tref psi, const block& X, ctx<node>& c) {
	using namespace decompose_detail;
	using tau = tree<node>;
	DBG(assert(psi != nullptr && !X.empty());)
	DBG(assert(is_child<node>(psi, tau::wff_and));)
	const trefs cs = members<node>(psi);

	// THE CENSUS, one pass over the member tops of the disjunctive
	// conjuncts. An ∧-member's conjuncts are not opened.
	subtree_map<node, coverage> census;
	for (tref s : cs) {
		if (!is_child<node>(s, tau::wff_or)) continue;
		for (tref m : members<node>(s)) {
			if (!is_literal<node>(m)) continue;
			coverage& k = census[atom_of<node>(m)];
			++(is_negated_atom<node>(m) ? k.neg : k.pos);
		}
	}
	std::vector<std::pair<tref, coverage>> ranked;
	for (const auto& [a, k] : census)
		if (k.pos > 0 && k.neg > 0) ranked.emplace_back(a, k);
	if (ranked.empty()) return std::nullopt;
	// The rank only orders the tries; the licence decides.
	const size_t n = std::min(census_top, ranked.size());
	std::partial_sort(ranked.begin(), ranked.begin() + n, ranked.end(),
		[](const std::pair<tref, coverage>& l,
			const std::pair<tref, coverage>& r)
		{
			const size_t lt = l.second.pos + l.second.neg;
			const size_t rt = r.second.pos + r.second.neg;
			if (lt != rt) return lt > rt;
			const size_t lw = std::min(l.second.pos, l.second.neg);
			const size_t rw = std::min(r.second.pos, r.second.neg);
			if (lw != rw) return lw > rw;
			return tau::subtree_less(l.first, r.first);
		});
	// A census key may be a chain member, which carries its right sibling.
	trefs cands;
	cands.reserve(n);
	for (size_t i = 0; i < n; ++i)
		cands.push_back(tau::trim_right_sibling(ranked[i].first));

	// PROBE FIRST: a candidate proven `F` erases every occurrence without a
	// guard or an arm, and the smaller body re-enters the ladder
	// (invariant 8); the wrapper narrows `X` to what it holds.
	for (tref a : cands)
		if (!fold_decided<node>({ a }, X, c))
			return push_block<node>(simplify<node>(
				subst_atom<node>(psi, a, false), c.order), X, c);

	// THE LICENCE on BUILT cofactors of the touched region,
	// `Σ|m[a ↦ T]| + |m[a ↦ F]| ≤ Σ|m| + Σ|m|/4` in integers: the
	// untouched conjuncts are one shared subtree in both arms.
	for (tref a : cands) {
		trefs on_T, on_F, untouched;
		size_t lhs = 0, rhs = 0;
		for (tref m : cs) {
			if (!has_atom<node>(m, a)) {
				untouched.push_back(m);
				continue;
			}
			on_T.push_back(subst_atom<node>(m, a, true));
			on_F.push_back(subst_atom<node>(m, a, false));
			rhs += formula_size<node>(m);
			lhs += formula_size<node>(on_T.back())
				+ formula_size<node>(on_F.back());
		}
		if (4 * lhs > 5 * rhs) continue;
		return decompose_arms<node>(a,
			simplified_and_join<node>(
				expand_detail::concat(on_T, untouched)),
			simplified_and_join<node>(
				expand_detail::concat(on_F, untouched)),
			X, c);
	}
	return std::nullopt;
}

// --- DECOMPOSE_ARMS -----------------------------------------------------------------

template <NodeType node>
tref decompose_arms(tref a, tref C_T, tref C_F, const block& X, ctx<node>& c) {
	using tau = tree<node>;
	DBG(assert(a != nullptr && C_T != nullptr && C_F != nullptr);)
	DBG(assert(!X.empty() && is_atomic_fm<node>(a));)
	a = tau::trim_right_sibling(a);

	// THE PIN, the innermost block variable first (§1 `prio`). An order
	// atom is no equation, and `term_of` is asked of an equation alone.
	tref x = nullptr, w = nullptr;
	if (is_child<node>(a, tau::bf_eq)) {
		block vs = fv_intersect<node>(term_of<node>(a, c.order), X);
		std::ranges::sort(vs, [&c](tref l, tref r) {
			return expand_detail::prio_of<node>(l, c)
				> expand_detail::prio_of<node>(r, c); });
		for (tref v : vs)
			if (const std::optional<pin<node>> p =
				cof_pin<node>(a, v, c))
			{
				x = v, w = p->witness;
				break;
			}
	}

	// A pin's T-arm keeps its guard, substituted into the residual
	// `p = 0`, and loses the binder.
	const tref guarded = simplified_and_join<node>({ a, C_T });
	std::array<std::pair<tref, block>, 2> arms{{
		x == nullptr ? std::pair<tref, block>{ guarded, X }
			: std::pair<tref, block>{
				subst_var<node>(guarded, x, w, c.order),
				without<node>(X, { x }) },
		{ simplified_and_join<node>({ complement_of<node>(a), C_F }),
			X } }};
	// Smaller first, the T-arm on a tie: `∃`'s `T` short-circuit leaves the
	// other arm unpushed.
	if (formula_size<node>(arms[1].first)
		< formula_size<node>(arms[0].first)) std::swap(arms[0], arms[1]);

	join_builder<node, false> acc;
	for (const auto& [phi, Y] : arms) {
		tref r = simplify<node>(phi, c.order);
		// An arm folded to a constant is no case, and is inserted as it
		// stands.
		if (!tau::get(r).equals_T() && !tau::get(r).equals_F())
			r = push_block<node>(r, Y, c);
		if (acc.insert(r)) break;
	}
	return acc.result();
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__PUSH__DECOMPOSE_TMPL_H__
