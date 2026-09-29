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
// true. A table grown past `max_nodes` live nodes, or a memo of the
// operations grown past `max_memo` entries (0 for no bound), sets `full`, and
// every result from then on is meaningless: the caller must check `full` and
// give up, or collect() and redo the work. Once full, the operations return
// at once, and remember nothing.
//
// collect() frees the nodes no root reaches and reuses their ids, so an id a
// root reaches never changes; any other id held across it is dangling.
struct data_bdd {
	using id = uint32_t;
	static constexpr id F = 0, T = 1;
	static constexpr uint32_t leaf = UINT32_MAX;
	// the variable of a freed node
	static constexpr uint32_t freed = UINT32_MAX - 1;
	struct nd { uint32_t var; id lo, hi; };
	// An operation's arguments and result; `op` is `leaf` in an empty slot.
	struct memo_entry { uint32_t op; id a, b, r; };

	// Linear probing reads the low bits, so every input bit must reach
	// them: the finalizer of MurmurHash3.
	static size_t hash(uint32_t x, uint32_t y, uint32_t z) {
		uint64_t h = x;
		h = h * 0x9E3779B97F4A7C15ull + y;
		h = h * 0x9E3779B97F4A7C15ull + z;
		h ^= h >> 33;
		h *= 0xFF51AFD7ED558CCDull;
		h ^= h >> 33;
		h *= 0xC4CEB9FE1A85EC53ull;
		h ^= h >> 33;
		return (size_t)h;
	}

	std::vector<nd> nodes{ { leaf, F, F }, { leaf, T, T } };
	std::vector<id> free_ids;
	// The hash-consing table and the memo, open addressing with linear
	// probing over a power of two of slots; a unique slot holds a node id,
	// F for none, since the leaves are never looked up. collect() rebuilds
	// both rather than erasing from them.
	std::vector<id> unique_slots;
	std::vector<memo_entry> memo_slots;
	size_t unique_count = 0, memo_count = 0;
	size_t max_nodes, max_memo;
	// mk() makes no node past this many live ones: max_nodes, or less
	// while grow_at_most() bounds the work under way
	size_t limit;
	bool full = false;
	// wants_collect() once this many nodes are live; never before the
	// table first fills, so that a table that never does costs nothing
	size_t next_collect = SIZE_MAX;
	size_t collections = 0;

	explicit data_bdd(size_t cap, size_t memo_cap = 0)
		: unique_slots(1024, F), memo_slots(1024, { leaf, 0, 0, 0 }),
		max_nodes(cap), max_memo(memo_cap), limit(cap) {}

	// The live nodes, the two leaves included.
	size_t size() const { return nodes.size() - free_ids.size(); }
	// The nodes that can still be made.
	size_t room() const { return max_nodes - std::min(size(), max_nodes); }
	size_t memo_size() const { return memo_count; }
	size_t unique_size() const { return unique_count; }

	// Lets at most `n` more nodes be live until grow_freely().
	void grow_at_most(size_t n) { limit = std::min(max_nodes, size() + n); }
	void grow_freely() { limit = max_nodes; }

	// The slot of node (v, lo, hi) in the unique table, or the empty slot
	// where it goes.
	size_t unique_slot(uint32_t v, id lo, id hi) const {
		const size_t mask = unique_slots.size() - 1;
		size_t i = hash(v, lo, hi) & mask;
		for (id n; (n = unique_slots[i]) != F; i = (i + 1) & mask) {
			const nd& x = nodes[n];
			if (x.var == v && x.lo == lo && x.hi == hi) break;
		}
		return i;
	}
	void rebuild_unique(size_t slots) {
		unique_slots.assign(slots, F);
		for (id n = 2; n < nodes.size(); ++n)
			if (const nd& x = nodes[n]; x.var != freed)
				unique_slots[unique_slot(x.var, x.lo, x.hi)] = n;
	}

	size_t memo_slot(uint32_t op, id a, id b) const {
		const size_t mask = memo_slots.size() - 1;
		size_t i = hash(op, a, b) & mask;
		for (; memo_slots[i].op != leaf; i = (i + 1) & mask) {
			const memo_entry& e = memo_slots[i];
			if (e.op == op && e.a == a && e.b == b) break;
		}
		return i;
	}
	const id* recall(uint32_t op, id a, id b) const {
		const memo_entry& e = memo_slots[memo_slot(op, a, b)];
		return e.op == leaf ? nullptr : &e.r;
	}
	void remember(uint32_t op, id a, id b, id r) {
		memo_entry& e = memo_slots[memo_slot(op, a, b)];
		if (e.op == leaf) ++memo_count;
		e = { op, a, b, r };
		if (memo_count * 3 > memo_slots.size() * 2)
			rebuild_memo(memo_slots.size() * 2, [](id) { return true; });
		if (max_memo && memo_count >= max_memo) full = true;
	}
	// Keeps the entries whose arguments and result `keep`.
	template <typename Keep>
	void rebuild_memo(size_t slots, Keep&& keep) {
		std::vector<memo_entry> old(slots, { leaf, 0, 0, 0 });
		old.swap(memo_slots);
		memo_count = 0;
		for (const memo_entry& e : old)
			if (e.op != leaf && keep(e.a) && keep(e.b) && keep(e.r)) {
				memo_slots[memo_slot(e.op, e.a, e.b)] = e;
				++memo_count;
			}
	}

	id mk(uint32_t v, id lo, id hi) {
		if (lo == hi) return lo;
		const size_t i = unique_slot(v, lo, hi);
		if (unique_slots[i] != F) return unique_slots[i];
		if (size() >= limit) { full = true; return F; }
		id n;
		if (free_ids.empty()) {
			n = (id)nodes.size();
			nodes.push_back({ v, lo, hi });
		} else {
			n = free_ids.back();
			free_ids.pop_back();
			nodes[n] = { v, lo, hi };
		}
		unique_slots[i] = n;
		if (++unique_count * 3 > unique_slots.size() * 2)
			rebuild_unique(unique_slots.size() * 2);
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
		if (const id* r = recall(op, a, b)) return *r;
		const nd x = nodes[a], y = nodes[b];
		const uint32_t v = std::min(x.var, y.var);
		const id lo = apply(op, x.var == v ? x.lo : a, y.var == v ? y.lo : b);
		const id hi = apply(op, x.var == v ? x.hi : a, y.var == v ? y.hi : b);
		const id r = mk(v, lo, hi);
		if (!full) remember(op, a, b, r);
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
		if (const id* r = recall(2, a, F)) return *r;
		const nd x = nodes[a];
		const id r = mk(x.var, neg(x.lo), neg(x.hi));
		if (!full) remember(2, a, F, r);
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

	// Whether the table is full enough that a caller holding its roots at
	// hand should collect().
	bool wants_collect() const { return size() >= next_collect; }

	// Whether work that began with `before` nodes of room and ran out of
	// them is worth redoing after a collect(): only when the room more
	// than doubled, which also bounds how often it is redone, and at most
	// three quarters of the table stay live. Past that, the work that
	// follows runs out again after a few steps, each paying a collection.
	bool worth_redoing(size_t before) const {
		return room() > 2 * before && size() <= max_nodes / 4 * 3;
	}

	// Frees every node that no root reaches: `each_root(mark)` calls
	// mark(r) for every root r. Memo entries naming a freed node go, and
	// `full` is cleared. Returns the nodes freed.
	template <typename Roots>
	size_t collect(Roots&& each_root) {
		std::vector<bool> live(nodes.size(), false);
		live[F] = live[T] = true;
		std::vector<id> todo;
		auto mark = [&](id r) {
			if (r < nodes.size() && !live[r]) {
				live[r] = true;
				todo.push_back(r);
			}
		};
		each_root(mark);
		while (!todo.empty()) {
			const nd x = nodes[todo.back()];
			todo.pop_back();
			mark(x.lo);
			mark(x.hi);
		}
		size_t freed_now = 0;
		for (id n = 2; n < nodes.size(); ++n) {
			nd& x = nodes[n];
			if (live[n] || x.var == freed) continue;
			x = { freed, F, F };
			free_ids.push_back(n);
			++freed_now;
		}
		// the fewest slots that keep the tables at most half full
		auto slots = [](size_t entries) {
			size_t n = 1024;
			while (n < 2 * entries) n *= 2;
			return n;
		};
		unique_count = size() - 2;
		rebuild_unique(slots(unique_count));
		rebuild_memo(slots(memo_count), [&](id n) { return live[n]; });
		full = false;
		++collections;
		// the next collection once half the room left is used, and not
		// before an eighth of the table is: past seven eighths live, only
		// a full table collects
		next_collect = std::max(max_nodes / 2,
			size() + std::max(room() / 2, max_nodes / 8));
		return freed_now;
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

// x * y modulo 2^n by shift and add; false when more than `cap` nodes are
// live on the way, since a product may grow the table exponentially.
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
		if (bdd.full || bdd.size() > cap) return false;
	}
	return true;
}

} // namespace bit_circuits

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BACKENDS__BDDS__DATA_BDD_H__
