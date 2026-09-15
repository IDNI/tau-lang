// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file terms.tmpl.h
 * @brief Template implementations for terms.h (package B). Included by
 * terms.h. terms.h says what each function means; the comments here say how
 * it is built.
 *
 * Conventions of the BDD library this file rides on (tau_bdd.h):
 *  - the library's universe map `U`, which links a `BDD_ID` node to its BDD,
 *    is keyed by the `BDD_ID` node under the `bf` wrapper (`key_of`). That is
 *    the same tref in every spelling of the wrapper, so a lookup steps to the
 *    child and never trims. `convert_to_tau_node` interns (tau_bdd.tmpl.h),
 *    so one BDD per type is one Tau node.
 *  - a BDD leaf's variable (`get_var`, or `get_var_term` as a `bf` term) is
 *    the TRIMMED Tau node under `bf`, a `bf_neg` node for an inverted leaf.
 *    `add(leaf)` takes the trimmed node and maps `bf_t` / `bf_f` to the
 *    terminals.
 *  - the memoised BDD walks are the library's own (`visit_nodes`,
 *    `map_leaves`, `bdd_cofactor`): recursive over BDD references with a
 *    per-call memo, the shape of its own workers (`bdd_ex`,
 *    `bdd_compose_impl`). Walks over the TAU tree go through `pre_order`
 *    (dag.h, tree.h) instead.
 *  - `build_bdd` owns the functional-quantifier SLIDE: it collects a chain
 *    whole, builds the body under the order minus the subscripts, and wraps
 *    the chain onto every leaf. Nothing here prepares a term for that, and
 *    every caller hands it the term as written.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__

// `syntactic_path_simplification` (heuristics/) and
// `syntactic_atomic_formula_simplification` (normal_forms.tmpl.h, declared
// nowhere else) are the simplification building blocks the two normalisers
// below reuse. The heuristics header must be included through
// normal_forms.h, which declares what its body uses before the body.
#include "normal_forms.h"

namespace idni::tau_lang::anti_prenexing {

namespace terms_detail {

template <NodeType node> using tbdd  = tau_term_bdd<node>;
template <NodeType node> using bref  = typename tau_term_bdd<node>::ref;
template <NodeType node> using thandle = term_handle<node>;

/**
 * @brief The MAXIMAL functional-quantifier chain hanging off the `bf` node
 * @p n: its prefix OUTERMOST FIRST, and the body it sits on. An empty prefix
 * (and @p n itself) when @p n is no chain. The whole nest is taken, kinds
 * mixed freely: it is ONE unit of work.
 */
template <NodeType node>
std::pair<typename tbdd<node>::quants, tref> strip_chain(tref n) {
	using tau = tree<node>;
	typename tbdd<node>::quants q;
	for (;;) {
		const tau& tn = tau::get(n);
		if (!tn.child_is(tau::bf_fall) && !tn.child_is(tau::bf_fex))
			break;
		const tau& c = tau::get(tn.first());
		q.emplace_back(tau::trim_right_sibling(c.first()),
			c.is(tau::bf_fall) ? tbdd<node>::all : tbdd<node>::ex);
		n = tau::trim_right_sibling(c.second());
	}
	return { std::move(q), n };
}

/// The order a chain quantifies under: its INNERMOST subscript ranks 1,
/// outward from there, over the subscripts alone. That is what `bdd_quant`
/// asserts of the reversed prefix.
template <NodeType node>
var_order<node> chain_order(const typename tbdd<node>::quants& q) {
	var_order<node> o;
	for (size_t i = 0; i < q.size(); ++i)
		o.emplace(q[i].first, int_t(q.size() - i));
	return o;
}

/// Every binder of the prefix has the same kind.
template <NodeType node>
bool one_kind(const typename tbdd<node>::quants& q) {
	for (size_t i = 1; i < q.size(); ++i)
		if (q[i].second != q[0].second) return false;
	return true;
}

/// The prefix nests along @p o: each subscript ranks strictly below the one
/// outside it. `bdd_quant`'s precondition for a MIXED prefix.
template <NodeType node>
bool nested_by_rank(const typename tbdd<node>::quants& q,
	const var_order<node>& o)
{
	for (size_t i = 1; i < q.size(); ++i)
		if (!tbdd<node>::less_then(q[i].first, q[i - 1].first, o))
			return false;
	return true;
}

/// ONE quantification of @p x by the WHOLE prefix @p q under @p o. A prefix of
/// one kind is a set quantification whose members commute, so it goes through
/// `bdd_ex` / `bdd_all`, which sort the set themselves and share the library's
/// set memo; a mixed prefix goes through `bdd_quant` and needs @p o to nest it.
template <NodeType node>
bref<node> quantify_prefix(bref<node> x, const typename tbdd<node>::quants& q,
	const var_order<node>& o)
{
	if (!one_kind<node>(q)) return tbdd<node>::bdd_quant(x, q, o);
	trefs ys;
	ys.reserve(q.size());
	for (size_t i = 0; i < q.size(); ++i) ys.push_back(q[i].first);
	return q[0].second == tbdd<node>::all
		? tbdd<node>::bdd_all(x, std::move(ys), o)
		: tbdd<node>::bdd_ex(x, ys, o);
}

/// A subscript of @p subs sits in a LEAF of @p x, where no quantification
/// reaches it: the §1 leaf hazard.
template <NodeType node>
bool hides_subscript(bref<node> x, const var_order<node>& subs) {
	for (tref v : thandle<node>::get_free_leaf_vars(x.b))
		if (subs.contains(v)) return true;
	return false;
}

/**
 * @brief Resolve ONE canonical chain: the prefix @p q, outermost first, over
 * the body @p body, whose own chains are resolved already. @p order is the
 * live order. Returns the chain itself when every subscript hides in a leaf.
 */
template <NodeType node>
tref resolve_chain(const typename tbdd<node>::quants& q, tref body,
	const var_order<node>& order)
{
	using tau = tree<node>;
	const var_order<node> o = chain_order<node>(q);
	const size_t ty = find_ba_type<node>(body);
	// THE LIVE PATH: keep mode's whole-block emission `Q_X (bf(BDD_ID))`
	// (§7 `DISCHARGE`). The subscripts are decision variables of the stored
	// BDD, so ONE quantification under the LIVE order settles the chain
	// where it lies, and the result is a BDD under that order — emitted
	// like every other term, plain when nothing branches, which is the
	// usual whole-block outcome.
	if (thandle<node>::is_bdd_backed(body)) {
		bref<node> s = thandle<node>::convert_to_handle(body).get();
		bool live = !order.empty();
		for (size_t i = 0; live && i < q.size(); ++i)
			if (!order.contains(q[i].first)) live = false;
		if (live) {
			DBG(assert(tbdd<node>::is_ordered(s, order));)
			if (!hides_subscript<node>(s, o)
				&& (one_kind<node>(q)
					|| nested_by_rank<node>(q, order)))
				return thandle<node>::convert_to_tau_node_or_term(
					thandle<node>(
						quantify_prefix<node>(s, q, order)),
					ty);
		}
	}
	// THE GENERAL PATH, over the chain's OWN order. A stored BDD that is
	// not legal under it is spelled out first, a `BDD_ID` being a node of
	// ONE order.
	bref<node> b;
	if (thandle<node>::is_bdd_backed(body)) {
		bref<node> s = thandle<node>::convert_to_handle(body).get();
		b = tbdd<node>::is_ordered(s, o) ? s
			: tbdd<node>::build_bdd(
				thandle<node>::convert_to_tau_terms(body), o);
	} else b = tbdd<node>::build_bdd(body, o);
	// The subscripts the quantification cannot reach, read off the leaves.
	trefs lfv = thandle<node>::get_free_leaf_vars(b.b);
	std::vector<bool> hidden(q.size(), false);
	bool partial = false;
	for (size_t i = 0; i < q.size(); ++i)
		if (std::binary_search(lfv.begin(), lfv.end(), q[i].first,
			tau::subtree_less))
			hidden[i] = partial = true;
	// PLAIN on this path: the chain's order is not the live one, so a
	// `BDD_ID` minted under it would be a node of the wrong order.
	if (!partial)
		return tbdd<node>::to_tau_term(
			quantify_prefix<node>(b, q, o), ty);
	// PARTIAL (§3 LEAF HAZARD): a hidden subscript stays in place and the
	// OTHER subscripts are resolved around it. From the INNERMOST subscript
	// outward: a hidden one is re-attached over the running result through
	// the constructor, and a maximal STRETCH of resolvable ones is one
	// pass. Building the stretch's BDD slides a re-attached chain onto the
	// leaves (the library's chain case), where the quantification leaves it
	// alone. A prefix that hides throughout re-attaches whole, which is the
	// canonical chain again.
	tref r = thandle<node>::is_bdd_backed(body)
		? thandle<node>::convert_to_tau_terms(body) : body;
	for (size_t e = q.size(); e-- > 0; ) {
		if (hidden[e]) {
			r = tbdd<node>::build_functional_quantifiers(
				typename tbdd<node>::quants{ q[e] }, r);
			continue;
		}
		size_t first = e;
		while (first > 0 && !hidden[first - 1]) --first;
		const typename tbdd<node>::quants st(q.begin() + first,
			q.begin() + e + 1);
		const var_order<node> so = chain_order<node>(st);
		r = tbdd<node>::to_tau_term(
			quantify_prefix<node>(tbdd<node>::build_bdd(r, so),
				st, so), ty);
		e = first;
	}
	return r;
}

/**
 * @brief The resolver's worker: @p memo is shared with every recursive call,
 * so a body — or a leaf — reached from two chains is resolved once per
 * public call.
 */
template <NodeType node>
tref resolve_chains(tref n, const var_order<node>& order,
	const keep_functional_fn<node>& keep,
	subtree_unordered_map<node, tref>& memo)
{
	using tau = tree<node>;
	n = tau::trim_right_sibling(n);
	if (auto it = memo.find(n); it != memo.end()) return it->second;
	// What this pass produced: a chain `keep` accepted, a chain the leaf
	// hazard left standing, a re-emitted BDD-backed term and a quantified
	// result. All of them are resolved already — the traversal refuses to
	// enter one, so a kept chain's inner binders are never looked at.
	subtree_unordered_set<node> done;
	auto f = [&](tref m) -> tref {
		const tau& tm = tau::get(m);
		if (!tm.is(tau::bf)) return m;
		// A BDD-backed term keeps its chains in the LEAVES, not in the
		// tree, and that is exactly SETTLE_FUNCTIONAL's input (§6:
		// `TERM_OF` of a prepared equation): resolve them there and
		// re-emit. A leaf whose chain is kept or hazardous comes back
		// as it was, and one that came to hold a decision variable is
		// re-canonicalised by `map_leaves`.
		if (thandle<node>::is_bdd_backed(m)) {
			bref<node> s = thandle<node>::convert_to_handle(m).get();
			DBG(assert(tbdd<node>::is_ordered(s, order));)
			auto in_leaf = [&](tref leaf) {
				return resolve_chains<node>(leaf, order, keep,
					memo);
			};
			bref<node> r = tbdd<node>::map_leaves(s, in_leaf, order);
			tref e = r == s ? m
				: thandle<node>::convert_to_tau_node_or_term(
					thandle<node>(r), find_ba_type<node>(m));
			done.insert(e);
			return e;
		}
		if (!tm.child_is(tau::bf_fall) && !tm.child_is(tau::bf_fex))
			return m;    // a chain may still sit deeper: descend
		// THE WHOLE NEST at once, its BODY resolved first — what comes
		// before this chain is what is NESTED inside it, never an
		// adjacent binder.
		auto [q0, b0] = strip_chain<node>(m);
		tref c = tbdd<node>::build_functional_quantifiers(q0,
			resolve_chains<node>(b0, order, keep, memo));
		// Canonical now: a degenerate or shadowed subscript is gone,
		// each run is in content order, an adjoining run of the body is
		// merged in, and a closed plain chain is already folded. `keep`
		// sees that prefix, ONCE, and its yes keeps the whole chain.
		auto [q, body] = strip_chain<node>(c);
		if (q.empty()) return done.insert(c), c;
		if (keep(q)) return done.insert(c), c;
		tref r = resolve_chain<node>(q, body, order);
		return done.insert(r), r;
	};
	auto visit = [&done](tref m) { return !done.contains(m); };
	tref r = pre_order<node>(n).apply_unique(f, visit);
	memo.emplace(n, r);
	return r;
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

// --- the representation boundary (PREPARE_TERMS) ---------------------------------

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
	// Memoised per term, within this call only.
	subtree_unordered_map<node, tref> term_memo;
	auto prep = [&](tref t) -> tref {
		t = tau::trim_right_sibling(t);
		if (thandle<node>::is_bdd_backed(t)) { // already prepared: idempotent
			DBG(assert(tbdd<node>::is_ordered(
				thandle<node>::convert_to_handle(t).get(), order));)
			return t;
		}
		if (!tbdd<node>::has_bdd_var(t, order)) return t;
		if (auto it = term_memo.find(t); it != term_memo.end())
			return it->second;
		tref r = thandle<node>::convert_to_tau_node_or_term(
			thandle<node>(tbdd<node>::build_bdd(t, order)),
			find_ba_type<node>(t));
		return term_memo.emplace(t, r).first->second;
	};
	// Equations only: an order atom is never cofactored and is consumed
	// plain by the solver path, so it is left as written.
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
	// reference and a temporal operator are opaque.
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
tref quantify_over(quantifier<node> kind, tref f, const block& X,
	const var_order<node>& order)
{
	using namespace terms_detail;
	if (!thandle<node>::is_bdd_backed(f) || X.empty()) return f;
	bref<node> r = thandle<node>::convert_to_handle(f).get();
	DBG(assert(tbdd<node>::is_ordered(r, order));)
	trefs v(X.begin(), X.end());
	bref<node> q = kind == tbdd<node>::all
		? tbdd<node>::bdd_all(r, std::move(v), order)
		: tbdd<node>::bdd_ex(r, v, order);
	return thandle<node>::convert_to_tau_node_or_term(thandle<node>(q),
		find_ba_type<node>(f));
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

template <NodeType node>
tref resolve_functional_quantifiers(tref n, const var_order<node>& order,
	const keep_functional_fn<node>& keep)
{
	// One memo for the whole call, shared with every recursive resolution:
	// a body, or a leaf, reached from two chains is resolved once.
	subtree_unordered_map<node, tref> memo;
	return terms_detail::resolve_chains<node>(n, order, keep, memo);
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
	bref<node> r = tbdd<node>::build_bdd(t, order);
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
	bref<node> a = tbdd<node>::build_bdd(l, order),
		b = tbdd<node>::build_bdd(r, order);
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
		// (variable, constant or reference) — counts 1; the wrappers are not
		// counted, and the walk does not enter a leaf's own nodes (a
		// variable's name, a reference's arguments) or a quantifier's
		// subscript. The traversal's `visit_subtree` predicate gates a node's
		// own visit, so a leaf is admitted by its parent and its children are
		// not.
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
	// The library's `get_free_leaf_vars`, by value and never stored: the
	// per-leaf sets are `get_free_vars`' own cached entries.
	return thandle<node>::get_free_leaf_vars(
		thandle<node>::convert_to_handle(f).get().b);
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__TERMS_TMPL_H__
