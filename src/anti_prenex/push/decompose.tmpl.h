// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file decompose.tmpl.h
 * @brief Template implementations for decompose.h. decompose.h says what each
 * procedure means; the comments here say how it is built.
 *
 * The pieces come from the files below: `members`, `formula_size`,
 * `fv_intersect`, `without`, `concat`, `complement_of` and the literal shapes
 * (foundations/dag.h) for the census, the licence and the arms, `has_atom`
 * and `subst_atom` (foundations/subst.h) for the touched region and its
 * cofactors, `term_of` (foundations/terms.h) with `cof_pin`
 * (witness/witness.h) for the pin and `subst_var` (foundations/subst.h) for
 * its arm, `guard_neg` (normalisers/nnf.h) for the guard units,
 * `fold_decided` (push/expand.h) for the probe, `prio_of`
 * (foundations/ctx.h) for the innermost-first order, the result joins
 * (normalisers/joins.h) for every assembly, `simplify`
 * (normalisers/simplify.h) at the arm edge, `rewrap` (foundations/prims.h)
 * with `taint` (foundations/ctx.h) for the exhaustion, and `push_block` —
 * the hub, `push/push_block.h` — for every arm.
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

/// §6 a JUNCTION a guard unit may occur in: a disjunctive conjunct of `ψ`,
/// or one of its ∧-members, with its members as a list and by content.
template <NodeType node>
struct guard_junction {
	bool conj = false;
	size_t conjunct = 0;
	tref m = nullptr;
	trefs ms;
	subtree_unordered_set<node> has;
};

/// `u` occurs in `j` as a member or, spelled with `j`'s connective, as a
/// group of its members.
template <NodeType node>
bool guard_occurs(tref u, const guard_junction<node>& j) {
	using tau = tree<node>;
	if (!is_child<node>(u, j.conj ? tau::wff_and : tau::wff_or))
		return j.has.contains(u);
	for (tref p : members<node>(u)) if (!j.has.contains(p)) return false;
	return true;
}

/// `j`'s members without those of the occurrence `u`.
template <NodeType node>
trefs guard_remove(tref u, const guard_junction<node>& j) {
	using tau = tree<node>;
	subtree_unordered_set<node> out;
	if (is_child<node>(u, j.conj ? tau::wff_and : tau::wff_or))
		for (tref p : members<node>(u)) out.insert(p);
	else out.insert(u);
	trefs rest;
	for (tref p : j.ms) if (!out.contains(p)) rest.push_back(p);
	return rest;
}

} // namespace decompose_detail

// --- TRY_DECOMPOSE ------------------------------------------------------------------

template <NodeType node>
std::optional<tref> try_decompose(tref psi, const block& X, ctx<node>& c) {
	using namespace decompose_detail;
	using tau = tree<node>;
	DBG(assert(psi != nullptr && !X.empty());)
	DBG(assert(is_child<node>(psi, tau::wff_and));)
	// Spent: `EXPAND` re-wraps and taints.
	if (c.expand_count >= c.expand_max) return std::nullopt;
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
	if (ranked.empty()) return try_guard_split<node>(cs, X, c);
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

	// PROBE FIRST. The wrapper narrows `X` to what the folded body holds.
	for (tref a : cands)
		if (!fold_decided<node>({ a }, X, c))
			return push_block<node>(simplify<node>(
				subst_atom<node>(psi, a, false), c.order), X, c);

	// THE LICENCE on BUILT cofactors of the touched region,
	// `Σ|m[a ↦ T]| + |m[a ↦ F]| ≤ Σ|m| + Σ|m|/4` multiplied out by 4: the
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
			simplified_and_join<node>(concat(on_T, untouched)),
			simplified_and_join<node>(concat(on_F, untouched)),
			X, c);
	}
	return try_guard_split<node>(cs, X, c);
}

// --- TRY_GUARD_SPLIT ----------------------------------------------------------------

template <NodeType node>
std::optional<tref> try_guard_split(const trefs& cs, const block& X,
	ctx<node>& c)
{
	using namespace decompose_detail;
	using tau = tree<node>;
	// A one-literal group is the atom census's.
	auto compound = [](const trefs& free) {
		return free.size() > 1
			|| (free.size() == 1 && !is_literal<node>(free[0]));
	};
	// The junctions, the cases of a conjunct before the conjunct itself,
	// and the units, in content order so that a tie goes to the first.
	std::vector<guard_junction<node>> js;
	subtree_set<node> units;
	for (size_t i = 0; i < cs.size(); ++i) {
		if (!is_child<node>(cs[i], tau::wff_or)) continue;
		guard_junction<node> top{ false, i, cs[i] };
		trefs free;
		for (tref m : members<node>(cs[i])) {
			top.ms.push_back(m), top.has.insert(m);
			if (!fv_meets<node>(m, X)) {
				free.push_back(m);
				if (!is_literal<node>(m)) units.insert(m);
				continue;
			}
			if (!is_child<node>(m, tau::wff_and)) continue;
			guard_junction<node> cj{ true, i, m };
			trefs f;
			for (tref e : members<node>(m)) {
				cj.ms.push_back(e), cj.has.insert(e);
				if (fv_meets<node>(e, X)) continue;
				f.push_back(e);
				if (!is_literal<node>(e)) units.insert(e);
			}
			if (compound(f) && f.size() < cj.ms.size())
				units.insert(simplified_and_join<node>(f));
			js.push_back(std::move(cj));
		}
		if (compound(free) && free.size() < top.ms.size())
			units.insert(simplified_or_join<node>(free));
		js.push_back(std::move(top));
	}
	tref best = nullptr, best_neg = nullptr;
	size_t best_cover = 0, best_weak = 0;
	for (tref g : units) {
		const tref ng = guard_neg<node>(g);
		if (ng == nullptr) continue;
		size_t pos = 0, neg = 0;
		for (const guard_junction<node>& j : js) {
			if (guard_occurs<node>(g, j)) ++pos;
			else if (guard_occurs<node>(ng, j)) ++neg;
		}
		if (pos == 0 || neg == 0) continue;
		const size_t cover = pos + neg, weak = std::min(pos, neg);
		if (best == nullptr || cover > best_cover
			|| (cover == best_cover && weak > best_weak))
			best = g, best_neg = ng, best_cover = cover,
			best_weak = weak;
	}
	if (best == nullptr) return std::nullopt;
	// `ψ[best ↦ v]` per conjunct: a case holding the unit or its `neg`
	// becomes `F` in one arm and loses the group in the other, then the
	// conjunct itself the same way, with its cases replaced.
	trefs on_T, on_F;
	size_t k = 0;
	for (size_t i = 0; i < cs.size(); ++i) {
		if (k == js.size() || js[k].conjunct != i) {
			on_T.push_back(cs[i]), on_F.push_back(cs[i]);
			continue;
		}
		subtree_unordered_map<node, std::pair<tref, tref>> cases;
		for (; js[k].conj; ++k) {
			const guard_junction<node>& j = js[k];
			if (guard_occurs<node>(best, j)) cases.emplace(j.m,
				std::pair{ simplified_and_join<node>(
					guard_remove<node>(best, j)), _F<node>() });
			else if (guard_occurs<node>(best_neg, j)) cases.emplace(j.m,
				std::pair{ _F<node>(), simplified_and_join<node>(
					guard_remove<node>(best_neg, j)) });
		}
		const guard_junction<node>& top = js[k++];
		for (const bool v : { true, false }) {
			trefs ms;
			if (guard_occurs<node>(best, top)) {
				if (v) continue;
				ms = guard_remove<node>(best, top);
			} else if (guard_occurs<node>(best_neg, top)) {
				if (!v) continue;
				ms = guard_remove<node>(best_neg, top);
			} else ms = top.ms;
			for (tref& m : ms)
				if (const auto it = cases.find(m); it != cases.end())
					m = v ? it->second.first : it->second.second;
			(v ? on_T : on_F).push_back(simplified_or_join<node>(ms));
		}
	}
	return decompose_arms<node>(best, simplified_and_join<node>(on_T),
		simplified_and_join<node>(on_F), X, c, best_neg);
}

// --- DECOMPOSE_ARMS -----------------------------------------------------------------

template <NodeType node>
tref decompose_arms(tref a, tref C_T, tref C_F, const block& X, ctx<node>& c,
	tref na)
{
	using tau = tree<node>;
	DBG(assert(a != nullptr && C_T != nullptr && C_F != nullptr);)
	DBG(assert(!X.empty() && (is_atomic_fm<node>(a) || na != nullptr));)
	DBG(assert(c.expand_count < c.expand_max);)
	a = tau::trim_right_sibling(a);

	// THE PIN, the innermost block variable first (§1 `prio`). An order
	// atom is no equation, and `term_of` is asked of an equation alone.
	tref x = nullptr, w = nullptr;
	if (is_child<node>(a, tau::bf_eq)) {
		block vs = fv_intersect<node>(term_of<node>(a, c.order), X);
		std::ranges::sort(vs, [&c](tref l, tref r) {
			return prio_of<node>(l, c) > prio_of<node>(r, c); });
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
		{ simplified_and_join<node>({
			na != nullptr ? na : complement_of<node>(a), C_F }),
			X } }};
	// Smaller first, the T-arm on a tie: `∃`'s `T` short-circuit leaves the
	// other arm unpushed.
	if (formula_size<node>(arms[1].first)
		< formula_size<node>(arms[0].first)) std::swap(arms[0], arms[1]);

	join_builder<node, false> acc;
	for (const auto& [phi, Y] : arms) {
		tref r = simplify<node>(phi, c.order);
		// An arm folded to a constant is no case, and charges nothing.
		if (!tau::get(r).equals_T() && !tau::get(r).equals_F()) {
			if (c.expand_count >= c.expand_max) {
				taint<node>();
				acc.insert(rewrap<node>(r, fv_intersect<node>(r, Y)));
				break;
			}
			++c.expand_count;
			r = push_block<node>(r, Y, c);
		}
		if (acc.insert(r)) break;
	}
	return acc.result();
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__PUSH__DECOMPOSE_TMPL_H__
