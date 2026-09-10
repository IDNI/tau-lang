// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file terms.tmpl.h
 * @brief Template implementations for terms.h (package B). Included by terms.h.
 *
 * Conventions of the BDD library this file rides on (tau_bdd.h):
 *  - `U` is keyed by the `bf(BDD_ID)` node WITHOUT a right sibling, so every
 *    lookup trims first; `convert_to_tau_node` interns (tau_bdd.tmpl.h), so
 *    one BDD per type is one Tau node.
 *  - a BDD leaf's variable (`get_var`) is the TRIMMED Tau node under `bf`
 *    (a `bf_neg` node for an inverted leaf); `add(leaf)` takes the trimmed
 *    node and maps `bf_t`/`bf_f` to the terminals.
 *  - the memoised BDD walks below (restrict, map_leaves, the counts, the
 *    Debug order check) are recursive over BDD refs with a per-call memo,
 *    the shape of the library's own workers (`bdd_ex`, `bdd_compose_impl`);
 *    walks over the TAU tree go through `pre_order` (dag.h, tree.h).
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__

#include <unordered_map>
#include <unordered_set>

// `syntactic_path_simplification` (heuristics/) and
// `syntactic_atomic_formula_simplification` (normal_forms.tmpl.h, declared
// nowhere else) — the simplification building blocks (ruling 4). The
// heuristics header must be included through normal_forms.h, which
// declares what its body uses before the body.
#include "normal_forms.h"
#include "dag.h"

namespace idni::tau_lang::anti_prenexing {

namespace terms_detail {

template <NodeType node> using tbdd  = tau_term_bdd<node>;
template <NodeType node> using bref  = typename tau_term_bdd<node>::ref;
template <NodeType node> using thandle = term_handle<node>;

/// `FV(t) ∩ keys(order) ≠ ∅` — the "touches P" test of §1, one cached FV read.
template <NodeType node>
bool touches(tref t, const var_order<node>& order) {
	if (order.empty()) return false;
	for (tref v : fv<node>(t)) if (order.contains(v)) return true;
	return false;
}

/// The handle behind a BDD-backed term (`U`, keys without right sibling).
template <NodeType node>
thandle<node> handle_of(tref term) {
	using tau = tree<node>;
	const auto& U = thandle<node>::U;
	auto it = U.find(tau::trim_right_sibling(term));
	DBG(assert(it != U.end());)
	return it->second;
}

/**
 * @brief The Tau term of a BDD ref: a terminal is the typed constant, a
 * single leaf is its own (plain) term, anything with a decision node is
 * the interned `BDD_ID` node. So a term is BDD-backed exactly when it has a
 * decision variable, i.e. touches `P` (§1); a term that stopped touching
 * `P` (a cofactor, a quantification over all of `P`) comes back plain.
 */
template <NodeType node>
tref intern(bref<node> r, size_t type) {
	using tau = tree<node>;
	using bdd = tbdd<node>;
	if (r == bdd::T) return tau::_1(type);
	if (r == bdd::F) return tau::_0(type);
	if (bdd::leaf(r)) return bdd::to_tau_term(r, type);
	return thandle<node>::convert_to_tau_node(thandle<node>(r), type);
}

#ifdef DEBUG
/**
 * @brief Ruling 7: the ref is a reduced ordered BDD under `order` — every
 * decision variable is a key of `order` and ranks strictly increase down
 * every path. Catches a mixed-order operand, a ref of a previous
 * component's order, and a term never prepared, at the first primitive
 * that touches it. Linear in the BDD once per call; Debug only.
 */
template <NodeType node>
bool is_ordered(bref<node> x, const var_order<node>& order,
	std::unordered_set<tref>& seen)
{
	using bdd = tbdd<node>;
	if (bdd::leaf(x)) return true;
	if (!seen.insert(x.b).second) return true;
	tref v = bdd::get_var(x);
	if (!order.contains(v)) return false;
	for (bref<node> c : { bdd::get_high(x), bdd::get_low(x) }) {
		if (!bdd::leaf(c) && !bdd::less_then(v, bdd::get_var(c), order))
			return false;
		if (!is_ordered<node>(c, order, seen)) return false;
	}
	return true;
}

template <NodeType node>
bool is_ordered(bref<node> x, const var_order<node>& order) {
	std::unordered_set<tref> seen;
	return is_ordered<node>(x, order, seen);
}
#endif // DEBUG

/// The Tau term of a leaf, as a `bf` (the inverter folded in as `bf_neg`).
template <NodeType node>
tref leaf_term(bref<node> x) {
	using tau = tree<node>;
	return tau::get(tau::bf, tbdd<node>::get_var(x));
}

/**
 * @brief The shared leaf walk: rebuild `x` with every non-terminal leaf `ℓ`
 * replaced by `fn(ℓ)` (both as `bf` terms), the decision structure kept.
 * NEW (ground rule 9): the library has no leaf-mapping operation — its
 * `bdd_compose` and `substitute` touch decision variables only, which is
 * the leaf hazard's forbidden version. Serves the slide, `simplify_term`
 * and `subst_term`.
 * A changed leaf that is still `P`-free is re-interned through `add`, so
 * leaves that became equal are one node and `h == l` folds; a changed leaf
 * that came to touch `P` (a substitution put block variables into it) is
 * built as a BDD. A node is rebuilt through `add` while both children stay
 * ordered below it, and through `bdd_ite` once a rebuilt leaf brought a
 * variable of equal or higher rank up, which re-canonicalises. Memoised
 * per call over the BDD's nodes.
 */
template <NodeType node, typename F>
bref<node> map_leaves(bref<node> x, F& fn, const var_order<node>& order,
	std::unordered_map<bref<node>, bref<node>>& memo)
{
	using tau = tree<node>;
	using bdd = tbdd<node>;
	if (x == bdd::T || x == bdd::F) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	bref<node> r;
	if (bdd::leaf(x)) {
		tref in  = leaf_term<node>(x);
		tref out = fn(in);
		if (out == in) r = x;
		else if (touches<node>(out, order)) r = bdd::build_bdd(out, order);
		else r = bdd::add(tau::trim(out));
	} else {
		bref<node> h = map_leaves<node>(bdd::get_high(x), fn, order, memo);
		bref<node> l = map_leaves<node>(bdd::get_low(x),  fn, order, memo);
		tref var = bdd::get_var(x);
		auto below = [&](bref<node> c) {
			return bdd::leaf(c) || bdd::less_then(var, bdd::get_var(c), order);
		};
		if (h == bdd::get_high(x) && l == bdd::get_low(x)) r = x;
		else if (below(h) && below(l)) r = bdd::add(var, h, l);
		else r = bdd::bdd_ite(bdd::from_bit(var), h, l, order);
	}
	return memo.emplace(x, r).first->second;
}

template <NodeType node, typename F>
bref<node> map_leaves(bref<node> x, F& fn, const var_order<node>& order) {
	std::unordered_map<bref<node>, bref<node>> memo;
	return map_leaves<node>(x, fn, order, memo);
}

/**
 * @brief Visit every non-terminal node of a BDD once (by node, whatever the
 * inverter it is reached with): `fn(x, is_leaf)` returns false to stop the
 * walk. The one walk behind `carries_functional_quantifier`, `mem_size`
 * and `leaf_fv`. Returns false iff `fn` stopped it.
 */
template <NodeType node, typename F>
bool visit_nodes(bref<node> x, F& fn, std::unordered_set<tref>& seen) {
	using bdd = tbdd<node>;
	if (x == bdd::T || x == bdd::F) return true;
	if (!seen.insert(x.b).second) return true;
	if (bdd::leaf(x)) return fn(x, true);
	return fn(x, false)
		&& visit_nodes<node>(bdd::get_high(x), fn, seen)
		&& visit_nodes<node>(bdd::get_low(x), fn, seen);
}

template <NodeType node, typename F>
bool visit_nodes(bref<node> x, F& fn) {
	std::unordered_set<tref> seen;
	return visit_nodes<node>(x, fn, seen);
}


/**
 * @brief §1 cofactor by child selection at any depth, as a restrict
 * traversal: a leaf is itself; a node above `xi` in the order is itself
 * (an ordered BDD holds `xi` at most once per path, never above); the node
 * of `xi` is its child by `bit`; any other node is rebuilt through `add`,
 * which keeps canonicity because both children stay ordered below it.
 * NEW rather than `bdd_compose(x, xi, T/F)` (ground rule 9): compose routes
 * every node through `bdd_ite`; the tests keep compose as the oracle.
 */
template <NodeType node>
bref<node> restrict_bit(bref<node> x, tref xi, bool bit,
	const var_order<node>& order,
	std::unordered_map<bref<node>, bref<node>>& memo)
{
	using tau = tree<node>;
	using bdd = tbdd<node>;
	if (bdd::leaf(x)) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	tref var = bdd::get_var(x);
	if (bdd::less_then(xi, var, order)) return x;
	bref<node> r;
	if (tau::subtree_equals(xi, var))
		r = bit ? bdd::get_high(x) : bdd::get_low(x);
	else r = bdd::add(var,
		restrict_bit<node>(bdd::get_high(x), xi, bit, order, memo),
		restrict_bit<node>(bdd::get_low(x),  xi, bit, order, memo));
	return memo.emplace(x, r).first->second;
}

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
		if (!touches<node>(body, order)) return n;
		// A term-level binder never shadows a block variable: binder ids
		// are canonical after phase 0.
		DBG(assert(!order.contains(y));)
		const binder k = c.is(tau::bf_fall) ? binder::all : binder::ex;
		bref<node> b = bdd::build_bdd(body, order);
		auto fq = [&](tref leaf) {
			return functional_quantifier<node>(k, block{ y }, leaf);
		};
		return intern<node>(map_leaves<node>(b, fq, order),
			find_ba_type<node>(n));
	};
	return pre_order<node>(t).apply_unique(down, visit, up);
}

/// The BDD of any term under the live order: a BDD-backed term's own
/// BDD; a plain term built over `P`, quantifiers slid, `BDD_ID` subterms
/// (a `bf_and` of two cofactors, a complement) absorbed by `build_bdd`.
template <NodeType node>
bref<node> to_ref(tref t, const var_order<node>& order) {
	if (is_bdd_backed<node>(t)) return handle_of<node>(t).get();
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
			const trefs& vars = fv<node>(n);
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

/// The operator node of an atom: an equation, or an order atom in any of
/// the shapes the grammar admits before phase 3.
template <NodeType node>
bool is_atom_node(const tree<node>& t) {
	using tau = tree<node>;
	switch (t.value.nt) {
		case tau::bf_eq: case tau::bf_neq:
		case tau::bf_lt: case tau::bf_nlt: case tau::bf_lteq:
		case tau::bf_nlteq: case tau::bf_gt: case tau::bf_ngt:
		case tau::bf_gteq: case tau::bf_ngteq: return true;
		default: return false;
	}
}

} // namespace terms_detail

// --- the representation boundary (PREPARE_TERMS, D2) -----------------------------

template <NodeType node>
bool is_bdd_backed(tref term) {
	using tau = tree<node>;
	return term && tau::get(term).is(tau::bf)
		&& tau::get(term).child_is(tau::BDD_ID);
}

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
		if (is_bdd_backed<node>(t)) { // a prepared input (D2 idempotence)
			DBG(assert(is_ordered<node>(handle_of<node>(t).get(), order));)
			return t;
		}
		if (!touches<node>(t, order)) return t;
		if (auto it = term_memo.find(t); it != term_memo.end())
			return it->second;
		tref r = intern<node>(to_ref<node>(t, order), find_ba_type<node>(t));
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

template <NodeType node>
tref finish_terms(tref phi) {
	using namespace terms_detail;
	auto f = [](tref n) -> tref {
		if (!is_bdd_backed<node>(n)) return n;
		tref plain = handle_of<node>(n).to_tau_term(find_ba_type<node>(n));
		// No BDD_ID nests inside a leaf (subst_term, the slide), so the
		// converted term needs no second look.
		DBG(assert(tree<node>::get(plain).find_top([](tref m) {
			return tree<node>::get(m).is(tree<node>::BDD_ID); }) == nullptr);)
		return plain;
	};
	// Every node kind is entered (binders, functional-quantifier bodies,
	// reference arguments, temporal scopes); a replaced term is not
	// re-entered.
	return pre_order<node>(phi).apply_unique_until_change(f);
}

// --- cofactors and quantification -----------------------------------------------

template <NodeType node>
tref cofactor(tref f, tref x, bool bit, const var_order<node>& order) {
	using tau = tree<node>;
	using namespace terms_detail;
	if (!is_bdd_backed<node>(f)) return f; // no decision variables at all
	x = tau::trim_right_sibling(x);
	DBG(assert(tau::get(x).is(tau::variable));)
	if (!order.contains(x)) return f;      // not a decision variable
	bref<node> r = handle_of<node>(f).get();
	DBG(assert(is_ordered<node>(r, order));)
	std::unordered_map<bref<node>, bref<node>> memo;
	return intern<node>(restrict_bit<node>(r, x, bit, order, memo),
		find_ba_type<node>(f));
}

template <NodeType node>
tref quantify_over(binder kind, tref f, const block& X,
	const var_order<node>& order)
{
	using namespace terms_detail;
	if (!is_bdd_backed<node>(f) || X.empty()) return f;
	bref<node> r = handle_of<node>(f).get();
	DBG(assert(is_ordered<node>(r, order));)
	trefs v(X.begin(), X.end());
	bref<node> q = kind == binder::all
		? tbdd<node>::bdd_all(r, std::move(v), order)
		: tbdd<node>::bdd_ex(r, v, order);
	return intern<node>(q, find_ba_type<node>(f));
}

template <NodeType node>
tref forall_over(tref f, const block& X, const var_order<node>& order) {
	return quantify_over<node>(binder::all, f, X, order);
}

template <NodeType node>
tref exists_over(tref f, const block& X, const var_order<node>& order) {
	return quantify_over<node>(binder::ex, f, X, order);
}

template <NodeType node>
tref functional_quantifier(binder kind, const block& Y, tref f) {
	using tau = tree<node>;
	f = tau::trim_right_sibling(f);
	if (tau::get(f).equals_0() || tau::get(f).equals_1()) return f;
	// Only the variables actually free in `f` (a binder over an absent
	// variable is degenerate), in content order, deduplicated.
	const trefs& vars = fv<node>(f);
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
	if (!is_bdd_backed<node>(f)) return has_fq(f);
	auto at_node = [&](bref<node> x, bool leaf) {
		return !(leaf && has_fq(leaf_term<node>(x)));
	};
	return !visit_nodes<node>(handle_of<node>(f).get(), at_node);
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
		const trefs& vars = fv<node>(f);
		if (!std::binary_search(vars.begin(), vars.end(), x, tau::subtree_less))
			return f;
	}
	// No BDD_ID ever enters a leaf (ruling 2): the term that goes into a
	// reference argument is plain.
	const tref t_plain = is_bdd_backed<node>(t) ? finish_terms<node>(t) : t;
	if (!is_bdd_backed<node>(f))
		return subst_plain<node>(f, x, t_plain, simplify_formula);
	bref<node> r = handle_of<node>(f).get();
	DBG(assert(is_ordered<node>(r, order));)
	// 1. the hidden occurrences, inside the leaves — on the ORIGINAL
	//    leaves, so an `x` inside `t` is not substituted again;
	auto leaf_subst = [&](tref leaf) -> tref {
		const trefs& vars = fv<node>(leaf);
		if (!std::binary_search(vars.begin(), vars.end(), x, tau::subtree_less))
			return leaf;
		return subst_plain<node>(leaf, x, t_plain, simplify_formula);
	};
	r = map_leaves<node>(r, leaf_subst, order);
	// 2. the decision variable, by BDD compose.
	if (order.contains(x))
		r = tbdd<node>::bdd_compose(r, x, to_ref<node>(t, order), order);
	return intern<node>(r, find_ba_type<node>(f));
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
	if (!is_bdd_backed<node>(t) && !touches<node>(t, order))
		return syntactic_path_simplification<node>(t);
	DBG(assert(!order.empty());)
	// BDD regime: the representation re-established (a BDD-backed term is
	// canonical already; a plain combination of BDD-backed subterms is
	// built over `P`), then every leaf through the path simplifier and
	// the rebuild that merges leaves that became equal.
	bref<node> r = to_ref<node>(t, order);
	DBG(assert(is_ordered<node>(r, order));)
	auto sps = [](tref leaf) { return syntactic_path_simplification<node>(leaf); };
	return intern<node>(map_leaves<node>(r, sps, order), find_ba_type<node>(t));
}

template <NodeType node>
tref simplify_atom(tref a, const var_order<node>& order) {
	using tau = tree<node>;
	using namespace terms_detail;
	auto [atom, negated] = unwrap_neg<node>(a);
	const tau& t = tau::get(atom);
	if (t.equals_T() || t.equals_F())
		return negated ? build_wff_neg<node>(atom) : a;
	DBG(assert(t.is(tau::wff) && is_atom_node<node>(t[0]));)
	tref l = tau::trim_right_sibling(t[0].first());
	tref r = tau::trim_right_sibling(t[0].second());
	tref res;
	if (is_bdd_backed<node>(l) || is_bdd_backed<node>(r)) {
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
	if (!is_bdd_backed<node>(l) && !is_bdd_backed<node>(r))
		return build_bf_xor<node>(l, r);
	// The ring sum of two BDD-backed sides is a BDD operation; a plain side
	// (a constant, a P-free term) is a leaf under the same order.
	bref<node> a = to_ref<node>(l, order), b = to_ref<node>(r, order);
	DBG(assert(is_ordered<node>(a, order) && is_ordered<node>(b, order));)
	bref<node> x = bdd::bdd_or(bdd::bdd_and(a, bdd::bdd_not(b), order),
		bdd::bdd_and(bdd::bdd_not(a), b, order), order);
	return intern<node>(x,
		find_ba_type<node>(is_bdd_backed<node>(l) ? l : r));
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
	if (!is_bdd_backed<node>(t)) {
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
	// The visited-set scheme of heuristics/bv_predicate_blasting.tmpl.h's
	// bdd_node_count, plus the leaf guard it does not need (its BDDs have
	// only T/F terminals; ours have Tau-term leaves without children).
	size_t n = 0;
	auto count = [&n](bref<node>, bool) { return ++n, true; };
	visit_nodes<node>(handle_of<node>(t).get(), count);
	return n;
}

template <NodeType node>
trefs leaf_fv(tref f) {
	using namespace terms_detail;
	using tau = tree<node>;
	f = tau::trim_right_sibling(f);
	if (!is_bdd_backed<node>(f)) return fv<node>(f);
	// NEW over `get_free_tau_vars` (ground rule 9): that worker merges the
	// FV of EVERY node's variable, decision variables included, which is
	// `fv(term)`; the leaf hazard wants the leaves alone. Not stored: the
	// per-leaf sets are `get_free_vars`' cached entries (each leaf is the
	// root of its own query), and their union is one walk of the BDD's
	// distinct nodes per call (Lucca, Sep 10: no `leaf_fv_memo`).
	subtree_set<node> merged;
	auto collect = [&](bref<node> x, bool leaf) {
		if (leaf) {
			const trefs& vars = fv<node>(leaf_term<node>(x));
			merged.insert(vars.begin(), vars.end());
		}
		return true;
	};
	visit_nodes<node>(handle_of<node>(f).get(), collect);
	return trefs(merged.begin(), merged.end());
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__
