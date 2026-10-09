// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file blast.tmpl.h
 * @brief Template implementations for blast.h. Included by blast.h. blast.h
 * says what `DECIDE_FINITE` means; the comments here say how the sweep is
 * built.
 *
 * THE ENUMERATION, one per query. Every bound variable of the canonical
 * query — a formula binder's variable or a chain's subscript — is ranked
 * `r ∈ 0..R−1` by first occurrence in pre-order, and its bit `i` is the
 * library variable `i·R + r + 1`: slice-major, so that an equation's `⋀ᵢ`
 * stays linear (§7). A constant is read against the same enumeration: its
 * bit `i` is the character at `width − 1 − i` of its binary string, most
 * significant first. Canonical ids make one rank per variable node sound:
 * sibling scopes that share an id share its bits, and each quantifies them
 * before the two are combined; an outer id exceeds every id below it, so no
 * scope shadows another.
 *
 * THE BUDGET: the universe `V` is append-only, so its growth since the
 * sweep began is the sweep's allocation count. Past `ctx.qbf_node_max` the
 * walk unwinds and the sweep returns nothing.
 *
 * TWO MEMOS local to the call, formula node → BDD and term node → its
 * slices, so a shared subformula is blasted once: a node's BDD is a function
 * of its variables' bits, which are fixed for the sweep.
 *
 * A variable's bit is minted with the engine's `add`, not with `bit`, which
 * also names the variable in the dictionary `sbf` shares and accepts ids
 * only in sequence; the sweep's ids name nothing.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__BLAST_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__BLAST_TMPL_H__

#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace idni::tau_lang::anti_prenexing {

namespace blast_detail {

using engine    = bdd<Bool, sweep_options>;
using handle    = bdd_handle<Bool, sweep_options>;
using sweep_bdd = hbdd<Bool, sweep_options>;
/// A term's BDDs, slice `i` for its bit `i`.
using slices    = std::vector<sweep_bdd>;

/// Unwinds the walk once the sweep outgrows its budget: an abandoned sweep
/// has no partial answer.
struct over_budget {};

/// `BIT_BLAST`'s `FAIL` (§7).
[[noreturn]] inline void fail() {
	throw std::logic_error(
		"BIT_BLAST: the query holds a node outside the pure fragment");
}

/// One sweep: its enumeration, its budget, and the blast's rules, each
/// memoised per node.
template <NodeType node>
struct sweeper {
	using tau = tree<node>;

	const ba_type_id type;
	const size_t width;
	const size_t budget;
	const subtree_unordered_map<node, size_t>& rank;
	const size_t start = engine::V.size();
	subtree_unordered_map<node, sweep_bdd> formulas;
	subtree_unordered_map<node, slices> terms;

	sweeper(ba_type_id type, size_t width, size_t budget,
		const subtree_unordered_map<node, size_t>& rank)
		: type(type), width(width), budget(budget), rank(rank) {}

	/// `r`, once the universe's growth is checked against the budget.
	sweep_bdd note(sweep_bdd r) const {
		if (engine::V.size() - start > budget) throw over_budget{};
		return r;
	}

	/// A node carrying a BA type of its own that is not the query's; the
	/// untyped id `0` is of it, as `is_pure_term` reads it.
	bool foreign(tref m) const {
		const ba_type_id t = tree<node>::get(m).get_ba_type();
		return t != 0 && t != type;
	}

	/// The library variable of bit `i` of the bound variable `v`.
	uint_t id(tref v, size_t i) const {
		const auto it = rank.find(v);
		if (it == rank.end()) fail();
		return static_cast<uint_t>(i * rank.size() + it->second + 1);
	}

	sweep_bdd formula(tref n);
	const slices& term(tref n);
};

template <NodeType node>
sweep_bdd sweeper<node>::formula(tref n) {
	if (auto it = formulas.find(n); it != formulas.end()) return it->second;
	const tau& t = tau::get(n);
	sweep_bdd r;
	if (t.equals_T()) r = handle::one();
	else if (t.equals_F()) r = handle::zero();
	else if (t.child_is(tau::wff_neg)) r = note(~formula(t[0].first()));
	else if (t.child_is(tau::wff_and) || t.child_is(tau::wff_or)) {
		const bool conj = t.child_is(tau::wff_and);
		for (tref m : t[0].children()) {
			const sweep_bdd b = formula(m);
			r = !r ? b : note(conj ? r & b : r | b);
		}
	} else if (is_child_quantifier<node>(n)) {
		const tref v = binder_var<node>(n);
		if (foreign(v)) fail();
		std::vector<uint_t> bits(width);
		for (size_t i = 0; i < width; ++i) bits[i] = id(v, i);
		r = note(formula(binder_body<node>(n))->quantify(std::move(bits),
			binder_kind<node>(n) == tau_term_bdd<node>::all));
	} else if (t.child_is(tau::bf_eq)) {
		// The general form `⋀ᵢ (⟦l⟧ᵢ ↔ ⟦r⟧ᵢ)`: a swallowed unit's body is
		// not in zero form (§7).
		const slices& ls = term(t[0].first());
		const slices& rs = term(t[0].second());
		slices eqs(width);
		for (size_t i = 0; i < width; ++i)
			eqs[i] = note(~(ls[i] ^ rs[i]));
		r = note(handle::and_many(eqs));
	} else fail();
	return formulas.emplace(n, r).first->second;
}

template <NodeType node>
const slices& sweeper<node>::term(tref n) {
	if (auto it = terms.find(n); it != terms.end()) return it->second;
	const tau& t = tau::get(n);
	if (!t.is(tau::bf) || foreign(n) || foreign(t.first())) fail();
	const tau& a = t[0];
	slices s(width);
	if (a.is(tau::variable)) {
		for (size_t i = 0; i < width; ++i)
			s[i] = note(handle::get(engine::add(id(t.first(), i),
				engine::T, engine::F)));
	} else if (a.is(tau::bf_t) || a.is(tau::bf_f)) {
		s.assign(width, a.is(tau::bf_t) ? handle::one() : handle::zero());
	} else if (a.is_ba_constant()) {
		const auto value = a.get_ba_constant();
		const bv* b = std::get_if<bv>(&value);
		if (b == nullptr || !b->isBitVectorValue()) fail();
		const std::string bits = b->getBitVectorValue();
		if (bits.size() != width) fail();
		for (size_t i = 0; i < width; ++i)
			s[i] = bits[width - 1 - i] == '1'
				? handle::one() : handle::zero();
	} else if (a.is(tau::bf_neg)) {
		const slices& x = term(a.first());
		for (size_t i = 0; i < width; ++i) s[i] = note(~x[i]);
	} else if (a.is(tau::bf_and) || a.is(tau::bf_or) || a.is(tau::bf_xor)) {
		const bool conj = a.is(tau::bf_and), disj = a.is(tau::bf_or);
		auto op = [conj, disj](const sweep_bdd& l, const sweep_bdd& r) {
			return conj ? l & r : disj ? l | r : l ^ r;
		};
		bool first = true;
		for (tref m : a.children()) {
			const slices& x = term(m);
			for (size_t i = 0; i < width; ++i)
				s[i] = first ? x[i] : note(op(s[i], x[i]));
			first = false;
		}
	} else if (a.is(tau::bf_fall) || a.is(tau::bf_fex)) {
		// The whole chain at once: slice `i` sees bit `i` of a subscript
		// alone, and the subscripts go innermost first (§7).
		const auto [prefix, body] = strip_chain<node>(n);
		for (const auto& q : prefix) if (foreign(q.first)) fail();
		const slices& x = term(body);
		for (size_t i = 0; i < width; ++i) {
			sweep_bdd b = x[i];
			// Neighbouring subscripts of one kind commute, so each
			// run of them is quantified in one pass.
			for (auto q = prefix.rbegin(); q != prefix.rend();) {
				const auto kind = q->second;
				std::vector<uint_t> ids;
				for (; q != prefix.rend() && q->second == kind; ++q)
					ids.push_back(id(q->first, i));
				b = note(b->quantify(std::move(ids),
					kind == tau_term_bdd<node>::all));
			}
			s[i] = b;
		}
	} else fail();
	return terms.emplace(n, std::move(s)).first->second;
}

/// The sweep over the canonical query `k`: its truth, or nothing when its
/// bits need more ids than the universe has or it outgrew
/// `ctx.qbf_node_max`.
template <NodeType node>
std::optional<bool> sweep(tref k, const ctx<node>& c) {
	using tau = tree<node>;
	DBG(assert(is_bv_type_family<node>(c.type));)
	[[maybe_unused]] static const bool initialised =
		(bdd_init<Bool, sweep_options>(), true);
	subtree_unordered_map<node, size_t> rank;
	auto enumerate = [&rank](tref m) {
		if (is_logical_or_functional_quant<node>(m))
			rank.try_emplace(tau::trim(m), rank.size());
		return true;
	};
	pre_order<node>(k).visit_unique(enumerate);
	const size_t width = get_bv_width<node>(c.type);
	// The library checks an id against the shift width in Debug alone.
	if (width * rank.size() >= size_t(1) << sweep_options.shiftW)
		return std::nullopt;
	sweeper<node> s(c.type, width, c.qbf_node_max, rank);
	try {
		const sweep_bdd r = s.formula(k);
		DBG(assert(r->is_one() || r->is_zero());)
		return r->is_one();
	} catch (const over_budget&) {
		return std::nullopt;
	}
}

} // namespace blast_detail

template <NodeType node>
std::optional<bool> decide_finite(tref k, ctx<node>& c) {
	if (const auto r = lookup<node, table::qbf_memo>(k)) return *r;
	// Constant before exponential (invariant 8): an earlier fallback's
	// verdict decides without a sweep; a cached `unknown` is a miss, since
	// the sweep may still succeed.
	if (const auto a = lookup<node, table::solver_memo>(k);
		a && *a != answer::unknown) return *a == answer::sat;
	if (const auto r = blast_detail::sweep<node>(k, c)) {
		store<node, table::qbf_memo>(k, *r);
		return r;
	}
	const answer a = ask<node>(k);
	if (a != answer::unknown) return a == answer::sat;
	taint<node>();
	return std::nullopt;
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__BLAST_TMPL_H__
