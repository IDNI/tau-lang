// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BACKENDS__BDDS__BABDD_H__
#define __IDNI__TAU__BACKENDS__BDDS__BABDD_H__

#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <array>
#include <variant>
#include <cstdint>
#include <cassert>
#include <memory>
#include <functional>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <limits>
#include <type_traits>

#include "backends/bdds/var_dict.h"
#include "boolean_algebras/bool_ba.h"

namespace idni::tau_lang {

/// Signed BDD literal / variable index type.
typedef int32_t int_t;
/// Unsigned BDD variable index type.
typedef uint32_t uint_t;
/// Shorthand for a shared pointer.
template<typename T> using sp = std::shared_ptr<T>;

/**
 * @brief Raised when a bdd node table had no id left for a new node.
 *
 * Ids are idW bits wide, so a table holds at most 2^idW entries. The node
 * that does not fit is not interned: F takes its place, and while the flag
 * is up every memoized operation answers F, so the result is unknown. No
 * memo, handle table or constant cache takes a write meanwhile, so what they
 * hold still describes exactly what it did before. The unit-of-work
 * boundaries turn the flag into an error without a value, empty the tree
 * caches the unknown result may have reached and lower the flag
 * (take_bdd_node_table_exhausted in tau_memory_budget.h).
 */
inline bool bdd_node_table_exhausted = false;

// A BDD literal is a signed variable id: positive = the variable, negative
// = its complement. These two convert across that boundary once.
/// @brief The variable of literal @p lit (its absolute value).
inline uint_t lit_var(int_t lit) {
	return static_cast<uint_t>(lit > 0 ? lit : -lit);
}
/// @brief The literal of variable @p v, negated when @p neg is true.
inline int_t signed_lit(uint_t v, bool neg) {
	return neg ? -static_cast<int_t>(v) : static_cast<int_t>(v);
}

#define neg_to_odd(x) (((x)<0?(((-(x))<<1)+1):((x)<<1)))
#define hash_pair(x, y) fpairing(neg_to_odd(x), neg_to_odd(y))
#define hash_tri(x, y, z) fpairing(hash_pair(x, y), neg_to_odd(z))
#define hash_upair(x, y) fpairing(x, y)
#define hash_utri(x, y, z) fpairing(hash_upair(x, y), z)

// Cantor pairing function: maps a pair of naturals to a single
// natural injectively; used as the hash combiner for node hashing.
inline std::uint64_t fpairing(std::uint64_t x, std::uint64_t y) {
	const std::uint64_t z = x + y;
	return y+((z * (z + 1))>>1);
}

/* Options for a bdd instantiation: the bit widths of a reference's id and
 * shift. Every engine uses input inverters (child swap in the reference),
 * output inverters (complement in the reference) and variable shifters
 * (variable-free skeletons), with the ascending variable order. Create by
 * the static functions bdd_options::create.
 */
class bdd_options {
	constexpr bdd_options(auto idWidth, auto shiftWidth) :
		idW(static_cast<uint8_t>(idWidth)), shiftW(static_cast<uint8_t>(shiftWidth)) {}

public:
	// bit width for id and shift
	const uint8_t idW;
	const uint8_t shiftW;

	/// @brief Options with id width @p idWidth and shift width
	/// @p shiftWidth; the widths must sum to at most 62.
	constexpr static bdd_options
	create(const auto idWidth, const auto shiftWidth) {
		DBG(assert(idWidth + shiftWidth <= 62));
		return bdd_options(idWidth, shiftWidth);
	}

	/// @brief Default options: 18-bit ids and 12-bit shifts.
	constexpr static bdd_options create() { return bdd_options(18, 12); }
};

/* Defines the reference type to reference a bdd_node in the bdd universe.
 *
 * Packed encoding:
 * - id:    index of the referenced entry in the universe vector V
 * - in:    input inverter -- the reference denotes the stored node with
 *          its high and low children exchanged
 * - out:   output inverter -- the reference denotes the complement of
 *          the stored function
 * - shift: the node's top variable. With shifters, V stores
 *          variable-free skeletons whose child references hold their
 *          variable RELATIVE to the parent's variable, so structurally
 *          equal bdds rooted at different variables share one entry.
 *          shift == 0 marks a leaf and is preserved by all rebasings.
 *
 * Relative encoding (v = parent variable, a = absolute variable):
 * relative = a - (v - 1).
 */
template<int_t ID_WIDTH, int_t SHIFT_WIDTH>
struct bdd_reference {
	typedef std::conditional<ID_WIDTH + SHIFT_WIDTH <= 30,
					uint32_t, uint64_t>::type ref_type;
	ref_type in: 1 = 0;
	ref_type out: 1 = 0;
	ref_type shift: SHIFT_WIDTH = 0;
	ref_type id: ID_WIDTH = 0;

	// The masks make each bitfield store visibly fit its width. in/out are
	// 0/1 inverter flags, table_full() keeps an id below 2^ID_WIDTH, and the
	// asserts catch a value that does not fit.
	static constexpr ref_type flag_mask = 1;
	static constexpr ref_type shift_mask = (ref_type{1} << SHIFT_WIDTH) - 1;
	static constexpr ref_type id_mask = (ref_type{1} << ID_WIDTH) - 1;

	bdd_reference() = default;
	bdd_reference(auto in, auto out, auto id)
		: in(static_cast<ref_type>(in) & flag_mask),
		  out(static_cast<ref_type>(out) & flag_mask),
		  id(static_cast<ref_type>(id) & id_mask)
	{
		DBG(assert(static_cast<ref_type>(in) == this->in);)
		DBG(assert(static_cast<ref_type>(out) == this->out);)
		DBG(assert(static_cast<ref_type>(id) == this->id);)
	}
	bdd_reference(auto in, auto out, auto shift, auto id)
		: in(static_cast<ref_type>(in) & flag_mask),
		  out(static_cast<ref_type>(out) & flag_mask),
		  shift(static_cast<ref_type>(shift) & shift_mask),
		  id(static_cast<ref_type>(id) & id_mask)
	{
		DBG(assert(static_cast<ref_type>(in) == this->in);)
		DBG(assert(static_cast<ref_type>(out) == this->out);)
		DBG(assert(static_cast<ref_type>(shift) == this->shift);)
		DBG(assert(static_cast<ref_type>(id) == this->id);)
	}

	// id indexes the universe vector V; ID_WIDTH never exceeds size_t's
	// range, checked once here rather than at each V[n.id] use.
	size_t idx() const {
		DBG(assert(id <= static_cast<ref_type>(
			std::numeric_limits<size_t>::max())));
		return static_cast<size_t>(id);
	}

	bool operator==(const bdd_reference x) const {
		return in == x.in && out == x.out && shift == x.shift &&
		       id == x.id;
	}

	/// Lexicographic order on (in, out, shift, id).
	auto operator<=>(const bdd_reference x) const {
		if (in != x.in) return (int)in <=> (int)x.in;
		if (out != x.out) return (int)out <=> (int)x.out;
		if (shift != x.shift) return (long long)shift <=> (long long)x.shift;
		return (long long)id <=> (long long)x.id;
	}

	// Toggle the input inverter (denotes the child-swapped node)
	static bdd_reference flip_in(const bdd_reference x) {
		return bdd_reference(x.in ? 0 : 1, x.out, x.shift, x.id);
	}

	// Toggle the output inverter (denotes the complement function)
	static bdd_reference flip_out(const bdd_reference x) {
		return bdd_reference(x.in, x.out == 1 ? 0 : 1, x.shift, x.id);
	}

	// Rebase x's absolute top variable to parent-relative form, v
	// being the parent's variable; used when storing children in a
	// skeleton. Leaves (shift == 0) pass through unchanged.
	// v shares x.shift's width, so a shift-domain value never narrows.
	static bdd_reference to_shift_node(const bdd_reference x, ref_type v) {
		if(x.shift == 0) return x;
		return bdd_reference(x.in, x.out, x.shift - (v - 1), x.id);
	}

	// Inverse of to_shift_node: restore a parent-relative child
	// reference to absolute variable numbering, v being the parent's
	// variable; used when decoding a stored skeleton.
	static bdd_reference to_bdd_node(const bdd_reference x, ref_type v) {
		if(x.shift == 0) return x;
		return bdd_reference(x.in, x.out, x.shift + (v - 1), x.id);
	}

	/// 64-bit hash of every field of @p x, identical on every platform.
	static std::uint64_t hash(const bdd_reference x) {
		std::uint64_t seed = 0;
		hash_combine(seed, x.id + x.in);
		hash_combine(seed, x.shift + x.out);
		return seed;
	}
};

/* A BDD decision node: variable v with high child h (taken when v is
 * true) and low child l (v false), plus a precomputed hash. R is the
 * reference type. It is the decoded, absolute-variable view that
 * bdd::get() reconstructs from a stored node_skeleton.
 */
template<typename R>
struct bdd_node {
	bdd_node(uint_t v, R h, R l) :
		v(v), h(h), l(l),
		hash(hash_utri(v, R::hash(h), R::hash(l))) {}
	uint_t v;
	R h, l;
	std::uint64_t hash;
	/// Structural equality; the hash is compared first as a fast reject.
	bool operator==(const auto& x) const {
		return hash == x.hash && v == x.v && h == x.h && l == x.l;
	}
};

/* The variable-free form of a bdd_node stored in the universe; R is the
 * reference type for the high and low pointer
 */
template<typename R>
struct node_skeleton {
	node_skeleton(R h, R l) :
		h(h), l(l), hash(hash_upair(R::hash(h), R::hash(l))) {}
	R h, l;
	std::uint64_t hash;
	/// Structural equality; the hash is compared first as a fast reject.
	bool operator==(const auto& x) const {
		return hash == x.hash && h == x.h && l == x.l;
	}
};

} // namespace idni::tau_lang

namespace std {

// The node hashes are 64 bits on every platform. Only std::hash narrows them
// to size_t, which is 32 bits on wasm32.
template<typename R>
struct hash<idni::tau_lang::bdd_node<R>> {
	size_t operator()(auto& n) const { return static_cast<size_t>(n.hash); }
};

template<typename R>
struct hash<idni::tau_lang::node_skeleton<R>> {
	size_t operator()(auto& n) const { return static_cast<size_t>(n.hash); }
};

template<idni::tau_lang::int_t IW, idni::tau_lang::int_t SW>
struct hash<array<idni::tau_lang::bdd_reference<IW, SW>, 2>> {
	size_t operator()(const auto& a) const {
		return static_cast<size_t>(idni::tau_lang::hash_upair(
			(idni::tau_lang::bdd_reference<IW, SW>::hash(a[0])),
			(idni::tau_lang::bdd_reference<IW, SW>::hash(a[1]))));
	}
};

template<idni::tau_lang::int_t IW, idni::tau_lang::int_t SW>
struct hash<idni::tau_lang::bdd_reference<IW, SW>> {
	size_t operator()(const auto& a) const {
		return static_cast<size_t>(
			idni::tau_lang::bdd_reference<IW, SW>::hash(a));
	}
};

template<idni::tau_lang::int_t IW, idni::tau_lang::int_t SW>
struct hash<pair<idni::tau_lang::bdd_reference<IW, SW>, idni::tau_lang::uint_t>> {
	size_t operator()(const auto& p) const {
		return static_cast<size_t>(idni::tau_lang::hash_upair(
			(hash<idni::tau_lang::bdd_reference<IW, SW>>{}(p.first)),
			p.second));
	}
};

} // namespace std

namespace idni::tau_lang {

// Constants of a base algebra B; overloaded for shared-pointer handle
// types in bdd_handle.h.
template<typename B> B get_zero() { return B::zero(); }
template<typename B> B get_one() { return B::one(); }

/* The BDD engine over base Boolean algebra B: a hash-consed,
 * multi-terminal BDD whose leaves are elements of B (not just 0/1), so
 * a bdd value is either a decision node or a constant of B. Over the
 * two-element algebra Bool it is a classical BDD: the only constants
 * are T and F, held as one sentinel universe entry (see bdd_init), so a
 * bdd value IS a decision node (no variant, no Mb leaf map) and
 * leaf(ref) is an id test.
 *
 * Ownership/interning model: all state is static per (B, o)
 * instantiation. V is the append-only universe owning every node (as
 * a variable-free node_skeleton) and every interned leaf constant; Mn
 * and Mb map nodes and constants back to their V index so
 * structurally equal bdds are stored exactly once. Nodes are never
 * freed. All functions are denoted by lightweight bdd_ref values
 * (index + inverter bits + shift, see bdd_reference); T and F are the
 * canonical constants. The universe is seeded once by the static
 * initializer I (bdd_init).
 *
 * Operation results are memoized in the *_memo caches; cache keys are
 * shift-normalized (see check_cache) so entries are shared between
 * uniformly shifted instances of the same structure.
 */
template<typename B, bdd_options o = bdd_options::create()>
struct bdd : std::conditional_t<std::is_same_v<B, Bool>,
	bdd_node<bdd_reference<o.idW, o.shiftW>>,
	std::variant<bdd_node<bdd_reference<o.idW, o.shiftW>>, B>>
{
	static constexpr bool is_bool = std::is_same_v<B, Bool>;
	using bdd_ref = bdd_reference<o.idW, o.shiftW>;
	typedef bdd_node<bdd_ref> bdd_node_t;
	typedef std::conditional_t<is_bool, bdd_node_t,
		std::variant<bdd_node_t, B>> base;

	// The entry of the bdd universe: a node skeleton or a leaf constant
	struct bdd_skeleton : std::variant<node_skeleton<bdd_ref>, B> {
		using base = std::variant<node_skeleton<bdd_ref>, B>;
		bdd_skeleton(bdd_ref h, bdd_ref l) : base(node_skeleton<bdd_ref>(h, l)) {}
		explicit bdd_skeleton(const auto& n) : base(n) {}

		bool leaf() const { return holds_alternative<B>(*this); }
	};

	/// Decision node (v ? h : l); not interned, see add().
	bdd(uint_t v, bdd_ref h, bdd_ref l) requires is_bool : base(v, h, l) {}
	bdd(uint_t v, bdd_ref h, bdd_ref l) requires (!is_bool)
		: base(bdd_node_t(v, h, l)) {}
	/// Value holding @p n, a bdd_node_t or a constant of B.
	explicit bdd(const auto& n) : base(n) {}

	// true iff this value / the referenced entry is a leaf constant; a
	// Bool constant is the id of the sentinel entry
	bool leaf() const requires (!is_bool) { return holds_alternative<B>(*this); }
	static bool leaf(bdd_ref n) {
		if constexpr (is_bool) return n.id == 0;
		else return holds_alternative<B>(V[n.idx()]);
	}
	// Variable-order comparator: var_cmp(a, b) is true when a comes
	// strictly before b in the (ascending) BDD order
	static bool var_cmp(uint_t vl, uint_t vr) { return vl < vr; }

	// Static-init hook: constructing I runs bdd_init<B, o>() once
	struct initializer { initializer(); };

	// The node universe: owns every node and interned leaf constant
	inline static std::vector<std::conditional_t<is_bool,
		node_skeleton<bdd_ref>, bdd_skeleton>> V;
	// Interning map: node -> its index in V
	inline static std::unordered_map<node_skeleton<bdd_ref>, size_t> Mn;
	// Interning map: leaf constant of B -> its index in V; empty for Bool
	inline static std::map<B, size_t> Mb;
	// Canonical references to the constants one and zero
	inline static bdd_ref T, F;
	inline static initializer I;

	// Caches for bdd operations
	inline static std::unordered_map<std::array<bdd_ref,2>, bdd_ref> and_memo;
	inline static std::unordered_map<std::pair<bdd_ref, uint_t>, bdd_ref> all_memo;

	/* Memo lookup/store with shift-normalized keys.
	 *
	 * Structurally equal bdds rooted at different variables share
	 * one skeleton and differ only in the shift fields, and the
	 * operations below commute with a uniform variable shift. Keys
	 * (operands, and for all the quantified variable) are therefore
	 * rebased so the topmost variable of the operation becomes
	 * canonical, letting a single cache entry serve every shifted
	 * instance; the cached result is stored rebased the same way and
	 * is rebased back to the query's level on a hit. Variants: (x, v)
	 * rebases by x's own top variable and also rebases the quantified
	 * variable v; (x, y) rebases both operands by their common
	 * reference level d (the first variable in the BDD order, the min
	 * of the shifts), so only the operands' relative offset is keyed.
	 * check_cache returns true on a hit and replaces x with the
	 * (rebased) cached result.
	 */
	// With a full table every memoized operation answers F at once: the
	// result is unknown anyway, and without memo writes the recursion
	// would otherwise not be bounded by the memos.
	static bool check_cache(bdd_ref& x, bdd_ref y, const auto& cache) {
		if (bdd_node_table_exhausted) return x = F, true;
		const auto d = std::min(x.shift, y.shift);
		if (auto it = cache.find({bdd_ref::to_shift_node(x, d),
					  bdd_ref::to_shift_node(y, d)});
			it != cache.end()) {
			x = bdd_ref::to_bdd_node(it->second, d);
			return true;
		} else return false;
	}

	static bool check_cache(bdd_ref& x, uint_t v, const auto& cache){
		if (bdd_node_table_exhausted) return x = F, true;
		if (auto it = cache.find(
				{bdd_ref::to_shift_node(x, x.shift),
				 v - (x.shift - 1)}); it != cache.end()) {
			x = bdd_ref::to_bdd_node(it->second, x.shift);
			return true;
		} else return false;
	}

	// Store result r under the key check_cache looks up for the same
	// operands, rebased the same way; no write while the node table is
	// exhausted.
	static void update_cache(bdd_ref x, uint_t v, bdd_ref r, auto& cache) {
		if (bdd_node_table_exhausted) return;
		cache.emplace(std::pair<bdd_ref, uint_t>{
				      bdd_ref::to_shift_node(x, x.shift),
				      v - (x.shift - 1)},
			      bdd_ref::to_shift_node(r, x.shift));
	}

	static void update_cache(bdd_ref x, bdd_ref y, bdd_ref r, auto &cache) {
		if (bdd_node_table_exhausted) return;
		const auto d = std::min(x.shift, y.shift);
		cache.emplace(std::array<bdd_ref, 2>{
				      bdd_ref::to_shift_node(x, d),
				      bdd_ref::to_shift_node(y, d)},
			      bdd_ref::to_shift_node(r, d));
	}

	// Canonicalize operand order of a commutative operation (sort by
	// id, then shift) so both argument orders hit one cache entry
	static void mk_order_canonical(bdd_ref& x, bdd_ref& y) {
		if (x.id == y.id && x.shift > y.shift) std::swap(x, y);
		if(x.id > y.id) std::swap(x,y);
	}

	// True iff y is x with the output inverter toggled, i.e. its
	// complement
	static bool negation_of(bdd_ref x, bdd_ref y) {
		return bdd_ref::flip_out(x) == y;
	}

	// Intern the decoded node n, see add(v, h, l)
	static bdd_ref add(const bdd_node_t& n) { return add(n.v, n.h, n.l); }

	// idW-bit ids address at most 2^idW entries; the node that does not
	// fit stays out of the table (see bdd_node_table_exhausted)
	static bool table_full() {
		if (V.size() < (uint64_t{1} << o.idW)) return false;
		return bdd_node_table_exhausted = true;
	}

	// Canonicalizing node constructor ("mk"): returns the unique
	// reference denoting (v ? h : l). Collapses h == l; orders the
	// children by id, recording a swap in the reference's in bit;
	// normalizes the stored node so its low child is non-inverted,
	// moving the complement into the reference's out bit -- so a
	// function and its complement share one stored node. The children
	// are rebased to v-relative form and the variable-free skeleton is
	// interned; v travels in the returned reference's shift field.
	static bdd_ref add(uint_t v, bdd_ref h, bdd_ref l) {
#ifdef DEBUG
		assert(v < pow(2, o.shiftW));
#endif
		if (h == l) return h;
#ifdef DEBUG
		if (!leaf(l)) assert(var_cmp(v, get_node(l).v));
		if (!leaf(h)) assert(var_cmp(v, get_node(h).v));
#endif
		bool in = false, out = false;
		if(h.id < l.id)
			std::swap(h, l), in = true;
		if (l.out) {
			h = bdd_ref::flip_out(h);
			l = bdd_ref::flip_out(l);
			out = true;
		}
		h = bdd_ref::to_shift_node(h, v);
		l = bdd_ref::to_shift_node(l, v);
		node_skeleton<bdd_ref> n(h, l);
		if (auto it = Mn.find(n); it != Mn.end())
			return bdd_ref(in, out, v, it->second);
		if (table_full()) return F;
		Mn.emplace(n, V.size());
		V.emplace_back(n);
		return bdd_ref(in, out, v, V.size() - 1);
	}

	// Intern the leaf constant b and return its reference; false and
	// true map to F and T, and an already interned ~b is reused via the
	// out bit
	static bdd_ref add(const B& b) requires (!is_bool) {
		if (b == false) return F;
		if (b == true) return T;
		if (auto it = Mb.find(b); it != Mb.end()) return bdd_ref(0,0,it->second);
		else if ((it = Mb.find(~b)) != Mb.end()) return bdd_ref(0,1,it->second);
		if (table_full()) return F;
		Mb.emplace(b, V.size());
		V.emplace_back(b);
		return bdd_ref(0, 0, V.size()-1);
	}

	// Decode a reference to a decision node: restore the children's
	// absolute variables from the stored skeleton, then apply the in
	// inverter (swap the children) and the out inverter (flip the
	// children's out bits). A Bool constant decodes to the degenerate
	// sentinel node, so callers test leaf(n) first where it matters.
	static bdd_node_t get_node(bdd_ref n) {
		const node_skeleton<bdd_ref>* s;
		if constexpr (is_bool) s = &V[n.idx()];
		else s = &std::get<node_skeleton<bdd_ref>>(V[n.idx()]);
		bdd_ref h = bdd_ref::to_bdd_node(s->h, n.shift),
			l = bdd_ref::to_bdd_node(s->l, n.shift);
		if (n.in) std::swap(h, l);
		if (n.out) h = bdd_ref::flip_out(h), l = bdd_ref::flip_out(l);
		return bdd_node_t(n.shift, h, l);
	}

	// Decode a reference known to denote a leaf: its constant,
	// complemented by the out inverter
	static B get_elem(bdd_ref n) {
		if constexpr (is_bool) return Bool(n == T);
		else {
			const B& b = std::get<B>(V[n.idx()]);
			return n.out ? ~b : b;
		}
	}

	// Decode a reference into the bdd value it denotes
	static bdd get(bdd_ref n) {
		if constexpr (is_bool) return bdd(get_node(n));
		else return leaf(n) ? bdd(get_elem(n)) : bdd(get_node(n));
	}

	// The literal of variable |v|: positive v gives the function "v",
	// negative v its complement; registers |v| in var_dict
	static bdd_ref bit(int_t v) {
		// Avoid later name clash by adding any new variable to dictionary
		// TODO (HIGH) dropped error: var_dict's report -- bit() returns bdd_ref, not result<>, so a stale or corrupt id has no channel.
		var_dict(v>0?v:-v);
		const uint_t av = lit_var(v);
		return v > 0 ? add(av, T, F) : add(av, F, T);
	}

	// Complement: O(1) out-bit flip
	static bdd_ref bdd_not(bdd_ref x) { return bdd_ref::flip_out(x); }

	// Conjunction with a constant of B, pushed down into the leaves
	static bdd_ref bdd_and(bdd_ref x, const B& b) requires (!is_bool) {
		if (x == T) return add(b);
		if (x == F) return F;
		if (check_cache(x, add(b), and_memo)) return x;
		if (leaf(x)) return add(b & get_elem(x));
		const bdd_node_t nx = get_node(x);
		bdd_ref y = add(nx.v, bdd_and(nx.h, b), bdd_and(nx.l, b));
#ifdef DEBUG
		if (b == true) assert(y == x);
#endif
		return update_cache(x, add(b), y, and_memo), y;
	}

	// Conjunction by Shannon expansion on the earlier top variable,
	// with terminal cases, O(1) complement detection, canonical
	// operand order and memoization
	static bdd_ref bdd_and(bdd_ref x, bdd_ref y) {
		if (x == F || y == F) return F;
		if (x == T) return y;
		if (y == T) return x;
		if (negation_of(x, y)) return F;
		mk_order_canonical(x,y);
		if (check_cache(x, y, and_memo)) return x;
		// The only Bool leaves are T and F, handled above
		if constexpr (!is_bool) {
			if (leaf(x)) return bdd_and(y, get_elem(x));
			if (leaf(y)) return bdd_and(x, get_elem(y));
		}
		const bdd_node_t nx = get_node(x), ny = get_node(y);
		bdd_ref r;
		if (nx.v == ny.v)
			r =	add(nx.v,
				bdd_and(nx.h, ny.h),
				bdd_and(nx.l, ny.l));
		else if (var_cmp(nx.v, ny.v))
			r = add(nx.v, bdd_and(nx.h, y), bdd_and(nx.l, y));
		else r = add(ny.v, bdd_and(ny.h, x), bdd_and(ny.l, x));
		return update_cache(x, y, r, and_memo), r;
	}

	// Disjunction: derived from bdd_and by De Morgan, complement being
	// free
	static bdd_ref bdd_or(bdd_ref x, bdd_ref y){
		return bdd_ref::flip_out(
			bdd_and(bdd_ref::flip_out(x), bdd_ref::flip_out(y)));
	}

	// Existential quantification of variable v, derived from all() by
	// De Morgan
	static bdd_ref ex(bdd_ref x, uint_t v) {
		return bdd_ref::flip_out(all(bdd_ref::flip_out(x), v));
	}

	// Universal quantification of variable v: the v node is replaced
	// by the conjunction of its branches; memoized
	static bdd_ref all(bdd_ref x, uint_t v) {
		if (check_cache(x, v, all_memo)) return x;
		if (leaf(x)) return x;
		const bdd_node_t nx = get_node(x);
		bdd_ref r;
		if (var_cmp(nx.v, v)) r = add(nx.v, all(nx.h, v), all(nx.l, v));
		else if (var_cmp(v, nx.v)) r = x;
		else r = bdd_and(nx.h, nx.l);
		update_cache(x, v, r, all_memo);
		return r;
	}

	// Value of x universally quantified over all of its variables:
	// the conjunction of all leaf constants (short-circuits at zero).
	// For Bool, x has no F leaf exactly when x is T.
	static B get_uelim(bdd_ref x) {
		if constexpr (is_bool) return Bool(x == T);
		else {
			if (leaf(x)) return get_elem(x);
			const bdd_node_t nx = get_node(x);
			if (B r = get_uelim(nx.h); r == false) return r;
			else return r & get_uelim(nx.l);
		}
	}

	// Value of x existentially quantified over all of its variables:
	// the disjunction of all leaf constants (short-circuits at one).
	// For Bool, x has no T leaf exactly when x is F.
	static B get_eelim(bdd_ref x) {
		if constexpr (is_bool) return Bool(x != F);
		else {
			if (leaf(x)) return get_elem(x);
			const bdd_node_t nx = get_node(x);
			if (B r = get_eelim(nx.h); r == true) return r;
			else return r | get_eelim(nx.l);
		}
	}

	// Substitute the function `with` for variable v. `with` may hold
	// variables ordered before v, so the result is the ite of `with`
	// over the two cofactors of x by v.
	static bdd_ref subst(bdd_ref x, uint_t v, bdd_ref with) {
		return ite(with, cofactor(x, v, true), cofactor(x, v, false));
	}

	// Restrict v := 1 (the high cofactor with respect to v) when high
	// is true, v := 0 (the low cofactor) otherwise
	static bdd_ref cofactor(bdd_ref x, uint_t v, bool high) {
		if (leaf(x)) return x;
		const bdd_node_t nx = get_node(x);
		if (var_cmp(nx.v, v))
			return add(nx.v, cofactor(nx.h, v, high),
				cofactor(nx.l, v, high));
		if (var_cmp(v, nx.v)) return x;
		return high ? nx.h : nx.l;
	}

	// Enumerate the DNF of x: for every path to a nonzero leaf, call
	// f with the leaf constant and the path's literals (+var for the
	// high branch, -var for the low). Stops and returns false as
	// soon as f does; v is the caller-supplied (normally empty) path
	// accumulator
	static bool dnf(bdd_ref x, std::vector<int_t>& v,
		std::function<bool(const std::pair<B, std::vector<int_t>>&)> f)
	{
		if (leaf(x)) {
			const B c = get_elem(x);
			if (!(c == false)) return f({c, v});
			return true;
		}
		const bdd_node_t n = get_node(x);
		// Literal indices are non-negative; the sign carries the polarity.
		v.push_back(signed_lit(n.v, false));
		if (!dnf(n.h, v, f)) return false;
		v.back() = signed_lit(n.v, true);
		if (!dnf(n.l, v, f)) return false;
		v.pop_back();
		return true;
	}

	// Collect the variables x depends on into s. Each node is read once:
	// two nodes of one variable can hold different variables below them.
	static void get_vars(bdd_ref x, std::set<int_t>& s) {
		std::set<bdd_ref> seen;
		std::vector<bdd_ref> todo{ x };
		while (!todo.empty()) {
			const bdd_ref y = todo.back();
			todo.pop_back();
			if (leaf(y) || !seen.insert(y).second) continue;
			const bdd_node_t n = get_node(y);
			// Variable indices are non-negative.
			s.insert(signed_lit(n.v, false));
			todo.push_back(n.h);
			todo.push_back(n.l);
		}
	}

	// if-then-else: x ? y : z
	static bdd_ref ite(bdd_ref x, bdd_ref y, bdd_ref z) {
		return bdd_or(bdd_and(x, y), bdd_and(bdd_not(x),z));
	}

	// Given x with at least one zero, fill m with a constant of B per
	// variable such that x evaluates to zero under m -- a witness
	// zero, as used by the LGRS construction (see bdd_handle::lgrs).
	// x == F succeeds with the empty map; an x with no zero is an
	// error.
	static result<bool> get_one_zero(bdd_ref, std::map<int_t, B>&);

	// Simultaneously substitute the mapped functions for the mapped
	// variables, bottom-up; variables absent from m are kept
	static bdd_ref compose(bdd_ref x, const std::map<int_t, bdd_ref>& m) {
		if (leaf(x)) return x;
		const bdd_node_t n = get_node(x);
		bdd_ref a = compose(n.h, m), b = compose(n.l, m);
		if (auto it = m.find(static_cast<int_t>(n.v)); it == m.end())
			return ite(add(n.v, T, F), a, b);
		else return ite(it->second, a, b);
	}

	// Evaluate x to a constant of B under the assignment m. Over B, m
	// must include all vars in x, otherwise use compose(); over Bool
	// the branches m selects are followed down to a leaf, so m must
	// include every variable on that path
	static B eval(bdd_ref x, const std::map<int_t, B>& m) {
		if constexpr (is_bool) {
			while (!leaf(x)) {
				const bdd_node_t n = get_node(x);
				auto it = m.find(static_cast<int_t>(n.v));
				assert(it != m.end());
				x = it->second == true ? n.h : n.l;
			}
			return Bool(x == T);
		} else {
			if (leaf(x)) return get_elem(x);
			const bdd_node_t n = get_node(x);
			B a = eval(n.h, m), b = eval(n.l, m);
			auto it = m.find(static_cast<int_t>(n.v));
			assert(it != m.end());
			return (it->second & a) | (~it->second & b);
		}
	}

	// remove the leftmost single clause in the bdd
	static bdd_ref rm_clause(bdd_ref x) {
		if (leaf(x)) return F;
		const bdd_node_t n = get_node(x);
		if (n.l == F) return add(n.v, rm_clause(n.h), F);
		if (leaf(n.h)) return add(n.v, n.h, rm_clause(n.l));
		return add(n.v, rm_clause(n.h), n.l);
	}

	// remove all except a single clause in the bdd
	static bdd_ref rm_all_except_one_clause(bdd_ref x) {
		if (leaf(x)) return F;
		const bdd_node_t n = get_node(x);
		if (n.h == T) return add(n.v, T, F);
		if (n.l == T) return add(n.v, F, T);
		if (n.h == F) return add(n.v, F, rm_all_except_one_clause(n.l));
		return add(n.v, rm_all_except_one_clause(n.h), F);
	}

	// remove half of all clauses in the bdd on average
	// by simply returning the high node
	static bdd_ref rm_half_clauses(bdd_ref x) {
		if (leaf(x)) return F;
		const bdd_node_t n = get_node(x);
		if (n.l == F) return add(n.v, rm_half_clauses(n.h), n.l);
		return add(n.v, n.h, F);
	}

	// Highest variable index in x; 0 for a leaf
	static uint_t highest_var(bdd_ref x) {
		if (leaf(x)) return 0;
		const bdd_node_t n = get_node(x);
		return std::max(n.v, std::max(highest_var(n.h), highest_var(n.l)));
	}

	// Conjoin x with the positive literal of a variable not present in
	// x (one past its highest; variable 1 for a leaf)
	static bdd_ref split_clause(bdd_ref x) {
		if(leaf(x)) return bdd_and(x, bit(1));
		// The new variable's index is non-negative.
		return bdd_and(x, bit(signed_lit(highest_var(x) + 1, false)));
	}
};

template<typename B, bdd_options o>
result<bool> bdd<B, o>::get_one_zero(bdd_ref x, std::map<int_t, B>& m) {
	result<bool> r;
	if constexpr (is_bool) {
		// Walk a path from x to the F leaf, preferring the low branch
		// (assign false) and taking the high branch (assign true) only
		// when the low child is T. A reduced node has distinct
		// children, so a child other than T always exists and every
		// function other than T reaches F. The assignment in m is
		// partial: the variables off the path are free.
		if (x == T)
			return r.with_assert_check_error(code::invalid_argument,
				"a constantly-one function has no zero witness");
		m.clear();
		while (!leaf(x)) {
			const bdd_node_t n = get_node(x);
			const bool high = n.l == T;
			m.emplace(static_cast<int_t>(n.v), high);
			x = high ? n.h : n.l;
		}
		DBG(assert(x == F);)
		return r.with_assert_check_value(true);
	} else {
		if (leaf(x)) {
			if (!(get_elem(x) == false))
				return r.with_assert_check_error(code::invalid_argument,
					"a function that is zero nowhere has no zero witness");
			return m.clear(), r.with_assert_check_value(true);
		}
		const bdd_node_t n = get_node(x);
		// Boole: v h | v' l is zero at v = l wherever h l is zero.
		TAU_TRY([[maybe_unused]] bool ok, get_one_zero(bdd_and(n.h, n.l), m));
		// h l may not mention a variable of l; any value does for it.
		std::set<int_t> vars;
		get_vars(n.l, vars);
		for (int_t v : vars) m.emplace(v, B::zero());
		m.emplace((int_t) n.v, eval(n.l, m));
		return r.with_assert_check_value(true);
	}
}

} // namespace idni::tau_lang

#undef neg_to_odd
#undef hash_pair
#undef hash_tri
#undef hash_upair
#undef hash_utri

#endif // __IDNI__TAU__BACKENDS__BDDS__BABDD_H__
