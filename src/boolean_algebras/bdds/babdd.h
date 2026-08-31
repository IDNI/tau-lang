// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BDDS__BABDD_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BDDS__BABDD_H__

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

#include "boolean_algebras/bdds/var_dict.h"
#include "boolean_algebras/bool_ba.h"

namespace idni::tau_lang {

typedef int32_t int_t;
typedef uint32_t uint_t;
template<typename T> using sp = std::shared_ptr<T>;

#define neg_to_odd(x) (((x)<0?(((-(x))<<1)+1):((x)<<1)))
#define hash_pair(x, y) fpairing(neg_to_odd(x), neg_to_odd(y))
#define hash_tri(x, y, z), N fpairing(hash_pair(x, y), neg_to_odd(z))
#define hash_upair(x, y) fpairing(x, y)
#define hash_utri(x, y, z) fpairing(hash_upair(x, y), z)

// Cantor pairing function: maps a pair of naturals to a single
// natural injectively; used as the hash combiner for node hashing.
inline size_t fpairing(size_t x, size_t y) {
	const size_t z = x + y;
	return y+((z * (z + 1))>>1);
}

/* Functionality options for bdd library
 * - use of input inverters
 * - use of output inverters
 * - use of variable shifters
 */
enum bdd_params {
	INV_IN = (1u << 0),
	INV_OUT = (1u << 1),
	VARSHIFT = (1u << 2),
	INV_ORDER = (1u << 3)
};

/* Options for bdd instantiation. The class handles dependencies and restrictions
 * between parameters. Create by static functions bdd_options::create.
 */
template<uint8_t params = INV_IN | INV_OUT | VARSHIFT>
class bdd_options {
	constexpr bdd_options(auto idWidth, auto shiftWidth) :
		idW(idWidth), shiftW(shiftWidth) {}

public:
	// bit width for id and shift
	const uint8_t idW;
	const uint8_t shiftW;

	constexpr static bdd_options
	create(const auto idWidth, const auto shiftWidth) {
		if constexpr (params & VARSHIFT) {
			DBG(assert(idWidth + shiftWidth <= 62));
			return bdd_options(idWidth, shiftWidth);
		} else {
			DBG(assert(idWidth <= 62));
			return bdd_options(idWidth, 0);
		}
	}

	constexpr static bdd_options create() {
		if constexpr (params & VARSHIFT)
			return bdd_options(18, 12);
		else return bdd_options(30, 0);
	}

	constexpr bool has_varshift() const {
		return params & static_cast<uint8_t>(VARSHIFT);
	}
	constexpr bool has_inv_in() const {
		return params & static_cast<uint8_t>(INV_IN);
	}
	constexpr bool has_inv_out() const {
		return params & static_cast<uint8_t>(INV_OUT);
	}
	constexpr bool has_inv_order() const {
		return params & static_cast<uint8_t>(INV_ORDER);
	}
};

/* Defines the reference type to reference a bdd_node in the bdd universe.
 *
 * Packed encoding (primary template, SHIFTED == true):
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
 * - INV_ORDER (descending order): relative = v + 1 - a; the map is its
 *   own inverse, so to_shift_node and to_bdd_node use the same formula.
 * - otherwise (ascending order):  relative = a - (v - 1).
 */
template<bool SHIFTED, bool INV_ORDER, int_t ID_WIDTH, int_t SHIFT_WIDTH>
struct bdd_reference {
	typedef std::conditional<ID_WIDTH + SHIFT_WIDTH <= 30,
					uint32_t, uint64_t>::type ref_type;
	ref_type in: 1 = 0;
	ref_type out: 1 = 0;
	ref_type shift: SHIFT_WIDTH = 0;
	ref_type id: ID_WIDTH = 0;

	bdd_reference() = default;
	bdd_reference(auto in, auto out, auto id) : in(in), out(out), id(id) {}
	bdd_reference(auto in, auto out, auto shift, auto id) : in(in), out(out),
								shift(shift),
								id(id) {}

	bool operator==(const bdd_reference x) const {
		return in == x.in && out == x.out && shift == x.shift &&
		       id == x.id;
	}

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
	static bdd_reference to_shift_node(const bdd_reference x, uint_t v) {
		if(x.shift == 0) return x;
		if constexpr (INV_ORDER)
			return bdd_reference(x.in, x.out, v + 1 - x.shift, x.id);
		else return bdd_reference(x.in, x.out, x.shift - (v - 1), x.id);
	}

	// Inverse of to_shift_node: restore a parent-relative child
	// reference to absolute variable numbering, v being the parent's
	// variable; used when decoding a stored skeleton.
	static bdd_reference to_bdd_node(const bdd_reference x, uint_t v) {
		if(x.shift == 0) return x;
		if constexpr (INV_ORDER)
			return bdd_reference(x.in, x.out, v - x.shift + 1, x.id);
		else return bdd_reference(x.in, x.out, x.shift + (v - 1), x.id);
	}

	// Rebase x relative to reference level s (the topmost variable
	// involved in a cached operation) for use as a cache key, so one
	// cache entry serves every uniformly shifted instance. INV_ORDER
	// only; requires s >= x.shift.
	static bdd_reference to_cache_node(const bdd_reference x, int_t s) {
		static_assert(INV_ORDER);
		if (x.shift == 0) return x;
		DBG(assert(s >= x.shift));
		return bdd_reference(x.in, x.out, abs(x.shift - s - 1), x.id);
	}

	// Inverse of to_cache_node: rebase a cached result back to the
	// query's reference level s. INV_ORDER only.
	static bdd_reference from_cache_node(const bdd_reference x, int_t s) {
		static_assert(INV_ORDER);
		if (x.shift == 0) return x;
		return bdd_reference(x.in, x.out, s - x.shift + 1, x.id);
	}

	static size_t hash(const bdd_reference x) {
		size_t seed = 0;
		hash_combine(seed, x.id + x.in);
		hash_combine(seed, x.shift + x.out);
		return seed;
	}
};

/* Specialization for SHIFTED == false (no variable shifters): a
 * reference is just the universe index plus the input/output inverter
 * bits; nodes carry their variable themselves, so there is no shift
 * field and no rebasing arithmetic.
 */
template<bool INV_ORDER, int_t ID_WIDTH, int_t SHIFT_WIDTH>
struct bdd_reference<false, INV_ORDER, ID_WIDTH, SHIFT_WIDTH> {
	typedef std::conditional<ID_WIDTH <= 30,
					uint32_t, uint64_t>::type ref_type;
	ref_type in: 1 = 0;
	ref_type out: 1 = 0;
	ref_type id: ID_WIDTH = 0;

	bdd_reference() = default;
	bdd_reference(auto in, auto out, auto id) : in(in), out(out), id(id) {}

	bool operator==(const bdd_reference x) const {
		return in == x.in && out == x.out && id == x.id;
	}

	auto operator<=>(const bdd_reference x) const {
		if (in != x.in) return (int)in <=> (int)x.in;
		if (out != x.out) return (int)out <=> (int)x.out;
		return (long long)id <=> (long long)x.id;
	}

	static bdd_reference flip_in(const bdd_reference x) {
		return bdd_reference(x.in == 1 ? 0 : 1, x.out, x.id);
	}

	static bdd_reference flip_out(const bdd_reference x) {
		return bdd_reference(x.in, x.out == 1 ? 0 : 1, x.id);
	}

	static size_t hash(const bdd_reference x) {
		return (x.id + x.in) ^ x.out;
	}
};

/* A BDD decision node: variable v with high child h (taken when v is
 * true) and low child l (v false), plus a precomputed hash. R is the
 * reference type. Stored directly in the universe when shifters are
 * off; with shifters it is the decoded, absolute-variable view that
 * bdd::get() reconstructs from a stored node_skeleton.
 */
template<typename R>
struct bdd_node {
	bdd_node(uint_t v, R h, R l) :
		v(v), h(h), l(l),
		hash(hash_utri(v, R::hash(h), R::hash(l))) {}
	uint_t v;
	R h, l;
	size_t hash;
	bool operator==(const auto& x) const {
		return hash == x.hash && v == x.v && h == x.h && l == x.l;
	}
};

/* This class represents a bdd_node in case variable shifters are used
 * R is the reference type for the high and low pointer
 */
template<typename R>
struct node_skeleton {
	node_skeleton(R h, R l) :
		h(h), l(l), hash(hash_upair(R::hash(h), R::hash(l))) {}
	R h, l;
	size_t hash;
	bool operator==(const auto& x) const {
		return hash == x.hash && h == x.h && l == x.l;
	}
};

} // namespace idni::tau_lang

namespace std {

template<typename R>
struct hash<idni::tau_lang::bdd_node<R>> {
	size_t operator()(auto& n) const { return n.hash; }
};

template<typename R>
struct hash<idni::tau_lang::node_skeleton<R>> {
	size_t operator()(auto& n) const { return n.hash; }
};

template<bool S, bool O, idni::tau_lang::int_t IW, idni::tau_lang::int_t SW>
struct hash<array<idni::tau_lang::bdd_reference<S, O, IW, SW>, 2>> {
	size_t operator()(const auto& a) const {
		return idni::tau_lang::hash_upair(
			(idni::tau_lang::bdd_reference<S, O, IW, SW>::hash(a[0])),
			(idni::tau_lang::bdd_reference<S, O, IW, SW>::hash(a[1])));
	}
};

template<bool S, bool O, idni::tau_lang::int_t IW, idni::tau_lang::int_t SW>
struct hash<idni::tau_lang::bdd_reference<S, O, IW, SW>> {
	size_t operator()(const auto& a) const {
		return idni::tau_lang::bdd_reference<S, O, IW, SW>::hash(a);
	}
};

template<bool S, bool O, idni::tau_lang::int_t IW, idni::tau_lang::int_t SW>
struct hash<pair<idni::tau_lang::bdd_reference<S, O, IW, SW>, idni::tau_lang::uint_t>> {
	size_t operator()(const auto& p) const {
		return idni::tau_lang::hash_upair(
			(hash<idni::tau_lang::bdd_reference<S, O, IW, SW>>{}(p.first)),
			p.second);
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
 * a bdd value is either a decision node or a constant of B.
 *
 * Ownership/interning model: all state is static per (B, o)
 * instantiation. V is the append-only universe owning every node
 * (variable-free node_skeletons when shifters are on, full bdd_nodes
 * otherwise) and every interned leaf constant; Mn and Mb map nodes and
 * constants back to their V index so structurally equal bdds are
 * stored exactly once. Nodes are never freed. All functions are
 * denoted by lightweight bdd_ref values (index + inverter bits +
 * shift, see bdd_reference); T and F are the canonical constants.
 * The universe is seeded once by the static initializer I (bdd_init).
 *
 * Operation results are memoized in the *_memo caches; with shifters,
 * cache keys are shift-normalized (see check_cache) so entries are
 * shared between uniformly shifted instances of the same structure.
 */
template<typename B, bdd_options o = bdd_options<>::create()>
struct bdd : std::variant<bdd_node<bdd_reference<o.has_varshift(), o.has_inv_order(), o.idW, o.shiftW>>, B> {
	using bdd_ref = bdd_reference<o.has_varshift(), o.has_inv_order(), o.idW, o.shiftW>;
	typedef bdd_node<bdd_ref> bdd_node_t;
	typedef std::variant<bdd_node_t, B> base;

	// This class is placed into the bdd universe if variable shifters are used
	struct bdd_skeleton : std::variant<node_skeleton<bdd_ref>, B> {
		using base = std::variant<node_skeleton<bdd_ref>, B>;
		bdd_skeleton(bdd_ref h, bdd_ref l) : base(node_skeleton<bdd_ref>(h, l)) {}
		explicit bdd_skeleton(const auto& n) : base(n) {}

		bool leaf() const { return holds_alternative<B>(*this); }
	};

	bdd(uint_t v, bdd_ref h, bdd_ref l) : base(bdd_node_t(v, h, l)) {}
	explicit bdd(const auto& n) : base(n) {}
	//bdd(const B& b) : base(b) {}

	// true iff this value / the referenced entry is a leaf constant
	bool leaf() const { return holds_alternative<B>(*this); }
	static bool leaf(bdd_ref n) { return holds_alternative<B>(V[n.id]); }
	// Variable-order comparator: var_cmp(a, b) is true when a comes
	// strictly before b in the BDD order (ascending by default,
	// descending with INV_ORDER)
	static bool (*var_cmp)(int, int);

	// Static-init hook: constructing I runs bdd_init<B, o>() once
	struct initializer { initializer(); };

	// The node universe: owns every node and interned leaf constant
	inline static std::conditional<o.has_varshift(),
			std::vector<bdd_skeleton>, std::vector<bdd>>::type V;
	// Interning map: node -> its index in V
	inline static std::conditional<o.has_varshift(),
			std::unordered_map<node_skeleton<bdd_ref>, size_t>,
			std::unordered_map<bdd_node_t, size_t>>::type Mn;
	// Interning map: leaf constant of B -> its index in V
	inline static std::map<B, size_t> Mb;
	// Canonical references to the constants one and zero
	inline static bdd_ref T, F;
	inline static initializer I;

	// Caches for bdd operations
	inline static std::unordered_map<std::array<bdd_ref,2>, bdd_ref> and_memo;
	inline static std::unordered_map<std::array<bdd_ref,2>, bdd_ref> or_memo;
	inline static std::unordered_map<bdd_ref, bdd_ref> not_memo;
	inline static std::unordered_map<std::pair<bdd_ref, uint_t>, bdd_ref> ex_memo;
	inline static std::unordered_map<std::pair<bdd_ref, uint_t>, bdd_ref> all_memo;

	/* Memo lookup/store with shift-normalized keys.
	 *
	 * With variable shifters, structurally equal bdds rooted at
	 * different variables share one skeleton and differ only in the
	 * shift fields, and the operations below commute with a uniform
	 * variable shift. Keys (operands, and for ex/all the quantified
	 * variable) are therefore rebased so the topmost variable of the
	 * operation becomes canonical, letting a single cache entry serve
	 * every shifted instance; the cached result is stored rebased the
	 * same way and is rebased back to the query's level on a hit.
	 * Variants: (x) rebases by x's own top variable; (x, v) also
	 * rebases the quantified variable v; (x, y) rebases both operands
	 * by their common reference level d (the first variable in the
	 * BDD order: max of the shifts with INV_ORDER, min otherwise), so
	 * only the operands' relative offset is keyed. Without shifters
	 * the keys are used as-is. check_cache returns true on a hit and
	 * replaces x with the (rebased) cached result.
	 */
	static bool check_cache(bdd_ref& x, const auto& cache) {
		if constexpr (o.has_varshift() && o.has_inv_order()) {
			if (auto it = cache.find(bdd_ref::to_cache_node(x, x.shift));
				it != cache.end()) {
				x = bdd_ref::from_cache_node(it->second, x.shift);
				return true;
			} else return false;
		}
		if constexpr (o.has_varshift()) {
			if (auto it = cache.find(bdd_ref::to_shift_node(x, x.shift));
				it != cache.end()){
				x = bdd_ref::to_bdd_node(it->second, x.shift);
				return true;
			} else return false;
		} else {
			if (auto it = cache.find(x); it != cache.end())
				return x = it->second, true;
			else return false;
		}
	}

	static bool check_cache(bdd_ref& x, bdd_ref y, const auto& cache) {
		if constexpr (o.has_varshift() && o.has_inv_order()) {
			auto d = max(x.shift, y.shift);
			if (auto it = cache.find({bdd_ref::to_cache_node(x, d),
						  bdd_ref::to_cache_node(y, d)});
				it != cache.end()) {
				x = bdd_ref::from_cache_node(it->second, d);
				return true;
			} else return false;
		}
		if constexpr (o.has_varshift()) {
			uint_t d = min(x.shift, y.shift);
			if (auto it = cache.find({bdd_ref::to_shift_node(x, d),
						  bdd_ref::to_shift_node(y, d)});
				it != cache.end()) {
				x = bdd_ref::to_bdd_node(it->second, d);
				return true;
			} else return false;
		} else {
			if (auto it = cache.find({x,y}); it != cache.end())
				return x = it->second, true;
			else return false;
		}
	}

	static bool check_cache(bdd_ref& x, uint_t v, const auto& cache){
		if constexpr (o.has_varshift() && o.has_inv_order()) {
			if (auto it = cache.find(
					{bdd_ref::to_cache_node(x, x.shift),
					 abs((int_t)v - x.shift - 1)}); it != cache.end()) {
				x = bdd_ref::from_cache_node(it->second, x.shift);
				return true;
			} else return false;
		}
		if constexpr (o.has_varshift()) {
			if (auto it = cache.find(
					{bdd_ref::to_shift_node(x, x.shift),
					 v - (x.shift - 1)}); it != cache.end()) {
				x = bdd_ref::to_bdd_node(it->second, x.shift);
				return true;
			} else return false;
		} else {
			if (auto it = cache.find({x,v}); it != cache.end())
				return x = it->second, true;
			else return false;
		}
	}

	static void update_cache(bdd_ref x, bdd_ref r, auto& cache) {
		if constexpr (o.has_varshift() && o.has_inv_order()) {
			cache.emplace(bdd_ref::to_cache_node(x, x.shift),
				      bdd_ref::to_cache_node(r, x.shift));
		}
		if constexpr (o.has_varshift()) {
			cache.emplace(bdd_ref::to_shift_node(x, x.shift),
				      bdd_ref::to_shift_node(r, x.shift));
		} else cache.emplace(move(x), move(r));
	}

	static void update_cache(bdd_ref x, uint_t v, bdd_ref r, auto& cache) {
		if constexpr (o.has_varshift() && o.has_inv_order()) {
			cache.emplace(std::pair<bdd_ref, uint_t>{
					      bdd_ref::to_cache_node(x, x.shift),
					      abs((int_t)v - x.shift - 1)},
				      bdd_ref::to_cache_node(r, x.shift));
		}
		if constexpr (o.has_varshift()) {
			cache.emplace(std::pair<bdd_ref, uint_t>{
					      bdd_ref::to_shift_node(x, x.shift),
					      v - (x.shift - 1)},
				      bdd_ref::to_shift_node(r, x.shift));
		} else cache.emplace(std::pair<bdd_ref, uint_t>{move(x), v}, move(r));
	}

	static void update_cache(bdd_ref x, bdd_ref y, bdd_ref r, auto &cache) {
		if constexpr (o.has_varshift() && o.has_inv_order()) {
			auto d = max(x.shift, y.shift);
			cache.emplace(std::array<bdd_ref, 2>{
					      bdd_ref::to_cache_node(x, d),
					      bdd_ref::to_cache_node(y, d)},
				      bdd_ref::to_cache_node(r, d));
		}
		if constexpr (o.has_varshift()) {
			uint_t d = min(x.shift, y.shift);
			cache.emplace(std::array<bdd_ref, 2>{
					      bdd_ref::to_shift_node(x, d),
					      bdd_ref::to_shift_node(y, d)},
				      bdd_ref::to_shift_node(r, d));
		} else cache.emplace(std::array<bdd_ref,2>{move(x),move(y)},move(r));
	}

	// Canonicalize operand order of a commutative operation (sort by
	// id, then shift) so both argument orders hit one cache entry
	static void mk_order_canonical(bdd_ref& x, bdd_ref& y) {
		if constexpr (o.has_varshift())
			if (x.id == y.id && x.shift > y.shift) swap(x, y);
		if(x.id > y.id) swap(x,y);
	}

	// Fast check if output inverters are enabled
	static bool negation_of(bdd_ref x, bdd_ref y) {
		if constexpr (o.has_inv_out()) {
			return bdd_ref::flip_out(x) == y;
		} else return false;
	}

	static bdd_ref add(const bdd_node_t& n) { return add(n.v, n.h, n.l); }

	// Canonicalizing node constructor ("mk"): returns the unique
	// reference denoting (v ? h : l). Collapses h == l; with input
	// inverters orders the children by id, recording a swap in the
	// reference's in bit; with output inverters normalizes the stored
	// node so its low child is non-inverted, moving the complement
	// into the reference's out bit -- so a function and its
	// complement share one stored node.
	static bdd_ref add(uint_t v, bdd_ref h, bdd_ref l) {
#ifdef DEBUG
		assert(V.size() < pow(2, o.idW));
		if constexpr (o.has_varshift()) assert(v < pow(2, o.shiftW));
#endif
		if (h == l) return h;
#ifdef DEBUG
		auto p = get(l), q = get(h);
		if constexpr (!o.has_inv_in()) {
			if (!p.leaf()) assert(var_cmp(v, std::get<bdd_node_t>(p).v));
			if (!q.leaf()) assert(var_cmp(v, std::get<bdd_node_t>(q).v));
		}
#endif
		bool in = false, out = false;
        	if constexpr (o.has_inv_in())
			if(h.id < l.id)
				swap(h, l), in = true;
		if constexpr (o.has_inv_out())
			if (l.out) {
				h = bdd_ref::flip_out(h);
				l = bdd_ref::flip_out(l);
				out = true;
			}
		if constexpr (o.has_varshift()) return add_with_shift(v, h, l, in, out);
		else return add_without_shift(v, h, l, in, out);
	}

	// Intern the canonicalized node (v, h, l) in Mn/V and return a
	// reference carrying the in/out inverter flags computed by add()
	static bdd_ref
	add_without_shift(uint_t v, const bdd_ref &h, const bdd_ref &l, bool in,
			  bool out) {
		bdd_node_t n(v, h, l);
		if (auto it = Mn.find(n); it != Mn.end())
			return bdd_ref(in, out, it->second);
		Mn.emplace(n, V.size());
		V.emplace_back(n);
		return bdd_ref(in, out, V.size() - 1);
	}

	// Shifted variant of add_without_shift: rebases the children to
	// v-relative form and interns the variable-free skeleton; the
	// variable v travels in the returned reference's shift field
	static bdd_ref
	add_with_shift(uint_t v, bdd_ref &h, bdd_ref &l, bool in, bool out) {
		h = bdd_ref::to_shift_node(h, v);
		l = bdd_ref::to_shift_node(l, v);
		node_skeleton<bdd_ref> n(h, l);
		if (auto it = Mn.find(n); it != Mn.end())
			return bdd_ref(in, out, v, it->second);
		Mn.emplace(n, V.size());
		V.emplace_back(n);
		return bdd_ref(in, out, v, V.size() - 1);
	}

	// Intern the leaf constant b and return its reference; false and
	// true map to F and T, and with output inverters an already
	// interned ~b is reused via the out bit
	static bdd_ref add(const B& b) {
		DBG(assert(V.size() < pow(2, o.idW)));
		if (b == false) return F;
		if (b == true) return T;
		if (auto it = Mb.find(b); it != Mb.end()) return bdd_ref(0,0,it->second);
		else if constexpr (o.has_inv_out())
			if ((it = Mb.find(~b)) != Mb.end()) return bdd_ref(0,1,it->second);
		Mb.emplace(b, V.size());
		V.emplace_back(b);
		return bdd_ref(0, 0, V.size()-1);
	}

	// Decode a reference into the bdd value it denotes: fetch the
	// stored entry (restoring the children's absolute variables from
	// a skeleton), then apply the out inverter (complement the leaf,
	// or flip the children's out bits) and the in inverter (swap the
	// children)
	static bdd get(bdd_ref n) {
		constexpr auto get_bdd_node = [](const bdd_ref n) {
			if constexpr (o.has_varshift()) {
				const bdd_skeleton& s = V[n.id];
				if(s.leaf()) return bdd(std::get<B>(s));
				const auto& x = std::get<node_skeleton<bdd_ref>>(s);
				return bdd(n.shift, bdd_ref::to_bdd_node(x.h, n.shift),
					   bdd_ref::to_bdd_node(x.l, n.shift));
			} else return V[n.id];
		};
#ifdef DEBUG
		if constexpr (!o.has_inv_out()) assert(!n.out);
		if constexpr (!o.has_inv_in()) assert(!n.in);
#endif
		const auto& b = get_bdd_node(n);
		if constexpr (!o.has_inv_out())
			if (n.id == 0) return b;
		if (!n.out) {
			if constexpr (!o.has_inv_in()) return b;
			if(leaf(n)) return b;
			const auto &t = std::get<bdd_node_t>(b);
			return !n.in ? b : bdd(t.v, t.l, t.h);
		}
		if (leaf(n)) return bdd(~std::get<B>(b));
		const auto& t = std::get<bdd_node_t>(b);
		return !n.in ? bdd(t.v, bdd_ref::flip_out(t.h), bdd_ref::flip_out(t.l)) :
		       bdd(t.v, bdd_ref::flip_out(t.l), bdd_ref::flip_out(t.h));
	}

	// Decode a reference known to denote a leaf / a decision node
	static B get_elem(bdd_ref x) { return std::get<B>(get(x)); }
	static bdd_node_t get_node(bdd_ref x) { return std::get<bdd_node_t>(get(x)); }

	// The literal of variable |v|: positive v gives the function "v",
	// negative v its complement
	static bdd_ref bit(int_t v) {
		// Avoid later name clash by adding any new variable to dictionary
		var_dict(v>0?v:-v);
		return v > 0 ? add(v, T, F) : add(-v, F, T);
	}

	// Complement: O(1) out-bit flip with output inverters, otherwise
	// a memoized recursion negating the leaves in B
	static bdd_ref bdd_not(bdd_ref x) {
		if constexpr (o.has_inv_out()) return bdd_ref::flip_out(x);
		if (x == T) return F;
		if (x == F) return T;
		if (check_cache(x, not_memo)) return x;

		const bdd& xx = get(x);
		if(xx.leaf()) {
			auto r = add(~std::get<B>(xx));
			return update_cache(x, r, not_memo), r;
		}
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		auto r = add(nx.v, bdd_not(nx.h), bdd_not(nx.l));
		return update_cache(x, r, not_memo), r;
	}

	// Conjunction with a constant of B, pushed down into the leaves
	static bdd_ref bdd_and(bdd_ref x, const B& b) {
		if (x == T) return add(b);
		if (x == F) return F;
		if (check_cache(x, add(b), and_memo)) return x;
		const bdd& xx = get(x);
		if (xx.leaf()) return add(b & std::get<B>(xx));
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
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
		const bdd &xx = get(x), &yy = get(y);
		if (xx.leaf()) return bdd_and(y, std::get<B>(xx));
		if (yy.leaf()) return bdd_and(x, std::get<B>(yy));
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		const bdd_node_t &ny = std::get<bdd_node_t>(yy);
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

	// Disjunction with a constant of B, pushed down into the leaves
	static bdd_ref bdd_or(bdd_ref x, const B& b) {
		if (x == T) return T;
		if (x == F) return add(b);
		if (check_cache(x, add(b), or_memo)) return x;
		const bdd& xx = get(x);
		if (xx.leaf()) {
			return add(b | std::get<B>(xx));
		}
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		bdd_ref y = add(nx.v, bdd_or(nx.h, b), bdd_or(nx.l, b));
#ifdef DEBUG
		if (b == true) assert(y == T);
#endif
		return update_cache(x, add(b), y, or_memo), y;
	}

	// Disjunction: derived from bdd_and by De Morgan when output
	// inverters make complement free, otherwise the dual of bdd_and
	static bdd_ref bdd_or(bdd_ref x, bdd_ref y){
		if constexpr (o.has_inv_out())
			return bdd_ref::flip_out(
				bdd_and(bdd_ref::flip_out(x), bdd_ref::flip_out(y)));
		if (x == T || y == T) return T;
		if (x == F) return y;
		if (y == F) return x;
		if (negation_of(x, y)) return T;
		mk_order_canonical(x,y);
		if (check_cache(x, y, or_memo)) return x;
		const bdd &xx = get(x), &yy = get(y);
		if (xx.leaf()) return bdd_or(y, std::get<B>(xx));
		if (yy.leaf()) return bdd_or(x, std::get<B>(yy));
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		const bdd_node_t &ny = std::get<bdd_node_t>(yy);
		bdd_ref r;
		if (nx.v == ny.v)
			r =	add(nx.v,
				       bdd_or(nx.h, ny.h),
				       bdd_or(nx.l, ny.l));
		else if (var_cmp(nx.v, ny.v))
			r = add(nx.v, bdd_or(nx.h, y), bdd_or(nx.l, y));
		else r = add(ny.v, bdd_or(ny.h, x), bdd_or(ny.l, x));
		return update_cache(x, y, r, or_memo), r;
	}

	// Existential quantification of variable v: the v node is
	// replaced by the disjunction of its branches; memoized
	static bdd_ref ex(bdd_ref x, uint_t v) {
		if (check_cache(x, v, ex_memo)) return x;
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		bdd_ref r;
		if (var_cmp(nx.v, v)) r = add(nx.v, ex(nx.h, v), ex(nx.l, v));
		else if (var_cmp(v, nx.v)) r = x;
		else r = bdd_or(nx.h, nx.l);
		update_cache(x, v, r, ex_memo);
		return r;
	}

	// Universal quantification of variable v: the v node is replaced
	// by the conjunction of its branches; memoized
	static bdd_ref all(bdd_ref x, uint_t v) {
		if (check_cache(x, v, all_memo)) return x;
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		bdd_ref r;
		if (var_cmp(nx.v, v)) r = add(nx.v, all(nx.h, v), all(nx.l, v));
		else if (var_cmp(v, nx.v)) r = x;
		else r = bdd_and(nx.h, nx.l);
		update_cache(x, v, r, all_memo);
		return r;
	}

	// Value of x universally quantified over all of its variables:
	// the conjunction of all leaf constants (short-circuits at zero)
	static B get_uelim(bdd_ref x) {
		const bdd &xx = get(x);
		if (xx.leaf()) return std::get<B>(xx);
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		if (B r = get_uelim(nx.h); r == false) return r;
		else return r & get_uelim(nx.l);
	}

	// Value of x existentially quantified over all of its variables:
	// the disjunction of all leaf constants (short-circuits at one)
	static B get_eelim(bdd_ref x) {
		const bdd &xx = get(x);
		if (xx.leaf()) return std::get<B>(xx);
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		if (B r = get_eelim(nx.h); r == true) return r;
		else return r | get_eelim(nx.l);
	}

	// Substitute the function `with` for variable v: at the v node
	// returns ite(with, high, low)
	static bdd_ref subst(bdd_ref x, uint_t v, bdd_ref with) {
		const bdd& xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node_t& nx = std::get<bdd_node_t>(xx);
		if (var_cmp(nx.v, v))
			return add(nx.v, subst(nx.h, v, with), subst(nx.l, v, with));
		if (var_cmp(v, nx.v)) return x;
		return ite(with, nx.h, nx.l);
	}

	// Restrict v := 0 (the low cofactor with respect to v)
	static bdd_ref sub0(bdd_ref x, uint_t v) {
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		if (var_cmp(nx.v, v)) return add(nx.v, sub0(nx.h, v), sub0(nx.l, v));
		if (var_cmp(v, nx.v)) return x;
		return nx.l;

	}

	// Restrict v := 1 (the high cofactor with respect to v)
	static bdd_ref sub1(bdd_ref x, uint_t v) {
		const bdd &xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node_t &nx = std::get<bdd_node_t>(xx);
		if (var_cmp(nx.v, v)) return add(nx.v, sub1(nx.h, v), sub1(nx.l, v));
		if (var_cmp(v, nx.v)) return x;
		return nx.h;
	}

	// Enumerate the DNF of x: for every path to a nonzero leaf, call
	// f with the leaf constant and the path's literals (+var for the
	// high branch, -var for the low). Stops and returns false as
	// soon as f does; v is the caller-supplied (normally empty) path
	// accumulator
	static bool dnf(bdd_ref x, std::vector<int_t>& v,
		std::function<bool(const std::pair<B, std::vector<int_t>>&)> f)
	{
		const bdd& xx = get(x);
		if (xx.leaf()) {
			if (!(std::get<B>(xx) == false))
				return f({std::get<B>(xx), v});
			return true;
		}
		const bdd_node_t& n = std::get<bdd_node_t>(xx);
		v.push_back(n.v);
		if (!dnf(n.h, v, f)) return false;
		v.back() = -n.v;
		if (!dnf(n.l, v, f)) return false;
		v.pop_back();
		return true;
	}

	// Collect variables of x into s; a subtree whose root variable is
	// already in s is not descended
	static void get_vars(bdd_ref x, std::set<int_t>& s) {
		const bdd& xx = get(x);
		if (xx.leaf()) return;
		const bdd_node_t& n = std::get<bdd_node_t>(xx);
		if (s.find(n.v) != s.end()) return;
		s.insert(n.v), get_vars(n.h, s), get_vars(n.l, s);
	}

	// if-then-else: x ? y : z
	static bdd_ref ite(bdd_ref x, bdd_ref y, bdd_ref z) {
		return bdd_or(bdd_and(x, y), bdd_and(bdd_not(x),z));
	}

	// Given x with at least one zero, fill m with a constant of B per
	// variable such that x evaluates to zero under m -- a witness
	// zero, as used by the LGRS construction (see bdd_handle::lgrs)
	static void get_one_zero(bdd_ref, std::map<int_t, B>&);

	// Simultaneously substitute the mapped functions for the mapped
	// variables, bottom-up; variables absent from m are kept
	static bdd_ref compose(bdd_ref x, const std::map<int_t, bdd_ref>& m) {
		if (leaf(x)) return x;
		const bdd_node_t& n = get_node(x);
		bdd_ref a = compose(n.h, m), b = compose(n.l, m);
		if (auto it = m.find(n.v); it == m.end())
			return ite(add(n.v, T, F), a, b);
		else return ite(it->second, a, b);
	}

	// Evaluate x to a constant of B under the total assignment m;
	// m must include all vars in x, otherwise use compose()
	static B eval(bdd_ref x, const std::map<int_t, B>& m) {
		if (leaf(x)) return get_elem(x);
		const bdd_node_t& n = get_node(x);
		B a = get_elem(eval(n.h, m)), b = get_elem(eval(n.l, m));
		if (auto it = m.find(n.v); it == m.end()) assert(0);
		else return ite(it->second, a, b);
	}

	// Conjunction of a leaf constant and signed literals -- the
	// inverse of one dnf() callback
	static bdd_ref from_clause(const std::pair<B, std::vector<int_t>>& v) {
		bdd_ref r = bdd_and(T, v.first);
		for (int_t t : v.second) r = bdd_and(r, bit(t));
		return r;
	}

	// Disjunction of clauses -- the inverse of dnf()
	static bdd_ref from_dnf(
		const std::set<std::pair<B, std::vector<int_t>>>& s)
	{
		bdd_ref r = F;
		for (auto& x : s) r = bdd_or(r, from_clause(x));
		return r;
	}

	// treat x as a *disjoint* union of elements of s
	static bdd_ref split(bdd_ref x, const B& e, const std::set<B>& s) {
		std::set<std::pair<B, std::vector<int_t>>> r;
		B p = get_one<B>();
		for (const B& y : s)
			if ((p = (p & ~y)) == false)
				break;
		dnf(x, [&r, &e, &s, &p](auto x) {
			if ((x.first & e) == false) r.insert(x);
			else {
				r.emplace(x.first & p, x.second);
				for (const B& y : x.second)
					r.emplace(x.first & y, x.second);
			}
		});
		return from_dnf(r);
	}

	// remove the leftmost single clause in the bdd
	static bdd_ref rm_clause(bdd_ref x) {
		if (leaf(x)) return F;
		const bdd_node_t& n = get_node(x);
		if (n.l == F) return add(n.v, rm_clause(n.h), F);
		if (leaf(n.h)) return add(n.v, n.h, rm_clause(n.l));
		return add(n.v, rm_clause(n.h), n.l);
	}

	// remove all except a single clause in the bdd
	static bdd_ref rm_all_except_one_clause(bdd_ref x) {
		if (leaf(x)) return F;
		const bdd_node_t& n = get_node(x);
		if (n.h == T) return add(n.v, T, F);
		if (n.l == T) return add(n.v, F, T);
		if (n.h == F) return add(n.v, F, rm_all_except_one_clause(n.l));
		return add(n.v, rm_all_except_one_clause(n.h), F);
	}

	// remove half of all clauses in the bdd on average
	// by simply returning the high node
	static bdd_ref rm_half_clauses(bdd_ref x) {
		if (leaf(x)) return F;
		const bdd_node_t& n = get_node(x);
		if (n.l == F) return add(n.v, rm_half_clauses(n.h), n.l);
		return add(n.v, n.h, F);
	}

	// Find highest variable name in bdd
	static uint_t highest_var(bdd_ref x) {
		const bdd& xx = get(x);
		if (xx.leaf()) return 0;
		const bdd_node_t& n = std::get<bdd_node_t>(xx);
		return max(n.v, max(highest_var(n.h), highest_var(n.l)));
	}

	// Find a variable not present in x
	static bdd_ref split_clause(bdd_ref x) {
		if constexpr (!o.has_inv_order()) {
			// First variable is smallest
			if(leaf(x)) return bdd_and(x, bit(1));
			return bdd_and(x, bit(highest_var(x) + 1));
		} else {
			// First variable is highest
			if(leaf(x)) return bdd_and(x, bit(1));
			const bdd_node_t& n = get_node(x);
			return bdd_and(x, bit(n.v + 1));
		}
	}
};

template<typename B, bdd_options o>
void bdd<B, o>::get_one_zero(bdd_ref x, std::map<int_t, B>& m) {
	assert(!leaf(x));
	const bdd_node_t& n = get_node(x);
	if (n.l == F) m.clear(), m.emplace(n.v, B::zero());
	else if (n.h == F) m.clear(), m.emplace(n.v, B::one());
	else if (!leaf(n.l))
		get_one_zero(bdd_and(n.l, n.h), m),
			m.empalce(n.v, get_elem(eval(n.l, m)));
	else if (leaf(n.h)) m.emplace(n.v, get_elem(n.l));
	else	get_one_zero(bdd_and(n.h, get_elem(n.l)), m),
			m.emplace(n.v, compose(bdd_not(n.h), m));
	DBG(assert(compose(x, m) == false);)
}

template<typename B, bdd_options o>
bool(*bdd<B, o>::var_cmp)(int_t, int_t) = [](int_t vl, int_t vr){
	if constexpr (!o.has_inv_order()) return vl < vr;
	else return vl > vr;
};

/* Specialization of the BDD engine for the two-element algebra Bool:
 * a classical BDD. The only constants are T and F, held as sentinel
 * universe entries (see create_universe(Bool)), so a bdd value IS a
 * decision node (no variant, no Mb leaf map) and leaf(ref) is an id
 * test. Members not commented here follow the contracts documented on
 * the primary template above. Additions over the primary template:
 * the n-ary conjunction bdd_and_many with its am_* machinery and the
 * and_many_memo cache.
 */

template<bdd_options o>
struct bdd<Bool, o> : bdd_node<bdd_reference<o.has_varshift(), o.has_inv_order(), o.idW, o.shiftW>> {
	using bdd_ref = bdd_reference<o.has_varshift(), o.has_inv_order(), o.idW, o.shiftW>;
	typedef bdd_node<bdd_ref> bdd_node_t;

	bdd(uint_t v, bdd_ref h, bdd_ref l) : bdd_node_t(v, h, l) {}
	explicit bdd(const auto& n) : bdd_node_t(n) {}
	struct initializer { initializer(); };

	inline static std::conditional<o.has_varshift(),
		std::vector<node_skeleton<bdd_ref>>, std::vector<bdd>>::type V;
	inline static std::conditional<o.has_varshift(),
		std::unordered_map<node_skeleton<bdd_ref>, size_t>,
		std::unordered_map<bdd_node_t, size_t>>::type Mn;
	inline static bdd_ref T, F;
	inline static initializer I;

	// Constant test by universe id: with output inverters T and F
	// share sentinel entry 0, otherwise they are entries 0 and 1
	static bool leaf (bdd_ref x) {
		if constexpr (o.has_inv_out()) return x.id == 0;
		else return x.id < 2;
	}

	static bool (*var_cmp)(int, int);
	// Coarse reference order for bdd_and_many operand lists: by id,
	// with the out-inverted twin first, so duplicates and complement
	// pairs end up adjacent after sorting (see am_sort)
	static bool (*am_cmp)(const bdd_ref&, const bdd_ref&);

	// Caches for bdd operations
	inline static std::unordered_map<std::array<bdd_ref,2>, bdd_ref> and_memo;
	inline static std::unordered_map<std::vector<bdd_ref>, bdd_ref> and_many_memo;
	inline static std::unordered_map<std::array<bdd_ref,2>, bdd_ref> or_memo;
	inline static std::unordered_map<bdd_ref, bdd_ref> not_memo;
	inline static std::unordered_map<std::pair<bdd_ref, uint_t>, bdd_ref> ex_memo;
	inline static std::unordered_map<std::pair<bdd_ref, uint_t>, bdd_ref> all_memo;

	static bool check_cache(bdd_ref& x, const auto& cache) {
		if constexpr (o.has_varshift() && o.has_inv_order()) {
			if (auto it = cache.find(bdd_ref::to_cache_node(x, x.shift));
				it != cache.end()) {
				x = bdd_ref::from_cache_node(it->second, x.shift);
				return true;
			} else return false;
		}
		if constexpr (o.has_varshift()) {
			if (auto it = cache.find(bdd_ref::to_shift_node(x, x.shift));
				it != cache.end()) {
				x = bdd_ref::to_bdd_node(it->second, x.shift);
				return true;
			} else return false;
		} else {
			if (auto it = cache.find(x); it != cache.end())
				return x = it->second, true;
			else return false;
		}
	}

	static bool check_cache(bdd_ref& x, bdd_ref y, const auto& cache) {
		if constexpr (o.has_varshift() && o.has_inv_order()) {
			auto d = max(x.shift, y.shift);
			if (auto it = cache.find({bdd_ref::to_cache_node(x, d),
						  bdd_ref::to_cache_node(y, d)});
				it != cache.end()) {
				x = bdd_ref::from_cache_node(it->second, d);
				return true;
			} else return false;
		}
		if constexpr (o.has_varshift()) {
			auto d = std::min(x.shift, y.shift);
			if (auto it = cache.find({bdd_ref::to_shift_node(x, d),
						  bdd_ref::to_shift_node(y, d)});
				it != cache.end()) {
				x = bdd_ref::to_bdd_node(it->second, d);
				return true;
			} else return false;
		} else {
			if (auto it = cache.find({x,y}); it != cache.end())
				return x = it->second, true;
			else return false;
		}
	}

	static bool check_cache(bdd_ref& x, uint_t v, const auto& cache){
		if constexpr (o.has_varshift() && o.has_inv_order()) {
			if (auto it = cache.find(
				{bdd_ref::to_cache_node(x, x.shift),
				 abs((int_t)v - x.shift - 1)}); it != cache.end()) {
				x = bdd_ref::from_cache_node(it->second, x.shift);
				return true;
			} else return false;
		}
		if constexpr (o.has_varshift()) {
			if (auto it = cache.find(
					{bdd_ref::to_shift_node(x, x.shift),
					 v - (x.shift - 1)}); it != cache.end()) {
				x = bdd_ref::to_bdd_node(it->second, x.shift);
				return true;
			} else return false;
		} else {
			if (auto it = cache.find({x,v}); it != cache.end())
				return x = it->second, true;
			else return false;
		}
	}

	static void update_cache(bdd_ref x, bdd_ref r, auto& cache) {
		if constexpr (o.has_varshift() && o.has_inv_order()) {
			cache.emplace(bdd_ref::to_cache_node(x, x.shift),
				      bdd_ref::to_cache_node(r, x.shift));
		}
		if constexpr (o.has_varshift()) {
			cache.emplace(bdd_ref::to_shift_node(x, x.shift),
				      bdd_ref::to_shift_node(r, x.shift));
		} else cache.emplace(move(x), move(r));
	}

	static void update_cache(bdd_ref x, uint_t v, bdd_ref r, auto& cache) {
		if constexpr (o.has_varshift() && o.has_inv_order()) {
			cache.emplace(std::pair<bdd_ref, uint_t>{
				bdd_ref::to_cache_node(x, x.shift),
				abs((int_t)v - x.shift - 1)},
				      bdd_ref::to_cache_node(r, x.shift));
		}
		if constexpr (o.has_varshift()) {
			cache.emplace(std::pair<bdd_ref, uint_t>{
					      bdd_ref::to_shift_node(x, x.shift),
					      v - (x.shift - 1)},
				      bdd_ref::to_shift_node(r, x.shift));
		} else cache.emplace(std::pair<bdd_ref, uint_t>{move(x), v}, move(r));
	}

	static void update_cache(bdd_ref x, bdd_ref y, bdd_ref r, auto &cache) {
		if constexpr (o.has_varshift() && o.has_inv_order()) {
			auto d = max(x.shift, y.shift);
			cache.emplace(std::array<bdd_ref, 2>{
				bdd_ref::to_cache_node(x, d),
				bdd_ref::to_cache_node(y, d)},
				      bdd_ref::to_cache_node(r, d));
		}
		if constexpr (o.has_varshift()) {
			auto d = std::min(x.shift, y.shift);
			cache.emplace(std::array<bdd_ref, 2>{
					      bdd_ref::to_shift_node(x, d),
					      bdd_ref::to_shift_node(y, d)},
				      bdd_ref::to_shift_node(r, d));
		} else cache.emplace(std::array<bdd_ref,2>{move(x),move(y)},move(r));
	}

	static void mk_order_canonical(bdd_ref& x, bdd_ref& y) {
		if constexpr (o.has_varshift())
			if (x.id == y.id && x.shift > y.shift) std::swap(x, y);
		if(x.id > y.id) std::swap(x,y);
	}

	// Fast check only if output inverters are enabled
	static bool negation_of(bdd_ref x, bdd_ref y) {
		if constexpr (o.has_inv_out()) {
			return bdd_ref::flip_out(x) == y;
		} else return false;
	}

	static bdd_ref add(bdd_node_t b) { return add(b.v, b.h, b.l); }

	static bdd_ref add(uint_t v, bdd_ref h, bdd_ref l) {
#ifdef DEBUG
		assert(V.size() < pow(2, o.idW));
		if constexpr (o.has_varshift()) assert(v < pow(2, o.shiftW));
#endif
		if (h == l) return h;
#ifdef DEBUG
		if (!leaf(l)) assert(var_cmp(v, get(l).v));
		if (!leaf(h)) assert(var_cmp(v, get(h).v));
#endif
		bool in = false, out = false;
		if constexpr (o.has_inv_in())
			if(h.id < l.id)
				std::swap(h, l), in = true;
		if constexpr (o.has_inv_out())
			if (l.out) {
				h = bdd_ref::flip_out(h);
				l = bdd_ref::flip_out(l);
				out = true;
			}
		if constexpr (o.has_varshift()) return add_with_shift(v, h, l, in, out);
		else return add_without_shift(v, h, l, in, out);
	}

	static bdd_ref
	add_without_shift(uint_t v, const bdd_ref &h, const bdd_ref &l, bool in,
			  bool out) {
		bdd_node_t n(v, h, l);
		if (auto it = Mn.find(n); it != Mn.end())
			return bdd_ref(in, out, it->second);
		Mn.emplace(n, V.size());
		V.emplace_back(n);
		return bdd_ref(in, out, V.size() - 1);
	}

	static bdd_ref
	add_with_shift(uint_t v, bdd_ref &h, bdd_ref &l, bool in, bool out) {
		h = bdd_ref::to_shift_node(h, v);
		l = bdd_ref::to_shift_node(l, v);
		node_skeleton<bdd_ref> n(h, l);
		if (auto it = Mn.find(n); it != Mn.end())
			return bdd_ref(in, out, v, it->second);
		Mn.emplace(n, V.size());
		V.emplace_back(n);
		return bdd_ref(in, out, v, V.size() - 1);
	}

	// Decode a reference into its node view (see the primary
	// template's get); constant references decode to the degenerate
	// sentinel node, so callers test leaf(n) first where it matters
	static bdd get(bdd_ref n) {
		constexpr auto get_bdd_node = [](const bdd_ref n) {
			if constexpr (o.has_varshift()) {
				const node_skeleton<bdd_ref>& s = V[n.id];
				return bdd(n.shift, bdd_ref::to_bdd_node(s.h, n.shift),
					   bdd_ref::to_bdd_node(s.l, n.shift));
			} else return V[n.id];
		};
#ifdef DEBUG
		if constexpr (!o.has_inv_out()) assert(!n.out);
		if constexpr (!o.has_inv_in()) assert(!n.in);
#endif
		const auto& b = get_bdd_node(n);
		if (!n.out) {
			if constexpr (!o.has_inv_in()) return b;
			return !n.in ? b : bdd(b.v, b.l, b.h);
		}
		return !n.in ? bdd(b.v, bdd_ref::flip_out(b.h), bdd_ref::flip_out(b.l)) :
		       bdd(b.v, bdd_ref::flip_out(b.l), bdd_ref::flip_out(b.h));
	}

	static bdd_ref bit(int_t v) {
		// Avoid later name clash by adding any new variable to dictionary
		var_dict(v>0?v:-v);
		return v > 0 ? add(v, T, F) : add(-v, F, T);
	}

	static bdd_ref bdd_not(bdd_ref x) {
		if constexpr (o.has_inv_out()) return bdd_ref::flip_out(x);
		if (x == T) return F;
		if (x == F) return T;
		if (check_cache(x, not_memo)) return x;
		const bdd& xx = get(x);
		auto r = add(xx.v, bdd_not(xx.h), bdd_not(xx.l));
		return update_cache(x, r, not_memo), r;
	}

	static bdd_ref bdd_and(bdd_ref x, bdd_ref y) {
		if (x == F || y == F) return F;
		if (x == T) return y;
		if (y == T) return x;
		if (negation_of(x, y)) return F;
		mk_order_canonical(x,y);
		if (check_cache(x, y, and_memo)) return x;
		const bdd &nx = get(x), &ny = get(y);
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

	// Normalize a conjunction operand list: sort by am_cmp, drop T
	// and duplicates; collapse to {F} on any F or complement pair
	static void am_sort(std::vector<bdd_ref>& b) {
		sort(b.begin(), b.end(), am_cmp);
		for (size_t n = 0; n < b.size();)
			if (b[n] == T) b.erase(b.begin() + n);
			else if (b[n] == F) { b = {F}; return; }
			else if (!n) { ++n; continue; }
			else if (b[n] == b[n-1]) b.erase(b.begin() + n);
			else if (b[n] == bdd_ref::flip_out(b[n-1])) { b = {F}; return; }
			else ++n;
	}

	/* One expansion step of the n-ary conjunction: picks m, the
	 * smallest top variable among the conjuncts of v, and fills h/l
	 * with each conjunct's high/low cofactor by m (the conjunct
	 * itself when m is not its top variable). Conjuncts common to h
	 * and l are factored out: they are conjoined once recursively and
	 * the result is re-inserted into both lists. v is expected
	 * am_sorted and free of constants.
	 *
	 * Returns:
	 * - 0: neither branch resolved -- the result is
	 *      add(m, bdd_and_many(h), bdd_and_many(l))
	 * - 1: the whole conjunction is resolved; res holds the final
	 *      result (always F as written)
	 * - 2: the low branch is F -- the result is
	 *      add(m, bdd_and_many(h), F)
	 * - 3: the high branch is F -- the result is
	 *      add(m, F, bdd_and_many(l))
	 */
	static size_t bdd_and_many_iter(std::vector<bdd_ref> v,
		std::vector<bdd_ref> &h, std::vector<bdd_ref> &l, bdd_ref &res,
		uint_t &m)
	{
		size_t i;
		bool flag = false;
		m = get(v[0]).v;
		for (i = 1; i != v.size(); ++i)
			if (!leaf(v[i])) {
				if (get(v[i]).v < m) m = get(v[i]).v;
			} else if (v[i] == F) return res = F, 1;
		h.reserve(v.size()), l.reserve(v.size());
		for (i = 0; i != v.size(); ++i) {
			const auto& b = get(v[i]);
			if (b.v != m) h.push_back(v[i]);
			else if (!leaf(b.h)) h.push_back(b.h);
			else if (b.h == F) { flag = true; break; }
		}
		if (!flag) am_sort(h);
		for (i = 0; i != v.size(); ++i) {
			const auto& b = get(v[i]);
			if (b.v != m) l.push_back(v[i]);
			else if (!leaf(b.l)) l.push_back(b.l);
			else if (b.l == F) return flag ? res = F, 1 : 2;
		}
		am_sort(l);
		if (!flag) { if (h.size() && h[0] == F) flag = true; }
		if (l.size() && l[0] == F) return flag ? 3 : 2;
		if (flag) return 3;
		std::vector<bdd_ref> x;
		set_intersection(h.begin(), h.end(), l.begin(), l.end(),
						back_inserter(x), am_cmp);
		am_sort(x);
		if (x.size() > 1) {
			for (size_t n = 0; n < h.size();)
				if (hasbc(x, h[n], am_cmp)) h.erase(h.begin() + n);
				else ++n;
			for (size_t n = 0; n < l.size();)
				if (hasbc(x, l[n], am_cmp)) l.erase(l.begin() + n);
				else ++n;
			h.shrink_to_fit(), l.shrink_to_fit(), x.shrink_to_fit();
			bdd_ref r = bdd_and_many(move(x));
			if (r == F) return res = F, 1;
			if (r != T) {
				if (!hasbc(h, r, am_cmp)) h.push_back(r), am_sort(h);
				if (!hasbc(l, r, am_cmp)) l.push_back(r), am_sort(l);
			}
		}
		return 0;
	}

	// n-ary conjunction: repeatedly collapses operand pairs already
	// in the binary and_memo, normalizes via am_sort, then Shannon-
	// expands on the smallest top variable via bdd_and_many_iter;
	// results are memoized per operand list in and_many_memo
	static bdd_ref bdd_and_many(std::vector<bdd_ref> v) {
		if (v.empty()) return T;
		if (v.size() == 1) return v[0];

		for (size_t n = 0; n < v.size(); ++n)
			for (size_t k = 0; k < n; ++k) {
				bdd_ref x = v[n], y = v[k];
				mk_order_canonical(x, y);
				if (check_cache(x, y, and_memo)) {
					v.erase(v.begin()+k), v.erase(v.begin()+n-1);
					v.push_back(x);
					n = k = 0;
					break;
				}
			}
		am_sort(v);
		if (v.empty()) return T;
		if (v.size() == 1) return v[0];
		auto it = and_many_memo.find(v);
		if (it != and_many_memo.end()) return it->second;
		if (v.size() == 2)
			return and_many_memo.emplace(v, bdd_and(v[0], v[1])).first->second;
		bdd_ref res = F, h, l;
		uint_t m = 0;
		std::vector<bdd_ref> vh, vl;
		switch (bdd_and_many_iter(v, vh, vl, res, m)) {
			case 0: l = bdd_and_many(move(vl)),
					h = bdd_and_many(move(vh));
				break;
			case 1: return and_many_memo.emplace(v, res), res;
			case 2: h = bdd_and_many(move(vh)), l = F; break;
			case 3: h = F, l = bdd_and_many(move(vl)); break;
			default: { DBG(assert(false)); }
		}
		return and_many_memo.emplace(v, bdd::add(m, h, l)).first->second;
	}


	static bdd_ref bdd_or(bdd_ref x, bdd_ref y){
		if constexpr (o.has_inv_out())
			return bdd_ref::flip_out(
				bdd_and(bdd_ref::flip_out(x), bdd_ref::flip_out(y)));
		if (x == T || y == T) return T;
		if (x == F) return y;
		if (y == F) return x;
		if (negation_of(x, y)) return T;
		mk_order_canonical(x,y);
		if (check_cache(x, y, or_memo)) return x;
		const bdd &nx = get(x), &ny = get(y);
		bdd_ref r;
		if (nx.v == ny.v)
			r =	add(nx.v,
				       bdd_or(nx.h, ny.h),
				       bdd_or(nx.l, ny.l));
		else if (var_cmp(nx.v, ny.v))
			r = add(nx.v, bdd_or(nx.h, y), bdd_or(nx.l, y));
		else r = add(ny.v, bdd_or(ny.h, x), bdd_or(ny.l, x));
		return update_cache(x, y, r, or_memo), r;
	}

	// Existential quantification; unlike the primary template it is
	// derived from all() by De Morgan when output inverters make
	// complement free
	static bdd_ref ex(bdd_ref x, uint_t v) {
		if constexpr (o.has_inv_out())
			return bdd_ref::flip_out(all(bdd_ref::flip_out(x), v));
		if (check_cache(x, v, ex_memo)) return x;
		if (leaf(x)) return x;
		const bdd &nx = get(x);
		bdd_ref r;
		if (var_cmp(nx.v, v)) r = add(nx.v, ex(nx.h, v), ex(nx.l, v));
		else if (var_cmp(v, nx.v)) r = x;
		else r = bdd_or(nx.h, nx.l);
		update_cache(x, v, r, ex_memo);
		return r;
	}

	static bdd_ref all(bdd_ref x, uint_t v) {
		if (check_cache(x, v, all_memo)) return x;
		if (leaf(x)) return x;
		const bdd &nx = get(x);
		bdd_ref r;
		if (var_cmp(nx.v, v)) r = add(nx.v, all(nx.h, v), all(nx.l, v));
		else if (var_cmp(v, nx.v)) r = x;
		else r = bdd_and(nx.h, nx.l);
		update_cache(x, v, r, all_memo);
		return r;
	}

	static Bool get_uelim(bdd_ref x) {
		const bdd& xx = get(x);
		if (xx.leaf()) return std::get<Bool>(xx);
		const bdd_node_t& nx = std::get<bdd_node_t>(xx);
		if (Bool r = get_uelim(nx.h); r == false) return r;
		else return r & get_uelim(nx.l);
	}

	static Bool get_eelim(bdd_ref x) {
		const bdd& xx = get(x);
		if (xx.leaf()) return std::get<Bool>(xx);
		const bdd_node_t& nx = std::get<bdd_node_t>(xx);
		if (Bool r = get_eelim(nx.h); r == true) return r;
		else return r | get_eelim(nx.l);
	}

	static bdd_ref subst(bdd_ref x, uint_t v, bdd_ref with) {
		const bdd& xx = get(x);
		if (xx.leaf()) return x;
		const bdd_node_t& nx = std::get<bdd_node_t>(xx);
		if (var_cmp(nx.v, v))
			return add(nx.v, subst(nx.h, v, with), subst(nx.l, v, with));
		if (var_cmp(v, nx.v)) return x;
		return ite(with, nx.h, nx.l);
	}

	static bdd_ref sub0(bdd_ref x, uint_t v) {
		if (leaf(x)) return x;
		const bdd &nx = get(x);
		if (var_cmp(nx.v, v)) return add(nx.v, sub0(nx.h, v), sub0(nx.l, v));
		if (var_cmp(v, nx.v)) return x;
		return nx.l;
	}

	static bdd_ref sub1(bdd_ref x, uint_t v) {
		if (leaf(x)) return x;
		const bdd &nx = get(x);
		if (var_cmp(nx.v, v)) return add(nx.v, sub1(nx.h, v), sub1(nx.l, v));
		if (var_cmp(v, nx.v)) return x;
		return nx.h;
	}

	static bool dnf(bdd_ref x, std::vector<int_t>& v,
		std::function<bool(const std::pair<Bool, std::vector<int_t>>&)>
									f)
	{
		if (x == F) return true;
		if (x == T) return f({Bool(true), v});
		const bdd& n = get(x);
		v.push_back(n.v);
		if (!dnf(n.h, v, f)) return false;
		v.back() = -n.v;
		if (!dnf(n.l, v, f)) return false;
		v.pop_back();
		return true;
	}

	static void get_vars(bdd_ref x, std::set<int_t>& s) {
		if (leaf(x)) return;
		const bdd& n = get(x);
		if (s.find(n.v) != s.end()) return;
		s.insert(n.v), get_vars(n.h, s), get_vars(n.l, s);
	}

	static bdd_ref ite(bdd_ref x, bdd_ref y, bdd_ref z) {
		return bdd_or(bdd_and(x, y), bdd_and(bdd_not(x),z));
	}

	// Witness zero for Bool: walks the low branches, assigning false,
	// until a node whose high child is F (assign true) or the F leaf
	// pins x to false; the assignment in m is partial. Requires
	// x != T; throws int(0) if the walk runs into the T leaf
	static void get_one_zero(bdd_ref x, std::map<int_t, Bool>& m) {
		DBG(assert(x != T);)
		m.clear();
		while (!leaf(x)) {
			const bdd_node_t& n = get(x);
			if (n.h == F) { m.emplace(n.v, true); return; }
			m.emplace(n.v, false);
			if ((x = n.l) == F) return;
		}
		throw 0;
	}

	static bdd_ref compose(bdd_ref x, const std::map<int_t, bdd_ref>& m) {
		if (leaf(x)) return x;
		const bdd_node_t& n = get(x);
		bdd_ref a = compose(n.h, m), b = compose(n.l, m);
		if (auto it = m.find(n.v); it == m.end())
			return ite(add(n.v, T, F), a, b);
		else return ite(it->second, a, b);
	}

	// m must include all vars in x, otherwise use compose()
	static Bool eval(bdd_ref x, const std::map<int_t, Bool>& m) {
		if (x == T) return {true};
		if (x == F) return {false};
		const bdd_node_t& n = get(x);
		Bool a = eval(n.h, m), b = eval(n.l, m);
		if (auto it = m.find(n.v); it == m.end()) assert(0);
		else return ite(it->second, a, b);
	}

	static bdd_ref from_clause(const std::pair<Bool, std::vector<int_t>>& v)
	{
		bdd_ref r = bdd_and(T, v.first);
		for (int_t t : v.second) r = bdd_and(r, bit(t));
		return r;
	}

	static bdd_ref from_dnf(
		const std::set<std::pair<Bool, std::vector<int_t>>>& s)
	{
		bdd_ref r = F;
		for (auto& x : s) r = bdd_or(r, from_clause(x));
		return r;
	}

	// treat x as a *disjoint* union of elements of s
	static bdd_ref split(bdd_ref x, const Bool& e, const std::set<Bool>& s){
		std::set<std::pair<Bool, std::vector<int_t>>> r;
		Bool p = Bool(true);
		for (const Bool& y : s)
			if ((p = (p & ~y)) == false)
				break;
		dnf(x, [&r, &e, &s, &p](auto x) {
			if ((x.first & e) == false) r.insert(x);
			else {
				r.emplace(x.first & p, x.second);
				for (const Bool& y : x.second)
					r.emplace(x.first & y, x.second);
			}
		});
		return from_dnf(r);
	}

	// remove the leftmost single clause in the bdd
	static bdd_ref rm_clause(bdd_ref x) {
		if (leaf(x)) return F;
		const bdd& n = get(x);
		if (n.l == F) return add(n.v, rm_clause(n.h), F);
		if (leaf(n.h)) return add(n.v, n.h, rm_clause(n.l));
		return add(n.v, rm_clause(n.h), n.l);
	}

	// remove all except a single clause in the bdd
	static bdd_ref rm_all_except_one_clause(bdd_ref x) {
		if (leaf(x)) return F;
		const bdd& n = get(x);
		if (n.h == T) return add(n.v, T, F);
		if (n.l == T) return add(n.v, F, T);
		if (n.h == F) return add(n.v, F, rm_all_except_one_clause(n.l));
		return add(n.v, rm_all_except_one_clause(n.h), F);
	}

	// remove half of all clauses in the bdd on average
	// by simply returning the high node
	static bdd_ref rm_half_clauses(bdd_ref x) {
		if (leaf(x)) return F;
		const bdd& n = get(x);
		if (n.l == F) return add(n.v, rm_half_clauses(n.h), n.l);
		return add(n.v, n.h, F);
	}

	// Find highest variable in bdd
	static uint_t highest_var(bdd_ref x) {
		if (leaf(x)) return 0;
		const bdd& n = get(x);
		return std::max(n.v, std::max(highest_var(n.h), highest_var(n.l)));
	}

	// Find a variable not present in x
	static bdd_ref split_clause(bdd_ref x) {
		if constexpr (!o.has_inv_order()) {
			// First variable is smallest
			if(leaf(x)) return bdd_and(x, bit(1));
			return bdd_and(x, bit(highest_var(x) + 1));
		} else {
			// First variable is highest
			if(leaf(x)) return bdd_and(x, bit(1));
			const bdd& n = get(x);
			return bdd_and(x, bit(n.v + 1));
		}
	}
};

template<bdd_options o>
bool(*bdd<Bool, o>::var_cmp)(int_t, int_t) = [](int_t vl, int_t vr){
	if constexpr (!o.has_inv_order()) return vl < vr;
	else return vl > vr;
};

template<bdd_options o>
bool
(*bdd<Bool, o>::am_cmp)(const bdd_ref &, const bdd_ref &) = [](const bdd_ref &x,
							       const bdd_ref &y)
{
	bool s = x.out && !y.out;
	return x.id < y.id || (x.id == y.id && s);
};

} // namespace idni::tau_lang

#undef neg_to_odd
#undef hash_pair
#undef hash_tri
#undef hash_upair
#undef hash_utri

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BDDS__BABDD_H__
