// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BDDS__BDD_HANDLE_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BDDS__BDD_HANDLE_H__

#include "boolean_algebras/bdds/babdd.h"
#include "boolean_algebras/bdds/var_dict.h"
#include "splitter_types.h"

namespace idni::tau_lang {

template<typename B, auto o> struct bdd_handle;
template<typename B, auto o = bdd_options<>::create()>
using hbdd = sp<bdd_handle<B, o>>;

// --- Custom operators for hbdd ---
// hbdd is a shared_ptr, so these give it value semantics: the
// comparisons dereference and compare the wrapped handles (i.e. their
// bdd_ref contents); operator== also asserts the interning invariant
// that equal contents implies the very same handle object. Comparison
// against bool tests for the constant one/zero. The logical operators
// &, | and ~ forward to the handle members below, and both + and ^
// denote symmetric difference (xor), built from &, | and ~.
template<typename B, auto o = bdd_options<>::create()>
auto operator<=> (const hbdd<B, o>& x, const hbdd<B, o>& y) {
	return *x <=> *y;
}
template<typename B, auto o = bdd_options<>::create()>
auto operator< (const hbdd<B, o>& x, const hbdd<B, o>& y) {
	return (x <=> y) < 0;
}
template<typename B, auto o = bdd_options<>::create()>
auto operator<= (const hbdd<B, o>& x, const hbdd<B, o>& y) {
	return (x <=> y) <= 0;
}
template<typename B, auto o = bdd_options<>::create()>
auto operator> (const hbdd<B, o>& x, const hbdd<B, o>& y) {
	return (x <=> y) > 0;
}
template<typename B, auto o = bdd_options<>::create()>
auto operator>= (const hbdd<B, o>& x, const hbdd<B, o>& y) {
	return (x <=> y) >= 0;
}
template<typename B, auto o = bdd_options<>::create()>
auto operator== (const hbdd<B, o>& x, const hbdd<B, o>& y) {
	assert((&*x == &*y) == (x->b == y->b));
	return *x == *y;
}
template<typename B, auto o = bdd_options<>::create()>
auto operator!= (const hbdd<B, o>& x, const hbdd<B, o>& y) {
	return !(x == y);
}
// --------------------------------

template<typename B, auto o = bdd_options<>::create()>
bool operator==(const hbdd<B, o>& x, bool b) {
	return b ? x->is_one() : x->is_zero();
}

template<typename B, auto o = bdd_options<>::create()>
hbdd<B, o> operator&(const hbdd<B, o> &x, const hbdd<B, o> &y) {
	return (*x) & y;
}

template<typename B, auto o = bdd_options<>::create()>
hbdd<B, o> operator|(const hbdd<B, o> &x, const hbdd<B, o> &y) {
	return (*x) | y;
}

template<typename B, auto o = bdd_options<>::create()>
hbdd<B, o> operator+(const hbdd<B, o> &x, const hbdd<B, o> &y) {
	return (y & ~x) | (x & ~y);
}

template<typename B, auto o = bdd_options<>::create()>
hbdd<B, o> operator^(const hbdd<B, o> &x, const hbdd<B, o> &y) {
	return x + y;
}

template<typename B, auto o = bdd_options<>::create()>
hbdd<B, o> operator~(const hbdd<B, o>& x) { return ~(*x); }

/* Shared-pointer handle over the static bdd<B, o> engine -- the public
 * BDD interface used by sbf_ba and the solver. A handle wraps one
 * bdd_ref; handles are hash-consed in the static maps Mn (nodes) and
 * Mb (leaf constants), so each distinct decoded node or constant has
 * exactly one live handle and pointer equality coincides with value
 * equality (see the operator== assert above). The maps hold owning
 * shared_ptrs, so handles live for the whole process. htrue/hfalse
 * are the interned constant handles, set up by bdd_init().
 */
template<typename B, auto o = bdd_options<>::create()>
struct bdd_handle {
	using bdd_ref = bdd_reference<o.has_varshift(), o.has_inv_order(), o.idW, o.shiftW>;
	typedef bdd_node<bdd_ref> bdd_node_t;
	typedef std::unordered_map<bdd_node_t, std::shared_ptr<bdd_handle>> mn_type;
	typedef std::map<B, std::shared_ptr<bdd_handle>> mb_type;
	inline static std::unordered_map<bdd_node_t, std::shared_ptr<bdd_handle>> Mn;
	inline static std::map<B, std::shared_ptr<bdd_handle>> Mb;
	inline static hbdd<B, o> htrue, hfalse;

	// nonworking hack to call init
	template<typename T, T> struct dummy_type {};
	typedef dummy_type<mn_type&, Mn> dummy_mn_type;
	typedef dummy_type<mb_type&, Mb> dummy_mb_type;
	static bool dummy;

//	bdd_handle();
	auto operator<=>(const bdd_handle&) const = default;

	// Hash-consing factories: return THE handle for a function,
	// creating and interning it (and its bdd universe entry) on
	// first sight.

	// Handle for a decoded decision node, keyed in Mn
	static hbdd<B, o> get(const bdd_node_t& x) {
		if (auto it = Mn.find(x); it != Mn.end())
			return it->second;//.lock();
		hbdd<B, o> h = std::make_shared<bdd_handle<B, o>>(); //(new bdd_handle);
		return h->b = bdd<B, o>::add(x), Mn.emplace(x, h), h;
	}

	// Handle for a leaf constant of B, keyed in Mb
	static hbdd<B, o> get(const B& x) {
		if (auto it = Mb.find(x); it != Mb.end())
			return it->second;//.lock();
		hbdd<B, o> h = std::make_shared<bdd_handle<B, o>>();//(new bdd_handle);
		return h->b = bdd<B, o>::add(x), Mb.emplace(x, h), h;
	}

	// Handle for a bdd value: dispatch on leaf vs node
	static hbdd<B, o>
	        get(const bdd<B, o>& x) {
		return	x.leaf() ? get(std::get<B>(x))
			: get(std::get<bdd_node_t>(x));
	}

	// Handle for an engine reference: decode, then dispatch
	static hbdd<B, o> get(bdd_ref t) {
		return get(bdd<B, o>::get(t));
	}

	// Decode this handle's reference into its bdd value
	bdd<B, o> get() const {
		return bdd<B, o>::get(b);
	}

	bool is_zero() const { return b == bdd<B, o>::F; }
	bool is_one() const { return b == bdd<B, o>::T; }

	static hbdd<B, o> one() {
		return get(bdd<B, o>::T);
	}

	static hbdd<B, o> zero() {
		return get(bdd<B, o>::F);
	}

	static hbdd<B, o> bit(bool b, uint_t v) {
		DBG(assert(v > 0);)
		hbdd<B, o> r = get(bdd<B, o>::bit(b ? v : -v));
		DBG(assert(r);)
		return r;
	}

	// Constant of B this function collapses to when universally
	// (resp. existentially) quantified over all its variables: the
	// conjunction (resp. disjunction) of its leaf constants
	B get_uelim() const { return bdd<B, o>::get_uelim(b); }
	B get_eelim() const { return bdd<B, o>::get_eelim(b); }

	// Conjunction. Leaf shortcuts: a true/false operand returns the
	// other operand/hfalse without touching the engine; two leaves
	// meet directly in B; a single leaf operand is pushed into the
	// other's leaves via bdd_and(ref, B)
	hbdd<B, o> operator&(const hbdd<B, o>& x) const {
		const bdd<B, o> &xx = x->get();
		const bdd<B, o> &yy = get();
		if (xx.leaf()) {
			if (std::get<B>(xx) == true) return get(*this);
			if (std::get<B>(xx) == false) return hfalse;
			if (yy.leaf())
				return	bdd_handle<B, o>::get(
					std::get<B>(xx) & std::get<B>(yy));
			return get(bdd<B, o>::bdd_and(b, std::get<B>(xx)));
		} else if (yy.leaf()) {
			if (std::get<B>(yy) == true) return x;
			if (std::get<B>(yy) == false) return hfalse;
			return get(bdd<B, o>::bdd_and(x->b, std::get<B>(yy)));
		}
		return get(bdd<B, o>::bdd_and(x->b, b));
	}

	// Disjunction: De Morgan over & when output inverters make
	// complement free; otherwise dual leaf shortcuts to operator&
	hbdd<B, o> operator|(const hbdd<B, o>& x) const {
		if constexpr (o.has_inv_out()) return ~((~x) & (~*this));

		const bdd<B, o> &xx = x->get();
		const bdd<B, o> &yy = get();
		if (xx.leaf()) {
			if (std::get<B>(xx) == true) return htrue;
			if (std::get<B>(xx) == false) return get(*this);
			if (yy.leaf())
				return	bdd_handle<B, o>::get(
					std::get<B>(xx) | std::get<B>(yy));
			return get(bdd<B, o>::bdd_or(b, std::get<B>(xx)));
		} else if (yy.leaf()) {
			if (std::get<B>(yy) == true) return htrue;
			if (std::get<B>(yy) == false) return x;
			return get(bdd<B, o>::bdd_or(x->b, std::get<B>(yy)));
		}
		return get(bdd<B, o>::bdd_or(x->b, b));
	}

	// Complement (the conjunction with T is an identity wrapper)
	hbdd<B, o> operator~() const {
		return get( bdd<B, o>::bdd_and(
			bdd<B, o>::T,
			bdd<B, o>::bdd_not(b)));
	}

	// Existential quantification of variable v
	hbdd<B, o> ex(int_t v) const {
		return get(bdd<B, o>::ex(b, v));
	}

	// Universal quantification of variable v
	hbdd<B, o> all(int_t v) const {
		return get(bdd<B, o>::all(b, v));
	}

	// Substitute the function x for variable v
	hbdd<B, o>
	subst(size_t v, const hbdd<B, o>& x) const {
		return get(bdd<B, o>::subst(b, v, x->b));
	}

	// Restrict v := 0 (low cofactor)
	hbdd<B, o> sub0(size_t v) const {
		return get(bdd<B, o>::sub0(b, v));
	}

	// Restrict v := 1 (high cofactor)
	hbdd<B, o> sub1(size_t v) const {
		return get(bdd<B, o>::sub1(b, v));
	}

	// Computes exactly
	//   this[v := f|v=0] | this[v := ~(f|v=1)],
	// i.e. this evaluated at the least (f|v=0) and greatest
	// (~(f|v=1)) solutions for v of the equation f = 0
	hbdd<B, o>
	condition(size_t v, const hbdd<B, o>& f) const {
		return subst(v, f->sub0(v)) | subst(v, ~(f->sub1(v)));
	}

	// Enumerate the DNF: f is called once per nonzero clause with its
	// leaf constant and signed literals; returning false stops the
	// enumeration (see bdd::dnf)
	void dnf(std::function<bool(const std::pair<B, std::vector<int_t>>&)> f)
		const
	{
		std::vector<int_t> v;
		bdd<B, o>::dnf(b, v, [f](
			const std::pair<B, std::vector<int_t>>& v)
		{
			return f(v);
		});
	}

	// The full DNF as a set of (leaf constant, literals) clauses
	std::set<std::pair<B, std::vector<int_t>>> dnf() const {
		std::set<std::pair<B, std::vector<int_t>>> r;
		dnf([&r](auto& x) { r.insert(x); return true; });
		return r;
	}

	std::set<int_t> get_vars() const {
		std::set<int_t> r;
		return bdd<B, o>::get_vars(b, r), r;
	}

	// Witness zero: an assignment of constants of B to variables
	// under which this function evaluates to zero (see
	// bdd::get_one_zero); this must have a zero
	std::map<int_t, B> get_one_zero() const {
		std::map<int_t, B> m;
		bdd<B, o>::get_one_zero(b, m);
		return m;
	}

	// Simultaneously substitute the mapped functions for the mapped
	// variables; unmapped variables are kept
	hbdd<B, o> compose(const std::map<int_t, hbdd<B, o>>& m) const {
		std::map<int_t, bdd_ref> p;
		for (auto& x : m) p.emplace(x.first, x.second->b);
		return get(bdd<B, o>::compose(b, p));
	}

	// Evaluate under the total assignment m (must cover all vars)
	B eval(std::map<int_t, B>& m) const { return bdd<B, o>::eval(b, m); }

	/* Loewenheim's General Reproductive Solution of f = 0, f being
	 * this function (Taba book, Theorem 1.8): with Z the witness zero
	 * from get_one_zero(), returns phi with
	 *   phi_i = z_i * f + x_i * f'
	 * per variable x_i. Substituting X by phi(X) satisfies f = 0 for
	 * every X, and the image of phi is exactly the solution set
	 * {X | f(X) = 0} (phi fixes every solution). Returns the empty
	 * map when f is zero (any X solves, no substitution needed);
	 * f must not be one (no solution exists).
	 */
	std::map<int_t, hbdd<B, o>> lgrs() const {
		std::map<int_t, hbdd<B, o>> r;
		if (b == bdd<B, o>::F) return r;
		DBG(assert((b != bdd<B, o>::T));)
		for (const auto& z : get_one_zero())
			r.emplace(z.first,	((*this) & get(z.second)) |
						(bit(true, z.first) & ~*this));
		return r;
	}

	// A splitter of this function: some s with 0 < s < this.
	// lower/middle/upper try to keep one clause / about half the
	// clauses / all but one clause (rm_all_except_one_clause,
	// rm_half_clauses, rm_clause); when the candidate degenerates
	// (F, or equal to this) -- and always for `bad` -- falls back to
	// split_clause, conjoining a variable not present in this
	hbdd<B, o> splitter (splitter_type st) {
		switch(st) {
			case splitter_type::lower: {
				bdd_ref s = bdd<B, o>::rm_all_except_one_clause(b);
				if (s != bdd<B, o>::F && s != b) return get(s);
				break;
			}
			case splitter_type::middle: {
				bdd_ref s = bdd<B, o>::rm_half_clauses(b);
				if (s != bdd<B, o>::F && s != b) return get(s);
				break;
			}
			case splitter_type::upper: {
				bdd_ref s = bdd<B,o>::rm_clause(b);
				if (s != bdd<B, o>::F && s != b) return get(s);
				break;
			}
			case splitter_type::bad: {
				return get(bdd<B,o>::split_clause(b));
			}
		}
		return get(bdd<B,o>::split_clause(b));
	}

	size_t hash () {return std::hash<bdd_ref>{}(b);}
#ifndef DEBUG
private:
#endif
	bdd_ref b;
};

/* Handle specialization for the two-element algebra Bool, wrapping the
 * bdd<Bool, o> engine specialization. Members not commented here
 * follow the contracts documented on the primary template above; the
 * leaf shortcuts of & and | are unnecessary (the only leaves are the
 * constants the engine already handles) and and_many() is added on
 * top of the engine's n-ary conjunction.
 */

template<bdd_options o>
struct bdd_handle<Bool, o> {
	using bdd_ref = bdd_reference<o.has_varshift(), o.has_inv_order(), o.idW, o.shiftW>;
	typedef bdd_node<bdd_ref> bdd_node_t;
	typedef std::unordered_map<bdd_node_t, std::shared_ptr<bdd_handle>> mn_type;
	typedef std::map<Bool, std::shared_ptr<bdd_handle>> mb_type;
	inline static std::unordered_map<bdd_node_t, std::shared_ptr<bdd_handle>> Mn;
	inline static std::map<Bool, std::shared_ptr<bdd_handle>> Mb;
	inline static hbdd<Bool, o> htrue, hfalse;

	// nonworking hack to call init
	template<typename T, T> struct dummy_type {};
	typedef dummy_type<mn_type&, Mn> dummy_mn_type;
	typedef dummy_type<mb_type&, Mb> dummy_mb_type;
	static bool dummy;

//	bdd_handle();
	auto operator<=>(const bdd_handle&) const = default;

	static hbdd<Bool, o> get(const bdd_node_t& x) {
		if (auto it = Mn.find(x); it != Mn.end())
			return it->second;//.lock();
		hbdd<Bool, o> h = std::make_shared<bdd_handle<Bool, o>>(); //(new bdd_handle);
		return h->b = bdd<Bool, o>::add(x), Mn.emplace(x, h), h;
	}

	static hbdd<Bool, o> get(bdd_ref t) {
		return get(bdd<Bool, o>::get(t));
	}

	// The only Bool constants are the interned true/false handles
	static hbdd<Bool, o> get(Bool b) {
		return b == true ? htrue : hfalse;
	}

	bdd<Bool, o> get() const {
		return bdd<Bool, o>::get(b);
	}

	bool is_zero() const { return b == bdd<Bool, o>::F; }
	bool is_one() const { return b == bdd<Bool, o>::T; }

	static hbdd<Bool, o> one() {
		return get(bdd<Bool, o>::T);
	}

	static hbdd<Bool, o> zero() {
		return get(bdd<Bool, o>::F);
	}

	static hbdd<Bool, o> bit(bool b, uint_t v) {
		DBG(assert(v > 0);)
		hbdd<Bool, o> r = get(bdd<Bool, o>::bit(b ? v : -v));
		DBG(assert(r);)
		return r;
	}

	Bool get_uelim() const { return bdd<Bool, o>::get_uelim(b); }
	Bool get_eelim() const { return bdd<Bool, o>::get_eelim(b); }

	hbdd<Bool, o> operator&(const hbdd<Bool, o>& x) const {
		return get(bdd<Bool, o>::bdd_and(x->b, b));
	}

	hbdd<Bool, o> operator~() const {
		return get(bdd<Bool, o>::bdd_not(b));
	}

	hbdd<Bool, o> operator|(const hbdd<Bool, o>& x) const {
		if constexpr (o.has_inv_out()) return ~((~x) & (~*this));
		return get(bdd<Bool, o>::bdd_or(x->b, b));
	}

	// n-ary conjunction of all handles in v (see bdd::bdd_and_many)
	static hbdd<Bool, o> and_many(const std::vector<hbdd<Bool, o>>& v) {
		std::vector<bdd_ref> x;
		for (const auto& e : v) x.push_back(e->b);
		return get(bdd<Bool, o>::bdd_and_many(x));
	}

	hbdd<Bool, o> ex(int_t v) const {
		return get(bdd<Bool, o>::ex(b, v));
	}

	hbdd<Bool, o> all(int_t v) const {
		return get(bdd<Bool, o>::all(b, v));
	}

	hbdd<Bool, o> subst(size_t v, const hbdd<Bool, o>& x) const {
		return get(bdd<Bool, o>::subst(b, v, x->b));
	}

	hbdd<Bool, o> sub0(size_t v) const {
		return get(bdd<Bool, o>::sub0(b, v));
	}

	hbdd<Bool, o> sub1(size_t v) const {
		return get(bdd<Bool, o>::sub1(b, v));
	}

	hbdd<Bool, o> condition(size_t v, const hbdd<Bool, o>& f) const {
		return subst(v, f->sub0(v)) | subst(v, ~(f->sub1(v)));
	}

	void dnf(std::function<bool(const std::pair<Bool, std::vector<int_t>>&)>
		f) const
	{
		std::vector<int_t> v;
		bdd<Bool, o>::dnf(b, v, [f](
			const std::pair<Bool, std::vector<int_t>>& v)
		{
			return f(v);
		});
	}

	std::set<std::pair<Bool, std::vector<int_t>>> dnf() const {
		std::set<std::pair<Bool, std::vector<int_t>>> r;
		dnf([&r](auto& x) { r.insert(x); return true; });
		return r;
	}

	std::set<int_t> get_vars() const {
		std::set<int_t> r;
		return bdd<Bool, o>::get_vars(b, r), r;
	}

	std::map<int_t, Bool> get_one_zero() const {
		std::map<int_t, Bool> m;
		bdd<Bool, o>::get_one_zero(b, m);
		return m;
	}

	hbdd<Bool, o> compose(const std::map<int_t, hbdd<Bool, o>>& m) const {
		std::map<int_t, bdd_ref> p;
		for (auto& x : m) p.emplace(x.first, x.second->b);
		return get(bdd<Bool, o>::compose(b, p));
	}

	Bool eval(std::map<int_t, Bool>& m) const {
		return bdd<Bool, o>::eval(b, m);
	}

	std::map<int_t, hbdd<Bool, o>> lgrs() const {
		std::map<int_t, hbdd<Bool, o>> r;
		if (b == bdd<Bool, o>::F) return r;
		DBG(assert((b != bdd<Bool, o>::T));)
		for (const auto& z : get_one_zero())
			r.emplace(z.first,	((*this) & get(z.second)) |
						  (bit(true, z.first) & ~*this));
		return r;
	}

	hbdd<Bool, o> splitter (splitter_type st) {
		switch(st) {
		case splitter_type::lower: {
			bdd_ref s = bdd<Bool, o>::rm_all_except_one_clause(b);
			if (s != bdd<Bool, o>::F && s != b) return get(s);
			break;
		}
		case splitter_type::middle: {
			bdd_ref s = bdd<Bool, o>::rm_half_clauses(b);
			if (s != bdd<Bool, o>::F && s != b) return get(s);
			break;
		}
		case splitter_type::upper: {
			bdd_ref s = bdd<Bool,o>::rm_clause(b);
			if (s != bdd<Bool, o>::F && s != b) return get(s);
			break;
		}
		case splitter_type::bad: {
			return get(bdd<Bool,o>::split_clause(b));
		}
		}
		return get(bdd<Bool,o>::split_clause(b));
	}

	size_t hash () {return std::hash<bdd_ref>{}(b);}

#ifndef DEBUG
	private:
#endif
	bdd_ref b;
};

// Trait: true for shared_ptr types (i.e. for hbdd handles)
template<typename T> constexpr bool is_sp{};
template<typename T> constexpr bool is_sp<sp<T>>{true};

// Overloads of babdd.h's get_one/get_zero for handle types: return
// the interned constant handles instead of constants of B
template<typename B> B get_one() requires is_sp<B> {
	return B::element_type::one();
}

template<typename B> B get_zero() requires is_sp<B> {
	return B::element_type::zero();
}

// Idempotent per-(B, o) engine setup, run at static-init time via
// bdd<B, o>::initializer: fixes the T/F references (with output
// inverters both point at universe entry 0, F carrying the out bit;
// otherwise F is entry 0 and T entry 1), seeds the universe with the
// constant entries (create_universe) and interns the htrue/hfalse
// handles
template<typename B, auto o = bdd_options<>::create()> void bdd_init() {
	using bdd_ref = bdd_reference<o.has_varshift(), o.has_inv_order(), o.idW, o.shiftW>;

	if (!bdd<B, o>::V.empty()) return;
	if constexpr (o.has_inv_out()) {
		bdd<B, o>::T = bdd_ref(0,0,0);
		bdd<B, o>::F = bdd_ref(0,1,0);
	} else {
		bdd<B, o>::F = bdd_ref(0,0,0);
		bdd<B, o>::T = bdd_ref(0,0,1);
	}
	create_universe<B, o>(get_one<B>());
	bdd_handle<B, o>::hfalse = bdd_handle<B, o>::get(bdd<B, o>::F);
	bdd_handle<B, o>::htrue = bdd_handle<B, o>::get(bdd<B, o>::T);
}

// Seed the universe with the leaf constants of B: zero at entry 0 and
// one at entry 1, or -- with output inverters -- only one at entry 0,
// zero being its out-inverted alias
template<typename B, bdd_options o> void create_universe(B) {
	auto one = get_one<B>();
	if constexpr (!o.has_inv_out()) {
		auto zero = ~one;
		bdd<B, o>::V.emplace_back(zero);
		bdd<B, o>::Mb.emplace(zero, 0);
		bdd<B, o>::V.emplace_back(one);
		bdd<B, o>::Mb.emplace(one, 1);
	} else {
		bdd<B, o>::V.emplace_back(one);
		bdd<B, o>::Mb.emplace(one, 0);
	}
}

// Bool overload: the universe stores degenerate sentinel node entries
// (children pointing at the constants themselves) in place of leaf
// values, so constant references decode to a node like any other; one
// shared entry with output inverters, separate F/T entries without
template<typename B, bdd_options o> void create_universe(Bool) {
	const auto &T = bdd<Bool, o>::T;
	const auto &F = bdd<Bool, o>::F;
	auto &V = bdd<Bool, o>::V;
	auto &Mn = bdd<Bool, o>::Mn;
	if constexpr (o.has_varshift()) {
		if constexpr (!o.has_inv_out()) {
			V.emplace_back(F,F);
			Mn.emplace(V[0],0);
			V.emplace_back(T,T);
			Mn.emplace(V[1],1);
		} else {
			V.emplace_back(T,T);
			Mn.emplace(V[0],0);
		}
	} else {
		if constexpr (!o.has_inv_out()) {
			V.emplace_back(0,F,F);
			Mn.emplace(V[0],0);
			V.emplace_back(0,T,T);
			Mn.emplace(V[1],1);
		} else {
			V.emplace_back(0, T, T);
			Mn.emplace(V[0], 0);
		}
	}
}

// ...auto o> fails to build here
template<typename B, bdd_options o>
bdd<B, o>::initializer::initializer() {
	bdd_init<B, o>();
}

template<bdd_options o>
bdd<Bool, o>::initializer::initializer() {
	bdd_init<Bool, o>();
}

// bdd printer taken from out.h
template<typename B, auto o = bdd_options<>::create()>
std::ostream& operator<<(std::ostream& os, const hbdd<B, o>& f) {
	if (f == bdd_handle<B, o>::htrue) return os << '1';
	if (f == bdd_handle<B, o>::hfalse) return os << '0';
	std::set<std::pair<B, std::vector<int_t>>> dnf = f->dnf();
	size_t n = dnf.size();
	std::set<std::string> ss;
	for (auto& c : dnf) {
		std::set<std::string> s;
		assert(!(c.first == false));
		std::stringstream t;
		if (!(c.first == true)) t << '{' << c.first << '}';
		for (int_t v : c.second)
			if (v < 0) s.insert(var_dict(-v) + "'");
			else s.insert(var_dict(v));
		bool first = true;
		for (auto& x : s) {
			if (!first) t << " "; else first = false;
			t << x;
		}
		ss.insert(t.str());
	}
	bool first = true;
	for (auto& s : ss) {
		if (!first) os << " ", first = false;
		os << s;
		if (--n) os << " | ";
	}
	return os;
}

} // namespace idni::tau_lang

#endif