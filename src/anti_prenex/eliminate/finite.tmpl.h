// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file finite.tmpl.h
 * @brief Template implementations for finite.h. finite.h says what the
 * method means; the comments here say how it is built.
 *
 * The freeze, the squeeze and the three conditions come from conditions.h,
 * the leaf-hazard test from atomless.h, the purity tests from terms.h, the
 * parts from parts.h and the decision from `decide_finite` (blast.h). What
 * is written here is the spec's step order, the subsumption scan, the query
 * closing and the per-part loop of tier 2.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__FINITE_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__FINITE_TMPL_H__

#include <algorithm>
#include <utility>
#include <vector>

namespace idni::tau_lang::anti_prenexing {

namespace finite_detail {

/// §7 a conversion emission, by shape: an ∨-node that is no negative tree,
/// the one non-literal, non-unit conjunct shape the push never builds.
template <NodeType node>
bool is_emission(tref m) {
	return is_child<node>(m, tree<node>::wff_or)
		&& !is_negative_tree<node>(m);
}

/**
 * @brief §7 the MIDDLE TIER on the one negative `t`, a nested tree: the
 * ∨-join of its root's literal conditions and, unless they decide it, the
 * tier-2 answer of the clause with the rest of the root in `t`'s place.
 * `nullptr` when the root holds no literal member.
 *
 * Exact: `∃X(f = 0 ∧ (⋁L ∨ ⋁T′)) = ⋁_L ∃X(f = 0 ∧ ℓ) ∨ ∃X(f = 0 ∧ ⋁T′)`,
 * and the second disjunct implies `pos`.
 */
template <NodeType node>
tref middle_tier(tref t, const trefs& conjuncts, const block& X,
	const std::vector<component<node>>& comps, ctx<node>& c)
{
	trefs L, rest;
	for (tref m : members<node>(t))
		(is_literal<node>(m) ? L : rest).push_back(m);
	if (L.empty()) return nullptr;
	join_builder<node, false> r;
	for (tref l : L)
		if (r.insert(negative_condition<node>(term_of<node>(l, c.order),
			comps, X, c))) return r.result();
	trefs clause = conjuncts;
	*std::ranges::find(clause, t) = simplified_or_join<node>(rest);
	r.insert(finite_tier2<node>(clause, X, c));
	return r.result();
}

} // namespace finite_detail

// --- opaque? ----------------------------------------------------------------------

template <NodeType node>
bool finite_opaque(tref conjunct, const block& X, ba_type_id type,
	const var_order<node>& order)
{
	using tau = tree<node>;
	auto literal_opaque = [&X, type, &order](tref l) {
		return !is_pure_equation<node>(l, type)
			|| atomless_opaque<node>(l, X, order);
	};
	if (is_child<node>(atom_of<node>(conjunct), tau::bf_eq))
		return literal_opaque(conjunct);
	if (is_negative_tree<node>(conjunct)) {
		// LITERAL-WISE: one walk down to the negated equations, each
		// once, stopping at the first opaque one and never entering a
		// term.
		bool opaque = false;
		auto check = [&opaque, &literal_opaque](tref m) {
			if (!is_negated_equation<node>(m) || !literal_opaque(m))
				return true;
			return opaque = true, false;
		};
		auto up = [](tref) {};
		pre_order<node>(conjunct).search_unique(check,
			while_is_formula<node>, up);
		return opaque;
	}
	// Swallowed whole into the tier-2 query when the sweep can read it.
	if (is_child_quantifier<node>(conjunct)
		|| finite_detail::is_emission<node>(conjunct))
		return !is_pure_formula<node>(conjunct, type);
	return true;
}

// --- SUBSUME_NEGATIVES ------------------------------------------------------------

namespace finite_detail {

/**
 * @brief `SUBSUME_NEGATIVES`' scan, its three rules in the spec's order.
 * `lits[i]` holds the literals conjunct `i` brings — itself for a single,
 * the members of a flat tree, none otherwise — and `out[i]` the conjunct as
 * it ends up: `nullptr` once dropped, a pruned tree rebuilt.
 *
 * Each rule tests against survivors alone, so every chain of drops ends at
 * a negative that stays: no two negatives drop each other.
 */
template <NodeType node>
void subsume_scan(tref f, std::vector<trefs>& lits, trefs& out, ctx<node>& c)
{
	using tau = tree<node>;
	const tref fn = simplify_term<node>(build_bf_neg<node>(f), c.order);
	auto below = [&fn, &c](tref gi, tref gj) {
		const tref t = simplify_term<node>(build_bf_and<node>(
			build_bf_and<node>(fn, gi), build_bf_neg<node>(gj)),
			c.order);
		return tau::get(build_bf_eq_0<node>(t)).equals_T();
	};
	// Some `g ∈ gs` with `g ⊑ h`, and some with `h ⊑ g`.
	auto some_below = [&below](const trefs& gs, tref h) {
		return std::ranges::any_of(gs, [&](tref g) {
			return below(g, h); });
	};
	auto some_above = [&below](const trefs& gs, tref h) {
		return std::ranges::any_of(gs, [&](tref g) {
			return below(h, g); });
	};
	std::vector<trefs> terms(lits.size());
	for (size_t i = 0; i < lits.size(); ++i)
		for (tref l : lits[i])
			terms[i].push_back(term_of<node>(l, c.order));
	// Inside a flat tree, a disjunction, the smaller member goes.
	std::vector<size_t> trees;
	for (size_t i = 0; i < lits.size(); ++i) {
		if (lits[i].size() < 2) continue;
		trefs kl, kt;
		for (size_t m = 0; m < lits[i].size(); ++m) {
			if (some_above(kt, terms[i][m])) continue;
			kl.push_back(lits[i][m]);
			kt.push_back(terms[i][m]);
		}
		if (kl.size() < lits[i].size()) {
			out[i] = simplified_or_join<node>(kl);
			lits[i] = std::move(kl);
			terms[i] = std::move(kt);
		}
		if (lits[i].size() >= 2) trees.push_back(i);
	}
	// In the conjunction of singles the larger goes.
	std::vector<size_t> keep;
	trefs kept;
	for (size_t j = 0; j < lits.size(); ++j) {
		if (lits[j].size() != 1) continue;
		if (some_below(kept, terms[j][0])) {
			out[j] = nullptr;
			continue;
		}
		keep.push_back(j);
		kept.push_back(terms[j][0]);
	}
	// Across, each rule against what survived the one before: a tree a
	// single implies, a single a tree implies, a tree another kept tree
	// implies.
	for (size_t t : trees)
		if (std::ranges::any_of(terms[t], [&](tref m) {
			return some_below(kept, m); })) out[t] = nullptr;
	for (size_t k : keep)
		if (std::ranges::any_of(trees, [&](size_t t) {
			return out[t] && std::ranges::all_of(terms[t],
				[&](tref m) { return below(m, terms[k][0]); });
		})) out[k] = nullptr;
	std::vector<size_t> kept_trees;
	for (size_t t : trees) {
		if (!out[t]) continue;
		if (std::ranges::any_of(kept_trees, [&](size_t s) {
			return std::ranges::all_of(terms[s], [&](tref m) {
				return some_above(terms[t], m); });
		})) out[t] = nullptr;
		else kept_trees.push_back(t);
	}
}

} // namespace finite_detail

template <NodeType node>
subsume_result<node> subsume_negatives(tref f, const trefs& conjuncts,
	const block& X, ctx<node>& c)
{
	using tau = tree<node>;
	// The gate counts the literals over the singles and the flat trees.
	std::vector<trefs> lits(conjuncts.size());
	size_t count = 0;
	for (size_t i = 0; i < conjuncts.size(); ++i) {
		if (is_negated_equation<node>(conjuncts[i]))
			lits[i] = { conjuncts[i] };
		else if (is_flat_tree<node>(conjuncts[i]))
			lits[i] = members<node>(conjuncts[i]);
		count += lits[i].size();
	}
	trefs out = conjuncts;
	if (count <= c.subsume_max)
		finite_detail::subsume_scan<node>(f, lits, out, c);
	// Gated or not, `X` keeps what still occurs, in its own order.
	subsume_result<node> r;
	subtree_unordered_set<node> held;
	for (tref m : out) {
		if (!m) continue;
		r.conjuncts.push_back(m);
		if (is_negated_equation<node>(m) || is_negative_tree<node>(m))
			r.negatives.push_back(m);
		for (tref x : fv_intersect<node>(m, X)) held.insert(x);
	}
	for (tref x : X)
		if (held.contains(x)) r.X.push_back(tau::trim_right_sibling(x));
	return r;
}

// --- the closing ------------------------------------------------------------------

template <NodeType node, typename Decide>
tref query_or_rewrap(const trefs& conjuncts, const block& X, Decide&& decide)
{
	using tau = tree<node>;
	using tbdd = tau_term_bdd<node>;
	// The re-wrap goes over the PREPARED clause; the finished form exists
	// for the query alone.
	const tref phi = simplified_and_join<node>(conjuncts);
	const tref q = finish_terms<node>(rewrap<node>(phi, X));
	if (tau::get(q).equals_T() || tau::get(q).equals_F()) return q;
	const trefs Z = get_free_vars<node>(q);
	if (Z.empty()) {
		if (const std::optional<bool> r =
			decide(canonicalise_binder_ids<node>(q)))
			return *r ? _T<node>() : _F<node>();
		return rewrap<node>(phi, X);
	}
	// Open: valid under `∀Z`, or unsatisfiable under `∃Z`; detection only.
	const tref k = canonicalise_binder_ids<node>(
		rewrap<node>(q, Z, tbdd::all));
	if (decide(k) == true) return _T<node>();
	// Ids depend on depth alone, so the `∃Z` key is the `∀Z` one with its
	// closing binders flipped.
	block Zk;
	tref body = k;
	for (size_t i = 0; i < Z.size(); ++i) {
		Zk.push_back(tau::trim_right_sibling(binder_var<node>(body)));
		body = tau::trim_right_sibling(binder_body<node>(body));
	}
	if (decide(rewrap<node>(body, Zk, tbdd::ex)) == false) return _F<node>();
	return rewrap<node>(phi, X);
}

// --- FINITE_TIER2 -----------------------------------------------------------------

template <NodeType node>
tref finite_tier2(const trefs& conjuncts, const block& X, ctx<node>& c) {
	using tau = tree<node>;
	const std::vector<part<node>> ps = parts<node>(conjuncts, X);
	// Smallest first by `Σ|conjunct|`, ties in the parts' order.
	std::vector<std::pair<size_t, size_t>> by_size;
	for (size_t i = 0; i < ps.size(); ++i) {
		if (std::ranges::all_of(ps[i].conjuncts, [](tref m) {
			return is_child<node>(m, tau::bf_eq); })) continue;
		size_t size = 0;
		for (tref m : ps[i].conjuncts) size += formula_size<node>(m);
		by_size.emplace_back(size, i);
	}
	std::ranges::sort(by_size);
	auto decide = [&c](tref q) { return decide_finite<node>(q, c); };
	join_builder<node, true> acc;
	trefs open;
	for (const auto& sized : by_size) {
		const size_t i = sized.second;
		// §7 FINITE_QUERY on the part.
		const tref r = query_or_rewrap<node>(ps[i].conjuncts,
			ps[i].vars, decide);
		if (!tau::get(r).equals_T() && !tau::get(r).equals_F())
			open.push_back(r);
		if (acc.insert(r)) return _F<node>();
	}
	// THE JOINT CLOSING, over all survivors once two share a free variable.
	bool shared = false;
	for (size_t i = 0; i < open.size() && !shared; ++i)
		for (size_t j = i + 1; j < open.size() && !shared; ++j)
			shared = fv_meets<node>(open[i],
				get_free_vars<node>(open[j]));
	if (shared) {
		const tref joint = simplified_and_join<node>(open);
		const trefs Z = get_free_vars<node>(joint);
		const tref q = finish_terms<node>(rewrap<node>(joint, Z));
		if (decide_finite<node>(canonicalise_binder_ids<node>(q), c)
			== false) return _F<node>();
	}
	return acc.result();
}

// --- ELIMINATE_FINITE_CLAUSE ------------------------------------------------------

template <NodeType node>
tref eliminate_finite_clause(const trefs& conjuncts, const block& X,
	ctx<node>& c)
{
	using tau = tree<node>;
	// 1. FREEZE, by connected component: what is left is pure and
	// readable, through its terms or as a query.
	const freeze_result<node> fr = freeze_opaque_components<node>(conjuncts,
		X, [&X, &c](tref m) {
			return finite_opaque<node>(m, X, c.type, c.order); });
	if (fr.X.empty()) return fr.frozen;
	// 2. SQUEEZE, then SUBSUME modulo the whole `f`. The components
	// stand: a variable the subsumption drops sat in no positive.
	const squeeze_result<node> s = squeeze<node>(fr.conjuncts, fr.X,
		c.order);
	if (s.decided_false) return _F<node>();
	const subsume_result<node> sb = subsume_negatives<node>(s.f,
		s.conjuncts, fr.X, c);
	if (sb.X.empty())
		return simplified_and_join<node>(concat(trefs{ fr.frozen },
			sb.conjuncts));
	// 3. `pos`, implied by the clause: its `F` decides before any query.
	const tref pos = positive_condition<node>(s.comps, c);
	if (tau::get(pos).equals_F()) return _F<node>();
	// 4. A unit or an emission does not factor: tier 2 whatever else.
	const bool swallowed = std::ranges::any_of(sb.conjuncts, [](tref m) {
		return is_child_quantifier<node>(m)
			|| finite_detail::is_emission<node>(m); });
	const trefs& negs = sb.negatives;
	if (!swallowed && negs.empty())
		return simplified_and_join<node>(trefs{ fr.frozen, pos });
	if (!swallowed && negs.size() == 1) {
		const tref n = negs[0];
		// TIERS 0–1: a flat tree's condition is the ∨-join of its
		// literals' conditions.
		if (is_literal<node>(n) || is_flat_tree<node>(n)) {
			const tref neg = is_negative_tree<node>(n)
				? tree_condition<node>(n, s.comps, sb.X, c)
				: negative_condition<node>(
					term_of<node>(n, c.order), s.comps,
					sb.X, c);
			if (tau::get(neg).equals_F()) return _F<node>();
			return simplified_and_join<node>(
				trefs{ fr.frozen, pos, neg });
		}
		// 4′. THE MIDDLE TIER.
		if (const tref r = finite_detail::middle_tier<node>(n,
			sb.conjuncts, sb.X, s.comps, c))
			return simplified_and_join<node>(
				trefs{ fr.frozen, pos, r });
	}
	// 5. TIER 2; `pos` stands beside it for the parts it already decides.
	return simplified_and_join<node>(trefs{ fr.frozen, pos,
		finite_tier2<node>(sb.conjuncts, sb.X, c) });
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__FINITE_TMPL_H__
