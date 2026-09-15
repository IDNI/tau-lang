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
 * @brief The maximal functional-quantifier chain hanging off the `bf` node
 * @p n: its prefix OUTERMOST FIRST, and the body it sits on. An empty prefix
 * (and @p n itself) when @p n is no chain. @p stop holds the chain nodes the
 * collection must not enter — the ones a resolution left standing.
 */
template <NodeType node>
std::pair<typename tbdd<node>::quants, tref> strip_chain(tref n,
	const subtree_unordered_set<node>& stop)
{
	using tau = tree<node>;
	typename tbdd<node>::quants q;
	while (!stop.contains(n)) {
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
	using tau = tree<node>;
	using namespace terms_detail;
	// The chains this call left standing — kept by `keep`, or blocked by
	// the leaf hazard. A chain above one stops its collection there, so a
	// decision taken inside holds across a change of kind, i.e. at a block
	// boundary; a SAME-kind run above it is the same block, which the
	// constructor merges and `keep` then decides once, on the whole run.
	subtree_unordered_set<node> standing;
	auto down  = [](tref m) { return m; };
	auto visit = [](tref) { return true; };
	// Post-order, so a chain nested in a body is resolved before the chain
	// above it (the spec's innermost first).
	auto up = [&](tref m) -> tref {
		const tau& tm = tau::get(m);
		if (!tm.is(tau::bf) || standing.contains(m)) return m;
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
				return resolve_functional_quantifiers<node>(
					leaf, order, keep);
			};
			bref<node> r = tbdd<node>::map_leaves(s, in_leaf, order);
			return r == s ? m
				: thandle<node>::convert_to_tau_node_or_term(
					thandle<node>(r), find_ba_type<node>(m));
		}
		if (!tm.child_is(tau::bf_fall) && !tm.child_is(tau::bf_fex))
			return m;
		// Canonicalise first: the constructor drops a degenerate or
		// shadowed subscript, merges the runs and already folds a
		// closed chain. What is left is a duplicate-free prefix whose
		// ranks below satisfy `bdd_quant`'s assertion, and `keep` sees
		// the canonical chain.
		auto [q0, b0] = strip_chain<node>(m, standing);
		tref c = tbdd<node>::build_functional_quantifiers(q0, b0);
		auto [q, body] = strip_chain<node>(c, standing);
		if (q.empty()) return c;        // folded, or nothing left to bind
		if (keep(q)) return standing.insert(c), c;
		// The chain's own order: the INNERMOST subscript lowest, over
		// the subscripts alone. A subscript is bound here, so it is
		// normally no key of the live order and the library's memos
		// survive the switch; where it is one (a settled sub-block's
		// emission, §1) the ranks disagree and the memos are dropped,
		// which is the cache sync doing its job.
		var_order<node> o;
		for (size_t i = 0; i < q.size(); ++i)
			o.emplace(q[i].first, int_t(q.size() - i));
		bool one_kind = true;
		trefs ys;
		for (const auto& [y, k] : q) {
			if (k != q[0].second) one_kind = false;
			ys.push_back(y);
		}
		// LEAF HAZARD (§1): a subscript inside a leaf is not reached by
		// any quantification, so the chain stays — the spec's partial
		// resolution, over the body its inner chains already resolved.
		auto hidden = [&o](bref<node> x) {
			for (tref v : thandle<node>::get_free_leaf_vars(x.b))
				if (o.contains(v)) return true;
			return false;
		};
		// One kind is a SET quantification (the members commute), which
		// shares the library's `ex` memo; a mixed prefix needs the
		// nesting to follow the ranks, `bdd_quant`'s own precondition.
		auto quantify = [&](bref<node> x, const var_order<node>& ord) {
			trefs v = ys;
			return !one_kind ? tbdd<node>::bdd_quant(x, q, ord)
				: q[0].second == tbdd<node>::all
					? tbdd<node>::bdd_all(x, std::move(v), ord)
					: tbdd<node>::bdd_ex(x, v, ord);
		};
		if (thandle<node>::is_bdd_backed(body)) {
			bref<node> s = thandle<node>::convert_to_handle(body).get();
			if (hidden(s)) return standing.insert(c), c;
			// The WHOLE-BLOCK emission `Q_X (bf(BDD_ID))` of keep
			// mode (§7 `DISCHARGE`): the subscripts are decision
			// variables of the stored BDD, so ONE quantification
			// under the LIVE order settles it where it lies, and
			// the result is a BDD under that order — emitted like
			// every other term, plain when nothing branches (the
			// usual whole-block outcome).
			bool live = !order.empty();
			for (tref y : ys)
				if (!order.contains(y)) { live = false; break; }
			if (live) {
				DBG(assert(tbdd<node>::is_ordered(s, order));)
				bool nested = true;
				for (size_t i = 1; i < q.size(); ++i)
					if (!tbdd<node>::less_then(q[i].first,
						q[i - 1].first, order))
					{ nested = false; break; }
				if (one_kind || nested)
					return thandle<node>::
						convert_to_tau_node_or_term(
							thandle<node>(quantify(s, order)),
							find_ba_type<node>(body));
			}
		}
		// The general path, over the chain's own order: a stored BDD
		// that is not legal under it is spelled out first (a `BDD_ID`
		// is a node of ONE order).
		bref<node> b;
		if (thandle<node>::is_bdd_backed(body)) {
			bref<node> s = thandle<node>::convert_to_handle(body).get();
			b = tbdd<node>::is_ordered(s, o) ? s
				: tbdd<node>::build_bdd(
					thandle<node>::convert_to_tau_terms(body), o);
		} else b = tbdd<node>::build_bdd(body, o);
		if (hidden(b)) return standing.insert(c), c;
		// PLAIN on this path: the chain's order is not the live one, so
		// a `BDD_ID` minted under it would be a node of the wrong order.
		return tbdd<node>::to_tau_term(quantify(b, o),
			find_ba_type<node>(body));
	};
	return pre_order<node>(n).apply_unique(down, visit, up);
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
