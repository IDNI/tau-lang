// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef TAU_TAU_BDD_TMPL_H
#define TAU_TAU_BDD_TMPL_H

#include <algorithm>

#include "tau_bdd.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "tau_bdd"

namespace idni::tau_lang {

/** @internal @copydoc tau_bdd_node::operator==(const tau_bdd_node&) const @endinternal */
template <NodeType node>
bool tau_bdd_node<node>::operator==(const tau_bdd_node<node>& other) const {
	using tau = tree<node>;
	return this->inv_v == other.inv_v && this->inv_h == other.inv_h &&
		this->inv_l == other.inv_l && tau::subtree_equals(this->v, other.v);
}

/** @internal @copydoc tau_bdd_node::operator!=(const tau_bdd_node&) const @endinternal */
template<NodeType node>
bool tau_bdd_node<node>::operator!=(const tau_bdd_node& other) const {
	return !(*this == other);
}

/** @internal @copydoc tau_bdd_node::operator<(const tau_bdd_node&) const @endinternal */
template<NodeType node>
bool tau_bdd_node<node>::operator<(const tau_bdd_node& other) const {
	using tau = tree<node>;
	if (tau::get(this->v) != tau::get(other.v))
		return tau::subtree_less(this->v, other.v);
	if (this->inv_v != other.inv_v) return this->inv_v < other.inv_v;
	if (this->inv_h != other.inv_h) return this->inv_h < other.inv_h;
	if (this->inv_l != other.inv_l) return this->inv_l < other.inv_l;
	return false;
}

/** @internal @copydoc tau_bdd_ref::operator==(const tau_bdd_ref&) const @endinternal */
template<NodeType node>
bool tau_bdd_ref<node>::operator==(const tau_bdd_ref& other) const {
	using bdd = bintree<tau_bdd_node<node>>;
	const auto& b_this = bdd::get(this->b);
	const auto& b_other = bdd::get(other.b);
	return this->inv == other.inv && b_this.l == b_other.l &&
		b_this.r == b_other.r && b_this.value == b_other.value;
}

/** @internal @copydoc tau_bdd_ref::operator!=(const tau_bdd_ref&) const @endinternal */
template<NodeType node>
bool tau_bdd_ref<node>::operator!=(const tau_bdd_ref& other) const {
	return !(*this == other);
}

/** @internal @copydoc tau_bdd_ref::operator<(const tau_bdd_ref&) const @endinternal */
template<NodeType node>
bool tau_bdd_ref<node>::operator<(const tau_bdd_ref& other) const {
	using bdd = bintree<tau_bdd_node<node>>;
	if (bdd::get(this->b) == bdd::get(other.b)) return this->inv < other.inv;
	return bdd::get(this->b) < bdd::get(other.b);
}

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::T = tau_term_bdd::ref(
	tau_term_bdd::get(bdd_node(tree<node>::get(tree<node>::bf_t)), nullptr, nullptr), false);

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::F = tau_term_bdd::ref(
	tau_term_bdd::get(bdd_node(tree<node>::get(tree<node>::bf_t)), nullptr, nullptr), true);

#ifdef TAU_CACHE
template<NodeType node>
tau_term_bdd<node>::cache_and_t tau_term_bdd<node>::and_memo;

template<NodeType node>
tau_term_bdd<node>::cache_ex_t tau_term_bdd<node>::ex_memo;

template<NodeType node>
tau_term_bdd<node>::cache_and_many_t tau_term_bdd<node>::and_many_memo;

template<NodeType node>
tau_term_bdd<node>::cache_quant_t tau_term_bdd<node>::quant_memo;

template<NodeType node>
tau_term_bdd<node>::cache_ite_t tau_term_bdd<node>::ite_memo;

template<NodeType node>
tau_term_bdd<node>::order tau_term_bdd<node>::last_order;

template<NodeType node>
bool tau_term_bdd<node>::has_last_order = false;

/** @internal @copydoc tau_term_bdd::clear_caches() @endinternal */
template<NodeType node>
void tau_term_bdd<node>::clear_caches() {
	and_memo.clear();
	ex_memo.clear();
	and_many_memo.clear();
	quant_memo.clear();
	ite_memo.clear();
	// The tables and the order they were populated under go together: a
	// caller clearing them by hand (the tests do, between cases) starts
	// the union afresh as well.
	last_order.clear();
	has_last_order = false;
}

/** @internal @copydoc tau_term_bdd::sync_order_cache(const order&) @endinternal */
template<NodeType node>
void tau_term_bdd<node>::sync_order_cache(const order& o) {
	// last_order is the UNION of the orders seen since the last clear. A
	// memo entry is keyed by BDD refs alone, so it stays valid under any
	// order that gives the keys it was computed under the SAME ranks; only
	// a key coming back with a different rank invalidates the tables.
	for (const auto& [k, r] : o)
		if (auto it = last_order.find(k);
			it != last_order.end() && it->second != r)
		{
			clear_caches();
			last_order = o;
			has_last_order = true;
			return;
		}
	last_order.insert(o.begin(), o.end());
	has_last_order = true;
}
#endif

/** @internal @copydoc tau_term_bdd::make_canonical(ref&, ref&) @endinternal */
template<NodeType node>
void tau_term_bdd<node>::make_canonical(ref& x, ref& y) {
	if (y < x) std::swap(x,y);
}

/** @internal @copydoc tau_term_bdd::less_then(tref, tref, const order&) @endinternal */
template<NodeType node>
bool tau_term_bdd<node>::less_then(tref x, tref y, const order& o) {
	const auto xiter = o.find(x), yiter = o.find(y);
	// Both variables must be present in the order for comparison
	if (xiter == o.end() || yiter == o.end()) return false;
	return xiter->second < yiter->second;
}

/** @internal @copydoc tau_term_bdd::add(tref, ref, ref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::add(tref v, ref h, ref l) {
	DBG(assert(v != nullptr && h.b != nullptr && l.b != nullptr));
	using tau = tree<node>;
	if (h == l) return h;
	auto bn = bdd_node(v);
	// Invariant for output inverters: v of h >= v of l
	if (tau::subtree_less(tau_term_bdd::get(h.b).value.v, tau_term_bdd::get(l.b).value.v)) {
		std::swap(h,l);
		bn.inv_v = true;
	}
	ref r;
	// Invariant for input inverters: l is not inverted
	if (l.inv) {
		h.inv = !h.inv;
		l.inv = !l.inv;
		r.inv = true;
	}
	bn.inv_h = h.inv;
	bn.inv_l = l.inv;
	r.b = tau_term_bdd::get(bn, h.b, l.b);
	return r;
}

/** @internal @copydoc tau_term_bdd::add(tref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::add(tref leaf) {
	using tau = tree<node>;
	// We remove the right sibling of leaf as a precaution
	// In principle, this makes comparisons between leaf not require
	// tau::subtree_equals
	leaf = tau::trim_right_sibling(leaf);
	const tau& tl = tau::get(leaf);
	if (tl.is(tau::bf_t)) return T;
	if (tl.is(tau::bf_f)) return F;
	// In case leaf is negated, use inverter
	if (tl.is(tau::bf_neg)) {
		const auto bn = bdd_node(tau::trim2(leaf));
		return ref(tau_term_bdd::get(bn, nullptr, nullptr), true);
	} else {
		const auto bn = bdd_node(leaf);
		return ref(tau_term_bdd::get(bn, nullptr, nullptr), false);
	}
}

/** @internal @copydoc tau_term_bdd::get_node(ref) @endinternal */
template<NodeType node>
tau_term_bdd<node> tau_term_bdd<node>::get_node(ref x) {
	// Check for input inverter
	const tau_term_bdd& y = get(x.b);
	const bdd_node& yv = y.value;
	if (x.inv) {
		// Check for output inverter
		if (yv.inv_v) {
			auto bn = bdd_node(yv.v, !yv.inv_v, !yv.inv_l, !yv.inv_h);
			return tau_term_bdd(bn, y.r, y.l);
		} else {
			auto bn = bdd_node(yv.v, yv.inv_v, !yv.inv_h, !yv.inv_l);
			return tau_term_bdd(bn, y.l, y.r);
		}
	} else {
		// Check for output inverter
		if (yv.inv_v) {
			auto bn = bdd_node(yv.v, !yv.inv_v, yv.inv_l, yv.inv_h);
			return tau_term_bdd(bn, y.r, y.l);
		} else {
			return y;
		}
	}
}

/** @internal @copydoc tau_term_bdd::get_var(ref) @endinternal */
template<NodeType node>
tref tau_term_bdd<node>::get_var(ref x) {
	using tau = tree<node>;
	tref v = tau_term_bdd::get(x.b).value.v;
	// If x is a leaf we need to consider inverter
	if (leaf(x)) {
		if (x.inv) v = tau::get(tau::bf_neg, tau::get(tau::bf, v));
	}
	DBG(assert(!tau::get(v).is(tau::bf)));
	return v;
}

/** @internal @copydoc tau_term_bdd::get_var_term(ref) @endinternal */
template<NodeType node>
tref tau_term_bdd<node>::get_var_term(ref x) {
	using tau = tree<node>;
	return tau::get(tau::bf, get_var(x));
}

/** @internal @copydoc tau_term_bdd::get_high(ref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::get_high(ref x) {
	const auto& y = tau_term_bdd::get(x.b);
	const bdd_node& yv = y.value;
	// Input inverter is automatically passed on
	// Check output inverter
	if (yv.inv_v) {
		x.b = y.r;
		if (yv.inv_l) x.inv = !x.inv;
		return x;
	} else {
		x.b = y.l;
		if (yv.inv_h) x.inv = !x.inv;
		return x;
	}
}

/** @internal @copydoc tau_term_bdd::get_low(ref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::get_low(ref x) {
	const auto& y = tau_term_bdd::get(x.b);
	const bdd_node& yv = y.value;
	// Input inverter is automatically passed on
	// Check output inverter
	if (yv.inv_v) {
		x.b = y.l;
		if (yv.inv_h) x.inv = !x.inv;
		return x;
	} else {
		x.b = y.r;
		if (yv.inv_l) x.inv = !x.inv;
		return x;
	}
}

/** @internal @copydoc tau_term_bdd::leaf(ref) @endinternal */
template<NodeType node>
bool tau_term_bdd<node>::leaf(ref l) {
	if (l == T || l == F) return true;
	const auto& tl = tau_term_bdd::get(l.b);
	return tl.l == nullptr && tl.r == nullptr;
}

/** @internal @copydoc tau_term_bdd::visit_nodes(ref, Fn&, std::unordered_set<tref>&) @endinternal */
template<NodeType node>
template<typename Fn>
bool tau_term_bdd<node>::visit_nodes(ref x, Fn& fn, std::unordered_set<tref>& seen) {
	if (x == T || x == F) return true;
	// Keyed on the node, so a node reached under both inverters is one visit.
	if (!seen.insert(x.b).second) return true;
	if (leaf(x)) return fn(x, true);
	return fn(x, false) && visit_nodes(get_high(x), fn, seen)
		&& visit_nodes(get_low(x), fn, seen);
}

/** @internal @copydoc tau_term_bdd::visit_nodes(ref, Fn&) @endinternal */
template<NodeType node>
template<typename Fn>
bool tau_term_bdd<node>::visit_nodes(ref x, Fn& fn) {
	std::unordered_set<tref> seen;
	return visit_nodes(x, fn, seen);
}

/** @internal @copydoc tau_term_bdd::node_count(ref) @endinternal */
template<NodeType node>
size_t tau_term_bdd<node>::node_count(ref x) {
	size_t n = 0;
	auto count = [&n](ref, bool) { return ++n, true; };
	visit_nodes(x, count);
	return n;
}

/** @internal @copydoc tau_term_bdd::is_ordered(ref, const order&, std::unordered_set<tref>&) @endinternal */
template<NodeType node>
bool tau_term_bdd<node>::is_ordered(ref x, const order& o,
	std::unordered_set<tref>& seen) {
	if (leaf(x)) return true;
	if (!seen.insert(x.b).second) return true;
	tref v = get_var(x);
	if (!o.contains(v)) return false;
	for (ref c : { get_high(x), get_low(x) }) {
		if (!leaf(c) && !less_then(v, get_var(c), o)) return false;
		if (!is_ordered(c, o, seen)) return false;
	}
	return true;
}

/** @internal @copydoc tau_term_bdd::is_ordered(ref, const order&) @endinternal */
template<NodeType node>
bool tau_term_bdd<node>::is_ordered(ref x, const order& o) {
	std::unordered_set<tref> seen;
	return is_ordered(x, o, seen);
}

/**
 * @internal
 * @brief Creates a BDD from a given Tau term
 * @param f The Tau term to build the BDD from
 * @param o The ordering of tau terms
 * @param o_bound The current highest ordering id
 * @return A reference to the BDD corresponding to f
 * @endinternal
 */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::build_bdd(tref f, const order& o) {
	using tau = tree<node>;
	DBG(assert(tau::get(f).get_type() != tau::wff));
	// If the current tree is considered to be a BDD variable, it is
	// contained in o
	if (auto it = o.find(f); it != o.end()) {
		// Current tree is variable
		return from_bit(f);
	}
	switch (auto nt = tau::get(f).get_type()) {
		case tau::bf: return build_bdd(tau::trim(f), o);
		case tau::bf_and: {
			refs bdds;
			for (tref c : get_cnf_bf_clauses<node>(f))
				bdds.emplace_back(build_bdd(c, o));
			return bdd_and_many(std::move(bdds), o);
		}
		case tau::bf_or: {
			refs bdds;
			for (tref d : get_dnf_bf_clauses<node>(f))
				bdds.emplace_back(build_bdd(d, o));
			return bdd_or_many(std::move(bdds), o);
		}
		case tau::bf_xor: {
			const tau& tf = tau::get(f);
			const ref a = build_bdd(tf.first(), o);
			const ref b = build_bdd(tf.second(), o);
			return bdd_xor(a, b, o);
		}
		case tau::bf_neg: {
			const tau& tf = tau::get(f);
			return bdd_not(build_bdd(tf.first(), o));
		}
		case tau::bf_fall:
		case tau::bf_fex: {
			// §1 the functional-quantifier CHAIN: `Q_Y b` over the
			// order's keys is `x·Q_Y b₁ ∪ x′·Q_Y b₀` for a key `x`
			// outside `Y`, i.e. the body's BDD with the chain pushed
			// onto every leaf. A subscript that IS a key is bound
			// here, so it is dropped from the order the body is
			// built under and re-bound by the wrap.
			quants q;
			tref body = nullptr;
			for (tref c = f;;) {
				const tau& tc = tau::get(c);
				q.emplace_back(tau::trim_right_sibling(tc.first()),
					tc.is(tau::bf_fall) ? all : ex);
				body = tau::trim_right_sibling(tc.second());
				const tau& tb = tau::get(body);
				if (!tb.child_is(tau::bf_fall)
					&& !tb.child_is(tau::bf_fex)) break;
				c = tb.first();
			}
			// The chain meets no key: ONE leaf, but a canonical
			// one -- the constructor puts an input chain into
			// content order, merges its runs, drops a degenerate
			// subscript and folds a closed chain, and `add` maps a
			// folded `bf_t`/`bf_f` to the terminals. The body is
			// never built under @p o (it may be arbitrarily large),
			// only inside that fold, under the chain's own order;
			// a chain nested in it lands in this same case with a
			// smaller body, so the recursion terminates. The free
			// variables of a chain are read off its `bf` wrapper,
			// which excludes the subscripts.
			if (!has_bdd_var(tau::get(tau::bf,
				tau::trim_right_sibling(f)), o))
				return add(tau::trim(
					build_functional_quantifiers(q, body)));
			bool keyed = false;
			for (const auto& qy : q)
				if (o.contains(qy.first)) { keyed = true; break; }
			order sub;
			const order* ob = &o;
			if (keyed) {
				// A stored BDD has the subscripts as decision
				// variables, which the sub-order no longer
				// admits: spell it out before dropping them.
				if (tau::get(body).find_top([](tref m) {
					return tau::get(m).is(tau::BDD_ID); }))
					body = term_handle<node>::
						convert_to_tau_terms(body);
				sub = o;
				for (const auto& qy : q) sub.erase(qy.first);
				ob = &sub;
			}
			// rebuild = false: a wrapped leaf is re-interned as a
			// leaf, never built as a BDD -- see the worker.
			auto wrap = [&q](tref leaf) {
				return build_functional_quantifiers(q, leaf);
			};
			std::unordered_map<ref, ref> memo;
			ref r = map_leaves(build_bdd(body, *ob), wrap, o, false,
				memo);
			DBG(assert(is_ordered(r, o));)
			return r;
		}
		case tau::BDD_ID: {
			// Get the BDD corresponding to the ID
			const auto& m = term_handle<node>::U;
			auto it = m.find(term_handle<node>::key_of(f));
			if (it != m.end()) {
				DBG(assert(is_ordered(it->second.get(), o));)
				return it->second.get();
			} else {
				// If the BDD id is not found,
				// something went wrong
				DBG(assert(false));
				return add(f);
			}
		}
		default: {
			return add(f);
		}
	}
}

/** @internal @copydoc tau_term_bdd::from_bit(tref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::from_bit(tref v) {
	return add(v, T, F);
}

/** @internal @copydoc tau_term_bdd::has_bdd_var(tref, const order&) @endinternal */
template<NodeType node>
bool tau_term_bdd<node>::has_bdd_var(tref term, const order& o) {
	if (o.empty()) return false;
	for (tref v : get_free_vars<node>(term)) if (o.contains(v)) return true;
	return false;
}

/** @internal @copydoc tau_term_bdd::build_functional_quantifiers(const quants&, tref) @endinternal */
template<NodeType node>
tref tau_term_bdd<node>::build_functional_quantifiers(const quants& q,
	tref body)
{
	using tau = tree<node>;
	body = tau::trim_right_sibling(body);
	DBG(assert(tau::get(body).is(tau::bf));)
	if (q.empty()) return body;
	if (tau::get(body).equals_0() || tau::get(body).equals_1()) return body;
	// 1. Drop the subscripts that do not occur free in `body` (a binder
	//    over an absent variable is degenerate) and keep a repeated one at
	//    its INNERMOST occurrence, the one that actually binds.
	const trefs& fv = get_free_vars<node>(body);
	quants kept;
	for (size_t i = 0; i < q.size(); ++i) {
		tref y = tau::trim_right_sibling(q[i].first);
		if (!std::binary_search(fv.begin(), fv.end(), y,
			tau::subtree_less)) continue;
		bool rebound = false;
		for (size_t j = i + 1; j < q.size() && !rebound; ++j)
			rebound = tau::subtree_equals(
				tau::trim_right_sibling(q[j].first), y);
		if (!rebound) kept.emplace_back(y, q[i].second);
	}
	if (kept.empty()) return body;
	// 2. The maximal runs of one kind: quantifiers of one run commute, so
	//    each run is sorted into content order and a permutation of a block
	//    gives one node.
	std::vector<std::pair<Quantifier, trefs>> segs;
	for (const auto& [y, k] : kept) {
		if (segs.empty() || segs.back().first != k)
			segs.emplace_back(k, trefs{});
		segs.back().second.push_back(y);
	}
	// 3. `body`'s own outermost run joins the new innermost one when the
	//    kinds agree, so a chain assembled in two steps is one node. Its
	//    subscripts are bound in `body`, hence distinct from the ones kept
	//    above.
	{
		const bool uni = segs.back().first == all;
		while (tau::get(body).child_is(uni ? tau::bf_fall : tau::bf_fex)) {
			const tau& c = tau::get(tau::get(body).first());
			segs.back().second.push_back(
				tau::trim_right_sibling(c.first()));
			body = tau::trim_right_sibling(c.second());
		}
	}
	// The chain, outermost first, and the order it quantifies under:
	// the INNERMOST subscript ranks 1, outward from there, which is what
	// `bdd_quant` asserts of the reversed prefix.
	quants prefix;
	for (auto& [k, ys] : segs) {
		std::sort(ys.begin(), ys.end(), tau::subtree_less);
		for (tref y : ys) prefix.emplace_back(y, k);
	}
	order o;
	for (size_t i = 0; i < prefix.size(); ++i)
		o.emplace(prefix[i].first, int_t(prefix.size() - i));
	// 4. FOLD: a chain binding every free variable of a PLAIN body is a
	//    CONSTANT -- once no subscript is left in a leaf either, the
	//    quantified BDD holds no variable at all, so its term is the
	//    chain's value whatever it spells (`_1`/`_0` for a terminal, the
	//    leaf itself for a variable-free leaf). Two bodies are left
	//    standing: one holding a `BDD_ID`, which belongs to another order
	//    and folds at the finish; and one with a subscript hidden inside a
	//    LEAF (§1's leaf hazard: a reference argument, a foreign-typed
	//    subterm), which `bdd_quant` does not reach. The cached free-var
	//    test comes first so the `BDD_ID` walk only runs on closed bodies.
	bool bound = true;
	for (tref v : get_free_vars<node>(body))
		if (!o.contains(v)) { bound = false; break; }
	if (bound) bound = tau::get(body).find_top([](tref m) {
		return tau::get(m).is(tau::BDD_ID); }) == nullptr;
	if (bound) {
		ref b = build_bdd(body, o);
		bool hidden = false;
		for (tref v : term_handle<node>::get_free_leaf_vars(b.b))
			if (o.contains(v)) { hidden = true; break; }
		if (!hidden) return to_tau_term(bdd_quant(b, prefix, o),
			find_ba_type<node>(body));
	}
	// 5. Wrap, innermost subscript first. The builders rename the bound
	//    variable by default; here the subscripts are spelled by the
	//    caller, so nothing is ever renamed.
	tref r = body;
	for (size_t i = prefix.size(); i--;)
		r = prefix[i].second == all
			? tau::build_bf_fall(prefix[i].first, r, false)
			: tau::build_bf_fex(prefix[i].first, r, false);
	return r;
}

/** @internal @copydoc tau_term_bdd::bdd_and(ref, tref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and(ref x, tref y) {
	using tau = tree<node>;
	if (x == T) return add(y);
	if (x == F) return F;
#ifdef TAU_CACHE
	if (auto it = and_memo.find({x, add(y)}); it != and_memo.end())
		return it->second;
#endif
	tref v = get_var(x);
	if (leaf(x)) return add(tau::trim(tau::build_bf_and(
		get_var_term(x), tau::get(tau::bf, y))));
	ref r = add(v, bdd_and(get_high(x), y), bdd_and(get_low(x), y));
#ifdef TAU_CACHE
	and_memo.emplace(std::array<ref, 2>{x, add(y)}, r);
#endif
	return r;
}

/** @internal @copydoc tau_term_bdd::bdd_and(ref, ref, const order&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and(ref x, ref y, const order& o) {
	// Fresh per-call memo when there is no persistent (TAU_CACHE) one to
	// thread instead -- see the memoised worker below: the recursion is
	// always memoized within this call, so shared sub-BDDs reached via
	// different paths are computed once, not once per path.
#ifdef TAU_CACHE
	sync_order_cache(o);
	return bdd_and(x, y, o, and_memo);
#else
	std::unordered_map<std::array<ref, 2>, ref> local_memo;
	return bdd_and(x, y, o, local_memo);
#endif
}

/** @internal @copydoc tau_term_bdd::bdd_and(ref, ref, const order&, std::unordered_map<std::array<ref,2>,ref>&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and(ref x, ref y, const order& o,
	std::unordered_map<std::array<ref, 2>, ref>& memo) {
	// Check trivial cases
	if (x == F || y == F || x == bdd_not(y)) return F;
	if (x == T || x == y) return y;
	if (y == T) return x;
	make_canonical(x, y);
	if (auto it = memo.find(std::array<ref, 2>{x, y}); it != memo.end())
		return it->second;
	// Check if x and y is a leaf
	if (leaf(x)) return bdd_and(y, get_var(x));
	if (leaf(y)) return bdd_and(x, get_var(y));
	// Compare v of x and y for further traversal
	tref xv = get_var(x);
	tref yv = get_var(y);
	ref r;
	if (tree<node>::subtree_equals(xv, yv)) {
		r = add(xv, bdd_and(get_high(x), get_high(y), o, memo),
			bdd_and(get_low(x), get_low(y), o, memo));
	} else if (less_then(xv, yv, o)) {
		// var of x is smaller than var of y
		r = add(xv, bdd_and(get_high(x), y, o, memo),
			bdd_and(get_low(x), y, o, memo));
	} else {
		r = add(yv, bdd_and(get_high(y), x, o, memo),
			bdd_and(get_low(y), x, o, memo));
	}
	memo.emplace(std::array<ref, 2>{x, y}, r);
	return r;
}

/** @internal @copydoc tau_term_bdd::bdd_or(ref, ref, const order&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_or(ref x, ref y, const order& o) {
	return bdd_not(bdd_and(bdd_not(x), bdd_not(y), o));
}

/** @internal @copydoc tau_term_bdd::bdd_not(ref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_not(ref x) {
	return x.inv = !x.inv, x;
}

/** @internal @copydoc tau_term_bdd::bdd_xor(ref, ref, const order&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_xor(ref x, ref y, const order& o) {
	return bdd_or(bdd_and(x, bdd_not(y), o), bdd_and(bdd_not(x), y, o), o);
}

/** @internal @copydoc tau_term_bdd::bdd_ite(ref, ref, ref, const order&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_ite(ref f, ref g, ref h,
	const order& o) {
	// See bdd_and's public entry: always memoize the recursion within
	// this call, via the static ite_memo (TAU_CACHE, cross-call too) or a
	// fresh local map otherwise.
#ifdef TAU_CACHE
	sync_order_cache(o);
	return bdd_ite(f, g, h, o, ite_memo);
#else
	std::unordered_map<std::array<ref, 3>, ref> local_memo;
	return bdd_ite(f, g, h, o, local_memo);
#endif
}

/** @internal @copydoc tau_term_bdd::bdd_ite(ref, ref, ref, const order&, std::unordered_map<std::array<ref,3>,ref>&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_ite(ref f, ref g, ref h,
	const order& o, std::unordered_map<std::array<ref, 3>, ref>& memo) {
	using tau = tree<node>;
	// Rule 1: normalize complement on f
	if (f.inv) { f.inv = false; std::swap(g, h); }
	// Terminal reductions (f.inv == false guaranteed)
	if (f == T) return g;
	if (f == F) return h;
	if (g == h) return g;
	if (g == T && h == F) return f;
	if (g == F && h == T) return bdd_not(f);
	if (g == T) return bdd_or(f, h, o);
	if (h == F) return bdd_and(f, g, o);
	if (g == F) return bdd_and(bdd_not(f), h, o);
	if (h == T) return bdd_or(bdd_not(f), g, o);
	if (g == f) return bdd_or(f, h, o);
	if (h == f) return bdd_and(f, g, o);
	if (g == bdd_not(f)) return bdd_and(bdd_not(f), h, o);
	if (h == bdd_not(f)) return bdd_or(bdd_not(f), g, o);
	// All-leaf fallback: delegate to composed form
	if (leaf(f) && leaf(g) && leaf(h))
		return bdd_or(bdd_and(f, g, o), bdd_and(bdd_not(f), h, o), o);
	// Rule 2: normalize common complement on g and h
	bool out_inv = false;
	if (g.inv && h.inv) { g.inv = false; h.inv = false; out_inv = true; }
	const std::array<ref, 3> key = {f, g, h};
	if (auto it = memo.find(key); it != memo.end())
		return out_inv ? bdd_not(it->second) : it->second;
	// Top variable: minimum rank among the three BDD roots
	tref top = get_var(f);
	auto upd = [&](ref x) {
		if (!leaf(x)) { tref v = get_var(x); if (less_then(v, top, o)) top = v; }
	};
	upd(g); upd(h);
	// Co-factor inline — no temporary BDD allocation
	auto cof = [&](ref x, bool hi) -> ref {
		if (leaf(x) || !tau::subtree_equals(get_var(x), top)) return x;
		return hi ? get_high(x) : get_low(x);
	};
	ref r = add(top,
		bdd_ite(cof(f, true),  cof(g, true),  cof(h, true),  o, memo),
		bdd_ite(cof(f, false), cof(g, false), cof(h, false), o, memo));
	memo.emplace(key, r);
	return out_inv ? bdd_not(r) : r;
}

/** @internal @copydoc tau_term_bdd::bdd_compose(ref, tref, ref, const order&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_compose(ref x, tref xi, ref g,
	const order& o) {
	// A terminal g IS a cofactor: child selection, without routing every
	// node through bdd_ite.
	if (g == T || g == F) return bdd_cofactor(x, xi, g == T, o);
	std::unordered_map<ref, ref> memo;
	return bdd_compose_impl(x, xi, g, o, memo);
}

/** @internal @copydoc tau_term_bdd::bdd_compose_impl(ref, tref, ref, const order&, std::unordered_map<ref, ref>&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_compose_impl(ref x, tref xi, ref g,
	const order& o, std::unordered_map<ref, ref>& memo) {
	using tau = tree<node>;
	if (leaf(x)) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	tref var = get_var(x);
	if (less_then(xi, var, o)) return x;
	ref r;
	if (tau::subtree_equals(xi, var))
		r = bdd_ite(g,
			bdd_compose_impl(get_high(x), xi, g, o, memo),
			bdd_compose_impl(get_low(x), xi, g, o, memo), o);
	else
		r = bdd_ite(from_bit(var),
			bdd_compose_impl(get_high(x), xi, g, o, memo),
			bdd_compose_impl(get_low(x), xi, g, o, memo), o);
	return memo.emplace(x, r).first->second;
}

/** @internal @copydoc tau_term_bdd::bdd_compose(ref, subs_t, const order&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_compose(ref x, subs_t subs,
	const order& o) {
	auto cmp = [&o](const auto& a, const auto& b) {
		return less_then(a.first, b.first, o); };
	sortc(subs, cmp);
	std::unordered_map<ref, ref> memo;
	return bdd_compose_impl(x, subs, 0, o, memo);
}

/** @internal @copydoc tau_term_bdd::bdd_compose_impl(ref, const subs_t&, size_t, const order&, std::unordered_map<ref, ref>&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_compose_impl(ref x,
	const subs_t& subs, size_t i, const order& o,
	std::unordered_map<ref, ref>& memo) {
	using tau = tree<node>;
	if (leaf(x) || i >= subs.size()) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	tref var = get_var(x);
	while (i < subs.size() && less_then(subs[i].first, var, o)) ++i;
	if (i >= subs.size()) return x;
	ref r;
	if (tau::subtree_equals(subs[i].first, var))
		r = bdd_ite(subs[i].second,
			bdd_compose_impl(get_high(x), subs, i + 1, o, memo),
			bdd_compose_impl(get_low(x), subs, i + 1, o, memo), o);
	else
		r = bdd_ite(from_bit(var),
			bdd_compose_impl(get_high(x), subs, i, o, memo),
			bdd_compose_impl(get_low(x), subs, i, o, memo), o);
	return memo.emplace(x, r).first->second;
}

/** @internal @copydoc tau_term_bdd::bdd_cofactor(ref, tref, bool, const order&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_cofactor(ref x, tref xi,
	bool bit, const order& o) {
	if (!o.contains(xi)) return x;
	std::unordered_map<ref, ref> memo;
	return bdd_cofactor_impl(x, xi, bit, o, memo);
}

/**
 * @internal
 * @brief Memoised worker for bdd_cofactor: a leaf is itself; a node whose
 * variable is above @p xi in the order is itself (an ordered BDD holds @p xi
 * at most once per path, never above); the node of @p xi is its child by
 * @p bit; any other node is rebuilt through `add`, which keeps canonicity
 * because both children stay ordered below it.
 * @endinternal
 */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_cofactor_impl(ref x, tref xi,
	bool bit, const order& o, std::unordered_map<ref, ref>& memo) {
	using tau = tree<node>;
	if (leaf(x)) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	tref var = get_var(x);
	if (less_then(xi, var, o)) return x;
	ref r;
	if (tau::subtree_equals(xi, var)) r = bit ? get_high(x) : get_low(x);
	else r = add(var, bdd_cofactor_impl(get_high(x), xi, bit, o, memo),
		bdd_cofactor_impl(get_low(x), xi, bit, o, memo));
	return memo.emplace(x, r).first->second;
}

/** @internal @copydoc tau_term_bdd::map_leaves(ref, Fn&, const order&, bool, std::unordered_map<ref, ref>&) @endinternal */
template<NodeType node>
template<typename Fn>
tau_term_bdd<node>::ref tau_term_bdd<node>::map_leaves(ref x, Fn& fn,
	const order& o, bool rebuild, std::unordered_map<ref, ref>& memo) {
	using tau = tree<node>;
	if (x == T || x == F) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	ref r;
	if (leaf(x)) {
		tref in  = get_var_term(x);
		tref out = fn(in);
		if (out == in) r = x;
		else if (rebuild && has_bdd_var(out, o)) r = build_bdd(out, o);
		else {
			// Without the rebuild the caller promises to add no
			// key of @p o a leaf did not already hold, so nothing
			// of the order is buried by the re-interning.
			DBG(assert(rebuild || !has_bdd_var(out, o)
				|| has_bdd_var(in, o));)
			r = add(tau::trim(out));
		}
	} else {
		ref h = map_leaves(get_high(x), fn, o, rebuild, memo);
		ref l = map_leaves(get_low(x), fn, o, rebuild, memo);
		tref var = get_var(x);
		auto below = [&](ref c) {
			return leaf(c) || less_then(var, get_var(c), o);
		};
		if (h == get_high(x) && l == get_low(x)) r = x;
		else if (below(h) && below(l)) r = add(var, h, l);
		else r = bdd_ite(from_bit(var), h, l, o);
	}
	return memo.emplace(x, r).first->second;
}

/** @internal @copydoc tau_term_bdd::map_leaves(ref, Fn&, const order&) @endinternal */
template<NodeType node>
template<typename Fn>
tau_term_bdd<node>::ref tau_term_bdd<node>::map_leaves(ref x, Fn& fn,
	const order& o) {
	std::unordered_map<ref, ref> memo;
	return map_leaves(x, fn, o, true, memo);
}

/** @internal @copydoc tau_term_bdd::bdd_ex(ref, trefs&, const order&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_ex(ref x, trefs& v,
	const order& o) {
	// sort v, so the smallest variable is up front
	auto cmp = [&o](tref e1, tref e2){return less_then(e1,e2, o);};
	sortc(v, cmp);
	// Always memoize the recursion within this call (fresh local map), on
	// top of which TAU_CACHE's static ex_memo, keyed by v, is the
	// optional cross-call persistence layer -- see bdd_and's public entry
	// for the same pattern.
#ifdef TAU_CACHE
	sync_order_cache(o);
	return bdd_ex(x, v, 0, o, ex_memo[v]);
#else
	std::unordered_map<ref, ref> local_memo;
	return bdd_ex(x, v, 0, o, local_memo);
#endif
}

/** @internal @copydoc tau_term_bdd::bdd_all(ref, trefs, const order&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_all(ref x, trefs v,
	const order& o) {
	// We utilize existential quantification in order to share cache
	return bdd_not(bdd_ex(bdd_not(x), v, o));
}

/** @internal @copydoc tau_term_bdd::bdd_quant(ref, const quants&, const order&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_quant(ref x, const quants& v,
	const order& o) {
	// v holds the logical quantification pattern, ie. we have the
	// assumption that the last entry is resolved first.
	// To ease the algorithm, we reverse the pattern
	quants v_rev (v.rbegin(), v.rend());
	// Assert that v is ordered correctly according to o
#ifdef DEBUG
	auto cmp = [&o](tref e1, tref e2){return less_then(e1,e2, o);};
	for (size_t i = 1; i < v_rev.size(); ++i)
		assert(cmp(v_rev[i-1].first, v_rev[i].first));
#endif
#ifdef TAU_CACHE
	sync_order_cache(o);
	return bdd_quant(x, v_rev, 0, o, quant_memo[v]);
#else
	std::unordered_map<ref, ref> local_memo;
	return bdd_quant(x, v_rev, 0, o, local_memo);
#endif
}

// Memoised worker, used unconditionally (not just under TAU_CACHE): the
// caller (bdd_ex's public entry above) always supplies a memo, either a
// fresh local one or the persistent static ex_memo.
/** @internal @copydoc tau_term_bdd::bdd_ex(ref, const trefs&, size_t, const order&, auto&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_ex(ref x, const trefs& v, size_t i,
	const order& o, auto& memo) {
	using tau = tree<node>;
	const tref var = get_var(x);
	if (leaf(x) || i >= v.size() || less_then(v.back(), var, o)) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	// while current variable is bigger, increase index
	while (i < v.size() && less_then(v[i], var, o)) ++i;
	if (i >= v.size()) return x;
	if (tau::subtree_equals(v[i], var))
		return bdd_ex(bdd_or(get_high(x), get_low(x), o), v, ++i, o, memo);
	return memo.emplace(x,
		add(var, bdd_ex(get_high(x), v, i, o, memo),
			bdd_ex(get_low(x), v, i, o, memo))).first->second;
}

// Memoised worker, used unconditionally -- see bdd_ex's worker above.
/** @internal @copydoc tau_term_bdd::bdd_quant(ref, const quants&, size_t, const order&, auto&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_quant(ref x, const quants& v,
	size_t i, const order& o, auto& memo) {
	using tau = tree<node>;
	const tref var = get_var(x);
	// If we have passed the last variable in v, we are done
	if (leaf(x) || i >= v.size() || less_then(v.back().first, var, o))
		return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	// while current variable is bigger, increase index
	while (i < v.size() && less_then(v[i].first, var, o)) ++i;
	if (i >= v.size()) return x;
	if (tau::subtree_equals(v[i].first, var)) {
		// Eliminate the quantifier
		if (v[i].second == Quantifier::ex)
			return bdd_quant(bdd_or(get_high(x),
				get_low(x), o), v, ++i, o, memo);
		// Otherwise Quantifier::all
		else return bdd_quant(bdd_and(get_high(x),
			get_low(x), o), v, ++i, o, memo);
	}
	return memo.emplace(x, add(var, bdd_quant(get_high(x), v, i, o, memo),
		bdd_quant(get_low(x), v, i, o, memo))).first->second;
}

/** @internal @copydoc tau_term_bdd::to_tau_term(ref, size_t) @endinternal */
template<NodeType node>
tref tau_term_bdd<node>::to_tau_term(ref x, size_t term_type) {
	std::unordered_map<ref, tref> memo;
	return to_tau_term(x, term_type, memo);
}

// Memoised worker: term_type is constant within one top-level call, and
// shared BDD nodes recur across paths, so a memo keyed by the BDD ref
// avoids rebuilding the same subterm once per path.
template<NodeType node>
tref tau_term_bdd<node>::to_tau_term(ref x, size_t term_type,
	std::unordered_map<ref, tref>& memo) {
	using tau = tree<node>;

	if (x == T) return tau::_1(term_type);
	if (x == F) return tau::_0(term_type);

	if (const auto it = memo.find(x); it != memo.end()) return it->second;

	tref v = get_var_term(x);
	ref h = get_high(x);
	ref l = get_low(x);

	if (leaf(x)) return memo.emplace(x, v), v;

	tref left = tau::build_bf_and(v, to_tau_term(h, term_type, memo));
	tref right = tau::build_bf_and(tau::build_bf_neg(v),
		to_tau_term(l, term_type, memo));
	tref res = tau::build_bf_or(left, right);
	return memo.emplace(x, res), res;
}

/** @internal @copydoc tau_term_bdd::bdd_and_many_iter(const refs&, refs&, refs&, ref&, tref&, const order&, std::unordered_map<refs,ref>&) @endinternal */
template<NodeType node>
size_t tau_term_bdd<node>::bdd_and_many_iter(const refs& v,
	refs& h, refs& l, ref& res, tref& m, const order& o,
	std::unordered_map<refs, ref>& memo) {
	using tau = tree<node>;
	size_t i;
	bool flag = false;
	// Choose first BDD that is not a leaf and assign variable to m
	for (i = 0; i != v.size(); ++i)
		if (!leaf(v[i])) {
			m = get_var(v[i]); break;
		} else if (v[i] == F) return res = F, 1;
	if (i == v.size()) {
		// All refs are leaves
		res = T;
		for (const ref& el : v) res = bdd_and(res, get_var(el));
		return 1;
	}
	// m becomes smallest variable
	for (; i != v.size(); ++i)
		if (!leaf(v[i])) {
			if ( less_then(get_var(v[i]), m, o) ) m = get_var(v[i]);
		} else if (v[i] == F) return res = F, 1;

	h.reserve(v.size()), l.reserve(v.size());
	for (i = 0; i != v.size(); ++i)
		// m holds the smallest variable, so unequal variables are bigger or leaves
		if (!tau::subtree_equals(get_var(v[i]), m)) h.push_back(v[i]);
		else if (get_high(v[i]) == F) { flag = true; break; }
		else if (get_high(v[i]) != T) h.push_back(get_high(v[i]));

	if (!flag) am_sort(h);
	for (i = 0; i != v.size(); ++i)
		// m holds the smallest variable, so unequal variables are bigger or leaves
		if (!tau::subtree_equals(get_var(v[i]), m)) l.push_back(v[i]);
		else if (get_low(v[i]) == F) return flag ? res = F, 1 : 2;
		else if (get_low(v[i]) != T) l.push_back(get_low(v[i]));

	am_sort(l);
	if (!flag) { if (h.size() && h[0] == F) flag = true; }
	if (l.size() && l[0] == F) return flag ? 3 : 2;
	if (flag) return 3;

	refs x;
	set_intersection(h.begin(),h.end(),l.begin(),l.end(),back_inserter(x),
			am_cmp);
	am_sort(x);
	if (x.size() > 1) {
		for (size_t n = 0; n < h.size();)
			if (hasbc(x, h[n], am_cmp)) h.erase(h.begin() + n);
			else ++n;
		for (size_t n = 0; n < l.size();)
			if (hasbc(x, l[n], am_cmp)) l.erase(l.begin() + n);
			else ++n;
		h.shrink_to_fit(), l.shrink_to_fit(), x.shrink_to_fit();
		ref r = bdd_and_many(std::move(x), o, memo);
		if (r == F) return res = F, 1;
		if (r != T) {
			if (!hasbc(h, r, am_cmp)) h.push_back(r), am_sort(h);
			if (!hasbc(l, r, am_cmp)) l.push_back(r), am_sort(l);
		}
	}
	return 0;
}

/** @internal @copydoc tau_term_bdd::bdd_and_many(refs, const order&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and_many(refs v, const order& o) {
	// See bdd_and's public entry: always memoize the recursion within
	// this call, via the static and_many_memo (TAU_CACHE, cross-call too)
	// or a fresh local map otherwise.
#ifdef TAU_CACHE
	sync_order_cache(o);
	return bdd_and_many(std::move(v), o, and_many_memo);
#else
	std::unordered_map<refs, ref> local_memo;
	return bdd_and_many(std::move(v), o, local_memo);
#endif
}

/** @internal @copydoc tau_term_bdd::bdd_and_many(refs, const order&, std::unordered_map<refs,ref>&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and_many(refs v, const order& o,
	std::unordered_map<refs, ref>& memo) {
#ifdef TAU_CACHE
	// Cross-call pairwise-duplicate elimination: only meaningful against
	// the persistent and_memo (a local per-call memo never sees a prior
	// call's pairs to begin with), so stays TAU_CACHE-only.
	for (size_t n = 0; n < v.size(); ++n)
		for (size_t k = 0; k < n; ++k) {
			ref x, y;
			// Make assignment to x and y canonical
			if (v[n] < v[k]) x = v[n], y = v[k];
			else x = v[k], y = v[n];
			if (auto jt = and_memo.find({x, y}); jt != and_memo.end()) {
				v.erase(v.begin()+k);
				v.erase(v.begin()+n-1);
				v.push_back(jt->second);
				n = n - 2;
				break;
			}
		}
#endif

	if (v.empty()) return T;
	if (v.size() == 1) return v[0];

#ifdef TAU_CACHE
	static refs v1;
	do {
		if (v1=v, am_simplify(v, memo), v.size()==1) return v[0];
	} while (v1 != v);
#endif

	if (v.empty()) return T;
	if (v.size() == 1) return v[0];

	if (auto it = memo.find(v); it != memo.end()) return it->second;

	if (v.size() == 2) {
		ref r = bdd_and(v[0], v[1], o);
		return memo.emplace(v, r).first->second;
	}

	ref res = F, h, l;
	tref m = nullptr;
	refs vh, vl;
	switch (bdd_and_many_iter(v, vh, vl, res, m, o, memo)) {
		case 0: l = bdd_and_many(std::move(vl), o, memo),
			h = bdd_and_many(std::move(vh), o, memo);
			break;
		case 1: return memo.emplace(v, res).first->second;
		case 2: h = bdd_and_many(std::move(vh), o, memo), l = F; break;
		case 3: h = F, l = bdd_and_many(std::move(vl), o, memo); break;
		default: { DBG(assert(false)); return ref(); }
	}
	return memo.emplace(v, add(m, h, l)).first->second;
}

/** @internal @copydoc tau_term_bdd::bdd_or_many(refs, const order&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_or_many(refs v, const order& o) {
	for (ref& el : v) el = bdd_not(el);
	return bdd_not(bdd_and_many(v, o));
}

/** @internal @copydoc tau_term_bdd::am_sort(refs&) @endinternal */
template<NodeType node>
void tau_term_bdd<node>::am_sort(refs& b) {
	sortc(b, am_cmp);
	for (size_t n = 0; n < b.size();)
		if (b[n] == T) b.erase(b.begin() + n);
		else if (b[n] == F) { b = {F}; return; }
		else if (!n) { ++n; continue; }
		else if (b[n] == b[n-1]) b.erase(b.begin() + n);
		else if (b[n] == bdd_not(b[n-1])) { b = {F}; return; }
		else if (leaf(b[n]) && leaf(b[n-1])) {
			b[n-1] = bdd_and(b[n-1], get_var(b[n]));
			b.erase(b.begin() + n);
		}
		else ++n;
}

/** @internal @copydoc tau_term_bdd::am_simplify(refs&, const std::unordered_map<refs, ref>&) @endinternal */
template<NodeType node>
bool tau_term_bdd<node>::am_simplify(refs& v,
	const std::unordered_map<refs, ref>& memo) {
	am_sort(v);
	for (auto x : memo)
		if (subset(x.first, v)) {
			if (x.second == F) return v={F}, true;
			for (size_t n = 0; n < v.size();)
				if (!hasbc(x.first, v[n], am_cmp)) ++n;
				else v.erase(v.begin() + n);
			if (!hasbc(v, x.second, am_cmp)) v.push_back(x.second);
			return true;
		}
	return false;
}

/** @internal @copydoc tau_term_bdd::subset(const refs&, const refs&) @endinternal */
template<NodeType node>
bool tau_term_bdd<node>::subset(const refs& small, const refs& big) {
	if ( big.size() < small.size() ||
		am_cmp(abs(big[big.size()-1]), abs(small[0])) ||
		am_cmp(abs(small[small.size()-1]), abs(big[0])))
		return false;
	for (const ref& t : small) if (!hasbc(big, t, am_cmp)) return false;
	return true;
}

/** @internal @copydoc tau_term_bdd::abs(ref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::abs(ref x) {
	return x.inv = false, x;
}

/** @internal @copydoc tau_term_bdd::collect_live_refs(std::unordered_set<tref>&) @endinternal */
template<NodeType node>
void tau_term_bdd<node>::collect_live_refs(std::unordered_set<tref>& keep) {
	for (const auto& [bn, _] : bintree<tau_bdd_node<node>>::M())
		if (bn.value.v) keep.insert(bn.value.v);
}

template<NodeType node>
tau_term_bdd_handle<node>::universe_t& tau_term_bdd_handle<node>::U =
	bintree<node>::template create_cache<universe_t>();

// Not a create_cache: that machinery promotes an entry's value trefs to
// reachable whenever the key's trefs are (vacuously, for a key without any),
// which would pin every minted node for the process lifetime. Interning
// only has to hold among LIVE nodes, so the map gets the post-sweep
// callback alone: an entry dies with its node, together with U's.
template<NodeType node>
tau_term_bdd_handle<node>::intern_t& tau_term_bdd_handle<node>::I =
	[]() -> intern_t& {
		static intern_t cache;
		std::unique_lock lock(bintree<node>::mtx_);
		bintree<node>::gc_callbacks.push_back(
			[](const std::unordered_set<tref>& kept) {
				for (auto it = cache.begin(); it != cache.end();)
					if (!kept.contains(it->second)) it = cache.erase(it);
					else ++it;
			});
		return cache;
	}();

/** @internal @copydoc tau_term_bdd_handle::tau_term_bdd_handle(ref) @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::tau_term_bdd_handle(ref x) {
	h = tbdd::geth(x.b);
	inv = x.inv;
}

/** @internal @copydoc tau_term_bdd_handle::build(tref, const order&) @endinternal */
template<NodeType node>
tau_term_bdd_handle<node> tau_term_bdd_handle<node>::build(tref term, const order& o) {
	return term_handle(tbdd::build_bdd(term, o));
}

/** @internal @copydoc tau_term_bdd_handle::convert_to_tau_node(term_handle, size_t) @endinternal */
template<NodeType node>
tref tau_term_bdd_handle<node>::convert_to_tau_node(term_handle handle, size_t term_type) {
	using tau = tree<node>;

	// Interned: the same BDD under the same type is the same `BDD_ID` node,
	// so a round trip through to_tau_term and build gives back the node it
	// started from and hash-consed identity holds across conversions. The
	// store holds the `BDD_ID` node (the key, see `U`); the `bf` wrapper is
	// re-derived through the hash-consed typed constructor: the same node
	// while it lives, a fresh content-equal one otherwise.
	const intern_key_t key{handle, term_type};
	tref id_node;
	if (auto it = I.find(key); it != I.end()) id_node = it->second;
	else {
		static size_t bdd_id = 0;
		id_node = tau::get_typed(tau::BDD_ID, tau::get_num(bdd_id),
			term_type);
		// Increment id for unique node creation
		++bdd_id;
		// Save the connection both ways
		U.emplace(id_node, handle);
		I.emplace(key, id_node);
	}
	return tau::get_typed(tau::bf, id_node, term_type);
}

/** @internal @copydoc tau_term_bdd_handle::convert_to_tau_node(tref, const order&) @endinternal */
template<NodeType node>
tref tau_term_bdd_handle<node>::convert_to_tau_node(tref term, const order& o) {
	return convert_to_tau_node(build(term, o), find_ba_type<node>(term));
}

/** @internal @copydoc tau_term_bdd_handle::convert_to_tau_node_or_term(term_handle, size_t) @endinternal */
template<NodeType node>
tref tau_term_bdd_handle<node>::convert_to_tau_node_or_term(term_handle handle,
	size_t term_type) {
	return tbdd::leaf(handle.get())
		? tbdd::to_tau_term(handle.get(), term_type)
		: convert_to_tau_node(handle, term_type);
}

/** @internal @copydoc tau_term_bdd_handle::is_bdd_backed(tref) @endinternal */
template<NodeType node>
bool tau_term_bdd_handle<node>::is_bdd_backed(tref term) {
	using tau = tree<node>;
	return term != nullptr && tau::get(term).is(tau::bf)
		&& tau::get(term).child_is(tau::BDD_ID);
}

/** @internal @copydoc tau_term_bdd_handle::convert_to_handle(tref) @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
convert_to_handle(tref tau_node) {
	auto it = U.find(key_of(tau_node));
	DBG(assert(it != U.end()));
	if (it != U.end()) return it->second;
	else return term_handle(tbdd::T);
}

/** @internal @copydoc tau_term_bdd_handle::key_of(tref) @endinternal */
template<NodeType node>
tref tau_term_bdd_handle<node>::key_of(tref tau_node) {
	using tau = tree<node>;
	const tau& t = tau::get(tau_node);
	if (t.is(tau::BDD_ID))
		return t.has_right_sibling() ? tau::trim_right_sibling(tau_node)
			: tau_node;
	DBG(assert(t.is(tau::bf) && t.child_is(tau::BDD_ID)));
	// The wrapper's one child carries no right sibling, so it is the
	// stored node itself, whatever the wrapper's own sibling.
	DBG(assert(!tau::get(t.first()).has_right_sibling()));
	return t.first();
}

/** @internal @copydoc tau_term_bdd_handle::to_tau_term(size_t) const @endinternal */
template<NodeType node>
tref tau_term_bdd_handle<node>::to_tau_term(size_t term_type) const {
	return tbdd::to_tau_term(get(), term_type);
}

/** @internal @copydoc tau_term_bdd_handle::convert_to_tau_terms(tref) @endinternal */
template<NodeType node>
tref tau_term_bdd_handle<node>::convert_to_tau_terms(tref formula) {
	auto f = [](tref n) -> tref {
		if (!is_bdd_backed(n)) return n;
		tref plain = convert_to_handle(n).to_tau_term(find_ba_type<node>(n));
		// The traversal does not re-enter what it replaced, and a leaf may
		// hold a BDD_ID of its own (inside a reference argument, say), so
		// the produced term is converted here.
		return convert_to_tau_terms(plain);
	};
	// Every node kind is entered: binders, functional-quantifier bodies,
	// reference arguments, temporal scopes.
	return pre_order<node>(formula).apply_unique_until_change(f);
}

/** @internal @copydoc tau_term_bdd_handle::bdd_and(term_handle, const order&) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_and(term_handle other, const order& o) const {
	return term_handle(tbdd::bdd_and(get(), other.get(), o));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_or(term_handle, const order&) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_or(term_handle other, const order& o) const {
	return term_handle(tbdd::bdd_or(get(), other.get(), o));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_not() const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::bdd_not() const {
	return term_handle(h, !inv);
}

/** @internal @copydoc tau_term_bdd_handle::bdd_and_many(const term_handles&, const order&) @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_and_many(const term_handles& bdds, const order& o) {
	std::vector<ref> refs;
	refs.reserve(bdds.size());
	for (const term_handle b : bdds) refs.emplace_back(b.get());
	return term_handle(tbdd::bdd_and_many(std::move(refs), o));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_or_many(const term_handles&, const order&) @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_or_many(const term_handles& bdds, const order& o) {
	std::vector<ref> refs;
	refs.reserve(bdds.size());
	for (const term_handle b : bdds) refs.emplace_back(b.get());
	return term_handle(tbdd::bdd_or_many(std::move(refs), o));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_ex(const trefs&, const order&) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_ex(trefs& v, const order& o) const {
	return term_handle(tbdd::bdd_ex(get(), v, o));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_all(const trefs&, const order&) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_all(const trefs& v, const order& o) const {
	return term_handle(tbdd::bdd_all(get(), v, o));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_quant(const quants&, const order&) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_quant(const quants& q, const order& o) const {
	return term_handle(tbdd::bdd_quant(get(), q, o));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_ite(term_handle, term_handle, const order&) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_ite(term_handle g, term_handle h, const order& o) const {
	return term_handle(tbdd::bdd_ite(get(), g.get(), h.get(), o));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_compose(tref, term_handle, const order&) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_compose(tref xi, term_handle g, const order& o) const {
	return term_handle(tbdd::bdd_compose(get(), xi, g.get(), o));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_compose(const std::vector<std::pair<tref, term_handle>>&, const order&) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_compose(const std::vector<std::pair<tref, term_handle>>& subs, const order& o) const {
	typename tbdd::subs_t raw;
	raw.reserve(subs.size());
	for (const auto& [v, th] : subs) raw.emplace_back(v, th.get());
	return term_handle(tbdd::bdd_compose(get(), std::move(raw), o));
}

/** @internal @copydoc tau_term_bdd_handle::get() const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::ref tau_term_bdd_handle<node>::get() const {
	return ref(h->get(), inv);
}

/** @internal @copydoc tau_term_bdd_handle::operator==(const tau_term_bdd_handle&) const @endinternal */
template<NodeType node>
bool tau_term_bdd_handle<node>::operator==(const tau_term_bdd_handle& other) const {
	return get() == other.get();
}

/** @internal @copydoc tau_term_bdd_handle::operator!=(const tau_term_bdd_handle&) const @endinternal */
template<NodeType node>
bool tau_term_bdd_handle<node>::operator!=(const tau_term_bdd_handle& other) const {
	return !(*this == other);
}

/** @internal @copydoc tau_term_bdd_handle::get_free_tau_vars(tref) @endinternal */
template<NodeType node>
const trefs& tau_term_bdd_handle<node>::get_free_tau_vars(tref bdd_tref) {
	static const trefs no_free_vars{};
	if (!bdd_tref) return no_free_vars;
	// The cache stores the free-vars vectors directly (the old global
	// free_vars_pool indirection was not gc-aware). It is unconditional —
	// not gated by TAU_CACHE — because the returned reference needs stable
	// storage.
	static bdd_fv_cache_t& cache =
		tbdd::template create_cache<bdd_fv_cache_t>();
	if (auto it = cache.find(bdd_tref); it != cache.end())
		return it->second;
	auto [it, _] = cache.emplace(bdd_tref,
		collect_free_tau_vars(bdd_tref, false));
	return it->second;
}

/** @internal @copydoc tau_term_bdd_handle::collect_free_tau_vars @endinternal */
template<NodeType node>
trefs tau_term_bdd_handle<node>::collect_free_tau_vars(tref bdd_tref,
	bool leaves_only)
{
	// One walk over the DISTINCT nodes (visit_nodes), so a shared sub-BDD
	// is entered once and not once per path. Each node's variables are
	// appended and the whole is sorted once, which costs one allocation
	// instead of one per variable.
	trefs merged;
	auto collect = [&merged, leaves_only](ref x, bool is_leaf) {
		if (leaves_only && !is_leaf) return true;
		const trefs& v_fvs = get_free_vars<node>(tbdd::get_var_term(x));
		merged.insert(merged.end(), v_fvs.begin(), v_fvs.end());
		return true;
	};
	tbdd::visit_nodes(ref(bdd_tref, false), collect);
	std::sort(merged.begin(), merged.end(),
		[](tref a, tref b) { return tree<node>::subtree_less(a, b); });
	merged.erase(std::unique(merged.begin(), merged.end(),
		[](tref a, tref b) { return tree<node>::subtree_equals(a, b); }),
		merged.end());
	return merged;
}

/** @internal @copydoc tau_term_bdd_handle::get_free_leaf_vars(tref) @endinternal */
template<NodeType node>
trefs tau_term_bdd_handle<node>::get_free_leaf_vars(tref bdd_tref) {
	if (!bdd_tref) return {};
	return collect_free_tau_vars(bdd_tref, true);
}

} // namespace idni::tau_lang

/** @internal @copydoc std::hash<idni::tau_lang::tau_bdd_node<T>>::operator()(auto&) const @endinternal */
template<typename T>
size_t std::hash<idni::tau_lang::tau_bdd_node<T>>::operator()(auto& n) const {
	size_t seed = 0;
	idni::hash_combine(seed, idni::hash_lcrs_tref<T>()(n.v), n.inv_v,
		n.inv_h, n.inv_l);
	return seed;
}

// `r.b` is a node of bintree<tau_bdd_node<T>>, and `hash_tref<T>` would
// read the hash field at bintree<T>'s offset — past the end of the BDD
// node, into whatever the store keeps next to it — so a ref's hash could
// change after a memo entry was made and the entry go unreachable.
/** @internal @copydoc std::hash<idni::tau_lang::tau_bdd_ref<T>>::operator()(auto&) const @endinternal */
template<typename T>
size_t std::hash<idni::tau_lang::tau_bdd_ref<T>>::operator()(auto& r) const {
	size_t seed = 0;
	idni::hash_combine(seed, idni::tau_lang::tau_term_bdd<T>::get(r.b).hash,
		r.inv);
	return seed;
}

/** @internal @copydoc std::hash<std::array<idni::tau_lang::tau_bdd_ref<T>, 2>>::operator()(auto&) const @endinternal */
template<typename T>
size_t std::hash<std::array<idni::tau_lang::tau_bdd_ref<T>, 2>>::operator()(auto& a) const {
	size_t seed = 0;
	idni::hash_combine(seed, a[0], a[1]);
	return seed;
}

/** @internal @copydoc std::hash<std::array<idni::tau_lang::tau_bdd_ref<T>, 3>>::operator()(auto&) const @endinternal */
template<typename T>
size_t std::hash<std::array<idni::tau_lang::tau_bdd_ref<T>, 3>>::operator()(auto& a) const {
	size_t seed = 0;
	idni::hash_combine(seed, a[0], a[1], a[2]);
	return seed;
}

// The handle's BDD node lives in bintree<tau_bdd_node<T>>, so its hash is
// read through tau_term_bdd<T>::get — not through hash_htree<T>, which
// dereferences the tref as a bintree<T> of the OUTER node type (this
// specialisation was uninstantiable before the interning map keyed on it).
/** @internal @copydoc std::hash<idni::tau_lang::term_handle<T>>::operator()(auto&) const @endinternal */
template<typename T>
size_t std::hash<idni::tau_lang::term_handle<T>>::operator()(auto& th) const {
	size_t seed = 0;
	idni::hash_combine(seed,
		idni::tau_lang::tau_term_bdd<T>::get(th.h->get()).hash, th.inv);
	return seed;
}

/** @internal @copydoc std::hash<std::pair<idni::tau_lang::term_handle<T>, size_t>>::operator()(auto&) const @endinternal */
template<typename T>
size_t std::hash<std::pair<idni::tau_lang::term_handle<T>, size_t>>::operator()(auto& k) const {
	size_t seed = 0;
	idni::hash_combine(seed, std::hash<idni::tau_lang::term_handle<T>>()(k.first),
		k.second);
	return seed;
}

#endif //TAU_TAU_BDD_TMPL_H
