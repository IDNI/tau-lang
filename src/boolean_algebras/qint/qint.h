// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_H__

#include <compare>
#include <map>
#include <string>
#include <optional>
#include <functional>

#include "tau_tree.h"
#include "ba_constants.h"
#include "splitter_types.h"
#include "boolean_algebras/qint/parser/qint_parser.generated.h"

namespace idni::tau_lang {

/** @brief Type tree of the qint type. */
template <NodeType node> tref qint_type();
/** @brief Type id of the qint type. */
template <NodeType node> size_t qint_type_id();


// -----------------------------------------------------------------------------
// qint — atomless Boolean algebra of left-closed right-open intervals [a, b)
// on the extended rational line. Constant syntax accepts rationals (1/3),
// decimals (0.25, 1e3) and integers; every endpoint is kept exactly.
//
// Represented as std::map<qint_rational, qint_rational> where each entry
// {lo -> hi} encodes the interval [lo, hi).  The map is kept sorted by lo and
// normalised (no overlapping or adjacent entries).
//
// Top element:    single entry { -inf -> +inf }
// Bottom element: empty map
//
// Integer-literal semantics: in `{n}:qint` sources the bare integers
// 0 and 1 are the ALGEBRAIC constants bottom and top, NOT intervals; any
// other bare number is rejected, since qint has no points.
// -----------------------------------------------------------------------------

/**
 * @brief An exact endpoint: p/q in lowest terms with q > 0, or an infinity,
 * encoded as q == 0 with p = +1 or -1.
 *
 * A literal or a split whose exact value does not fit a 64-bit numerator and
 * denominator is rejected rather than rounded.
 */
struct qint_rational {
	long long p = 0, q = 1;

	/// @brief Zero.
	qint_rational() = default;
	/// @brief The integer @p n.
	qint_rational(long long n) : p(n), q(1) {}
	/**
	 * @brief The rational @p num / @p den, reduced to lowest terms.
	 *
	 * @p den must not be 0; use pos_inf() / neg_inf() for the infinities.
	 * When @p den is 0 or the reduced value does not fit, the value is 0.
	 */
	qint_rational(long long num, long long den);

	/// @brief The endpoint +inf.
	static qint_rational pos_inf() { qint_rational r; r.p = 1; r.q = 0; return r; }
	/// @brief The endpoint -inf.
	static qint_rational neg_inf() { qint_rational r; r.p = -1; r.q = 0; return r; }

	/// @brief True for either infinity.
	bool is_inf()     const noexcept { return q == 0; }
	/// @brief True for +inf.
	bool is_pos_inf() const noexcept { return q == 0 && p > 0; }
	/// @brief True for -inf.
	bool is_neg_inf() const noexcept { return q == 0 && p < 0; }

	/// @brief Exact equality; sound because values are kept reduced.
	bool operator==(const qint_rational& o) const noexcept {
		return p == o.p && q == o.q;
	}
	/// @brief Numeric order on the extended line: -inf < finite < +inf.
	std::strong_ordering operator<=>(const qint_rational& o) const noexcept;
};

namespace qint_detail {

/**
 * @brief Parse an endpoint exactly.
 *
 * Accepts, after trimming whitespace: `+inf`/`inf`/`+infinity`/`infinity`,
 * `-inf`/`-infinity`, integers, decimals with an optional exponent (1.5,
 * .5, 2e-3), and a fraction of two such numbers.
 * @param s Endpoint text.
 * @param out Set to the value on success, untouched otherwise.
 * @return False when @p s is malformed, divides by zero, or its exact value
 *         does not fit a 64-bit numerator and denominator.
 */
bool parse_endpoint(const std::string& s, qint_rational& out);

/// @brief Format an endpoint for display: "+inf"/"-inf", an integer, a
/// terminating decimal, or p/q.
std::string endpoint_to_string(const qint_rational& v);

} // namespace qint_detail

// -----------------------------------------------------------------------------
// qint: finite union of disjoint half-open intervals [lo, hi).
// std::map key = lo, value = hi.  Sorted and normalised by construction.
// -----------------------------------------------------------------------------

struct qint {
	/// Disjoint, non-adjacent intervals, key = lo, value = hi.
	std::map<qint_rational, qint_rational> intervals; // key=lo, value=hi

	// --- factories ---
	/// @brief The empty set (the BA zero).
	static qint bottom();
	/// @brief The whole line [-inf, +inf) (the BA one).
	static qint top();

	/// @brief True for bottom.
	bool is_empty() const noexcept { return intervals.empty(); }
	/// @brief True for top.
	bool is_full()  const noexcept;

	/// @brief Set equality (the representation is canonical).
	bool operator==(const qint& o) const noexcept;
	/// @brief Negation of operator==(const qint&).
	bool operator!=(const qint& o) const noexcept;
	/// @brief Compare with a Boolean constant: true means top, false bottom.
	bool operator==(bool b)        const noexcept;
	/// @brief Negation of operator==(bool).
	bool operator!=(bool b)        const noexcept;

	/// @brief Ordering for use in ordered containers (lexicographic on map
	/// entries); not the BA order.
	bool operator<(const qint& o) const noexcept;
	/// @brief Three-way form of operator<, consistent with operator==.
	std::strong_ordering operator<=>(const qint& o) const noexcept;

	/// @brief Union.
	qint operator|(const qint& o) const;
	/// @brief Intersection.
	qint operator&(const qint& o) const;
	/// @brief Complement in [-inf, +inf).
	qint operator~()              const;
	/// @brief Symmetric difference.
	qint operator^(const qint& o) const;

	/// @brief "bot", "top", or the intervals as `[lo, hi)` joined by " | ".
	std::string to_string() const;

private:
	// Merge overlapping/adjacent intervals in a map sorted by lo
	static qint normalize_map(std::map<qint_rational, qint_rational> m);
};

// --- stream output ---
/// @brief Write qint::to_string() of @p d.
std::ostream& operator<<(std::ostream& os, const qint& d);

// --- free functions expected by the dispatcher ---

/// @brief True when @p x is bottom.
bool is_qint_zero(const qint& x);
/// @brief True when @p x is top.
bool is_qint_one (const qint& x);
/// @brief Identity: a qint is kept normalized by construction.
qint normalize_qint(const qint& x);
/// @brief Identity: qint has no symbol-level simplification.
tref simplify_qint_symbol(tref sym);
/// @brief Identity: qint has no term-level simplification.
tref simplify_qint_term(tref t);
/**
 * @brief A splitter of @p x: a piece of its first interval [l, h).
 *
 * Returns [l, h-1) when l is -inf, [l, l+1) when h is +inf, [-inf, 0) for
 * top, and [l, midpoint) otherwise; bottom for bottom. Unlike sbf_splitter
 * (which honors every splitter_type and always makes progress), this
 * splitter ignores @p st and MAY RETURN @p x's first interval UNCHANGED when
 * the cut (h-1, l+1 or the midpoint) does not fit a 64-bit numerator and
 * denominator. Callers looping "split until proper subset" must guard
 * against a fixpoint.
 * @param x Element to split.
 * @param st Ignored.
 */
qint qint_splitter(const qint& x, splitter_type st);
/// @brief A fixed splitter of top: [0, 1/2).
qint qint_splitter_one();

// --- parsing helpers (declared here, defined in qint.cpp) ---

/**
 * @brief Evaluate a parsed `[lo, hi)` interval node.
 * @return The interval, or nullopt when an endpoint is missing or malformed
 *         or lo is not below hi.
 */
std::optional<qint> qint_eval_interval(
	const qint_parser::tree::traverser& interval_node);

/**
 * @brief Evaluate a parsed qint literal (top, bottom, an interval or a
 * union of intervals).
 * @param t Traverser at the `qint` node.
 * @return The element; no value and no error when an interval is
 *         malformed; an internal error for an unknown node.
 */
result<qint> qint_eval_parse_tree(
	const qint_parser::tree::traverser& t);

/// Content hash in uint64_t, the same on every platform.
std::uint64_t qint_hash(const qint& d) noexcept;

} // namespace idni::tau_lang

// --- Hash specialization for qint ---
/// @brief std::hash over qint_hash.
template<>
struct std::hash<idni::tau_lang::qint> {
	size_t operator()(const idni::tau_lang::qint& d) const noexcept;
};

#include "boolean_algebras/qint/qint.tmpl.h"
#include "boolean_algebras/qint/qint_descriptor.tmpl.h"
#include "boolean_algebras/qint/qint_types.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_H__
