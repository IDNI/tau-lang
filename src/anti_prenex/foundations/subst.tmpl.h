// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file subst.tmpl.h
 * @brief Template implementations for subst.h (package C). Included by subst.h.
 *
 * Each substitution, and the `atoms` fill, is ONE `pre_order::visit` (tree.h;
 * the `up` callback is the post-order visit) over the ORIGINAL formula, with
 * a call-local structural memo `node → result` — §10's "one memo per
 * rewrite" — that also prunes: a node the memo holds, or one the occurrence
 * guard clears, is not entered, and a lookup miss means "unchanged". Nothing
 * is rebuilt by the traversal itself. NEW over the `apply_*` family and
 * `rewriter::replace` (ground rule 9): those rebuild a changed parent as the
 * binary node it was — a rewritten chain is never re-canonicalised (D1) —
 * run a pre-order `f` once per OCCURRENCE, and offer no per-node facet
 * guard; here every node is rebuilt in `up` from its children's results, a
 * chain top through the raw canonical constructors from its FULL member view
 * (D1), everything else through the hooked `tau::get` (D4), which re-links
 * every child from its value and left child, so a child carrying a right
 * sibling is fine. (The traversals' own rebuilds are hooked too —
 * `lcrs_tree::get` applies the process-wide hook — so a term rewritten by
 * B's `subst_plain` already folds `X & X′` before its atom is rebuilt here.)
 *
 * SPINE nodes — a `wff` wrapping the same connective as its parent operator
 * node, either nesting side — are transparent: entered unconditionally,
 * never asked for a facet (`fv` and `atoms` publish every root they are
 * asked about, and a k-chain has k spine nodes with O(k) sets each), never
 * given a memo entry or an `atoms_memo` row; the chain top folds them
 * through `members`. A spine node met later STANDALONE is entered again
 * (its members are hits) and rebuilt there — which is why the walks are
 * `visit` with memo-based pruning, not `visit_unique`: the traversal's own
 * set would skip that occurrence and leave the parent's lookup dangling.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_TMPL_H__

#include <algorithm>

#include "ctx.h"

namespace idni::tau_lang::anti_prenexing {

namespace subst_detail {

/// The operator nodes `[atm ↦ T/F]` and `atoms` descend through — the two
/// connectives and the negation, their common REACH (§4; ruling 1: units,
/// references and temporal operators opaque). `[x ← t]` adds binders,
/// references and the pre-NNF connectives in its own walk.
template <NodeType node>
bool reach_structural(tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	return t.is(tau::wff_and) || t.is(tau::wff_or) || t.is(tau::wff_neg);
}

/// `n` is a SPINE node: a `wff` wrapping the same connective as its parent
/// operator node `parent` (both nesting sides, as `members` flattens).
template <NodeType node>
bool is_spine(tref n, tref parent) {
	using tau = tree<node>;
	if (!parent) return false;
	const auto& p = tau::get(parent);
	if (p.is(tau::wff_and)) return is_conjunction<node>(n);
	if (p.is(tau::wff_or))  return is_disjunction<node>(n);
	return false;
}

/// `x ∈ FV(n)`: the occurrence guard of `[x ← t]`, one cached test (§10).
template <NodeType node>
bool has_free(tref n, tref x) {
	const trefs& vars = fv<node>(n);
	return std::binary_search(vars.begin(), vars.end(), x,
		tree<node>::subtree_less);
}

/// A chain top rebuilt from its FULL member view: every member mapped through
/// `result`, a mapped member that is itself a same-connective node spliced
/// (one level — a member cannot become one post-NNF; the generic path can
/// hand one back), then the raw canonical constructor (D1): sorted,
/// deduplicated, left-nested, `T`/`F` folded along the fold by the hooks.
/// `n` itself when no member changed (an identity substitution is the
/// identity; nothing is re-shaped for its own sake).
template <NodeType node, typename Result>
tref rebuild_chain(tref n, Result& result) {
	const bool conj = is_conjunction<node>(n);
	trefs out;
	bool changed = false;
	for (tref m : members<node>(n)) {
		tref r = result(m);
		changed |= r != m;
		if (conj ? is_conjunction<node>(r) : is_disjunction<node>(r))
			for (tref mm : members<node>(r)) out.push_back(mm);
		else out.push_back(r);
	}
	if (!changed) return n;
	return conj ? canonical_and<node>(std::move(out))
		    : canonical_or<node>(std::move(out));
}

/// The generic hooked rebuild: the same value over the mapped children, or
/// `n` itself when none changed. For a `wff` wrapper this is where the
/// construction hooks fire (`¬T`, a constant-only atom, `T`/`F` under a
/// pre-NNF connective); an operator node below it is minted raw, as the
/// builders mint it.
template <NodeType node, typename Result>
tref rebuild_generic(tref n, Result& result) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	trefs ch;
	bool changed = false;
	for (tref c : t.children()) {
		tref r = result(c);
		changed |= r != c;
		ch.push_back(r);
	}
	if (!changed) return n;
	return tau::get(t.value, ch);
}

/// The operator nodes `[x ← t]` enters below a `wff` it admitted: the
/// connectives, the negation, the binders, a reference down to its
/// arguments, and the pre-NNF connectives (ruling 3). Not a temporal
/// operator (ruling 1), not a binder's variable, not a reference's symbol,
/// offsets, type or fallback, not an atom's operator (an admitted atom is
/// rewritten whole by the visitor, so this is never asked about one).
template <NodeType node>
bool var_enters(const tree<node>& t) {
	using tau = tree<node>;
	switch (t.value.nt) {
		case tau::wff_and: case tau::wff_or: case tau::wff_neg:
		case tau::wff_ex: case tau::wff_all:
		case tau::wff_ref: case tau::ref: case tau::ref_args:
		case tau::ref_arg:
		case tau::wff_imply: case tau::wff_rimply: case tau::wff_equiv:
		case tau::wff_xor: case tau::wff_conditional: case tau::bf_interval:
			return true;
		default: return false;
	}
}

} // namespace subst_detail

// --- φ[x ← t] ----------------------------------------------------------------------

template <NodeType node>
tref subst_var(tref phi, tref x, tref t, const var_order<node>& order,
	const simplify_formula_fn& simplify_formula)
{
	using tau = tree<node>;
	using namespace subst_detail;
	DBG(assert(phi != nullptr && x != nullptr && t != nullptr);)
	x = tau::trim_right_sibling(x);
	DBG(assert(tau::get(x).is(tau::variable));)
	DBG(assert(tau::get(phi).is(tau::wff));)
	if (!has_free<node>(phi, x)) return phi;
#ifdef DEBUG
	// A COPY: `fv` hands out a reference into get_free_vars' table.
	const trefs t_vars = fv<node>(t);
	auto binds_t_var = [&t_vars](tref v) {
		return std::binary_search(t_vars.begin(), t_vars.end(),
			tau::trim_right_sibling(v), tau::subtree_less);
	};
#endif
	subtree_unordered_map<node, tref> memo;
	auto result = [&memo](tref m) -> tref {
		auto it = memo.find(m);
		return it == memo.end() ? m : it->second;
	};
	// An atom: both sides through subst_term (compose plus leaf rewrite when
	// BDD-backed, plain replace otherwise, a reference argument inside a
	// term re-simplified there), rebuilt through the hooked constructor as
	// simplify_atom rebuilds one (terms.tmpl.h) — a constant-only atom
	// folds; on a non-bitvector type a rebuilt order atom is the hooks'
	// equation (ruling 2).
	auto rewrite_atom = [&](tref m) -> tref {
		const tau& op = tau::get(m)[0];
		tref l = tau::trim_right_sibling(op.first());
		tref r = tau::trim_right_sibling(op.second());
		tref l2 = subst_term<node>(l, x, t, order, simplify_formula);
		tref r2 = subst_term<node>(r, x, t, order, simplify_formula);
		if (l2 == l && r2 == r) return m;
		return tau::get(tau::wff, tau::get(op.value.nt, l2, r2));
	};
	// Entered: a `wff` with `x` free (a spine node unconditionally), a `bf`
	// with `x` free — a reference's term argument, or a term child of a
	// pre-NNF shape; an atom's sides are never reached — and the operator
	// nodes of `var_enters`.
	// (the traversal calls the predicate on the root without a parent)
	auto descend = [&](tref m, tref parent = nullptr) -> bool {
		const tau& tm = tau::get(m);
		if (tm.is(tau::wff))
			return !memo.contains(m)
				&& (is_spine<node>(m, parent) || has_free<node>(m, x));
		if (tm.is(tau::bf))
			return !memo.contains(m) && has_free<node>(m, x);
		return var_enters<node>(tm);
	};
	// Pre-order: the leaves of this walk are decided here — a term argument
	// and an atom are rewritten whole, a temporal operator is left as it is
	// (ruling 1). A binder on the path is where capture would happen: the
	// depth rule of phase 0 says it cannot, Debug checks it.
	auto visitor = [&](tref m) -> bool {
		const tau& tm = tau::get(m);
		if (tm.is(tau::bf)) {
			tref r = subst_term<node>(m, x, t, order, simplify_formula);
			if (r != m) memo.emplace(m, r);
			return false;
		}
		if (!tm.is(tau::wff)) return true;
		if (is_atom<node>(m)) {
			tref r = rewrite_atom(m);
			if (r != m) memo.emplace(m, r);
			return false;
		}
		if (is_temporal<node>(m)) return false;
		DBG(if (is_binder<node>(m))
			assert(!binds_t_var(binder_var<node>(m)));)
		return true;
	};
	// Post-order: a node is rebuilt from its children's results. A chain
	// top from its member view, a spine node not at all, an argument node
	// re-emitted through the simplifier of its kind (inv. 6, §1) exactly
	// once, everything else generically through the hooked constructor.
	auto up = [&](tref m, tref parent) {
		const tau& tm = tau::get(m);
		if (tm.is(tau::wff)) {
			if (memo.contains(m) || is_spine<node>(m, parent)) return;
			tref r = (is_conjunction<node>(m) || is_disjunction<node>(m))
				? rebuild_chain<node>(m, result)
				: rebuild_generic<node>(m, result);
			if (r != m) memo.emplace(m, r);
			return;
		}
		if (tm.is(tau::wff_and) || tm.is(tau::wff_or)) return;
		if (tm.is(tau::ref_arg)) {
			tref a = tm.first();
			tref a2 = result(a);
			if (a2 == a) return;
			// The plain regime, as subst_term re-emits a nested argument:
			// an argument is never BDD-backed and must not become one.
			a2 = tau::get(a2).is(tau::wff) ? simplify_formula(a2)
						       : simplify_term<node>(a2);
			memo.emplace(m, tau::get(tm.value, a2));
			return;
		}
		tref r = rebuild_generic<node>(m, result);
		if (r != m) memo.emplace(m, r);
	};
	pre_order<node>(phi).visit(visitor, descend, up);
	return result(phi);
}

// --- φ[atm ↦ T/F] ------------------------------------------------------------------

template <NodeType node>
tref subst_atom(tref phi, tref atm, bool value) {
	using tau = tree<node>;
	using namespace subst_detail;
	DBG(assert(phi != nullptr && atm != nullptr);)
	atm = tau::trim_right_sibling(atm);
	DBG(assert(is_atom<node>(atm));)
	DBG(assert(tau::get(phi).is(tau::wff));)
	// The guard's first call fills atoms_memo for every non-spine wrapper in
	// the reach below `phi`, so every later guard is a lookup and a binary
	// search.
	if (!has_atom<node>(phi, atm)) return phi;
	const tref cst = value ? tau::_T() : tau::_F();
	subtree_unordered_map<node, tref> memo;
	auto result = [&memo](tref m) -> tref {
		auto it = memo.find(m);
		return it == memo.end() ? m : it->second;
	};
	auto descend = [&](tref m, tref parent = nullptr) -> bool {
		if (tau::get(m).is(tau::wff))
			return !memo.contains(m)
				&& (is_spine<node>(m, parent) || has_atom<node>(m, atm));
		return reach_structural<node>(m);
	};
	// An admitted atom's vocabulary is itself, so it IS `atm` — compared by
	// content: the atom as it occurs, never re-spelled (§3).
	auto visitor = [&](tref m) -> bool {
		if (!tau::get(m).is(tau::wff) || !is_atom<node>(m)) return true;
		if (tau::subtree_equals(m, atm)) memo.emplace(m, cst);
		return false;
	};
	// A negated occurrence receives the complement here: the `¬` wrapper is
	// rebuilt over `T`/`F` and the hook folds it.
	auto up = [&](tref m, tref parent) {
		const tau& tm = tau::get(m);
		if (tm.is(tau::wff)) {
			if (memo.contains(m) || is_spine<node>(m, parent)) return;
			tref r = (is_conjunction<node>(m) || is_disjunction<node>(m))
				? rebuild_chain<node>(m, result)
				: rebuild_generic<node>(m, result);
			if (r != m) memo.emplace(m, r);
			return;
		}
		if (!tm.is(tau::wff_neg)) return; // wff_and/wff_or: the wrapper's
		tref r = rebuild_generic<node>(m, result);
		if (r != m) memo.emplace(m, r);
	};
	pre_order<node>(phi).visit(visitor, descend, up);
	return result(phi);
}

// --- atoms_memo ----------------------------------------------------------------------

template <NodeType node>
const trefs& atoms(tref n) {
	using tau = tree<node>;
	using namespace subst_detail;
	DBG(assert(n != nullptr && tau::get(n).is(tau::wff));)
	if (const tref_set* row = find<node, table::atoms_memo>(n); row)
		return row->items;
	// Post-order fill over the reach: a wrapper's row is computed in `up`
	// from its children's rows — a chain top from its full member view,
	// which never names a spine node — and stored at once; a spine node gets
	// no row (ruling 5), the queried root always does. A wrapper that has a
	// row already is not entered.
	auto descend = [](tref m, tref = nullptr) -> bool {
		if (tau::get(m).is(tau::wff))
			return find<node, table::atoms_memo>(m) == nullptr;
		return reach_structural<node>(m);
	};
	auto visitor = [](tref) { return true; };
	auto up = [](tref m, tref parent) {
		const tau& tm = tau::get(m);
		if (!tm.is(tau::wff) || is_spine<node>(m, parent)) return;
		if (find<node, table::atoms_memo>(m)) return;
		trefs items;
		if (is_atom<node>(m)) items.push_back(tau::trim_right_sibling(m));
		else if (is_conjunction<node>(m) || is_disjunction<node>(m)) {
			// Every member is a non-spine wrapper of the reach, filled
			// before its chain top.
			subtree_set<node> merged;
			for (tref mm : members<node>(m)) {
				const tref_set* row = find<node, table::atoms_memo>(mm);
				DBG(assert(row != nullptr);)
				if (row) merged.insert(row->items.begin(), row->items.end());
			}
			items.assign(merged.begin(), merged.end());
		} else if (tm.child_is(tau::wff_neg)) {
			const tref_set* row =
				find<node, table::atoms_memo>(tau::trim2(m));
			DBG(assert(row != nullptr);)
			if (row) items = row->items;
		}
		// A binder, a reference, a temporal operator, a constant, anything
		// else: opaque — the empty vocabulary (§4, ruling 1).
		store<node, table::atoms_memo>(m, tref_set{ std::move(items) });
	};
	pre_order<node>(n).visit(visitor, descend, up);
	const tref_set* row = find<node, table::atoms_memo>(n);
	DBG(assert(row != nullptr);)
	return row->items;
}

template <NodeType node>
bool has_atom(tref n, tref atm) {
	const trefs& as = atoms<node>(n);
	return std::binary_search(as.begin(), as.end(), atm,
		tree<node>::subtree_less);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__SUBST_TMPL_H__
