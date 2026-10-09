// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef TAU_TAU_BDD_TMPL_H
#define TAU_TAU_BDD_TMPL_H

#include <algorithm>
#include <cctype>
#include <charconv>
#include <limits>

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
// The tables are caches keyed by BDD refs, valid under the one global order
// in every call; a tree collection clears them, which bounds their growth.
// Every operation reaches `and_memo`, so its definition hooks them in.
template<NodeType node>
tau_term_bdd<node>::cache_and_t& tau_term_bdd<node>::and_memo =
	[]() -> cache_and_t& {
		static cache_and_t memo;
		std::unique_lock lock(bintree<node>::mtx_);
		bintree<node>::gc_callbacks.push_back(
			[](const std::unordered_set<tref>&) { clear_caches(); });
		return memo;
	}();

template<NodeType node>
tau_term_bdd<node>::cache_ex_t tau_term_bdd<node>::ex_memo;

template<NodeType node>
tau_term_bdd<node>::cache_and_many_t tau_term_bdd<node>::and_many_memo;

template<NodeType node>
tau_term_bdd<node>::cache_and_many_index_t tau_term_bdd<node>::and_many_index;

template<NodeType node>
tau_term_bdd<node>::cache_ite_t tau_term_bdd<node>::ite_memo;

/** @internal @copydoc tau_term_bdd::clear_caches() @endinternal */
template<NodeType node>
void tau_term_bdd<node>::clear_caches() {
	and_memo.clear();
	ex_memo.clear();
	and_many_memo.clear();
	and_many_index.clear();
	ite_memo.clear();
}
#endif

/** @internal @copydoc tau_term_bdd::make_canonical(ref&, ref&) @endinternal */
template<NodeType node>
void tau_term_bdd<node>::make_canonical(ref& x, ref& y) {
	if (y < x) std::swap(x,y);
}

/** @internal @copydoc tau_term_bdd::sort_key_of(tref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::sort_key tau_term_bdd<node>::sort_key_of(tref v) {
#ifdef TAU_CACHE
	using keys_t = std::unordered_map<tref, sort_key>;
	static keys_t& keys = tree<node>::template create_cache<keys_t>();
	if (auto it = keys.find(v); it != keys.end()) return it->second;
#endif
	sort_key k;
	k.name = get_var_name_sid<node>(v);
	const std::string& name = dict(k.name);
	k.binder = !name.empty() && std::ranges::all_of(name,
		[](unsigned char c) { return std::isdigit(c) != 0; });
	// An id past the range only ties with another such id, which the
	// content order then breaks.
	if (k.binder && std::from_chars(name.data(), name.data() + name.size(),
		k.id).ec != std::errc{})
		k.id = std::numeric_limits<std::uint64_t>::max();
#ifdef TAU_CACHE
	keys.emplace(v, k);
#endif
	return k;
}

/** @internal @copydoc tau_term_bdd::less_then(tref, tref) @endinternal */
template<NodeType node>
bool tau_term_bdd<node>::less_then(tref x, tref y) {
	using tau = tree<node>;
	DBG(assert(tau::get(x).is(tau::variable) && tau::get(y).is(tau::variable));)
	if (x == y) return false;
	const sort_key a = sort_key_of(x), b = sort_key_of(y);
	if (a.binder != b.binder) return b.binder;
	if (a.binder) { if (a.id != b.id) return a.id > b.id; }
	else if (a.name != b.name) return dict(a.name) < dict(b.name);
	DBG(assert(tau::subtree_less(x, y) || tau::subtree_less(y, x)
		|| tau::subtree_equals(x, y));)
	return tau::subtree_less(x, y);
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
		DBG(assert(!tau::get(tau::trim2(leaf)).is(tau::variable));)
		const auto bn = bdd_node(tau::trim2(leaf));
		return ref(tau_term_bdd::get(bn, nullptr, nullptr), true);
	} else {
		// A variable is a decision variable, never a leaf.
		DBG(assert(!tl.is(tau::variable));)
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

/** @internal @copydoc tau_term_bdd::is_ordered(ref, std::unordered_set<tref>&) @endinternal */
template<NodeType node>
bool tau_term_bdd<node>::is_ordered(ref x, std::unordered_set<tref>& seen) {
	using tau = tree<node>;
	if (leaf(x)) return true;
	if (!seen.insert(x.b).second) return true;
	tref v = get_var(x);
	if (!tau::get(v).is(tau::variable)) return false;
	for (ref c : { get_high(x), get_low(x) }) {
		if (leaf(c)) continue;
		tref cv = get_var(c);
		if (!tau::get(cv).is(tau::variable) || !less_then(v, cv)
			|| !is_ordered(c, seen)) return false;
	}
	return true;
}

/** @internal @copydoc tau_term_bdd::is_ordered(ref) @endinternal */
template<NodeType node>
bool tau_term_bdd<node>::is_ordered(ref x) {
	std::unordered_set<tref> seen;
	return is_ordered(x, seen);
}

/** @internal @copydoc tau_term_bdd::build_bdd(tref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::build_bdd(tref f) {
	using tau = tree<node>;
	DBG(assert(tau::get(f).get_type() != tau::wff));
	switch (auto nt = tau::get(f).get_type()) {
		case tau::variable: return from_bit(f);
		case tau::bf: return build_bdd(tau::trim(f));
		case tau::bf_and: {
			refs bdds;
			for (tref c : get_cnf_bf_clauses<node>(f))
				bdds.emplace_back(build_bdd(c));
			return bdd_and_many(std::move(bdds));
		}
		case tau::bf_or: {
			refs bdds;
			for (tref d : get_dnf_bf_clauses<node>(f))
				bdds.emplace_back(build_bdd(d));
			return bdd_or_many(std::move(bdds));
		}
		case tau::bf_xor: {
			const tau& tf = tau::get(f);
			const ref a = build_bdd(tf.first());
			const ref b = build_bdd(tf.second());
			return bdd_xor(a, b);
		}
		case tau::bf_neg: {
			const tau& tf = tau::get(f);
			return bdd_not(build_bdd(tf.first()));
		}
		case tau::bf_fall:
		case tau::bf_fex: {
			// §1 the functional-quantifier CHAIN `Q_Y b`, collected
			// whole from a binder node down, outermost first.
			auto strip = [](tref n) {
				std::pair<quants, tref> r;
				for (;;) {
					const tau& tn = tau::get(n);
					r.first.emplace_back(
						tau::trim_right_sibling(tn.first()),
						tn.is(tau::bf_fall) ? all : ex);
					r.second = tau::trim_right_sibling(tn.second());
					const tau& tb = tau::get(r.second);
					if (!tb.child_is(tau::bf_fall)
						&& !tb.child_is(tau::bf_fex)) return r;
					n = tb.first();
				}
			};
			const auto [q0, b0] = strip(f);
			// The constructor puts it in content order, merges its
			// runs, drops a degenerate subscript and folds a closed
			// chain; what is no chain any more is built as it is.
			const tref c = build_functional_quantifiers(q0, b0);
			const tau& tc = tau::get(c);
			if (!tc.child_is(tau::bf_fall) && !tc.child_is(tau::bf_fex))
				return build_bdd(c);
			// THE SLIDE over the body's BDD, a chain nested in the
			// body slid already by that build.
			const auto [q, body] = strip(tc.first());
			trefs ys;
			ys.reserve(q.size());
			for (const auto& yk : q) ys.push_back(yk.first);
			std::sort(ys.begin(), ys.end(), tau::subtree_less);
			// The canonical prefix repeats no subscript, so a piece's
			// fold only drops the subscripts it does not hold.
			DBG(assert(std::adjacent_find(ys.begin(), ys.end(),
				tau::subtree_equals) == ys.end());)
			std::unordered_map<ref, ref> memo;
			std::unordered_map<ref, tref> spelled;
			return slide(build_bdd(body), q, ys, find_ba_type<node>(body),
				memo, spelled);
		}
		case tau::BDD_ID: {
			// Get the BDD corresponding to the ID
			const auto& m = term_handle<node>::U;
			auto it = m.find(term_handle<node>::key_of(f));
			if (it != m.end()) {
				DBG(assert(is_ordered(it->second.get()));)
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
	DBG(assert(tree<node>::get(v).is(tree<node>::variable));)
	return add(v, T, F);
}

/** @internal @copydoc tau_term_bdd::build_functional_quantifiers(const quants&, tref, const ref*) @endinternal */
template<NodeType node>
tref tau_term_bdd<node>::build_functional_quantifiers(const quants& q,
	tref body, const ref* body_bdd)
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
			body_bdd = nullptr;
		}
	}
	// The chain, outermost first.
	quants prefix;
	trefs subscripts;
	for (auto& [k, ys] : segs) {
		std::sort(ys.begin(), ys.end(), tau::subtree_less);
		for (tref y : ys) prefix.emplace_back(y, k);
		subscripts.insert(subscripts.end(), ys.begin(), ys.end());
	}
	std::sort(subscripts.begin(), subscripts.end(), tau::subtree_less);
	auto is_subscript = [&subscripts](tref v) {
		return std::binary_search(subscripts.begin(), subscripts.end(), v,
			tau::subtree_less);
	};
	// 4. FOLD: a chain binding every free variable of its body, plain or
	//    BDD-backed, is a CONSTANT -- once no subscript is left in a leaf
	//    either, the quantified BDD holds no variable at all, so its term
	//    is the chain's value (`_1`/`_0` for a terminal, the leaf itself
	//    for a variable-free leaf). A subscript hidden inside a LEAF (§1's
	//    leaf hazard: a reference argument, a foreign-typed subterm) is
	//    reached by no quantification, and that chain stands.
	if (std::ranges::all_of(get_free_vars<node>(body), is_subscript)) {
		const ref b = body_bdd ? *body_bdd : build_bdd(body);
		if (std::ranges::none_of(
			term_handle<node>::get_free_leaf_vars(b.b), is_subscript))
			return to_tau_term(bdd_quant(b, prefix),
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

/** @internal @copydoc tau_term_bdd::bdd_and_leaves(std::span<const ref>) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and_leaves(
	std::span<const ref> leaves)
{
	using tau = tree<node>;
	// The typed BA constant a member spells as `{c}` or `{c}′`, else null.
	auto constant_of = [](tref t) -> const tau* {
		const tau& u = tau::get(t);
		const tau& c = u.child_is(tau::bf_neg)
			? tau::get(tau::trim2(t))[0] : u[0];
		return c.is_ba_constant() && c.get_ba_type() > 0 ? &c : nullptr;
	};
	// Leaves are compared by identity and a product is reached in any
	// grouping and order, so it is spelled from its members in content
	// order.
	trefs m;
	for (ref l : leaves) {
		if (l == T) continue;
		if (l == F) return F;
		get_leaves<node>(tau::get(tau::bf, get_var(l)), tau::bf_and, m);
	}
	if (m.empty()) return T;
	std::sort(m.begin(), m.end(), tau::subtree_less);
	m.erase(std::unique(m.begin(), m.end(), tau::subtree_equals), m.end());
	for (tref t : m)
		if (tau::get(t).child_is(tau::bf_neg) && std::binary_search(
			m.begin(), m.end(), tau::trim2(t), tau::subtree_less))
			return F;
	// For the same reason its BA constants are evaluated, by the operations
	// `cte_and` and `cte_neg` use: one constant member, never inverted.
	std::optional<typename tau::constant> k;
	size_t type = 0, n = 0;
	bool inverted = false;
	for (tref t : m)
		if (const tau* c = constant_of(t)) {
			const bool inv = tau::get(t).child_is(tau::bf_neg);
			auto v = inv ? ~c->get_ba_constant() : c->get_ba_constant();
			// Emplaced: a BA constant need not be assignable.
			if (n++ == 0) k.emplace(std::move(v)), type = c->get_ba_type();
			else k.emplace(*k & v);
			inverted = inverted || inv;
		}
	if (n > 1 || inverted) {
		tref c = build_bf_ba_constant<node>(*k, type);
		if (tau::get(c).equals_0()) return F;
		std::erase_if(m, [&constant_of](tref t) {
			return constant_of(t) != nullptr; });
		if (!tau::get(c).equals_1()) m.insert(std::upper_bound(
			m.begin(), m.end(), c, tau::subtree_less), c);
		if (m.empty()) return T;
	}
	tref p = m[0];
	for (size_t i = 1; i < m.size(); ++i) p = tau::build_bf_and(p, m[i]);
	ref r = add(tau::trim(p));
	DBG(trefs rm; get_leaves<node>(get_var_term(r), tau::bf_and, rm);)
	DBG(assert(std::ranges::count_if(rm, [&constant_of](tref t) {
		return constant_of(t) != nullptr; }) <= 1);)
	DBG(assert(std::ranges::none_of(rm, [&constant_of](tref t) {
		return constant_of(t) && tau::get(t).child_is(tau::bf_neg); }));)
	return r;
}

/** @internal @copydoc tau_term_bdd::bdd_and(ref, tref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and(ref x, tref y) {
	if (x == T) return add(y);
	if (x == F) return F;
	// No memo key has a leaf first, so a leaf skips the lookup.
	if (leaf(x)) return bdd_and_leaves(std::array{x, add(y)});
#ifdef TAU_CACHE
	if (auto it = and_memo.find({x, add(y)}); it != and_memo.end())
		return it->second;
#endif
	tref v = get_var(x);
	ref r = add(v, bdd_and(get_high(x), y), bdd_and(get_low(x), y));
#ifdef TAU_CACHE
	and_memo.emplace(std::array<ref, 2>{x, add(y)}, r);
#endif
	return r;
}

/** @internal @copydoc tau_term_bdd::bdd_and(ref, ref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and(ref x, ref y) {
	// Fresh per-call memo when there is no persistent (TAU_CACHE) one to
	// thread instead -- see the memoised worker below: the recursion is
	// always memoized within this call, so shared sub-BDDs reached via
	// different paths are computed once, not once per path.
#ifdef TAU_CACHE
	return bdd_and(x, y, and_memo);
#else
	std::unordered_map<std::array<ref, 2>, ref> local_memo;
	return bdd_and(x, y, local_memo);
#endif
}

/** @internal @copydoc tau_term_bdd::bdd_and(ref, ref, std::unordered_map<std::array<ref,2>,ref>&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and(ref x, ref y,
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
		r = add(xv, bdd_and(get_high(x), get_high(y), memo),
			bdd_and(get_low(x), get_low(y), memo));
	} else if (less_then(xv, yv)) {
		// var of x is smaller than var of y
		r = add(xv, bdd_and(get_high(x), y, memo),
			bdd_and(get_low(x), y, memo));
	} else {
		r = add(yv, bdd_and(get_high(y), x, memo),
			bdd_and(get_low(y), x, memo));
	}
	memo.emplace(std::array<ref, 2>{x, y}, r);
	return r;
}

/** @internal @copydoc tau_term_bdd::bdd_or(ref, ref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_or(ref x, ref y) {
	return bdd_not(bdd_and(bdd_not(x), bdd_not(y)));
}

/** @internal @copydoc tau_term_bdd::bdd_not(ref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_not(ref x) {
	return x.inv = !x.inv, x;
}

/** @internal @copydoc tau_term_bdd::bdd_xor(ref, ref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_xor(ref x, ref y) {
	return bdd_or(bdd_and(x, bdd_not(y)), bdd_and(bdd_not(x), y));
}

/** @internal @copydoc tau_term_bdd::bdd_ite(ref, ref, ref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_ite(ref f, ref g, ref h) {
	// See bdd_and's public entry: always memoize the recursion within
	// this call, via the static ite_memo (TAU_CACHE, cross-call too) or a
	// fresh local map otherwise.
#ifdef TAU_CACHE
	return bdd_ite(f, g, h, ite_memo);
#else
	std::unordered_map<std::array<ref, 3>, ref> local_memo;
	return bdd_ite(f, g, h, local_memo);
#endif
}

/** @internal @copydoc tau_term_bdd::bdd_ite(ref, ref, ref, std::unordered_map<std::array<ref,3>,ref>&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_ite(ref f, ref g, ref h,
	std::unordered_map<std::array<ref, 3>, ref>& memo) {
	using tau = tree<node>;
	// Rule 1: normalize complement on f
	if (f.inv) { f.inv = false; std::swap(g, h); }
	// Terminal reductions (f.inv == false guaranteed)
	if (f == T) return g;
	if (f == F) return h;
	if (g == h) return g;
	if (g == T && h == F) return f;
	if (g == F && h == T) return bdd_not(f);
	if (g == T) return bdd_or(f, h);
	if (h == F) return bdd_and(f, g);
	if (g == F) return bdd_and(bdd_not(f), h);
	if (h == T) return bdd_or(bdd_not(f), g);
	if (g == f) return bdd_or(f, h);
	if (h == f) return bdd_and(f, g);
	if (g == bdd_not(f)) return bdd_and(bdd_not(f), h);
	if (h == bdd_not(f)) return bdd_or(bdd_not(f), g);
	// All-leaf fallback: delegate to composed form
	if (leaf(f) && leaf(g) && leaf(h))
		return bdd_or(bdd_and(f, g), bdd_and(bdd_not(f), h));
	// Rule 2: normalize common complement on g and h
	bool out_inv = false;
	if (g.inv && h.inv) { g.inv = false; h.inv = false; out_inv = true; }
	const std::array<ref, 3> key = {f, g, h};
	if (auto it = memo.find(key); it != memo.end())
		return out_inv ? bdd_not(it->second) : it->second;
	// Top variable: the least decision variable among the non-leaf roots;
	// a leaf holds no decision variable, so splitting on it never shrinks
	// anything.
	tref top = nullptr;
	auto upd = [&](ref x) {
		if (leaf(x)) return;
		tref v = get_var(x);
		if (!top || less_then(v, top)) top = v;
	};
	upd(f); upd(g); upd(h);
	// Co-factor inline — no temporary BDD allocation
	auto cof = [&](ref x, bool hi) -> ref {
		if (leaf(x) || !tau::subtree_equals(get_var(x), top)) return x;
		return hi ? get_high(x) : get_low(x);
	};
	ref r = add(top,
		bdd_ite(cof(f, true),  cof(g, true),  cof(h, true),  memo),
		bdd_ite(cof(f, false), cof(g, false), cof(h, false), memo));
	memo.emplace(key, r);
	return out_inv ? bdd_not(r) : r;
}

/** @internal @copydoc tau_term_bdd::bdd_compose(ref, tref, ref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_compose(ref x, tref xi, ref g) {
	// A terminal g IS a cofactor: child selection, without routing every
	// node through bdd_ite.
	if (g == T || g == F) return bdd_cofactor(x, xi, g == T);
	std::unordered_map<ref, ref> memo;
	return bdd_compose_impl(x, xi, g, memo);
}

/** @internal @copydoc tau_term_bdd::bdd_compose_impl(ref, tref, ref, std::unordered_map<ref, ref>&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_compose_impl(ref x, tref xi,
	ref g, std::unordered_map<ref, ref>& memo) {
	using tau = tree<node>;
	if (leaf(x)) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	tref var = get_var(x);
	if (less_then(xi, var)) return x;
	ref r;
	if (tau::subtree_equals(xi, var))
		r = bdd_ite(g,
			bdd_compose_impl(get_high(x), xi, g, memo),
			bdd_compose_impl(get_low(x), xi, g, memo));
	else
		r = bdd_ite(from_bit(var),
			bdd_compose_impl(get_high(x), xi, g, memo),
			bdd_compose_impl(get_low(x), xi, g, memo));
	return memo.emplace(x, r).first->second;
}

/** @internal @copydoc tau_term_bdd::bdd_compose(ref, subs_t) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_compose(ref x, subs_t subs) {
	auto cmp = [](const auto& a, const auto& b) {
		return less_then(a.first, b.first); };
	sortc(subs, cmp);
	std::unordered_map<ref, ref> memo;
	return bdd_compose_impl(x, subs, 0, memo);
}

/** @internal @copydoc tau_term_bdd::bdd_compose_impl(ref, const subs_t&, size_t, std::unordered_map<ref, ref>&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_compose_impl(ref x,
	const subs_t& subs, size_t i, std::unordered_map<ref, ref>& memo) {
	using tau = tree<node>;
	if (leaf(x) || i >= subs.size()) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	tref var = get_var(x);
	while (i < subs.size() && less_then(subs[i].first, var)) ++i;
	if (i >= subs.size()) return x;
	ref r;
	if (tau::subtree_equals(subs[i].first, var))
		r = bdd_ite(subs[i].second,
			bdd_compose_impl(get_high(x), subs, i + 1, memo),
			bdd_compose_impl(get_low(x), subs, i + 1, memo));
	else
		r = bdd_ite(from_bit(var),
			bdd_compose_impl(get_high(x), subs, i, memo),
			bdd_compose_impl(get_low(x), subs, i, memo));
	return memo.emplace(x, r).first->second;
}

/** @internal @copydoc tau_term_bdd::bdd_cofactor(ref, tref, bool) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_cofactor(ref x, tref xi,
	bool bit) {
	std::unordered_map<ref, ref> memo;
	return bdd_cofactor_impl(x, xi, bit, memo);
}

/**
 * @internal
 * @brief Memoised worker for bdd_cofactor: a leaf is itself; a node whose
 * variable comes after @p xi in the order is itself (on an ordered path
 * @p xi can only sit above it); the node of @p xi is its child by @p bit;
 * any other node is rebuilt through `add`, which keeps canonicity because
 * both children stay ordered below it.
 * @endinternal
 */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_cofactor_impl(ref x, tref xi,
	bool bit, std::unordered_map<ref, ref>& memo) {
	using tau = tree<node>;
	if (leaf(x)) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	tref var = get_var(x);
	if (less_then(xi, var)) return x;
	ref r;
	if (tau::subtree_equals(xi, var)) r = bit ? get_high(x) : get_low(x);
	else r = add(var, bdd_cofactor_impl(get_high(x), xi, bit, memo),
		bdd_cofactor_impl(get_low(x), xi, bit, memo));
	return memo.emplace(x, r).first->second;
}

/**
 * @internal
 * @brief A node whose variable is no subscript stays, rebuilt through `add`,
 * which keeps canonicity because every piece comes back a leaf or a
 * terminal; the first subscript node or leaf on a path is a PIECE. A closed
 * piece folds on its BDD; only one that stays a chain is spelled.
 * @endinternal
 */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::slide(ref x, const quants& q,
	const trefs& ys, size_t type, std::unordered_map<ref, ref>& memo,
	std::unordered_map<ref, tref>& spelled)
{
	using tau = tree<node>;
	if (x == T || x == F) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	auto is_subscript = [&ys](tref v) {
		return std::binary_search(ys.begin(), ys.end(), v,
			tau::subtree_less);
	};
	ref r;
	if (!leaf(x) && !is_subscript(get_var(x)))
		r = add(get_var(x), slide(get_high(x), q, ys, type, memo, spelled),
			slide(get_low(x), q, ys, type, memo, spelled));
	else {
		// Canonical ids put the subscripts at the bottom, so a piece of a
		// chain over binder variables holds no other decision variable.
		DBG(auto only_ys = [&is_subscript](ref n, bool is_leaf) {
			return is_leaf || is_subscript(get_var(n)); };)
		DBG(assert(!std::ranges::all_of(ys, [](tref y) {
			return sort_key_of(y).binder; }) || visit_nodes(x, only_ys));)
		const trefs& fv = term_handle<node>::get_free_tau_vars(x.b);
		auto bare_leaf = [](ref n, bool is_leaf) {
			return !is_leaf || get_free_vars<node>(get_var_term(n)).empty();
		};
		if (std::ranges::none_of(fv, is_subscript)) r = x;
		else if (std::includes(ys.begin(), ys.end(), fv.begin(), fv.end(),
			tau::subtree_less) && visit_nodes(x, bare_leaf))
		{
			// CLOSED, no subscript hidden in a leaf: the constructor's
			// fold, taken on the BDD over the subscripts the piece holds.
			quants held;
			for (const auto& yk : q)
				if (std::binary_search(fv.begin(), fv.end(), yk.first,
					tau::subtree_less)) held.push_back(yk);
			r = bdd_quant(x, held);
		} else {
			// The chain stands: ONE LEAF over the piece's spelling.
			const tref c = build_functional_quantifiers(q,
				to_tau_term(x, type, spelled), &x);
			DBG(assert(tau::get(c).child_is(tau::bf_fall)
				|| tau::get(c).child_is(tau::bf_fex));)
			r = add(tau::trim(c));
		}
	}
	return memo.emplace(x, r).first->second;
}

/** @internal @copydoc tau_term_bdd::map_leaves(ref, Fn&, std::unordered_map<ref, ref>&) @endinternal */
template<NodeType node>
template<typename Fn>
tau_term_bdd<node>::ref tau_term_bdd<node>::map_leaves(ref x, Fn& fn,
	std::unordered_map<ref, ref>& memo) {
	if (x == T || x == F) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	ref r;
	if (leaf(x)) {
		tref in  = get_var_term(x);
		tref out = fn(in);
		r = out == in ? x : build_bdd(out);
	} else {
		ref h = map_leaves(get_high(x), fn, memo);
		ref l = map_leaves(get_low(x), fn, memo);
		tref var = get_var(x);
		auto below = [&](ref c) {
			return leaf(c) || less_then(var, get_var(c));
		};
		if (h == get_high(x) && l == get_low(x)) r = x;
		else if (below(h) && below(l)) r = add(var, h, l);
		else r = bdd_ite(from_bit(var), h, l);
	}
	return memo.emplace(x, r).first->second;
}

/** @internal @copydoc tau_term_bdd::map_leaves(ref, Fn&) @endinternal */
template<NodeType node>
template<typename Fn>
tau_term_bdd<node>::ref tau_term_bdd<node>::map_leaves(ref x, Fn& fn) {
	std::unordered_map<ref, ref> memo;
	return map_leaves(x, fn, memo);
}

/** @internal @copydoc tau_term_bdd::bdd_ex(ref, trefs&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_ex(ref x, trefs& v) {
	// sort v, so the smallest variable is up front
	auto cmp = [](tref e1, tref e2) { return less_then(e1, e2); };
	sortc(v, cmp);
	// Always memoize the recursion within this call (fresh local map), on
	// top of which TAU_CACHE's static ex_memo, keyed by v, is the
	// optional cross-call persistence layer -- see bdd_and's public entry
	// for the same pattern.
#ifdef TAU_CACHE
	return bdd_ex(x, v, 0, ex_memo[v]);
#else
	std::unordered_map<ref, ref> local_memo;
	return bdd_ex(x, v, 0, local_memo);
#endif
}

/** @internal @copydoc tau_term_bdd::bdd_all(ref, trefs) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_all(ref x, trefs v) {
	// We utilize existential quantification in order to share cache
	return bdd_not(bdd_ex(bdd_not(x), v));
}

/** @internal @copydoc tau_term_bdd::bdd_quant(ref, const quants&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_quant(ref x, const quants& v) {
	// A run of one kind commutes, so it is one set quantification; the
	// runs themselves are taken innermost first, which holds whatever the
	// order puts where.
	for (size_t end = v.size(); end > 0 && !leaf(x);) {
		size_t begin = end - 1;
		while (begin > 0 && v[begin - 1].second == v[end - 1].second)
			--begin;
		trefs run;
		run.reserve(end - begin);
		for (size_t i = begin; i < end; ++i) run.push_back(v[i].first);
		x = v[end - 1].second == ex ? bdd_ex(x, run)
			: bdd_all(x, std::move(run));
		end = begin;
	}
	return x;
}

// Memoised worker, used unconditionally (not just under TAU_CACHE): the
// caller (bdd_ex's public entry above) always supplies a memo, either a
// fresh local one or the persistent static ex_memo.
/** @internal @copydoc tau_term_bdd::bdd_ex(ref, const trefs&, size_t, auto&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_ex(ref x, const trefs& v,
	size_t i, auto& memo) {
	using tau = tree<node>;
	if (leaf(x) || i >= v.size()) return x;
	const tref var = get_var(x);
	if (less_then(v.back(), var)) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	// while current variable is bigger, increase index
	while (i < v.size() && less_then(v[i], var)) ++i;
	if (i >= v.size()) return x;
	if (tau::subtree_equals(v[i], var))
		return bdd_ex(bdd_or(get_high(x), get_low(x)), v, ++i, memo);
	return memo.emplace(x,
		add(var, bdd_ex(get_high(x), v, i, memo),
			bdd_ex(get_low(x), v, i, memo))).first->second;
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

/** @internal @copydoc tau_term_bdd::bdd_and_many_iter(const refs&, refs&, refs&, ref&, tref&, std::unordered_map<refs,ref>&) @endinternal */
template<NodeType node>
size_t tau_term_bdd<node>::bdd_and_many_iter(const refs& v,
	refs& h, refs& l, ref& res, tref& m,
	std::unordered_map<refs, ref>& memo) {
	using tau = tree<node>;
	size_t i;
	bool flag = false;
	// Choose first BDD that is not a leaf and assign variable to m
	for (i = 0; i != v.size(); ++i)
		if (!leaf(v[i])) {
			m = get_var(v[i]); break;
		} else if (v[i] == F) return res = F, 1;
	if (i == v.size()) return res = bdd_and_leaves(v), 1;
	// m becomes smallest variable
	for (; i != v.size(); ++i)
		if (!leaf(v[i])) {
			if (less_then(get_var(v[i]), m)) m = get_var(v[i]);
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
		ref r = bdd_and_many(std::move(x), memo);
		if (r == F) return res = F, 1;
		if (r != T) {
			if (!hasbc(h, r, am_cmp)) h.push_back(r), am_sort(h);
			if (!hasbc(l, r, am_cmp)) l.push_back(r), am_sort(l);
		}
	}
	return 0;
}

/** @internal @copydoc tau_term_bdd::bdd_and_many(refs) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and_many(refs v) {
	// See bdd_and's public entry: always memoize the recursion within
	// this call, via the static and_many_memo (TAU_CACHE, cross-call too)
	// or a fresh local map otherwise.
#ifdef TAU_CACHE
	return bdd_and_many(std::move(v), and_many_memo);
#else
	std::unordered_map<refs, ref> local_memo;
	return bdd_and_many(std::move(v), local_memo);
#endif
}

/** @internal @copydoc tau_term_bdd::bdd_and_many(refs, std::unordered_map<refs,ref>&) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and_many(refs v,
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
		if (v1=v, am_simplify(v), v.size()==1) return v[0];
	} while (v1 != v);
#endif

	if (v.empty()) return T;
	if (v.size() == 1) return v[0];

	if (auto it = memo.find(v); it != memo.end()) return it->second;

	if (v.size() == 2) {
		return am_memoize(memo, v, bdd_and(v[0], v[1]));
	}

	ref res = F, h, l;
	tref m = nullptr;
	refs vh, vl;
	switch (bdd_and_many_iter(v, vh, vl, res, m, memo)) {
		case 0: l = bdd_and_many(std::move(vl), memo),
			h = bdd_and_many(std::move(vh), memo);
			break;
		case 1: return am_memoize(memo, v, res);
		case 2: h = bdd_and_many(std::move(vh), memo), l = F; break;
		case 3: h = F, l = bdd_and_many(std::move(vl), memo); break;
		default: { DBG(assert(false)); return ref(); }
	}
	return am_memoize(memo, v, add(m, h, l));
}

/** @internal @copydoc tau_term_bdd::bdd_or_many(refs) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_or_many(refs v) {
	for (ref& el : v) el = bdd_not(el);
	return bdd_not(bdd_and_many(std::move(v)));
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
		else ++n;
	// Each maximal run of adjacent leaves becomes one leaf, at the place
	// of the run's first.
	size_t w = 0;
	for (size_t i = 0, j; i < b.size(); i = j) {
		j = i + 1;
		if (leaf(b[i])) while (j < b.size() && leaf(b[j])) ++j;
		b[w++] = j - i == 1 ? b[i]
			: bdd_and_leaves(std::span(b).subspan(i, j - i));
	}
	b.resize(w);
}

#ifdef TAU_CACHE
/** @internal @copydoc tau_term_bdd::am_simplify(refs&) @endinternal */
template<NodeType node>
bool tau_term_bdd<node>::am_simplify(refs& v) {
	am_sort(v);
	// Memo keys are am_sorted, so a subset of v is indexed under a member of v.
	for (const ref& first : v)
		if (auto it = and_many_index.find(first); it != and_many_index.end())
			for (const auto* x : it->second)
				if (subset(x->first, v)) {
					if (x->second == F) return v={F}, true;
					for (size_t n = 0; n < v.size();)
						if (!hasbc(x->first, v[n], am_cmp)) ++n;
						else v.erase(v.begin() + n);
					if (!hasbc(v, x->second, am_cmp)) v.push_back(x->second);
					return true;
				}
	return false;
}
#endif

/** @internal @copydoc tau_term_bdd::am_memoize(std::unordered_map<refs, ref>&, const refs&, ref) @endinternal */
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::am_memoize(
	std::unordered_map<refs, ref>& memo, const refs& v, ref r) {
	[[maybe_unused]] const auto [it, fresh] = memo.emplace(v, r);
#ifdef TAU_CACHE
	if (fresh) and_many_index[v[0]].push_back(&*it);
#endif
	return it->second;
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

// Keyed by an id rather than a tref, so `I`'s sweep drops its entries.
template<NodeType node>
tau_term_bdd_handle<node>::universe_t& tau_term_bdd_handle<node>::U =
	[]() -> universe_t& { static universe_t u; return u; }();

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
					if (!kept.contains(it->second.id_node)) {
						U.erase(it->second.id);
						it = cache.erase(it);
					} else ++it;
			});
		return cache;
	}();

/** @internal @copydoc tau_term_bdd_handle::tau_term_bdd_handle(ref) @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::tau_term_bdd_handle(ref x) {
	h = tbdd::geth(x.b);
	inv = x.inv;
}

/** @internal @copydoc tau_term_bdd_handle::build(tref) @endinternal */
template<NodeType node>
tau_term_bdd_handle<node> tau_term_bdd_handle<node>::build(tref term) {
	return term_handle(tbdd::build_bdd(term));
}

/** @internal @copydoc tau_term_bdd_handle::convert_to_tau_node(term_handle, size_t) @endinternal */
template<NodeType node>
tref tau_term_bdd_handle<node>::convert_to_tau_node(term_handle handle, size_t term_type) {
	using tau = tree<node>;

	// Interned, so a round trip through to_tau_term and build gives back
	// the node it started from and hash-consed identity holds.
	const intern_key_t key{handle, term_type};
	tref id_node;
	if (auto it = I.find(key); it != I.end()) id_node = it->second.id_node;
	else {
		static size_t next_id = 0;
		const size_t id = next_id++;
		// The node's hash reads its BDD through `U`.
		U.emplace(id, handle);
		id_node = tau::get(node(tau::BDD_ID, id, true, term_type));
		I.emplace(key, interned_t{ id_node, id });
	}
	return tau::get_typed(tau::bf, id_node, term_type);
}

/** @internal @copydoc tau_term_bdd_handle::convert_to_tau_node(tref) @endinternal */
template<NodeType node>
tref tau_term_bdd_handle<node>::convert_to_tau_node(tref term) {
	return convert_to_tau_node(build(term), find_ba_type<node>(term));
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
size_t tau_term_bdd_handle<node>::key_of(tref tau_node) {
	using tau = tree<node>;
	const tau& t = tau::get(tau_node);
	if (t.is(tau::BDD_ID)) return t.data();
	DBG(assert(t.is(tau::bf) && t.child_is(tau::BDD_ID)));
	return tau::get(t.first()).data();
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

/** @internal @copydoc tau_term_bdd_handle::bdd_and(term_handle) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_and(term_handle other) const {
	return term_handle(tbdd::bdd_and(get(), other.get()));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_or(term_handle) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_or(term_handle other) const {
	return term_handle(tbdd::bdd_or(get(), other.get()));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_not() const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::bdd_not() const {
	return term_handle(h, !inv);
}

/** @internal @copydoc tau_term_bdd_handle::bdd_and_many(const term_handles&) @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_and_many(const term_handles& bdds) {
	std::vector<ref> refs;
	refs.reserve(bdds.size());
	for (const term_handle b : bdds) refs.emplace_back(b.get());
	return term_handle(tbdd::bdd_and_many(std::move(refs)));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_or_many(const term_handles&) @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_or_many(const term_handles& bdds) {
	std::vector<ref> refs;
	refs.reserve(bdds.size());
	for (const term_handle b : bdds) refs.emplace_back(b.get());
	return term_handle(tbdd::bdd_or_many(std::move(refs)));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_ex(trefs&) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_ex(trefs& v) const {
	return term_handle(tbdd::bdd_ex(get(), v));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_all(const trefs&) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_all(const trefs& v) const {
	return term_handle(tbdd::bdd_all(get(), v));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_quant(const quants&) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_quant(const quants& q) const {
	return term_handle(tbdd::bdd_quant(get(), q));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_ite(term_handle, term_handle) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_ite(term_handle g, term_handle h) const {
	return term_handle(tbdd::bdd_ite(get(), g.get(), h.get()));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_compose(tref, term_handle) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_compose(tref xi, term_handle g) const {
	return term_handle(tbdd::bdd_compose(get(), xi, g.get()));
}

/** @internal @copydoc tau_term_bdd_handle::bdd_compose(const std::vector<std::pair<tref, term_handle>>&) const @endinternal */
template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_compose(const std::vector<std::pair<tref, term_handle>>& subs) const {
	typename tbdd::subs_t raw;
	raw.reserve(subs.size());
	for (const auto& [v, th] : subs) raw.emplace_back(v, th.get());
	return term_handle(tbdd::bdd_compose(get(), std::move(raw)));
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
	// appended to one vector that is sorted at the end, which keeps the
	// allocations to that vector's growth rather than one per variable.
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
