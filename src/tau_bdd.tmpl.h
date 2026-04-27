// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef TAU_TAU_BDD_TMPL_H
#define TAU_TAU_BDD_TMPL_H

#include "tau_bdd.h"

namespace idni::tau_lang {

template <NodeType node>
bool tau_bdd_node<node>::operator==(const tau_bdd_node<node>& other) const {
	using tau = tree<node>;
	return this->inv_v == other.inv_v && this->inv_h == other.inv_h &&
		this->inv_l == other.inv_l && tau::subtree_equals(this->v, other.v);
}

template<NodeType node>
bool tau_bdd_node<node>::operator!=(const tau_bdd_node& other) const {
	return !(*this == other);
}

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

template<NodeType node>
bool tau_bdd_ref<node>::operator==(const tau_bdd_ref& other) const {
	using bdd = bintree<tau_bdd_node<node>>;
	const auto& b_this = bdd::get(this->b);
	const auto& b_other = bdd::get(other.b);
	return this->inv == other.inv && b_this.l == b_other.l &&
		b_this.r == b_other.r && b_this.value == b_other.value;
}

template<NodeType node>
bool tau_bdd_ref<node>::operator!=(const tau_bdd_ref& other) const {
	return !(*this == other);
}

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
void tau_term_bdd<node>::clear_caches() {
	and_memo.clear();
	ex_memo.clear();
	and_many_memo.clear();
	quant_memo.clear();
}
#endif

template<NodeType node>
void tau_term_bdd<node>::make_canonical(ref& x, ref& y) {
	if (y < x) std::swap(x,y);
}

template<NodeType node>
bool tau_term_bdd<node>::less_then(tref x, tref y, const order& o) {
	DBG(assert(o.contains(x) && o.contains(y)));
	const auto xiter = o.find(x), yiter = o.find(y);
	// If one variable is not found in o, there is nothing to compare
	if (xiter == o.end() || yiter == o.end()) return false;
	return xiter->second < yiter->second;
}

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

template<NodeType node>
bool tau_term_bdd<node>::leaf(ref l) {
	if (l == T || l == F) return true;
	const auto& tl = tau_term_bdd::get(l.b);
	return tl.l == nullptr && tl.r == nullptr;
}

/**
 * @brief Creates a BDD from a given Tau term
 * @param f The Tau term to build the BDD from
 * @param o The ordering of tau terms
 * @param o_bound The current highest ordering id
 * @return A reference to the BDD corresponding to f
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
			return bdd_or(
				bdd_and(a, bdd_not(b), o),
				bdd_and(bdd_not(a), b, o), o);
		}
		case tau::bf_neg: {
			const tau& tf = tau::get(f);
			return bdd_not(build_bdd(tf.first(), o));
		}
		case tau::BDD_ID: {
			// Get the BDD corresponding to the ID
			const auto& m = term_handle<node>::U;
			auto it = m.find(tau::get(tau::bf, f));
			if (it != m.end()) {
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

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::from_bit(tref v) {
	return add(v, T, F);
}

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
		tau::get(tau::bf, v), tau::get(tau::bf, y))));
	ref r = add(v, bdd_and(get_high(x), y), bdd_and(get_low(x), y));
#ifdef TAU_CACHE
	and_memo.emplace(std::array<ref, 2>{x, add(y)}, r);
#endif
	return r;
}

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and(ref x, ref y, const order& o) {
	// Check trivial cases
	if (x == F || y == F || x == bdd_not(y)) return F;
	if (x == T || x == y) return y;
	if (y == T) return x;
#ifdef TAU_CACHE
	make_canonical(x, y);
	if (auto it = and_memo.find({x,y}); it != and_memo.end())
		return it->second;
#endif
	// Check if x and y is a leaf
	if (leaf(x)) return bdd_and(y, get_var(x));
	if (leaf(y)) return bdd_and(x, get_var(y));
	// Compare v of x and y for further traversal
	tref xv = get_var(x);
	tref yv = get_var(y);
	ref r;
	if (tree<node>::subtree_equals(xv, yv)) {
		r = add(xv, bdd_and(get_high(x), get_high(y), o),
			bdd_and(get_low(x), get_low(y), o));
	} else if (less_then(xv, yv, o)) {
		// var of x is smaller than var of y
		r = add(xv, bdd_and(get_high(x), y, o), bdd_and(get_low(x), y, o));
	} else {
		r = add(yv, bdd_and(get_high(y), x, o), bdd_and(get_low(y), x, o));
	}
#ifdef TAU_CACHE
	and_memo.emplace(std::array<ref, 2>{x,y}, r);
#endif
	return r;
}

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_or(ref x, ref y, const order& o) {
	return bdd_not(bdd_and(bdd_not(x), bdd_not(y), o));
}

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_not(ref x) {
	return x.inv = !x.inv, x;
}

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_ex(ref x, trefs& v,
	const order& o) {
	// sort v, so the smallest variable is up front
	auto cmp = [&o](tref e1, tref e2){return less_then(e1,e2, o);};
	sortc(v, cmp);
#ifdef TAU_CACHE
	return bdd_ex(x, v, 0, o, ex_memo[v]);
#else
	return bdd_ex(x, v, 0, o);
#endif
}

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_all(ref x, trefs v,
	const order& o) {
	// We utilize existential quantification in order to share cache
	return bdd_not(bdd_ex(bdd_not(x), v, o));
}

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
	return bdd_quant(x, v_rev, 0, o, quant_memo[v]);
#else
	return bdd_quant(x, v_rev, 0, o);
#endif
}

#ifdef TAU_CACHE
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_ex(ref x, const trefs& v, size_t i,
	const order& o, auto& memo) {
	using tau = tree<node>;
	const tref var = get_var(x);
	if (leaf(x) || less_then(v.back(), var)) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	// while current variable is bigger, increase index
	while (less_then(v[i], var, o)) ++i;
	if (tau::subtree_equals(v[i], var))
		return bdd_ex(bdd_or(get_high(x), get_low(x), o), v, ++i, o);
	return memo.emplace(x,
		add(var, bdd_ex(get_high(x), v, i, o),
			bdd_ex(get_low(x), v, i, o))).first->second;
}

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_quant(ref x, const quants& v,
	size_t i, const order& o, auto& memo) {
	using tau = tree<node>;
	const tref var = get_var(x);
	// If we have passed the last variable in v, we are done
	if (leaf(x) || less_then(v.back().first, var, o)) return x;
	if (auto it = memo.find(x); it != memo.end()) return it->second;
	// while current variable is bigger, increase index
	while (less_then(v[i].first, var, o)) ++i;
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

#else
template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_ex(ref x, const trefs& v, size_t i,
	const order& o) {
	using tau = tree<node>;
	const tref var = get_var(x);
	if (leaf(x) || less_then(v.back(), var, o)) return x;
	// while current variable is bigger, increase index
	while (less_then(v[i], var, o)) ++i;
	if (tau::subtree_equals(v[i], var))
		return bdd_ex(bdd_or(get_high(x), get_low(x), o), v, ++i, o);
	return add(var, bdd_ex(get_high(x), v, i, o),
			bdd_ex(get_low(x), v, i, o));
}

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_quant(ref x, const quants& v,
	size_t i, const order& o) {
	using tau = tree<node>;
	const tref var = get_var(x);
	// If we have passed the last variable in v, we are done
	if (leaf(x) || less_then(v.back().first, var, o)) return x;
	// while current variable is bigger, increase index
	while (less_then(v[i].first, var, o)) ++i;
	if (tau::subtree_equals(v[i].first, var)) {
		// Eliminate the quantifier
		if (v[i].second == Quantifier::ex)
			return bdd_quant(bdd_or(get_high(x),
				get_low(x), o), v, ++i, o);
		// Otherwise Quantifier::all
		else return bdd_quant(bdd_and(get_high(x),
			get_low(x), o), v, ++i, o);
	} else return add(var, bdd_quant(get_high(x), v, i, o),
				bdd_quant(get_low(x), v, i, o));
}
#endif

template<NodeType node>
tref tau_term_bdd<node>::to_tau_term(ref x, size_t term_type) {
	using tau = tree<node>;

	if (x == T) return tau::_1(term_type);
	if (x == F) return tau::_0(term_type);

	tref v = tau::get(tau::bf, get_var(x));
	ref h = get_high(x);
	ref l = get_low(x);

	if (leaf(x)) return v;

	tref left = tau::build_bf_and(v, to_tau_term(h, term_type));
	tref right = tau::build_bf_and(tau::build_bf_neg(v),
		to_tau_term(l, term_type));
	return tau::build_bf_or(left, right);
}

template<NodeType node>
size_t tau_term_bdd<node>::bdd_and_many_iter(const refs& v,
	refs& h, refs& l, ref& res, tref& m, const order& o) {
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
		ref r = bdd_and_many(move(x), o);
		if (r == F) return res = F, 1;
		if (r != T) {
			if (!hasbc(h, r, am_cmp)) h.push_back(r), am_sort(h);
			if (!hasbc(l, r, am_cmp)) l.push_back(r), am_sort(l);
		}
	}
	return 0;
}

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_and_many(refs v, const order& o) {
#ifdef TAU_CACHE
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
		if (v1=v, am_simplify(v, and_many_memo), v.size()==1) return v[0];
	} while (v1 != v);
#endif

	if (v.empty()) return T;
	if (v.size() == 1) return v[0];

#ifdef TAU_CACHE
	auto it = and_many_memo.find(v);
	if (it != and_many_memo.end()) return it->second;
#endif

	if (v.size() == 2) {
#ifdef TAU_CACHE
		return and_many_memo.emplace(v, bdd_and(v[0], v[1], o)).first->second;
#endif
		return bdd_and(v[0], v[1], o);
	}

	ref res = F, h, l;
	tref m = nullptr;
	refs vh, vl;
	switch (bdd_and_many_iter(v, vh, vl, res, m, o)) {
		case 0: l = bdd_and_many(move(vl), o),
			h = bdd_and_many(move(vh), o);
			break;
		case 1: {
#ifdef TAU_CACHE
			return and_many_memo.emplace(v, res).first->second;
#endif
			return res;
		}
		case 2: h = bdd_and_many(move(vh), o), l = F; break;
		case 3: h = F, l = bdd_and_many(move(vl), o); break;
		default: { DBG(assert(false)); return ref(); }
	}
#ifdef TAU_CACHE
	return and_many_memo.emplace(v, add(m, h, l)).first->second;
#endif
	return add(m, h, l);
}

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::bdd_or_many(refs v, const order& o) {
	for (ref& el : v) el = bdd_not(el);
	return bdd_not(bdd_and_many(v, o));
}

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

template<NodeType node>
bool tau_term_bdd<node>::subset(const refs& small, const refs& big) {
	if ( big.size() < small.size() ||
		am_cmp(abs(big[big.size()-1]), abs(small[0])) ||
		am_cmp(abs(small[small.size()-1]), abs(big[0])))
		return false;
	for (const ref& t : small) if (!hasbc(big, t, am_cmp)) return false;
	return true;
}

template<NodeType node>
tau_term_bdd<node>::ref tau_term_bdd<node>::abs(ref x) {
	return x.inv = false, x;
}

template<NodeType node>
tau_term_bdd_handle<node>::universe_t& tau_term_bdd_handle<node>::U =
	bintree<node>::template create_cache<universe_t>();

template<NodeType node>
tau_term_bdd_handle<node>::tau_term_bdd_handle(ref x) {
	h = tbdd::geth(x.b);
	inv = x.inv;
}

template<NodeType node>
tau_term_bdd_handle<node> tau_term_bdd_handle<node>::build(tref term, const order& o) {
	return term_handle(tbdd::build_bdd(term, o));
}

template<NodeType node>
tref tau_term_bdd_handle<node>::convert_to_tau_node(term_handle handle, size_t term_type) {
	using tau = tree<node>;

	static size_t bdd_id = 0;
	tref tau_node = tau::get_typed(tau::bf, tau::get_typed(tau::BDD_ID,
		tau::get_num(bdd_id), term_type), term_type);
	// Increment id for unique node creation
	++bdd_id;
	// Save connection in U
	U.emplace(tau_node, handle);
	return tau_node;
}

template<NodeType node>
tref tau_term_bdd_handle<node>::convert_to_tau_node(tref term, const order& o) {
	return convert_to_tau_node(build(term, o), find_ba_type<node>(term));
}

template<NodeType node>
tref tau_term_bdd_handle<node>::to_tau_term(size_t term_type) const {
	return tbdd::to_tau_term(get(), term_type);
}

template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_and(term_handle other, const order& o) const {
	return term_handle(tbdd::bdd_and(get(), other.get(), o));
}

template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_or(term_handle other, const order& o) const {
	return term_handle(tbdd::bdd_or(get(), other.get(), o));
}

template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::bdd_not() const {
	return term_handle(h, !inv);
}

template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_and_many(const term_handles& bdds, const order& o) {
	std::vector<ref> refs;
	refs.reserve(bdds.size());
	for (const term_handle b : bdds) refs.emplace_back(b.get());
	return term_handle(tbdd::bdd_and_many(std::move(refs), o));
}

template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_or_many(const term_handles& bdds, const order& o) {
	std::vector<ref> refs;
	refs.reserve(bdds.size());
	for (const term_handle b : bdds) refs.emplace_back(b.get());
	return term_handle(tbdd::bdd_or_many(std::move(refs), o));
}

template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_ex(const trefs& v, const order& o) const {
	return term_handle(tbdd::bdd_ex(get(), v, o));
}

template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_all(const trefs& v, const order& o) const {
	return term_handle(tbdd::bdd_all(get(), v, o));
}

template<NodeType node>
tau_term_bdd_handle<node>::term_handle tau_term_bdd_handle<node>::
bdd_quant(const quants& q, const order& o) const {
	return term_handle(tbdd::bdd_quant(get(), q, o));
}

template<NodeType node>
tau_term_bdd_handle<node>::ref tau_term_bdd_handle<node>::get() const {
	return ref(h->get(), inv);
}

template<NodeType node>
bool tau_term_bdd_handle<node>::operator==(const tau_term_bdd_handle& other) const {
	return get() == other.get();
}

template<NodeType node>
bool tau_term_bdd_handle<node>::operator!=(const tau_term_bdd_handle& other) const {
	return !(*this == other);
}

}

template<typename T>
size_t std::hash<idni::tau_lang::tau_bdd_node<T>>::operator()(auto& n) const {
	size_t seed = 0;
	idni::hash_combine(seed, idni::hash_lcrs_tref<T>()(n.v), n.inv_v,
		n.inv_h, n.inv_l);
	return seed;
}

template<typename T>
size_t std::hash<idni::tau_lang::tau_bdd_ref<T>>::operator()(auto& r) const {
	size_t seed = 0;
	idni::hash_combine(seed, idni::hash_tref<T>()(r.b), r.inv);
	return seed;
}

template<typename T>
size_t std::hash<std::array<idni::tau_lang::tau_bdd_ref<T>, 2>>::operator()(auto& a) const {
	size_t seed = 0;
	idni::hash_combine(seed, a[0], a[1]);
	return seed;
}

template<typename T>
size_t std::hash<idni::tau_lang::term_handle<T>>::operator()(auto& th) const {
	size_t seed = 0;
	idni::hash_combine(seed, idni::hash_htree<T>()(th.h), th.inv);
	return seed;
}

#endif //TAU_TAU_BDD_TMPL_H
