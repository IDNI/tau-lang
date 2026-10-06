// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ω-categorical type enumeration for Algorithm D (direct parity-game
// construction), the qlt LTL synthesis and the qlt semantic revision.  Standalone header with NO dependency on qlt.h — the
// full qlt.h include chain pulls in hooks.tmpl.h which requires the
// template-parameter context that only a full tau-lang TU sets up.  We
// instead use plain std::pair<long long, long long> rationals here; the
// helpers in this header are pure combinatorics over a sorted list of
// user-supplied constants.
//
// Provides:
//   - idni::tau_lang::omcat::rational                   plain rational type.
//   - idni::tau_lang::omcat::qlt_type1              1-type of (ℚ, <, Σ).
//   - enumerate_qlt_T1(constants)                  returns 2|Σ|+1 1-types.
//   - qlt_type1::realize()                          concrete rational witness.
//   - qlt_type_of(value, sorted_consts)            type of a value.
//   - qlt_type2 / enumerate_qlt_T2(constants)      2-types (m, x).
//   - qlt_type3 / enumerate_qlt_T3(constants)      3-types (m, x, y).

#ifndef __IDNI__TAU__OMCAT_TYPES_H__
#define __IDNI__TAU__OMCAT_TYPES_H__

#include <algorithm>
#include <cstdint>
#include <utility>
#include <limits>
#include <vector>
#if defined(_MSC_VER) && !defined(__clang__)
#	include <__msvc_int128.hpp>
#endif

namespace idni::tau_lang::omcat {

/**
 * @brief Plain rational (p/q) -- we avoid qlt_rational here to keep this
 * header free of the tau_tree template chain.  Internal use only.
 *
 * The constructor normalises the sign so that `q` is non-negative.
 */
struct rational {
	/// Numerator p and denominator q; q is never negative.
	long long p = 0, q = 1;
	rational() = default;
	rational(long long p_, long long q_) : p(p_), q(q_) {
		if (q < 0) { p = -p; q = -q; }
	}
};

/// @brief 128-bit integer used for overflow-free cross-multiplication.
#if defined(_MSC_VER) && !defined(__clang__)
using omcat_int128_ = std::_Signed128;
#else
__extension__ typedef __int128 omcat_int128_;
#endif

/**
 * @brief Three-way compare two rationals by 128-bit cross-multiplication.
 * @param a Left operand.
 * @param b Right operand.
 * @return -1, 0 or +1 as `a` is less than, equal to or greater than `b`.
 */
inline int cmp(const rational& a, const rational& b) {
	// a.p/a.q  vs  b.p/b.q :  cross-multiply in 128 bits:
	// long long products overflow for parse-reachable magnitudes and
	// silently corrupt T1/T2/T3 orderings.
	omcat_int128_ lhs = (omcat_int128_) a.p * b.q,
		rhs = (omcat_int128_) b.p * a.q;
	if (lhs < rhs) return -1;
	if (lhs > rhs) return +1;
	return 0;
}

/**
 * @brief A 1-type over (Q, <) with named finite constants c_0 < c_1 < ... <
 * c_{k-1}.
 *
 * Encoded as an integer position:
 *   pos = 2i       means c_{i-1} < x < c_i      (interval; i=0 ⇒ (-∞, c_0); i=k ⇒ (c_{k-1}, +∞))
 *   pos = 2i+1     means x = c_i                (point, 0 ≤ i < k)
 * Total: 2k+1 types.
 */
struct qlt_type1 {
	/// Encoded position, in 0..2k.
	int pos = 0;
	/// The named constants, sorted and deduplicated.
	std::vector<rational> constants;

	/// @brief True iff the type is a point x = c_i (odd position).
	bool is_point() const { return (pos & 1) == 1; }
	/// @brief True iff the type is an open interval (even position).
	bool is_interval() const { return !is_point(); }
	/// @brief The encoded position.
	int index() const { return pos; }
	/// @brief Number of named constants k.
	int num_constants() const { return (int)constants.size(); }

	/// @brief True iff every x of this type satisfies x < c_j.
	/// Interval pos=2i spans (c_{i-1}, c_i) (with c_{-1}=-∞, c_k=+∞).
	/// Point pos=2i+1 is x = c_i.
	/// @param j index of the named constant, 0 <= j < k.
	bool less_than(int j) const {
		const int hp = pos >> 1;
		return is_point() ? hp < j : hp <= j;
	}
	/// @brief True iff this type is the point x = c_j (@p j a constant index).
	bool equal_to(int j) const { return is_point() && (pos >> 1) == j; }
	/// @brief True iff every x of this type satisfies x > c_j (@p j a constant index).
	bool greater_than(int j) const {
		const int hp = pos >> 1;
		// interval (c_{i-1}, c_i): x > c_j iff i-1 >= j, equivalently i > j.
		return hp > j;
	}

	/**
	 * @brief A concrete rational witness of this type.
	 *
	 * AL-R2: all arithmetic in 128 bits, reduced by gcd before narrowing
	 * (the rationale for `cmp` applies here verbatim: long long
	 * products of parse-reachable p/q pairs overflow, and a corrupted
	 * witness mis-orders T2/T3 enumeration).  The interior witness is the
	 * MEDIANT (a.p+b.p)/(a.q+b.q), which lies strictly between a < b for
	 * positive denominators and never needs a product at all.
	 * @pre pos lies in 0..2k.
	 * @return The constant itself for a point type; c_0 - 1, c_{k-1} + 1
	 * or the mediant of the two bounding constants for an interval, and 0
	 * when there are no constants at all.
	 */
	rational realize() const {
		const size_t k = constants.size();
		// A 1-type position encodes a constant count, so the half it names
		// is always a valid constant index.
		const size_t i = (size_t)(pos >> 1);
		if (is_point()) return constants[i];
		if (i == 0) {
			if (k == 0) return rational(0, 1);
			rational c = constants[0];
			// c - 1 as p/q - 1 = (p - q)/q.
			return make_reduced((omcat_int128_)c.p - c.q, c.q);
		}
		if (i == k) {
			rational c = constants[k - 1];
			return make_reduced((omcat_int128_)c.p + c.q, c.q);
		}
		rational a = constants[i - 1], b = constants[i];
		return make_reduced((omcat_int128_)a.p + b.p,
		                    (omcat_int128_)a.q + b.q);
	}

private:
	/// @brief Reduce num/den by their gcd and narrow to long long (halving
	/// as a best-effort approximation when the reduced value does not fit).
	static rational make_reduced(omcat_int128_ num, omcat_int128_ den) {
		if (den < 0) { num = -num; den = -den; }
		omcat_int128_ x = num < 0 ? -num : num, y = den;
		while (y != 0) { omcat_int128_ r = x % y; x = y; y = r; }
		if (x > 1) { num /= x; den /= x; }
		// The reduced value fits long long for every parser-produced
		// constant pair (|p|+|p'| < 2^63).  Beyond that the halving below
		// is a best-effort approximation of the mediant that can land on a
		// neighbour; `cmp` stays exact, so callers comparing witnesses see
		// at worst a duplicate type, never UB.
		const omcat_int128_ lim = std::numeric_limits<long long>::max();
		while (num > lim || num < -lim || den > lim) { num /= 2; den /= 2; }
		if (den == 0) den = 1;
		return rational((long long)num, (long long)den);
	}
public:
};

/**
 * @brief Enumerate the 2k+1 1-types of (Q, <) over the given constants.
 *
 * The constants are sorted and deduplicated first; every returned type
 * carries that normalised list.
 * @param constants Named constants (any order, duplicates allowed).
 * @return The 1-types in position order 0..2k.
 */
inline std::vector<qlt_type1> enumerate_qlt_T1(std::vector<rational> constants) {
	std::sort(constants.begin(), constants.end(),
	    [](const rational& a, const rational& b) { return cmp(a, b) < 0; });
	constants.erase(std::unique(constants.begin(), constants.end(),
	    [](const rational& a, const rational& b) { return cmp(a, b) == 0; }),
	    constants.end());
	const int k = (int)constants.size();
	std::vector<qlt_type1> out;
	out.reserve(2 * constants.size() + 1);
	for (int pos = 0; pos <= 2 * k; ++pos) {
		qlt_type1 t;
		t.pos = pos;
		t.constants = constants;
		out.push_back(std::move(t));
	}
	return out;
}

/**
 * @brief The 1-type position of a value against sorted constants.
 * @param v Value to classify.
 * @param sorted_consts Sorted, deduplicated constants.
 * @return 2i for c_{i-1} < v < c_i, 2i+1 for v = c_i, 2k when v exceeds
 * every constant.
 */
inline int qlt_type_of(const rational& v, const std::vector<rational>& sorted_consts) {
	int i = 0;
	for (const rational& cst : sorted_consts) {
		int c = cmp(v, cst);
		if (c < 0)  return 2 * i;
		if (c == 0) return 2 * i + 1;
		++i;
	}
	return 2 * i;
}

/// @brief Order relation between two free variables.
enum class relation : uint8_t { LT = 0, EQ = 1, GT = 2 };

/**
 * @brief A 2-type over (Q, <) with finite constants: the order of two free
 * variables m, x among themselves and against the named constants.
 *
 * Encoded as (pos_m, pos_x, rel) where rel ∈ {LT, EQ, GT} describes the
 * order relation between m and x.  This is the natural exocat T_2 for
 * (memory, input) at a single time step.
 */
struct qlt_type2 {
	/// 1-type position of m.
	int pos_m = 0;
	/// 1-type position of x.
	int pos_x = 0;
	/// Order of m against x.
	relation rel = relation::LT;
	/// The named constants, sorted and deduplicated.
	std::vector<rational> constants;

	/// @brief Restriction onto the m-component: just the 1-type of m.
	qlt_type1 restrict_m() const {
		qlt_type1 t;
		t.pos = pos_m;
		t.constants = constants;
		return t;
	}
	/// @brief Restriction onto the x-component: just the 1-type of x.
	qlt_type1 restrict_x() const {
		qlt_type1 t;
		t.pos = pos_x;
		t.constants = constants;
		return t;
	}
};

/**
 * @brief Enumerate the 2-types of (Q, <) with the given named constants.
 *
 * Not every (pos_m, pos_x, rel) is admissible: if m's 1-type and x's
 * 1-type already fix their relative order (e.g., m = c_0 and x = c_1 with
 * c_0 < c_1 implies rel must be LT), we emit only the consistent triples.
 * @param constants Named constants (any order, duplicates allowed).
 * @return The admissible 2-types, ordered by (pos_m, pos_x, rel).
 */
inline std::vector<qlt_type2> enumerate_qlt_T2(const std::vector<rational>& constants) {
	auto t1 = enumerate_qlt_T1(constants);
	std::vector<qlt_type2> out;
	// For each pair (tm, tx) of 1-types, determine which relation values are
	// consistent.  Use realize() witnesses to test consistency.
	for (const auto& tm : t1) {
		for (const auto& tx : t1) {
			rational mv = tm.realize();
			rational xv = tx.realize();
			int c = cmp(mv, xv);
			// If both are point types, rel is forced by their values.
			if (tm.is_point() && tx.is_point()) {
				relation r = c < 0 ? relation::LT : (c == 0 ? relation::EQ : relation::GT);
				qlt_type2 s{tm.pos, tx.pos, r, tm.constants};
				out.push_back(std::move(s));
				continue;
			}
			// If they land in the same interval (both non-point, same pos),
			// all three relations are consistent (interior of a dense order).
			if (tm.is_interval() && tx.is_interval() && tm.pos == tx.pos) {
				for (auto r : {relation::LT, relation::EQ, relation::GT}) {
					qlt_type2 s{tm.pos, tx.pos, r, tm.constants};
					out.push_back(std::move(s));
				}
				continue;
			}
			// Otherwise the positions differ: the witness comparison gives
			// the unique consistent relation.
			relation r = c < 0 ? relation::LT : (c == 0 ? relation::EQ : relation::GT);
			qlt_type2 s{tm.pos, tx.pos, r, tm.constants};
			out.push_back(std::move(s));
		}
	}
	return out;
}

// ── 3-types (m, x, y) for Algorithm A (binary T_3 encoding). ────────────

/**
 * @brief The order relation two 1-types force between their elements.
 *
 * Returns -1 if both 1-types lie in the same interval (free ordering),
 * otherwise the forced relation: 0=LT, 1=EQ, 2=GT.
 * @param ta First 1-type.
 * @param tb Second 1-type.
 * @return -1, or the forced relation as an int.
 */
inline int forced_rel_between(const qlt_type1& ta, const qlt_type1& tb) {
	if (ta.is_interval() && tb.is_interval() && ta.pos == tb.pos) return -1;
	rational av = ta.realize(), bv = tb.realize();
	int c = cmp(av, bv);
	return c < 0 ? 0 : (c == 0 ? 1 : 2);
}

/**
 * @brief Transitivity check for a (r_mx, r_xy, r_my) triple.
 * @param r_mx Relation between m and x.
 * @param r_xy Relation between x and y.
 * @param r_my Relation between m and y.
 * @return `true` iff the triple is consistent with a linear order.
 */
inline bool rel3_consistent(relation r_mx, relation r_xy, relation r_my) {
	if (r_mx == relation::LT && r_xy == relation::LT) return r_my == relation::LT;
	if (r_mx == relation::LT && r_xy == relation::EQ) return r_my == relation::LT;
	if (r_mx == relation::EQ && r_xy == relation::LT) return r_my == relation::LT;
	if (r_mx == relation::EQ && r_xy == relation::EQ) return r_my == relation::EQ;
	if (r_mx == relation::EQ && r_xy == relation::GT) return r_my == relation::GT;
	if (r_mx == relation::GT && r_xy == relation::EQ) return r_my == relation::GT;
	if (r_mx == relation::GT && r_xy == relation::GT) return r_my == relation::GT;
	return true; // (LT,GT) or (GT,LT): any r_my consistent
}

/// @brief 3-type of (memory m, input x, output y) over (Q, <, Sigma).
struct qlt_type3 {
	/// 1-type positions of m, x and y.
	int pos_m = 0, pos_x = 0, pos_y = 0;
	/// Pairwise order of m, x and y.
	relation rel_mx = relation::LT, rel_my = relation::LT, rel_xy = relation::LT;
	/// The named constants, sorted and deduplicated.
	std::vector<rational> constants;

	/// @brief The 1-type of m.
	qlt_type1 restrict_m() const { qlt_type1 t; t.pos = pos_m; t.constants = constants; return t; }
	/// @brief The 1-type of x.
	qlt_type1 restrict_x() const { qlt_type1 t; t.pos = pos_x; t.constants = constants; return t; }
	/// @brief The 1-type of y.
	qlt_type1 restrict_y() const { qlt_type1 t; t.pos = pos_y; t.constants = constants; return t; }
};

/**
 * @brief Enumerate all 3-types for (Q, <) with the given named constants.
 *
 * Filters the T_1^3 product by forced-relation consistency and transitivity.
 * The count grows cubically in the number of constants.
 * @param constants Named constants (any order, duplicates allowed).
 * @return The admissible 3-types.
 */
inline std::vector<qlt_type3> enumerate_qlt_T3(const std::vector<rational>& constants) {
	auto t1 = enumerate_qlt_T1(constants);
	std::vector<qlt_type3> out;
	for (const auto& tm : t1) {
		for (const auto& tx : t1) {
			for (const auto& ty : t1) {
				int f_mx = forced_rel_between(tm, tx);
				int f_my = forced_rel_between(tm, ty);
				int f_xy = forced_rel_between(tx, ty);
				for (int i_mx = 0; i_mx < 3; ++i_mx) {
					if (f_mx >= 0 && i_mx != f_mx) continue;
					for (int i_xy = 0; i_xy < 3; ++i_xy) {
						if (f_xy >= 0 && i_xy != f_xy) continue;
						for (int i_my = 0; i_my < 3; ++i_my) {
							if (f_my >= 0 && i_my != f_my) continue;
							relation r_mx = (relation)i_mx, r_xy = (relation)i_xy, r_my = (relation)i_my;
							if (rel3_consistent(r_mx, r_xy, r_my)) {
								qlt_type3 s;
								s.pos_m = tm.pos; s.pos_x = tx.pos; s.pos_y = ty.pos;
								s.rel_mx = r_mx; s.rel_my = r_my; s.rel_xy = r_xy;
								s.constants = tm.constants;
								out.push_back(std::move(s));
							}
						}
					}
				}
			}
		}
	}
	return out;
}

} // namespace idni::tau_lang::omcat

#endif // __IDNI__TAU__OMCAT_TYPES_H__
