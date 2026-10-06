// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_H__

#include <compare>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <functional>
#include <sstream>
#include <numeric>
#include <charconv>
#include <cstdint>

#include "utility/hashing.h"
#include "tau_string_hash.h"


#include "tau_tree.h"
#include "ba_constants.h"
#include "env_limits.h"
#include "splitter_types.h"
#include "boolean_algebras/qlt/parser/qlt_parser.generated.h"

namespace idni::tau_lang {

/**
 * @brief Cap on the data atoms the omcat (qlt) T3 encodings accept: the
 * A/B/D skeletons and the semantic PWR compute `1 << K` and enumerate 2^K
 * masks, so K is bounded here. Above the cap the default
 * ABA-oracle path decides instead. Runtime parameter by policy (qlt's own
 * `qlt-t3-cap` CLI/REPL option); clamped to 30 (a signed shift is
 * undefined at 31); 0 = unlimited within that bound.
 *
 * The sentinel -1 means "not set", in which case `TAU_QLT_T3_CAP` is
 * consulted and 20 applies when that is absent too; the option always wins
 * over the variable. Read through @ref qlt_t3_encoding_cap.
 */
inline long qlt_t3_encoding_cap_param = -1;

/**
 * @brief Cap on the output-position combinations the constant-output fast
 * path in front of Algorithm B enumerates (LA-10): |T1|^outputs candidate
 * constant assignments, each checked with `ltlfilt`. Above it the fast
 * path declines and Algorithm B decides. Runtime parameter by policy
 * (qlt's own `qlt-const-output-max` CLI/REPL option); 0 = unlimited.
 *
 * The sentinel -1 means "not set", in which case `TAU_QLT_CONST_OUTPUT_MAX`
 * is consulted and 100 applies when that is absent too. Read through
 * @ref qlt_const_output_max.
 */
inline long qlt_const_output_max_param = -1;

/**
 * @brief Effective data-atom cap of the T3 encodings before the 30 bound.
 *
 * Precedence: @ref qlt_t3_encoding_cap_param when set (>= 0), else
 * `TAU_QLT_T3_CAP`, else 20.
 */
inline size_t qlt_t3_encoding_cap() {
	if (qlt_t3_encoding_cap_param >= 0)
		return (size_t) qlt_t3_encoding_cap_param;
	return env_limit_count("TAU_QLT_T3_CAP", 20);
}

/**
 * @brief Effective cap on the constant-output assignments the fast path in
 * front of Algorithm B enumerates (0 = unlimited).
 *
 * Precedence: @ref qlt_const_output_max_param when set (>= 0), else
 * `TAU_QLT_CONST_OUTPUT_MAX`, else 100.
 */
inline size_t qlt_const_output_max() {
	if (qlt_const_output_max_param >= 0)
		return (size_t) qlt_const_output_max_param;
	return env_limit_count("TAU_QLT_CONST_OUTPUT_MAX", 100);
}

/**
 * @brief Cap on the formula instances one decision by cells evaluates (the
 * quantifier elimination, the comparisons over named endpoints and the point
 * solver); past it the decision is left open. Runtime parameter by policy
 * (qlt's own `qlt-cells-budget` option). The sentinel -1 means "not set":
 * `TAU_QLT_CELLS_BUDGET` is consulted, and 65536 applies when that is absent
 * too. 0 = unlimited.
 */
inline long qlt_cells_budget_param = -1;

/**
 * @brief Cap on the other free variables, and on the named endpoints, a
 * decision by cells ranges over; above it the decision is left open. Runtime
 * parameter by policy (qlt's own `qlt-cells-max-params` option). The
 * sentinel -1 means "not set": `TAU_QLT_CELLS_MAX_PARAMS` is consulted, and 2
 * applies when that is absent too. 0 = unlimited.
 */
inline long qlt_cells_max_params_param = -1;

/// Effective cell budget (0 = unlimited).
inline size_t qlt_cells_budget() {
	if (qlt_cells_budget_param >= 0) return (size_t) qlt_cells_budget_param;
	return env_limit_count("TAU_QLT_CELLS_BUDGET", 1 << 16);
}

/// Effective cap on the parameters of a decision by cells (0 = unlimited).
inline size_t qlt_cells_max_params() {
	if (qlt_cells_max_params_param >= 0)
		return (size_t) qlt_cells_max_params_param;
	return env_limit_count("TAU_QLT_CELLS_MAX_PARAMS", 2);
}

/// Effective T3 atom cap: `qlt_t3_encoding_cap()` bounded by 30.
inline int qlt_t3_encoding_cap_effective() {
	const size_t hard = 30;
	const size_t cap = qlt_t3_encoding_cap();
	if (cap == 0 || cap > hard) return (int) hard;
	return (int) cap;
}


/** @brief Type tree of the qlt type. */
template <NodeType node> tref qlt_type();
/** @brief Type id of the qlt type. */
template <NodeType node> size_t qlt_type_id();


// Type definitions for qlt (Q,<)


// -----------------------------------------------------------------------------
// qlt — the theory (Q, <).
//
// qlt is NOT a Boolean algebra.  It is the first-order theory of the
// dense linear order without endpoints over the rationals, which is
// ω-categorical (all countable models are isomorphic) and hence admits
// quantifier elimination.  The project supports it because of ω-categoricity,
// not because it is a BA.  A qlt variable or stream denotes one rational
// POINT; a qlt constant denotes a definable subset of Q, held as a finite
// normalised union of intervals.  Where a term combines a variable x with
// constants, x reads as the singleton {x}: `c & x != 0` says x lies in c,
// `x = c` that c is {x}, and `x = 0`, `x = 1` never hold.  Quantifiers over
// qlt variables are eliminated by qlt_qe.tmpl.h and solved by
// qlt_solver.tmpl.h with that reading.
//
// Interval endpoints are exact rationals p/q (long long numerator, positive
// denominator, reduced by gcd), together with a bound_type flag for each end:
//   OPEN   — endpoint not included
//   CLOSED — endpoint included
// Additionally we allow +inf / -inf (represented with special sentinels).
// -----------------------------------------------------------------------------

/// Whether an interval endpoint belongs to the interval.
enum class qlt_bound : uint8_t { OPEN = 0, CLOSED = 1 };

/**
 * @brief A rational number endpoint for qlt intervals.
 *
 * Three kinds:
 *   - Specific rational: sym empty, not inf  (p/q in reduced form, q > 0)
 *   - ±infinity:         sym empty, pos_inf/neg_inf set
 *   - Named constant:    sym non-empty — an uninterpreted constant whose
 *                        position in Q is unknown.  Named constants are placed
 *                        AFTER +inf in the normalisation order (lex by name) so
 *                        the sorting/merging algorithm keeps symbolic pieces
 *                        isolated from specific-rational pieces.
 */
struct qlt_rational {
	/// Numerator and denominator of a specific rational.
	long long p = 0, q = 1; // value = p/q, q > 0, normalised by gcd
	/// +inf when set (and sym is empty).
	bool pos_inf = false;
	/// -inf when set (and sym is empty).
	bool neg_inf = false;
	/// Name of a named constant; empty otherwise.
	std::string sym; // non-empty → named/uninterpreted constant

	/// The rational 0.
	qlt_rational() = default;
	/// The rational p_/q_, normalised; q_ must not be 0.
	qlt_rational(long long p_, long long q_) : p(p_), q(q_) { normalise(); }

	/// +inf.
	static qlt_rational make_pos_inf() { qlt_rational r; r.pos_inf = true; return r; }
	/// -inf.
	static qlt_rational make_neg_inf() { qlt_rational r; r.neg_inf = true; return r; }
	/// The named constant @p name.
	static qlt_rational make_sym(const std::string& name) {
		qlt_rational r; r.sym = name; return r;
	}

	/// True for a named constant.
	bool is_sym()     const { return !sym.empty(); }
	/// True for +inf.
	bool is_pos_inf() const { return pos_inf && sym.empty(); }
	/// True for -inf.
	bool is_neg_inf() const { return neg_inf && sym.empty(); }
	/// True for a specific rational.
	bool is_finite()  const { return !pos_inf && !neg_inf && sym.empty(); }

	/// Make q positive and reduce p/q by their gcd; a named constant is left
	/// as is.
	void normalise();

	/// Equal names for named constants, equal infinities, or equal p/q.
	bool operator==(const qlt_rational& o) const;
	/// Negation of operator==.
	bool operator!=(const qlt_rational& o) const { return !(*this == o); }

	/// Total order for normalisation: -inf < finite < +inf < sym (lex).
	/// For symbolic endpoints this is purely a canonical order — it does NOT
	/// imply a semantic ordering relative to specific rationals.  Use
	/// `qlt_sem_cmp` (below) wherever the *semantic* order is meant: read as
	/// semantic, this one makes `{c} & ~{c}` evaluate to `(c,+inf)`. Finite
	/// values compare in 128 bits, so the cross products do not overflow.
	bool operator<(const qlt_rational& o) const;
	/// Canonical order, see operator<.
	bool operator<=(const qlt_rational& o) const { return !(o < *this); }
	/// Canonical order, see operator<.
	bool operator>(const qlt_rational& o) const  { return o < *this; }
	/// Canonical order, see operator<.
	bool operator>=(const qlt_rational& o) const { return !(*this < o); }

	/// Midpoint between two finite rationals: (p1/q1 + p2/q2) / 2, computed
	/// in 128 bits and reduced; a reduced result outside `long long` is
	/// truncated.
	qlt_rational midpoint(const qlt_rational& o) const;
	/// Sum of two finite rationals, computed in 128 bits and reduced;
	/// nullopt when the sum does not fit `long long`.
	std::optional<qlt_rational> add(const qlt_rational& o) const;

	/// The name, `+inf`, `-inf`, `p` or `p/q`.
	std::string to_string() const;

	/// Parse a rational from a string: integer, p/q, decimal (`0.45`, `.5`,
	/// `5.`), +inf/inf/-inf, or identifier. Identifiers (letter/underscore
	/// start, alnum/underscore body) are parsed as named (uninterpreted)
	/// constants. Surrounding whitespace is ignored.
	/// @return Whether @p s parsed; @p out is written only on success.
	static bool parse(const std::string& s, qlt_rational& out);
};

/// An endpoint of an interval: a rational with open/closed bound
struct qlt_endpoint {
	/// The endpoint's value.
	qlt_rational val;
	/// Whether the endpoint belongs to the interval.
	qlt_bound    bound; // OPEN or CLOSED

	/// The open endpoint at 0.
	qlt_endpoint() : val(), bound(qlt_bound::OPEN) {}
	/// The endpoint @p v with bound @p b.
	qlt_endpoint(qlt_rational v, qlt_bound b) : val(v), bound(b) {}

	/// Structural equality of value and bound.
	bool operator==(const qlt_endpoint& o) const {
		return val == o.val && bound == o.bound;
	}
};

/// A single interval piece
struct qlt_piece {
	/// Lower and upper endpoint.
	qlt_endpoint lo, hi;
	/// Structural equality of both endpoints.
	bool operator==(const qlt_piece& o) const {
		return lo == o.lo && hi == o.hi;
	}
};

// --- free function declarations ---

/// Semantic order of two endpoint values, as opposed to the canonical order of
/// `qlt_rational::operator<`.  A named constant denotes an unknown rational, so
/// only three facts about it are decidable: it is above -inf, below +inf, and
/// equal to itself.  Everything else -- a named constant against a specific
/// rational, or against a differently-named constant -- is genuinely unknown
/// and is reported as `unordered`.
std::partial_ordering qlt_sem_cmp(const qlt_rational& a, const qlt_rational& b);
/// True when @p x is above the lower endpoint @p lo (on it, if closed);
/// false whenever a named constant is involved.
bool qlt_above_lo(const qlt_endpoint& lo, const qlt_rational& x);
/// True when @p x is below the upper endpoint @p hi (on it, if closed);
/// false whenever a named constant is involved.
bool qlt_below_hi(const qlt_endpoint& hi, const qlt_rational& x);
/// True when upper endpoint @p a is semantically below @p b, or at the same
/// value but open where @p b is closed; false when undecidable.
bool qlt_hi_less(const qlt_endpoint& a, const qlt_endpoint& b);
/// The intersection endpoints: the tighter lower (qlt_lo_max) or upper
/// (qlt_hi_min) endpoint.  `nullopt` means the two endpoints are not
/// semantically comparable (see qlt_sem_cmp), so no exact answer exists;
/// callers over-approximate rather than invent one.
std::optional<qlt_endpoint> qlt_lo_max(const qlt_endpoint& a, const qlt_endpoint& b);
/// See qlt_lo_max.
std::optional<qlt_endpoint> qlt_hi_min(const qlt_endpoint& a, const qlt_endpoint& b);
/// The looser of two upper endpoints (for a union); @p a when undecidable.
qlt_endpoint qlt_hi_max(const qlt_endpoint& a, const qlt_endpoint& b);
/// True when @p p is provably empty; a piece whose emptiness is undecidable
/// counts as non-empty (over-approximation).
bool qlt_piece_empty(const qlt_piece& p);
/// True when the pieces provably share a point; undecidable is false.
bool qlt_pieces_overlap(const qlt_piece& a, const qlt_piece& b);
/// True when @p a ends where @p b starts, at the same value, with at least
/// one of the two touching endpoints closed.
bool qlt_pieces_adjacent(const qlt_piece& a, const qlt_piece& b);
/// True when the pieces overlap or are adjacent and their union is
/// representable (both lower and both upper endpoints comparable).
bool qlt_pieces_mergeable(const qlt_piece& a, const qlt_piece& b);
/// Union of two mergeable pieces (see qlt_pieces_mergeable).
qlt_piece qlt_merge(const qlt_piece& a, const qlt_piece& b);
/// Intersection of two pieces; `nullopt` when the result is empty.  When the
/// endpoints are not comparable the intersection is over-approximated by the
/// symbolic operand (see the definition in qlt.cpp).
std::optional<qlt_piece> qlt_piece_intersect(const qlt_piece& a, const qlt_piece& b);

// -----------------------------------------------------------------------------
// qlt: finite normalised union of intervals
// -----------------------------------------------------------------------------

/**
 * @brief A qlt constant: a definable subset of Q held as a finite
 * normalised union of interval pieces.
 */
struct qlt {
	/// The interval pieces.
	std::vector<qlt_piece> pieces; // sorted by lo, disjoint, normalised
	/// Set when `pieces` OVER-approximates the true set: an intersection
	/// whose endpoint comparison was undecidable kept a whole operand
	/// (qlt_piece_intersect), or a piece's emptiness is undecidable
	/// (qlt_piece_empty). The flag propagates through `|` and `&`, and
	/// `operator~` returns `top` for an inexact value: complementing an
	/// over-approximation exactly would UNDER-approximate, so that
	/// `x = {c} && ~({c} & [0,1])` would reach a wrong UNSAT. Structural
	/// equality ignores the flag (it compares the representation).
	bool inexact = false;

	/// The empty set.
	static qlt bottom() { return {}; }
	/// All of Q, the single piece (-inf, +inf).
	static qlt top();

	/// True when there is no piece.
	bool is_empty() const { return pieces.empty(); }
	/// True when the value is the single piece (-inf, +inf).
	///
	/// Structural, so it can under-report.  With symbolic endpoints
	/// `normalise` cannot always merge (the union of two pieces whose relative
	/// order is unknown is not representable), so a value covering all of Q may
	/// still be held as several pieces.  For the same reason `==` and hence
	/// associativity of `|` are structural, not semantic, once named constants
	/// are involved.
	bool is_full() const;

	/// Structural equality of the pieces; ignores `inexact`.
	bool operator==(const qlt& o) const { return pieces == o.pieces; }
	/// Negation of operator==.
	bool operator!=(const qlt& o) const { return !(*this == o); }
	/// Against `true`: is_full(); against `false`: is_empty().
	bool operator==(bool b) const { return b ? is_full() : is_empty(); }
	/// Negation of operator==(bool).
	bool operator!=(bool b) const { return !(*this == b); }
	/// Canonical order: by piece count, then piece by piece in the canonical
	/// endpoint order; ignores `inexact`.
	bool operator<(const qlt& o) const;
	/// Three-way form of operator== and operator<.
	std::strong_ordering operator<=>(const qlt& o) const;

	/// Union, normalised; inexact if either operand is.
	qlt operator|(const qlt& o) const;
	/// Pairwise intersection of the pieces, normalised; inexact if either
	/// operand is or an intersection had to be over-approximated.
	qlt operator&(const qlt& o) const;
	/// Complement. `top` for an inexact value; an inexact `top` when the
	/// pieces cannot be placed in the order of Q (qlt_pieces_unordered).
	qlt operator~() const;
	/// Symmetric difference, `(a | b) & ~(a & b)`.
	qlt operator^(const qlt& o) const;
	/// `bot`, `top`, or the pieces joined by ` | ` (a closed singleton as its
	/// value alone).
	std::string to_string() const;

private:
	/// Drop the empty pieces, sort, and merge what is mergeable.
	static qlt normalise(std::vector<qlt_piece> ps);
};

/// True when the emptiness of @p p cannot be decided (a symbolic endpoint
/// against an incomparable one); such a piece is kept as non-empty, which
/// over-approximates (see qlt_piece_empty).
bool qlt_piece_emptiness_undecidable(const qlt_piece& p);

/// True when the pieces of @p q cannot be placed in the order of Q: a piece's
/// emptiness, or the order of two consecutive pieces, depends on a named
/// endpoint. Such a value is exact as a set but cannot be complemented.
bool qlt_pieces_unordered(const qlt& q);

// --- stream output ---
/// Print `q.to_string()` to @p os.
std::ostream& operator<<(std::ostream& os, const qlt& q);

// --- parsing helpers (called from templates in qlt.tmpl.h) ---
/// The single-piece value of an `interval` parse node; a parse error when it
/// lacks a bracket or two endpoints, an endpoint does not parse, or the
/// (non-symbolic) interval is empty.
result<qlt> qlt_eval_interval(
	const qlt_parser::tree::traverser& interval_node);
/// The value of a `qlt` parse node: top, bot, a singleton, an interval or a
/// union. A parse error when a singleton or an interval does not evaluate;
/// an internal error for an unknown node.
result<qlt> qlt_eval_parse_tree(
	const qlt_parser::tree::traverser& t);

// --- free functions expected by the dispatcher ---

/// True when @p x is the empty set.
bool is_qlt_zero(const qlt& x);
/// True when @p x is structurally all of Q (see qlt::is_full).
bool is_qlt_one(const qlt& x);
/// Identity: a qlt value is kept normalised by construction.
qlt normalize_qlt(const qlt& x);
/// Identity: qlt has no symbol simplification.
tref simplify_qlt_symbol(tref sym);
/// Identity: qlt has no term simplification.
tref simplify_qlt_term(tref t);
/// Return a non-empty part of the first piece of @p x: (-1, 0) for all of
/// Q, the lower half (by midpoint, or one unit wide toward an infinite
/// side) otherwise; bottom for bottom.
///
/// Contract note: unlike sbf_splitter (which honors every
/// splitter_type and always makes progress), this splitter ignores @p st
/// and MAY RETURN @p x UNCHANGED when the element is atomic/degenerate
/// (e.g. a singleton piece). Callers looping "split until proper subset"
/// must guard against a fixpoint.
qlt qlt_splitter(const qlt& x, splitter_type st);
/// A fixed element that is neither bottom nor top: (0, 1).
qlt qlt_splitter_one();

/// Content hashes in uint64_t, the same on every platform.
inline std::uint64_t qlt_rational_hash(const qlt_rational& r) {
	if (!r.sym.empty()) return tau_string_hash(r.sym) * 17239ULL;
	if (r.pos_inf) return UINT64_MAX;
	if (r.neg_inf) return UINT64_MAX - 1;
	std::uint64_t h = idni::portable_hash(r.p);
	h ^= idni::portable_hash(r.q) * 2654435761ULL;
	return h;
}

/// Content hash of the pieces (endpoints and bounds); ignores `inexact`.
inline std::uint64_t qlt_hash(const qlt& q) {
	std::uint64_t h = 0;
	for (auto& p : q.pieces) {
		h ^= qlt_rational_hash(p.lo.val) * 2654435761ULL;
		h ^= qlt_rational_hash(p.hi.val) * 2246822519ULL;
		h ^= (idni::portable_hash(p.lo.bound) * 7)
			^ (idni::portable_hash(p.hi.bound) * 13);
	}
	return h;
}

} // namespace idni::tau_lang

/// Hash specialization for qlt_rational
template<>
struct std::hash<idni::tau_lang::qlt_rational> {
	size_t operator()(const idni::tau_lang::qlt_rational& r) const noexcept {
		return static_cast<size_t>(idni::tau_lang::qlt_rational_hash(r));
	}
};

/// Hash specialization for qlt
template<>
struct std::hash<idni::tau_lang::qlt> {
	size_t operator()(const idni::tau_lang::qlt& q) const noexcept {
		return static_cast<size_t>(idni::tau_lang::qlt_hash(q));
	}
};

#include "boolean_algebras/qlt/qlt.tmpl.h"
#include "boolean_algebras/qlt/qlt_descriptor.tmpl.h"
#include "boolean_algebras/qlt/qlt_types.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_H__
