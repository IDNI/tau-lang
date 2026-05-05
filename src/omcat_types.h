// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ω-categorical type enumeration for Algorithm D (direct parity-game
// construction).  Standalone header with NO dependency on qlt.h — the
// full qlt.h include chain pulls in hooks.tmpl.h which requires the
// template-parameter context that only a full tau-lang TU sets up.  We
// instead use plain std::pair<long long, long long> rationals here; the
// helpers in this header are pure combinatorics over a sorted list of
// user-supplied constants.
//
// Provides:
//   - idni::tau_lang::omcat::Rat                   plain rational type.
//   - idni::tau_lang::omcat::QltType1              1-type of (ℚ, <, Σ).
//   - enumerate_qlt_T1(constants)                  returns 2|Σ|+1 1-types.
//   - QltType1::realize()                          concrete rational witness.
//   - qlt_type_of(value, sorted_consts)            type of a value.

#ifndef __IDNI__TAU__OMCAT_TYPES_H__
#define __IDNI__TAU__OMCAT_TYPES_H__

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace idni::tau_lang::omcat {

// Plain rational (p/q) — we avoid qlt_rational here to keep this header
// free of the tau_tree template chain.  Internal use only.
struct Rat {
	long long p = 0, q = 1;
	Rat() = default;
	Rat(long long p_, long long q_) : p(p_), q(q_) {
		if (q < 0) { p = -p; q = -q; }
	}
};

inline int cmp(const Rat& a, const Rat& b) {
	// a.p/a.q  vs  b.p/b.q :  cross-multiply.
	long long lhs = a.p * b.q, rhs = b.p * a.q;
	if (lhs < rhs) return -1;
	if (lhs > rhs) return +1;
	return 0;
}

// A 1-type over (ℚ, <) with named finite constants c_0 < c_1 < ... < c_{k-1}.
// Encoded as an integer position:
//   pos = 2i       means c_{i-1} < x < c_i      (interval; i=0 ⇒ (-∞, c_0); i=k ⇒ (c_{k-1}, +∞))
//   pos = 2i+1     means x = c_i                (point, 0 ≤ i < k)
// Total: 2k+1 types.
struct QltType1 {
	int pos = 0;
	std::vector<Rat> constants;

	bool is_point() const { return (pos & 1) == 1; }
	bool is_interval() const { return !is_point(); }
	int index() const { return pos; }
	int num_constants() const { return (int)constants.size(); }

	// Interval pos=2i spans (c_{i-1}, c_i) (with c_{-1}=-∞, c_k=+∞).
	// Point pos=2i+1 is x = c_i.  Ordering against the named constant c_j:
	bool less_than(int j) const {
		const int hp = pos >> 1;
		return is_point() ? hp < j : hp <= j;
	}
	bool equal_to(int j) const { return is_point() && (pos >> 1) == j; }
	bool greater_than(int j) const {
		const int hp = pos >> 1;
		// interval (c_{i-1}, c_i): x > c_j iff i-1 >= j, equivalently i > j.
		return is_point() ? hp > j : hp > j;
	}

	Rat realize() const {
		int k = (int)constants.size();
		if (is_point()) return constants[pos >> 1];
		int i = pos >> 1;
		if (i == 0) {
			if (k == 0) return Rat(0, 1);
			Rat c = constants[0];
			// c - 1 as p/q - 1 = (p - q)/q.
			return Rat(c.p - c.q, c.q);
		}
		if (i == k) {
			Rat c = constants[k - 1];
			return Rat(c.p + c.q, c.q);
		}
		Rat a = constants[i - 1], b = constants[i];
		long long num = a.p * b.q + b.p * a.q;
		long long den = 2 * a.q * b.q;
		return Rat(num, den);
	}
};

inline std::vector<QltType1> enumerate_qlt_T1(std::vector<Rat> constants) {
	std::sort(constants.begin(), constants.end(),
	    [](const Rat& a, const Rat& b) { return cmp(a, b) < 0; });
	constants.erase(std::unique(constants.begin(), constants.end(),
	    [](const Rat& a, const Rat& b) { return cmp(a, b) == 0; }),
	    constants.end());
	const int k = (int)constants.size();
	std::vector<QltType1> out;
	out.reserve(2 * k + 1);
	for (int pos = 0; pos <= 2 * k; ++pos) {
		QltType1 t;
		t.pos = pos;
		t.constants = constants;
		out.push_back(std::move(t));
	}
	return out;
}

inline int qlt_type_of(const Rat& v, const std::vector<Rat>& sorted_consts) {
	const int k = (int)sorted_consts.size();
	for (int i = 0; i < k; ++i) {
		int c = cmp(v, sorted_consts[i]);
		if (c < 0)  return 2 * i;
		if (c == 0) return 2 * i + 1;
	}
	return 2 * k;
}

// A 2-type over (ℚ, <) with finite constants: the order of two free
// variables m, x among themselves and against the named constants.
// Encoded as (pos_m, pos_x, rel) where rel ∈ {LT, EQ, GT} describes the
// order relation between m and x.  This is the natural exocat T_2 for
// (memory, input) at a single time step.
enum class Rel : uint8_t { LT = 0, EQ = 1, GT = 2 };

struct QltType2 {
	int pos_m = 0;
	int pos_x = 0;
	Rel rel = Rel::LT;
	std::vector<Rat> constants;

	// Restriction onto the m-component: just the 1-type of m.
	QltType1 restrict_m() const {
		QltType1 t;
		t.pos = pos_m;
		t.constants = constants;
		return t;
	}
	QltType1 restrict_x() const {
		QltType1 t;
		t.pos = pos_x;
		t.constants = constants;
		return t;
	}
};

// Enumerate the 2-types of (ℚ, <) with the given named constants.
// Not every (pos_m, pos_x, rel) is admissible: if m's 1-type and x's
// 1-type already fix their relative order (e.g., m = c_0 and x = c_1 with
// c_0 < c_1 implies rel must be LT), we emit only the consistent triples.
inline std::vector<QltType2> enumerate_qlt_T2(const std::vector<Rat>& constants) {
	auto t1 = enumerate_qlt_T1(constants);
	std::vector<QltType2> out;
	// For each pair (tm, tx) of 1-types, determine which Rel values are
	// consistent.  Use realize() witnesses to test consistency.
	for (const auto& tm : t1) {
		for (const auto& tx : t1) {
			Rat mv = tm.realize();
			Rat xv = tx.realize();
			int c = cmp(mv, xv);
			// If both are point types, rel is forced by their values.
			if (tm.is_point() && tx.is_point()) {
				Rel r = c < 0 ? Rel::LT : (c == 0 ? Rel::EQ : Rel::GT);
				QltType2 s{tm.pos, tx.pos, r, tm.constants};
				out.push_back(std::move(s));
				continue;
			}
			// If they land in the same interval (both non-point, same pos),
			// all three relations are consistent (interior of a dense order).
			if (tm.is_interval() && tx.is_interval() && tm.pos == tx.pos) {
				for (auto r : {Rel::LT, Rel::EQ, Rel::GT}) {
					QltType2 s{tm.pos, tx.pos, r, tm.constants};
					out.push_back(std::move(s));
				}
				continue;
			}
			// Otherwise the positions differ: the witness comparison gives
			// the unique consistent relation.
			Rel r = c < 0 ? Rel::LT : (c == 0 ? Rel::EQ : Rel::GT);
			QltType2 s{tm.pos, tx.pos, r, tm.constants};
			out.push_back(std::move(s));
		}
	}
	return out;
}

// ── 3-types (m, x, y) for Algorithm A (binary T_3 encoding). ────────────

// Returns -1 if both 1-types lie in the same interval (free ordering),
// otherwise the forced relation: 0=LT, 1=EQ, 2=GT.
inline int forced_rel_between(const QltType1& ta, const QltType1& tb) {
	if (ta.is_interval() && tb.is_interval() && ta.pos == tb.pos) return -1;
	Rat av = ta.realize(), bv = tb.realize();
	int c = cmp(av, bv);
	return c < 0 ? 0 : (c == 0 ? 1 : 2);
}

// Transitivity check for a (r_mx, r_xy, r_my) triple.
inline bool rel3_consistent(Rel r_mx, Rel r_xy, Rel r_my) {
	if (r_mx == Rel::LT && r_xy == Rel::LT) return r_my == Rel::LT;
	if (r_mx == Rel::LT && r_xy == Rel::EQ) return r_my == Rel::LT;
	if (r_mx == Rel::EQ && r_xy == Rel::LT) return r_my == Rel::LT;
	if (r_mx == Rel::EQ && r_xy == Rel::EQ) return r_my == Rel::EQ;
	if (r_mx == Rel::EQ && r_xy == Rel::GT) return r_my == Rel::GT;
	if (r_mx == Rel::GT && r_xy == Rel::EQ) return r_my == Rel::GT;
	if (r_mx == Rel::GT && r_xy == Rel::GT) return r_my == Rel::GT;
	return true; // (LT,GT) or (GT,LT): any r_my consistent
}

// 3-type of (memory m, input x, output y) over (ℚ, <, Σ).
struct QltType3 {
	int pos_m = 0, pos_x = 0, pos_y = 0;
	Rel rel_mx = Rel::LT, rel_my = Rel::LT, rel_xy = Rel::LT;
	std::vector<Rat> constants;

	QltType1 restrict_m() const { QltType1 t; t.pos = pos_m; t.constants = constants; return t; }
	QltType1 restrict_x() const { QltType1 t; t.pos = pos_x; t.constants = constants; return t; }
	QltType1 restrict_y() const { QltType1 t; t.pos = pos_y; t.constants = constants; return t; }
};

// Enumerate all 3-types for (ℚ, <) with the given named constants.
// Filters the T_1^3 product by forced-relation consistency and transitivity.
inline std::vector<QltType3> enumerate_qlt_T3(const std::vector<Rat>& constants) {
	auto t1 = enumerate_qlt_T1(constants);
	std::vector<QltType3> out;
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
							Rel r_mx = (Rel)i_mx, r_xy = (Rel)i_xy, r_my = (Rel)i_my;
							if (rel3_consistent(r_mx, r_xy, r_my)) {
								QltType3 s;
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

// ────────────────────────────────────────────────────────────────────────
// The controllable-predecessor `Pre` operator for Algorithm D (exocat).
//
// For a target subset W ⊆ T_1 (memory-types), Pre(W) is the set of memory
// types b from which the system can force the next memory type to land
// in W against any environment input.  When a constraint A ⊆ T_3 is
// imposed on the current step's full (m, x, y) type, Pre_A(W) restricts
// the system's witness τ to τ ∈ A.
//
// For the qlt-only POC we approximate T_3 as admissible pairs of
// (QltType2, Rat-witnessed next-memory-type ρ'), avoiding the full T_3
// enumeration.  For each (b, σ), the system picks a concrete y (any
// rational) producing next memory ρ' = type of y.
//
// This is the core operator referenced in LTL_ABA_DESIGN.md's Algorithm
// D Phase 3; the μ/ν fixpoints over 2^{T_1} assemble from it.
// ────────────────────────────────────────────────────────────────────────

// Controllable-predecessor over T_1.  Returns memory types b such that:
//   ∀ σ ∈ T_2 with σ|_m = b,  ∃ ρ' ∈ next_memory_admissible(σ) ∩ W.
// `next_memory_admissible(σ)` enumerates the 1-types ρ' that the system
// can achieve as y's type given the current (m, x) pair of type σ.
// For the qlt-only case (no extra constraint A), any ρ' is achievable
// because the system can output any rational — so next_memory_admissible
// = T_1 for every σ.  The quantifier reduces to:
//   ∀ σ ∈ T_2 with σ|_m = b,  W ≠ ∅.
// Equivalently, `Pre(W) = T_1 if W ≠ ∅ else ∅`.
//
// The structure below anticipates the general case where A ⊊ T_3 via a
// callable `admissible_next` that takes (σ) and returns the ρ's the
// system can reach under the current-step constraint.  Callers of Pre_A
// build such a callable using their preferred ι(ψ) encoding.
template <class AdmissibleNext>
inline std::vector<int> Pre_over_T1(
    const std::vector<QltType1>& T1,
    const std::vector<QltType2>& T2,
    const std::vector<int>& W_indices,
    AdmissibleNext admissible_next)
{
	// Index W for fast membership.
	std::vector<char> in_W(T1.size(), 0);
	for (int w : W_indices) in_W[w] = 1;
	std::vector<int> result;
	for (const auto& b : T1) {
		bool forceable = true;
		for (const auto& s : T2) {
			if (s.pos_m != b.pos) continue;
			// For every σ ∈ T_2 with σ|_m = b, there must exist ρ' in
			// admissible_next(σ) ∩ W.
			auto admissible = admissible_next(s);
			bool ok = false;
			for (int rho_prime : admissible) {
				if (in_W[rho_prime]) { ok = true; break; }
			}
			if (!ok) { forceable = false; break; }
		}
		if (forceable) result.push_back(b.pos);
	}
	return result;
}

// Unconstrained Pre: system can reach any ρ' at the next step.
inline std::vector<int> Pre_over_T1_any(
    const std::vector<QltType1>& T1,
    const std::vector<QltType2>& T2,
    const std::vector<int>& W_indices)
{
	auto admissible = [&](const QltType2&) {
		std::vector<int> all;
		for (const auto& t : T1) all.push_back(t.pos);
		return all;
	};
	return Pre_over_T1<decltype(admissible)>(T1, T2, W_indices, admissible);
}

// ── Fixpoint operators over 2^{T_1} (Algorithm D Phase 3). ─────────────

// νX. f(X).  Start with X = T_1, iterate until stable.
template <class F>
inline std::vector<int> nu_fixpoint(const std::vector<QltType1>& T1, F f) {
	std::vector<int> cur;
	for (const auto& t : T1) cur.push_back(t.pos);
	for (;;) {
		auto next = f(cur);
		std::sort(next.begin(), next.end());
		std::vector<int> sorted_cur = cur;
		std::sort(sorted_cur.begin(), sorted_cur.end());
		if (next == sorted_cur) return next;
		cur = std::move(next);
	}
}

// μX. f(X).  Start with X = ∅, iterate until stable.
template <class F>
inline std::vector<int> mu_fixpoint(const std::vector<QltType1>& /*T1*/, F f) {
	std::vector<int> cur;
	for (;;) {
		auto next = f(cur);
		std::sort(next.begin(), next.end());
		std::vector<int> sorted_cur = cur;
		std::sort(sorted_cur.begin(), sorted_cur.end());
		if (next == sorted_cur) return next;
		cur = std::move(next);
	}
}

// ── Opt-1: forward reachability pruning on T_1. ───────────────────────
//
// Given an initial memory type b_0 and a transition relation
// next: (b, σ) → set of reachable ρ' memory types (what the system can
// produce in response to environment input σ), compute the set of
// 1-types reachable from b_0 via any sequence of env+system moves.
//
// Intended use:  pass this set as a mask into the main game so that
// subsequent Pre evaluations only scan reachable types.
template <class NextRelation>
inline std::vector<int> reachable_from(
    int b0,
    const std::vector<QltType1>& T1,
    const std::vector<QltType2>& T2,
    NextRelation next)
{
	std::vector<char> seen(T1.size(), 0);
	std::vector<int> queue{b0};
	seen[b0] = 1;
	while (!queue.empty()) {
		int b = queue.back(); queue.pop_back();
		for (const auto& s : T2) {
			if (s.pos_m != b) continue;
			for (int rho : next(s)) {
				if (rho >= 0 && rho < (int)T1.size() && !seen[rho]) {
					seen[rho] = 1;
					queue.push_back(rho);
				}
			}
		}
	}
	std::vector<int> out;
	for (size_t i = 0; i < seen.size(); ++i) if (seen[i]) out.push_back((int)i);
	return out;
}

} // namespace idni::tau_lang::omcat

#endif // __IDNI__TAU__OMCAT_TYPES_H__
