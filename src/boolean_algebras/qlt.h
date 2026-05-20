// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT_H__

#include <compare>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <functional>
#include <sstream>
#include <numeric>
#include <charconv>


#include "tau_tree.h"
#include "splitter_types.h"
#include "../parser/qlt_parser.generated.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// qlt — the theory (Q, <).
//
// NOTE: qlt is NOT a Boolean algebra.  It is the first-order theory of the
// dense linear order without endpoints over the rationals, which is
// ω-categorical (all countable models are isomorphic) and hence admits
// quantifier elimination.  The project supports it because of ω-categoricity,
// not because it is a BA.  The C++ implementation uses finite normalised
// unions of intervals as a convenient representation of definable subsets of
// Q, but qlt values are rational POINTS, not sets.
//
// Interval endpoints are exact rationals p/q (long long numerator, positive
// denominator, reduced by gcd), together with a bound_type flag for each end:
//   OPEN   — endpoint not included
//   CLOSED — endpoint included
// Additionally we allow +inf / -inf (represented with special sentinels).
// -----------------------------------------------------------------------------

enum class qlt_bound : uint8_t { OPEN = 0, CLOSED = 1 };

// qlt_rational: a rational number endpoint for qlt intervals.
// Three kinds:
//   • Specific rational: sym empty, not inf  (p/q in reduced form, q > 0)
//   • ±infinity:         sym empty, pos_inf/neg_inf set
//   • Named constant:    sym non-empty — an uninterpreted constant whose
//                        position in Q is unknown.  Named constants are placed
//                        AFTER +inf in the normalisation order (lex by name) so
//                        the sorting/merging algorithm keeps symbolic pieces
//                        isolated from specific-rational pieces.
struct qlt_rational {
	long long p = 0, q = 1; // value = p/q, q > 0, normalised by gcd
	bool pos_inf = false;
	bool neg_inf = false;
	std::string sym; // non-empty → named/uninterpreted constant

	qlt_rational() = default;
	qlt_rational(long long p_, long long q_) : p(p_), q(q_) { normalise(); }

	static qlt_rational make_pos_inf() { qlt_rational r; r.pos_inf = true; return r; }
	static qlt_rational make_neg_inf() { qlt_rational r; r.neg_inf = true; return r; }
	static qlt_rational make_sym(const std::string& name) {
		qlt_rational r; r.sym = name; return r;
	}

	bool is_sym()     const { return !sym.empty(); }
	bool is_pos_inf() const { return pos_inf && sym.empty(); }
	bool is_neg_inf() const { return neg_inf && sym.empty(); }
	bool is_finite()  const { return !pos_inf && !neg_inf && sym.empty(); }

	void normalise();

	bool operator==(const qlt_rational& o) const;
	bool operator!=(const qlt_rational& o) const { return !(*this == o); }

	// Total order for normalisation: -inf < finite < +inf < sym (lex).
	// For symbolic endpoints this is purely a canonical order — it does NOT
	// imply a semantic ordering relative to specific rationals.
	bool operator<(const qlt_rational& o) const;
	bool operator<=(const qlt_rational& o) const { return !(o < *this); }
	bool operator>(const qlt_rational& o) const  { return o < *this; }
	bool operator>=(const qlt_rational& o) const { return !(*this < o); }

	// Midpoint between two finite rationals: (p1/q1 + p2/q2) / 2
	qlt_rational midpoint(const qlt_rational& o) const;
	qlt_rational operator+(const qlt_rational& o) const;

	std::string to_string() const;

	// Parse a rational from a string: integer, p/q, +inf/-inf, or identifier.
	// Identifiers (letter/underscore start, alnum/underscore body) are parsed as
	// named (uninterpreted) constants.
	static bool parse(const std::string& s, qlt_rational& out);
};

// An endpoint of an interval: a rational with open/closed bound
struct qlt_endpoint {
	qlt_rational val;
	qlt_bound    bound; // OPEN or CLOSED

	qlt_endpoint() : val(), bound(qlt_bound::OPEN) {}
	qlt_endpoint(qlt_rational v, qlt_bound b) : val(v), bound(b) {}

	bool operator==(const qlt_endpoint& o) const {
		return val == o.val && bound == o.bound;
	}
};

// A single interval piece
struct qlt_piece {
	qlt_endpoint lo, hi;
	bool operator==(const qlt_piece& o) const {
		return lo == o.lo && hi == o.hi;
	}
};

// --- free function declarations ---

bool qlt_lo_less(const qlt_piece& a, const qlt_piece& b);
bool qlt_above_lo(const qlt_endpoint& lo, const qlt_rational& x);
bool qlt_below_hi(const qlt_endpoint& hi, const qlt_rational& x);
bool qlt_hi_less(const qlt_endpoint& a, const qlt_endpoint& b);
qlt_endpoint qlt_lo_max(const qlt_endpoint& a, const qlt_endpoint& b);
qlt_endpoint qlt_hi_min(const qlt_endpoint& a, const qlt_endpoint& b);
qlt_endpoint qlt_hi_max(const qlt_endpoint& a, const qlt_endpoint& b);
bool qlt_piece_empty(const qlt_piece& p);
bool qlt_pieces_overlap(const qlt_piece& a, const qlt_piece& b);
bool qlt_pieces_adjacent(const qlt_piece& a, const qlt_piece& b);
qlt_piece qlt_merge(const qlt_piece& a, const qlt_piece& b);

// -----------------------------------------------------------------------------
// qlt: finite normalised union of intervals
// -----------------------------------------------------------------------------

struct qlt {
	std::vector<qlt_piece> pieces; // sorted by lo, disjoint, normalised

	static qlt bottom() { return {}; }
	static qlt top();

	bool is_empty() const { return pieces.empty(); }
	bool is_full() const;

	bool operator==(const qlt& o) const { return pieces == o.pieces; }
	bool operator!=(const qlt& o) const { return !(*this == o); }
	bool operator==(bool b) const { return b ? is_full() : is_empty(); }
	bool operator!=(bool b) const { return !(*this == b); }
	bool operator<(const qlt& o) const;
	std::strong_ordering operator<=>(const qlt& o) const;

	qlt operator|(const qlt& o) const;
	qlt operator&(const qlt& o) const;
	qlt operator~() const;
	qlt operator^(const qlt& o) const;
	std::string to_string() const;

private:
	static qlt normalise(std::vector<qlt_piece> ps);
};

// --- stream output ---
std::ostream& operator<<(std::ostream& os, const qlt& q);

// --- parsing helpers (called from templates in qlt.tmpl.h) ---
std::optional<qlt> qlt_eval_interval(
	const qlt_parser::tree::traverser& interval_node);
std::optional<qlt> qlt_eval_parse_tree(
	const qlt_parser::tree::traverser& t);

// --- free functions expected by the dispatcher ---

bool is_qlt_zero(const qlt& x);
bool is_qlt_one(const qlt& x);
qlt normalize_qlt(const qlt& x);
tref simplify_qlt_symbol(tref sym);
tref simplify_qlt_term(tref t);
qlt qlt_splitter(const qlt& x, splitter_type st);
qlt qlt_splitter_one();

} // namespace idni::tau_lang

// Hash specialization for qlt_rational
template<>
struct std::hash<idni::tau_lang::qlt_rational> {
	size_t operator()(const idni::tau_lang::qlt_rational& r) const noexcept {
		if (!r.sym.empty()) return std::hash<std::string>{}(r.sym) * 17239ULL;
		if (r.pos_inf) return 0xFFFFFFFFFFFFFFFFULL;
		if (r.neg_inf) return 0xFFFFFFFFFFFFFFFEULL;
		size_t h = std::hash<long long>{}(r.p);
		h ^= std::hash<long long>{}(r.q) * 2654435761ULL;
		return h;
	}
};

// Hash specialization for qlt
template<>
struct std::hash<idni::tau_lang::qlt> {
	size_t operator()(const idni::tau_lang::qlt& q) const noexcept {
		size_t h = 0;
		auto hr = std::hash<idni::tau_lang::qlt_rational>{};
		for (auto& p : q.pieces) {
			h ^= hr(p.lo.val) * 2654435761ULL;
			h ^= hr(p.hi.val) * 2246822519ULL;
			h ^= ((size_t)p.lo.bound * 7) ^ ((size_t)p.hi.bound * 13);
		}
		return h;
	}
};

#include "boolean_algebras/qlt.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT_BA_H__
