// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file terms.tmpl.h
 * @brief Template implementations for terms.h (package B). Included by terms.h.
 *
 * Conventions of the BDD library this file rides on (tau_bdd.h):
 *  - `U` is keyed by the `BDD_ID` node under the `bf` wrapper (`key_of`),
 *    the same tref in every spelling of the wrapper, so a lookup steps to
 *    the child and never trims; `convert_to_tau_node` interns
 *    (tau_bdd.tmpl.h), so one BDD per type is one Tau node.
 *  - a BDD leaf's variable (`get_var`, as a `bf` term `get_var_term`) is the
 *    TRIMMED Tau node under `bf` (a `bf_neg` node for an inverted leaf);
 *    `add(leaf)` takes the trimmed node and maps `bf_t`/`bf_f` to the
 *    terminals.
 *  - the memoised BDD walks are the library's (`visit_nodes`, `map_leaves`,
 *    `bdd_cofactor`): recursive over BDD refs with a per-call memo, the
 *    shape of its own workers (`bdd_ex`, `bdd_compose_impl`). Walks over the
 *    TAU tree go through `pre_order` (dag.h, tree.h).
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__

// `syntactic_path_simplification` (heuristics/) and
// `syntactic_atomic_formula_simplification` (normal_forms.tmpl.h, declared
// nowhere else) — the simplification building blocks (ruling 4). The
// heuristics header must be included through normal_forms.h, which
// declares what its body uses before the body.
#include "normal_forms.h"

namespace idni::tau_lang::anti_prenexing {

namespace terms_detail {

template <NodeType node> using tbdd  = tau_term_bdd<node>;
template <NodeType node> using bref  = typename tau_term_bdd<node>::ref;
template <NodeType node> using thandle = term_handle<node>;

/**
 * @brief §1 SLIDE of the functional quantifiers inside a plain term: every
 * `∀_y b` / `∃_y b` whose body touches `P` becomes the `BDD_ID` of
 * `y·∀_y b₁ ∪ y′·∀_y b₀`, i.e. the body's BDD over `P` with the quantifier
 * pushed onto every leaf — innermost first, since the walk is post-order
 * (`pre_order`'s `up`), and `build_bdd` absorbs a `BDD_ID` subterm through
 * `U`. A quantifier whose body is `P`-free is a leaf as it stands.
 */
template <NodeType node>
tref slide_quantifiers(tref t, const var_order<node>& order) {
	using tau = tree<node>;
	using bdd = tbdd<node>;
	auto down  = [](tref n) { return n; };
	auto visit = [](tref n) { return while_is_boolean_operation<node>(n); };
	auto up = [&order](tref n) -> tref {
		const tau& tn = tau::get(n);
		if (!tn.is(tau::bf)) return n;
		const tau& c = tn[0];
		if (!(c.is(tau::bf_fall) || c.is(tau::bf_fex))) return n;
		tref y    = tau::trim_right_sibling(c.first());
		tref body = tau::trim_right_sibling(c.second());
		if (!bdd::has_bdd_var(body, order)) return n;
		// A term-level binder never shadows a block variable: binder ids
		// are canonical after phase 0.
		DBG(assert(!order.contains(y));)
		const binder k = c.is(tau::bf_fall) ? binder::all : binder::ex;
		bref<node> b = bdd::build_bdd(body, order);
		auto fq = [&](tref leaf) {
			return functional_quantifier<node>(k, block{ y }, leaf);
		};
		return thandle<node>::convert_to_tau_node_or_term(
			thandle<node>(bdd::map_leaves(b, fq, order)),
			find_ba_type<node>(n));
	};
	return pre_order<node>(t).apply_unique(down, visit, up);
}

/// The BDD of any term under the live order: `build_bdd` resolves a
/// `bf(BDD_ID)` through `U` itself (a BDD-backed term is its own BDD, and
/// the slide is a no-op on it), and builds a plain term over `P` with the
/// quantifiers slid, absorbing `BDD_ID` subterms (a `bf_and` of two
/// cofactors, a complement) as it goes.
template <NodeType node>
bref<node> to_ref(tref t, const var_order<node>& order) {
	return tbdd<node>::build_bdd(slide_quantifiers<node>(t, order), order);
}

/**
 * @brief `f[x ← t]` on a PLAIN term: every `bf(x)` becomes `t`, and every
 * reference argument that held `x` is re-emitted through `simplify_term`
 * (a formula argument through `simplify_formula`) after its own rewrite,
 * invariant 6. NEW over `rewriter::replace` (ground rule 9) for that
 * re-simplification alone, which needs the parent of the argument node.
 * Capture-safe by the canonical binder ids of phase 0 (a term-level binder
 * inside `f` never binds `x`; Debug asserts it). One `pre_order` walk,
 * memoised per node.
 */
template <NodeType node>
tref subst_plain(tref f, tref x, tref t,
	const simplify_formula_fn& simplify_formula)
{
	using tau = tree<node>;
	const tref bx = tau::get(tau::bf, x);
	auto cb = [&](tref n, tref parent) -> tref {
		const tau& tn = tau::get(n);
		if (tn.is(tau::bf) && tau::subtree_equals(n, bx)) return t;
		DBG(if (is_functional_quantifier<node>(n))
			assert(!tau::subtree_equals(tau::trim_right_sibling(tn.first()), x));)
		if (parent && tau::get(parent).is(tau::ref_arg)
			&& (tn.is(tau::bf) || tn.is(tau::wff)))
		{
			const trefs& vars = get_free_vars<node>(n);
			if (!std::binary_search(vars.begin(), vars.end(), x, tau::subtree_less))
				return n;
			tref arg = rewriter::replace<node>(n, bx, t);
			return tn.is(tau::wff) ? simplify_formula(arg)
				: simplify_term<node>(arg);
		}
		return n;
	};
	return pre_order<node>(f).apply_unique_until_change(cb);
}

/// The atom under one optional `¬`, and whether there was one.
template <NodeType node>
std::pair<tref, bool> unwrap_neg(tref a) {
	using tau = tree<node>;
	if (tau::get(a).child_is(tau::wff_neg))
		return { tau::trim_right_sibling(tau::get(a)[0].first()), true };
	return { a, false };
}

} // namespace terms_detail

// --- the representation boundary (PREPARE_TERMS, D2) -----------------------------

template <NodeType node>
tref prepare_terms(tref body, [[maybe_unused]] const block& P,
	const var_order<node>& order)
{
	using tau = tree<node>;
	using namespace terms_detail;
#ifdef DEBUG
	assert(P.size() == order.size());
	for (tref p : P) assert(order.contains(p));
#endif
	// Per term, within this call (ground rule 2: a per-pass memo).
	subtree_unordered_map<node, tref> term_memo;
	auto prep = [&](tref t) -> tref {
		t = tau::trim_right_sibling(t);
		if (thandle<node>::is_bdd_backed(t)) { // a prepared input (D2 idempotence)
			DBG(assert(tbdd<node>::is_ordered(
				thandle<node>::convert_to_handle(t).get(), order));)
			return t;
		}
		if (!tbdd<node>::has_bdd_var(t, order)) return t;
		if (auto it = term_memo.find(t); it != term_memo.end())
			return it->second;
		tref r = thandle<node>::convert_to_tau_node_or_term(
			thandle<node>(to_ref<node>(t, order)), find_ba_type<node>(t));
		return term_memo.emplace(t, r).first->second;
	};
	// Equations only (ruling 2): an order atom is never cofactored and is
	// consumed plain by the solver path, so it is left as written.
	auto f = [&](tref n) -> tref {
		const tau& tn = tau::get(n);
		if (!tn.is(tau::wff) || !tn.child_is(tau::bf_eq)) return n;
		tref l = tau::trim_right_sibling(tn[0].first());
		tref r = tau::trim_right_sibling(tn[0].second());
		tref l2 = prep(l), r2 = prep(r);
		if (l2 == l && r2 == r) return n;
		return build_bf_eq<node>(l2, r2);
	};
	// Descend through connectives and negation only: a binder, a
	// reference and a temporal operator are opaque (ruling 2).
	auto visit = [](tref n) {
		const tau& tn = tau::get(n);
		return tn.is(tau::wff) || tn.is(tau::wff_and) || tn.is(tau::wff_or)
			|| tn.is(tau::wff_neg);
	};
	return pre_order<node>(body).apply_unique(f, visit);
}

// --- cofactors and quantification -----------------------------------------------

template <NodeType node>
tref cofactor(tref f, tref x, bool bit, const var_order<node>& order) {
	using tau = tree<node>;
	using namespace terms_detail;
	if (!thandle<node>::is_bdd_backed(f)) return f; // no decision variables
	x = tau::trim_right_sibling(x);
	DBG(assert(tau::get(x).is(tau::variable));)
	if (!order.contains(x)) return f;      // not a decision variable
	bref<node> r = thandle<node>::convert_to_handle(f).get();
	DBG(assert(tbdd<node>::is_ordered(r, order));)
	return thandle<node>::convert_to_tau_node_or_term(
		thandle<node>(tbdd<node>::bdd_cofactor(r, x, bit, order)),
		find_ba_type<node>(f));
}

template <NodeType node>
tref quantify_over(binder kind, tref f, const block& X,
	const var_order<node>& order)
{
	using namespace terms_detail;
	if (!thandle<node>::is_bdd_backed(f) || X.empty()) return f;
	bref<node> r = thandle<node>::convert_to_handle(f).get();
	DBG(assert(tbdd<node>::is_ordered(r, order));)
	trefs v(X.begin(), X.end());
	bref<node> q = kind == binder::all
		? tbdd<node>::bdd_all(r, std::move(v), order)
		: tbdd<node>::bdd_ex(r, v, order);
	return thandle<node>::convert_to_tau_node_or_term(thandle<node>(q),
		find_ba_type<node>(f));
}

template <NodeType node>
tref functional_quantifier(binder kind, const block& Y, tref f) {
	using tau = tree<node>;
	f = tau::trim_right_sibling(f);
	if (tau::get(f).equals_0() || tau::get(f).equals_1()) return f;
	// Only the variables actually free in `f` (a binder over an absent
	// variable is degenerate), in content order, deduplicated.
	const trefs& vars = get_free_vars<node>(f);
	trefs ys;
	for (tref y : Y) {
		y = tau::trim_right_sibling(y);
		if (std::binary_search(vars.begin(), vars.end(), y, tau::subtree_less))
			ys.push_back(y);
	}
	if (ys.empty()) return f;
	std::sort(ys.begin(), ys.end(), tau::subtree_less);
	ys.erase(std::unique(ys.begin(), ys.end(),
		[](tref a, tref b) { return tau::subtree_equals(a, b); }), ys.end());
	// Outermost first in `ys`, so the chain is folded from the back. The
	// builders rename the bound variable by default; the module never does
	// (ground rule 4), so every call passes `false`.
	tref r = f;
	for (auto it = ys.rbegin(); it != ys.rend(); ++it)
		r = kind == binder::all ? build_bf_fall<node>(*it, r, false)
			: build_bf_fex<node>(*it, r, false);
	return r;
}

template <NodeType node>
bool carries_functional_quantifier(tref f) {
	using tau = tree<node>;
	using namespace terms_detail;
	auto has_fq = [](tref t) {
		return tau::get(t).find_top(
			(bool(*)(tref)) is_functional_quantifier<node>) != nullptr;
	};
	if (!thandle<node>::is_bdd_backed(f)) return has_fq(f);
	auto at_node = [&](bref<node> x, bool leaf) {
		return !(leaf && has_fq(tbdd<node>::get_var_term(x)));
	};
	return !tbdd<node>::visit_nodes(
		thandle<node>::convert_to_handle(f).get(), at_node);
}

// --- substitution inside a term -----------------------------------------------

template <NodeType node>
tref subst_term(tref f, tref x, tref t, const var_order<node>& order,
	const simplify_formula_fn& simplify_formula)
{
	using tau = tree<node>;
	using namespace terms_detail;
	x = tau::trim_right_sibling(x);
	DBG(assert(tau::get(x).is(tau::variable));)
	{
		const trefs& vars = get_free_vars<node>(f);
		if (!std::binary_search(vars.begin(), vars.end(), x, tau::subtree_less))
			return f;
	}
	// Ruling 2: no BDD_ID ever enters a leaf, so the term that goes into a
	// leaf or a reference argument is plain.
	const tref t_plain = thandle<node>::is_bdd_backed(t)
		? thandle<node>::convert_to_tau_terms(t) : t;
	if (!thandle<node>::is_bdd_backed(f))
		return subst_plain<node>(f, x, t_plain, simplify_formula);
	bref<node> r = thandle<node>::convert_to_handle(f).get();
	DBG(assert(tbdd<node>::is_ordered(r, order));)
	// 1. the hidden occurrences, inside the leaves — on the ORIGINAL
	//    leaves, so an `x` inside `t` is not substituted again;
	auto leaf_subst = [&](tref leaf) -> tref {
		const trefs& vars = get_free_vars<node>(leaf);
		if (!std::binary_search(vars.begin(), vars.end(), x, tau::subtree_less))
			return leaf;
		return subst_plain<node>(leaf, x, t_plain, simplify_formula);
	};
	r = tbdd<node>::map_leaves(r, leaf_subst, order);
	// 2. the decision variable, by BDD compose.
	if (order.contains(x))
		r = tbdd<node>::bdd_compose(r, x, to_ref<node>(t, order), order);
	return thandle<node>::convert_to_tau_node_or_term(thandle<node>(r),
		find_ba_type<node>(f));
}

// --- the two aggressive normalisers of invariant 6 --------------------------------

template <NodeType node>
tref simplify_term(tref t, const var_order<node>& order) {
	using tau = tree<node>;
	using namespace terms_detail;
	t = tau::trim_right_sibling(t);
	if (tau::get(t).equals_0() || tau::get(t).equals_1()) return t;
	// Plain regime: the empty order (phases 1, 2, 5) or a term that does
	// not touch `P`.
	if (!thandle<node>::is_bdd_backed(t) && !tbdd<node>::has_bdd_var(t, order))
		return syntactic_path_simplification<node>(t);
	DBG(assert(!order.empty());)
	// BDD regime: the representation re-established (a BDD-backed term is
	// canonical already; a plain combination of BDD-backed subterms is
	// built over `P`), then every leaf through the path simplifier and
	// the rebuild that merges leaves that became equal.
	bref<node> r = to_ref<node>(t, order);
	DBG(assert(tbdd<node>::is_ordered(r, order));)
	auto sps = [](tref leaf) { return syntactic_path_simplification<node>(leaf); };
	return thandle<node>::convert_to_tau_node_or_term(
		thandle<node>(tbdd<node>::map_leaves(r, sps, order)),
		find_ba_type<node>(t));
}

template <NodeType node>
tref simplify_atom(tref a, const var_order<node>& order) {
	using tau = tree<node>;
	using namespace terms_detail;
	auto [atom, negated] = unwrap_neg<node>(a);
	const tau& t = tau::get(atom);
	if (t.equals_T() || t.equals_F())
		return negated ? build_wff_neg<node>(atom) : a;
	DBG(assert(is_atomic_fm<node>(atom));)
	tref l = tau::trim_right_sibling(t[0].first());
	tref r = tau::trim_right_sibling(t[0].second());
	tref res;
	if (thandle<node>::is_bdd_backed(l) || thandle<node>::is_bdd_backed(r)) {
		// BDD regime: side-wise, the atom never reshaped; a constant-only
		// atom folds through the construction hooks.
		tref l2 = simplify_term<node>(l, order);
		tref r2 = simplify_term<node>(r, order);
		res = (l2 == l && r2 == r) ? atom
			: tau::get(tau::wff, tau::get(t[0].value.nt, l2, r2));
	} else {
		// Plain regime: the full existing atom simplifier — the
		// norm/denorm round trip on the joint `l + r` and the per-variable
		// pass — idempotent, so "as written" is "as simplified".
		res = syntactic_atomic_formula_simplification<node>(atom);
	}
	return negated ? build_wff_neg<node>(res) : res;
}

// --- reading and rewriting equations ---------------------------------------------

template <NodeType node>
tref term_of(tref atom, const var_order<node>& order) {
	using namespace terms_detail;
	using tau = tree<node>;
	using bdd = tbdd<node>;
	auto [eq, _] = unwrap_neg<node>(atom);
	const tau& t = tau::get(eq);
	if (!t.is(tau::wff) || !t.child_is(tau::bf_eq)) {
		DBG(assert(false && "term_of: not an equation");)
		return nullptr;
	}
	tref l = tau::trim_right_sibling(t[0].first());
	tref r = tau::trim_right_sibling(t[0].second());
	if (!thandle<node>::is_bdd_backed(l) && !thandle<node>::is_bdd_backed(r))
		return build_bf_xor<node>(l, r);
	// The ring sum of two BDD-backed sides is a BDD operation; a plain side
	// (a constant, a P-free term) is a leaf under the same order.
	bref<node> a = to_ref<node>(l, order), b = to_ref<node>(r, order);
	DBG(assert(bdd::is_ordered(a, order) && bdd::is_ordered(b, order));)
	return thandle<node>::convert_to_tau_node_or_term(
		thandle<node>(bdd::bdd_xor(a, b, order)),
		find_ba_type<node>(thandle<node>::is_bdd_backed(l) ? l : r));
}

template <NodeType node>
tref norm_equation(tref atom, const var_order<node>& order) {
	using tau = tree<node>;
	using namespace terms_detail;
	auto [eq, negated] = unwrap_neg<node>(atom);
	if (!tau::get(eq).child_is(tau::bf_eq)) return atom;
	tref res = build_bf_eq_0<node>(term_of<node>(eq, order));
	return negated ? build_wff_neg<node>(res) : res;
}

// --- in-memory size and the leaf hazard -------------------------------------------

template <NodeType node>
size_t mem_size(tref t) {
	using namespace terms_detail;
	if (!thandle<node>::is_bdd_backed(t)) {
		// Term-structure nodes only, each distinct one once: every child of
		// a `bf` wrapper — an operator, a functional quantifier, a leaf
		// (variable, constant, reference) — counts 1; the wrappers are not
		// counted, and the walk does not enter a leaf's own nodes (a
		// variable's name, a reference's arguments) or a quantifier's
		// subscript. `visit_subtree` gates a node's own visit, so a leaf is
		// admitted by its parent and its children are not.
		using tau = tree<node>;
		size_t n = 0;
		auto count = [&n](tref m) {
			if (!tau::get(m).is(tau::bf)) ++n;
			return true;
		};
		// (the traversal calls the predicate on the root without a parent)
		auto admit = [](tref m, tref parent = nullptr) {
			return while_is_boolean_operation<node>(m)
				|| (parent && tau::get(parent).is(tau::bf));
		};
		auto up = [](tref) {};
		pre_order<node>(t).visit_unique(count, admit, up);
		return n;
	}
	return tbdd<node>::node_count(thandle<node>::convert_to_handle(t).get());
}

template <NodeType node>
trefs leaf_fv(tref f) {
	using namespace terms_detail;
	using tau = tree<node>;
	f = tau::trim_right_sibling(f);
	if (!thandle<node>::is_bdd_backed(f)) return get_free_vars<node>(f);
	// The library's `get_free_leaf_vars`: by value, not stored (Lucca,
	// Sep 10: no `leaf_fv_memo`).
	return thandle<node>::get_free_leaf_vars(
		thandle<node>::convert_to_handle(f).get().b);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__
