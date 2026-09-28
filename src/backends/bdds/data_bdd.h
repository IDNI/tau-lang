// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// data_bdd.h - A small BDD with a node budget, and bit-level circuits on it

#ifndef __IDNI__TAU__BACKENDS__BDDS__DATA_BDD_H__
#define __IDNI__TAU__BACKENDS__BDDS__DATA_BDD_H__

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace idni::tau_lang {

// A reduced ordered BDD without complement edges; node 0 is false and node 1
// true. A table grown past `max_nodes`, or a memo of the operations grown
// past `max_memo` (0 for no bound), sets `full`, and every result from then
// on is meaningless: the caller must check `full` and give up. Once full,
// the operations return at once.
struct data_bdd {
	using id = uint32_t;
	static constexpr id F = 0, T = 1;
	static constexpr uint32_t leaf = UINT32_MAX;
	struct nd { uint32_t var; id lo, hi; };
	struct key_hash {
		size_t operator()(const std::array<uint32_t, 3>& k) const {
			uint64_t h = k[0];
			h = h * 0x9E3779B97F4A7C15ull + k[1];
			h = h * 0x9E3779B97F4A7C15ull + k[2];
			return (size_t)(h ^ (h >> 29));
		}
	};
	std::vector<nd> nodes{ { leaf, F, F }, { leaf, T, T } };
	std::unordered_map<std::array<uint32_t, 3>, id, key_hash> unique, memo;
	size_t max_nodes, max_memo;
	bool full = false;

	explicit data_bdd(size_t cap, size_t memo_cap = 0)
		: max_nodes(cap), max_memo(memo_cap) {}

	void remember(const std::array<uint32_t, 3>& k, id r) {
		memo.emplace(k, r);
		if (max_memo && memo.size() >= max_memo) full = true;
	}

	id mk(uint32_t v, id lo, id hi) {
		if (lo == hi) return lo;
		std::array<uint32_t, 3> k{ v, lo, hi };
		if (auto it = unique.find(k); it != unique.end()) return it->second;
		if (nodes.size() >= max_nodes) { full = true; return F; }
		nodes.push_back({ v, lo, hi });
		const id n = (id)(nodes.size() - 1);
		unique.emplace(k, n);
		return n;
	}
	id var(uint32_t v, bool pos = true) {
		return pos ? mk(v, F, T) : mk(v, T, F);
	}
	// op 0 conjunction, 1 disjunction, 3 exclusive or
	id apply(uint32_t op, id a, id b) {
		if (op == 0) {
			if (a == F || b == F) return F;
			if (a == T) return b;
			if (b == T || a == b) return a;
		} else if (op == 1) {
			if (a == T || b == T) return T;
			if (a == F) return b;
			if (b == F || a == b) return a;
		} else {
			if (a == b) return F;
			if (a == F) return b;
			if (b == F) return a;
			if (a == T) return neg(b);
			if (b == T) return neg(a);
		}
		if (full) return F;
		if (a > b) std::swap(a, b);
		std::array<uint32_t, 3> k{ op, a, b };
		if (auto it = memo.find(k); it != memo.end()) return it->second;
		const nd x = nodes[a], y = nodes[b];
		const uint32_t v = std::min(x.var, y.var);
		const id lo = apply(op, x.var == v ? x.lo : a, y.var == v ? y.lo : b);
		const id hi = apply(op, x.var == v ? x.hi : a, y.var == v ? y.hi : b);
		const id r = mk(v, lo, hi);
		remember(k, r);
		return r;
	}
	id conj(id a, id b) { return apply(0, a, b); }
	id disj(id a, id b) { return apply(1, a, b); }
	id exor(id a, id b) { return apply(3, a, b); }
	id iff(id a, id b) { return neg(exor(a, b)); }
	id ite(id c, id a, id b) { return disj(conj(c, a), conj(neg(c), b)); }
	id neg(id a) {
		if (a <= T) return a == T ? F : T;
		if (full) return F;
		std::array<uint32_t, 3> k{ 2, a, 0 };
		if (auto it = memo.find(k); it != memo.end()) return it->second;
		const nd x = nodes[a];
		const id r = mk(x.var, neg(x.lo), neg(x.hi));
		remember(k, r);
		return r;
	}
	// Quantifies the variables flagged in `qs`.
	id quantify(id a, const std::vector<bool>& qs, bool exists) {
		std::unordered_map<id, id> seen;
		std::function<id(id)> go = [&](id n) -> id {
			if (n <= T || full) return n <= T ? n : F;
			if (auto it = seen.find(n); it != seen.end()) return it->second;
			const nd x = nodes[n];
			const id lo = go(x.lo), hi = go(x.hi);
			const id r = x.var < qs.size() && qs[x.var]
				? apply(exists ? 1 : 0, lo, hi) : mk(x.var, lo, hi);
			seen.emplace(n, r);
			return r;
		};
		return go(a);
	}
	// Every variable v of `a` renamed to[v]; false in `ok` when to[v] is
	// `leaf`. The renaming must keep the order of the variables it meets.
	id rename(id a, const std::vector<uint32_t>& to, bool& ok) {
		std::unordered_map<id, id> seen;
		std::function<id(id)> go = [&](id n) -> id {
			if (n <= T) return n;
			if (auto it = seen.find(n); it != seen.end()) return it->second;
			const nd x = nodes[n];
			if (x.var >= to.size() || to[x.var] == leaf) {
				ok = false;
				return F;
			}
			const id r = mk(to[x.var], go(x.lo), go(x.hi));
			seen.emplace(n, r);
			return r;
		};
		return go(a);
	}
	// The variables `a` reads.
	std::set<uint32_t> support(id a) const {
		std::set<uint32_t> vs;
		std::unordered_set<id> seen;
		std::vector<id> todo{ a };
		while (!todo.empty()) {
			const id n = todo.back();
			todo.pop_back();
			if (n <= T || !seen.insert(n).second) continue;
			vs.insert(nodes[n].var);
			todo.push_back(nodes[n].lo);
			todo.push_back(nodes[n].hi);
		}
		return vs;
	}
};

// Circuits over bit vectors of data_bdd functions, least significant bit
// first, with unsigned modular semantics.
namespace bit_circuits {

using bits = std::vector<data_bdd::id>;

// x + y + carry, modulo 2^n.
inline bits add(data_bdd& bdd, const bits& x, const bits& y,
	data_bdd::id carry = data_bdd::F)
{
	bits out(x.size());
	for (size_t i = 0; i < x.size(); ++i) {
		const data_bdd::id h = bdd.exor(x[i], y[i]);
		out[i] = bdd.exor(h, carry);
		carry = bdd.disj(bdd.conj(x[i], y[i]), bdd.conj(carry, h));
	}
	return out;
}

// x - y, modulo 2^n.
inline bits sub(data_bdd& bdd, const bits& x, const bits& y) {
	bits ny(y.size());
	for (size_t i = 0; i < y.size(); ++i) ny[i] = bdd.neg(y[i]);
	return add(bdd, x, ny, data_bdd::T);
}

// x < y, unsigned.
inline data_bdd::id less(data_bdd& bdd, const bits& x, const bits& y) {
	data_bdd::id lt = data_bdd::F;
	for (size_t i = 0; i < x.size(); ++i)
		lt = bdd.disj(bdd.conj(bdd.neg(x[i]), y[i]),
			bdd.conj(bdd.iff(x[i], y[i]), lt));
	return lt;
}

inline data_bdd::id same(data_bdd& bdd, const bits& x, const bits& y) {
	data_bdd::id r = data_bdd::T;
	for (size_t i = 0; i < x.size(); ++i)
		r = bdd.conj(r, bdd.iff(x[i], y[i]));
	return r;
}

// c ? x : y, bit by bit.
inline bits select(data_bdd& bdd, data_bdd::id c, const bits& x,
	const bits& y)
{
	bits out(x.size());
	for (size_t i = 0; i < x.size(); ++i) out[i] = bdd.ite(c, x[i], y[i]);
	return out;
}

// x shifted by y: left towards the high bits, else right, logically; by n
// or more, 0.
inline bits shifted(data_bdd& bdd, const bits& x, const bits& y, bool left) {
	const size_t n = x.size();
	bits r = x;
	for (size_t j = 0; j < y.size(); ++j) {
		bits moved(n, data_bdd::F);
		if (j < 63 && (size_t{1} << j) < n) {
			const size_t d = size_t{1} << j;
			for (size_t i = 0; i < n; ++i) {
				if (left && i >= d) moved[i] = r[i - d];
				if (!left && i + d < n) moved[i] = r[i + d];
			}
		}
		r = select(bdd, y[j], moved, r);
	}
	return r;
}

// x * y modulo 2^n by shift and add; false when the table grows past
// `cap` nodes on the way, since a product may grow it exponentially.
inline bool mul(data_bdd& bdd, const bits& x, const bits& y, size_t cap,
	bits& out)
{
	const size_t n = x.size();
	out.assign(n, data_bdd::F);
	for (size_t i = 0; i < n; ++i) {
		if (y[i] == data_bdd::F) continue;
		bits part(n, data_bdd::F);
		for (size_t j = i; j < n; ++j)
			part[j] = bdd.conj(x[j - i], y[i]);
		out = add(bdd, out, part);
		if (bdd.full || bdd.nodes.size() > cap) return false;
	}
	return true;
}

} // namespace bit_circuits

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BACKENDS__BDDS__DATA_BDD_H__
