// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file dag.tmpl.h
 * @brief Template implementations for dag.h (package A). Included by dag.h.
 *
 * Everything here is a facet of the existing hash-consed tree, so the
 * implementations are wrappers over what the codebase already has:
 * `get_free_vars` for FV, `hash_lcrs_tref` and `subtree_less` for the content
 * hash and the content order, `get_leaves` for the D1 member view,
 * `subtree_vec_contains` for the membership scan, `build_wff_*` for the raw
 * constructors, the `is_child_*` family for the wrapper-form classification,
 * and `pre_order` (whose `up` callback is the post-order visit) for the two
 * walks that fill a table. The three cached facets reach their tables through
 * ctx.h's `find`/`store` and never spell `#ifdef TAU_CACHE`.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_TMPL_H__

#include <algorithm>

#include "ctx.h"

namespace idni::tau_lang::anti_prenexing {

// --- sizes, free variables, hash ---------------------------------------------

namespace detail {

/// The nodes `formula_size` walks through: a `wff` wrapper, and the operator
/// nodes whose formula children carry their own `|·|` — the two connectives,
/// the negation and the two binders. Everything below anything else (an
/// atom's operator node and its terms, `wff_ref`, the temporal operators, the
/// constants, a binder's variable) is opaque and is never entered, so the
/// wrapper above it counts 1.
template <NodeType node>
bool size_structural(tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	return t.is(tau::wff) || t.is(tau::wff_and) || t.is(tau::wff_or)
		|| t.is(tau::wff_neg) || t.is(tau::wff_ex) || t.is(tau::wff_all);
}

/// A `wff` wrapper whose formula children count towards `|·|`: an ∧-node, an
/// ∨-node, a negation or a binder. Every other wrapper — an atom, a
/// reference, a temporal operator, a constant, and any shape this module does
/// not restructure — counts 1 on its own (§1, "opaque like an atom").
template <NodeType node>
bool size_has_formula_children(tref n) {
	using tau = tree<node>;
	return is_conjunction<node>(n) || is_disjunction<node>(n)
		|| is_binder<node>(n) || is_child<node>(n, tau::wff_neg);
}

} // namespace detail

template <NodeType node>
size_t formula_size(tref n) {
	using tau = tree<node>;
	if (n == nullptr) return 0;
	if (const size_t* cached = find<node, table::size_memo>(n); cached)
		return *cached;

	// Post-order fill: `up` runs on a node after its children, so a
	// wrapper's formula children are measured before its own entry is
	// computed. `local` holds this walk's results; a wrapper pruned
	// because an earlier query already measured it is read back from
	// `size_memo`, and anything opaque (a term, a variable) contributes
	// nothing. The traversal is iterative, which a chain needs: a spine
	// is as deep as it is long.
	subtree_unordered_map<node, size_t> local;
	auto measured = [&local](tref m) -> size_t {
		if (auto it = local.find(m); it != local.end()) return it->second;
		if (const size_t* c = find<node, table::size_memo>(m); c)
			return *c;
		return 0;
	};
	auto down = [](tref) { return true; };
	auto descend = [](tref m) {
		// A wrapper is entered unless it is already measured; below it,
		// only the operator nodes that have formula children are.
		// Pruning here and not one level up is deliberate: a pruned node
		// gets no `up` call, and every wrapper needs one.
		if (tree<node>::get(m).is(tree<node>::wff))
			return find<node, table::size_memo>(m) == nullptr;
		return detail::size_structural<node>(m);
	};
	auto up = [&local, &measured](tref m) {
		using tau = tree<node>;
		const auto& t = tau::get(m);
		if (!t.is(tau::wff)) return;
		size_t sz = 1;
		if (detail::size_has_formula_children<node>(m))
			for (tref c : tau::get(t.first()).children())
				sz += measured(c);
		local.emplace(m, sz);
		store<node, table::size_memo>(m, sz);
	};
	pre_order<node>(n).visit_unique(down, descend, up);
	return measured(n);
}

template <NodeType node>
const trefs& fv(tref n) {
	return get_free_vars<node>(n);
}

template <NodeType node>
bool fv_meets(tref n, const block& X) {
	using tau = tree<node>;
	const trefs& vars = fv<node>(n);
	if (vars.empty()) return false;
	for (tref x : X)
		if (std::binary_search(vars.begin(), vars.end(), x,
			tau::subtree_less)) return true;
	return false;
}

template <NodeType node>
block fv_intersect(tref n, const block& X) {
	using tau = tree<node>;
	block out;
	{
		// `vars` points into `get_free_vars`' table, which a GC sweep
		// rebuilds, so nothing that can construct a node runs while it
		// is held: the trim below comes after.
		const trefs& vars = fv<node>(n);
		if (vars.empty()) return out;
		out.reserve(std::min(X.size(), vars.size()));
		for (tref x : X)
			if (std::binary_search(vars.begin(), vars.end(), x,
				tau::subtree_less)) out.push_back(x);
	}
	// A block variable read off a binder carries the body as its right
	// sibling; `fv` hands out trimmed refs, so trim to match.
	for (tref& x : out) x = tau::trim_right_sibling(x);
	return out;
}

template <NodeType node>
size_t content_hash(tref n) {
	return hash_lcrs_tref<node>{}(n);
}

// --- content order --------------------------------------------------------------

template <NodeType node>
bool content_less(tref a, tref b) {
	return tree<node>::subtree_less(a, b);
}

// --- member view and raw canonical chains (D1) ---------------------------------

template <NodeType node>
trefs members(tref n) {
	using tau = tree<node>;
	if (is_conjunction<node>(n)) return get_leaves<node>(n, tau::wff_and);
	if (is_disjunction<node>(n)) return get_leaves<node>(n, tau::wff_or);
	return trefs{ n };
}

template <NodeType node>
bool is_member(tref n, tref m) {
	// By CONTENT: a member that is not the last child of its operator node
	// carries a right sibling, so it is a different `tref` from the same
	// formula standing alone.
	return subtree_vec_contains<node>(members<node>(n), m);
}

namespace detail {

/// The shared body of `canonical_and` and `canonical_or`: sort in content
/// order, drop content-equal duplicates, fold left through the binary
/// builder. `neutral` is returned for an empty list.
template <NodeType node, typename Build>
tref canonical_chain(trefs& ms, Build&& build, tref neutral) {
	using tau = tree<node>;
	if (ms.empty()) return neutral;
	std::sort(ms.begin(), ms.end(), tau::subtree_less);
	ms.erase(std::unique(ms.begin(), ms.end(), [](tref a, tref b) {
		return tau::subtree_equals(a, b);
	}), ms.end());
	tref result = tau::get(ms.front()).has_right_sibling()
		? tau::trim_right_sibling(ms.front()) : ms.front();
	for (size_t i = 1; i < ms.size(); ++i) result = build(result, ms[i]);
	return result;
}

} // namespace detail

template <NodeType node>
tref canonical_and(trefs members) {
	DBG(assert(!members.empty());)
	return detail::canonical_chain<node>(members,
		[](tref l, tref r) { return build_wff_and<node>(l, r); },
		_T<node>());
}

template <NodeType node>
tref canonical_or(trefs members) {
	DBG(assert(!members.empty());)
	return detail::canonical_chain<node>(members,
		[](tref l, tref r) { return build_wff_or<node>(l, r); },
		_F<node>());
}

template <NodeType node>
tref complement_of(tref l) {
	using tau = tree<node>;
	if (is_child<node>(l, tau::wff_neg)) return tau::trim2(l);
	return build_wff_neg<node>(l);
}

// --- neg(φ) slot ----------------------------------------------------------------

template <NodeType node>
tref neg_of(tref n) {
	const tref* cached = find<node, table::neg_memo>(n);
	return cached ? *cached : nullptr;
}

template <NodeType node>
void set_neg(tref n, tref negated) {
	DBG(assert(n != nullptr && negated != nullptr);)
	store<node, table::neg_memo>(n, negated);
}

// --- classification (§1 atom shapes, literals, binders, trees) -------------------

template <NodeType node>
bool is_atom(tref n) { return is_atomic_fm<node>(n); }

template <NodeType node>
bool is_equation(tref n) { return is_child<node>(n, tree<node>::bf_eq); }

template <NodeType node>
bool is_order_atom(tref n) {
	using tau = tree<node>;
	return is_child<node>(n, tau::bf_lteq) || is_child<node>(n, tau::bf_lt);
}

template <NodeType node>
bool is_negated_atom(tref n) {
	using tau = tree<node>;
	return is_child<node>(n, tau::wff_neg) && is_atom<node>(tau::trim2(n));
}

template <NodeType node>
bool is_literal(tref n) { return is_atom<node>(n) || is_negated_atom<node>(n); }

template <NodeType node>
bool is_negated_equation(tref n) {
	using tau = tree<node>;
	return is_child<node>(n, tau::wff_neg)
		&& is_equation<node>(tau::trim2(n));
}

template <NodeType node>
tref atom_of(tref literal) {
	return is_negated_atom<node>(literal) ? tree<node>::trim2(literal)
					     : literal;
}

template <NodeType node>
bool is_true(tref n) { return tree<node>::get(n).equals_T(); }

template <NodeType node>
bool is_false(tref n) { return tree<node>::get(n).equals_F(); }

template <NodeType node>
bool is_conjunction(tref n) { return is_child<node>(n, tree<node>::wff_and); }

template <NodeType node>
bool is_disjunction(tref n) { return is_child<node>(n, tree<node>::wff_or); }

template <NodeType node>
bool is_binder(tref n) { return is_child_quantifier<node>(n); }

template <NodeType node>
bool is_reference(tref n) { return is_child<node>(n, tree<node>::wff_ref); }

template <NodeType node>
bool is_temporal(tref n) { return is_child_temporal_quantifier<node>(n); }

template <NodeType node>
bool is_negative_tree(tref n) {
	if (!is_disjunction<node>(n)) return false;
	if (const bool* cached = find<node, table::negative_tree_memo>(n);
		cached) return *cached;
	bool negative = true;
	// The walk stays on the mixed ∨/∧ skeleton: `skeleton` lets it enter a
	// wrapper and the two connective nodes and nothing else, so it stops
	// on top of every leaf, and `check` decides there. A leaf that is not
	// a negated equation returns false, which ends the search.
	auto check = [&negative](tref m) {
		using tau = tree<node>;
		if (!tau::get(m).is(tau::wff)) return true;
		if (is_conjunction<node>(m) || is_disjunction<node>(m))
			return true;
		if (is_negated_equation<node>(m)) return true;
		return negative = false;
	};
	auto skeleton = [](tref m) {
		using tau = tree<node>;
		const auto& t = tau::get(m);
		return t.is(tau::wff) || t.is(tau::wff_and) || t.is(tau::wff_or);
	};
	auto up = [](tref) {};
	pre_order<node>(n).search(check, skeleton, up);
	store<node, table::negative_tree_memo>(n, negative);
	return negative;
}

template <NodeType node>
bool is_flat_tree(tref n) {
	if (!is_negative_tree<node>(n)) return false;
	for (tref m : members<node>(n))
		if (!is_literal<node>(m)) return false;
	return true;
}

template <NodeType node>
bool is_plain_conjunct(tref n) { return !is_disjunction<node>(n); }

template <NodeType node>
bool is_disjunctive_conjunct(tref n) { return is_disjunction<node>(n); }

// --- binder accessors -----------------------------------------------------------

template <NodeType node>
binder binder_kind(tref n) {
	DBG(assert(is_binder<node>(n));)
	return is_child<node>(n, tree<node>::wff_ex) ? binder::ex : binder::all;
}

template <NodeType node>
tref binder_var(tref n) {
	DBG(assert(is_binder<node>(n));)
	return tree<node>::trim2(n);
}

template <NodeType node>
tref binder_body(tref n) {
	DBG(assert(is_binder<node>(n));)
	return tree<node>::get(n)[0].second();
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__DAG_TMPL_H__
