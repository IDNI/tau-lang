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
// Integer-literal semantics (BA1-7): in `{n}:qint` sources the bare integers
// 0 and 1 are the ALGEBRAIC constants bottom and top, NOT intervals; every
// other integer n denotes the interval [n, n+1). Consequently [0,1) and
// [1,2) are unreachable through bare-integer syntax -- write them as
// explicit intervals instead.
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

	qint_rational() = default;
	qint_rational(long long n) : p(n), q(1) {}
	/// @p den must not be 0; use pos_inf() / neg_inf() for the infinities.
	qint_rational(long long num, long long den);

	static qint_rational pos_inf() { qint_rational r; r.p = 1; r.q = 0; return r; }
	static qint_rational neg_inf() { qint_rational r; r.p = -1; r.q = 0; return r; }

	bool is_inf()     const noexcept { return q == 0; }
	bool is_pos_inf() const noexcept { return q == 0 && p > 0; }
	bool is_neg_inf() const noexcept { return q == 0 && p < 0; }

	bool operator==(const qint_rational& o) const noexcept {
		return p == o.p && q == o.q;
	}
	std::strong_ordering operator<=>(const qint_rational& o) const noexcept;
};

namespace qint_detail {

// Parse an endpoint exactly. Accepts: +inf/-inf, integers, decimals with an
// optional exponent (1.5, .5, 2e-3), and a fraction of two such numbers.
bool parse_endpoint(const std::string& s, qint_rational& out);

// Format an endpoint for display: "+inf"/"-inf", an integer, a terminating
// decimal, or p/q.
std::string endpoint_to_string(const qint_rational& v);

} // namespace qint_detail

// -----------------------------------------------------------------------------
// qint: finite union of disjoint half-open intervals [lo, hi).
// std::map key = lo, value = hi.  Sorted and normalised by construction.
// -----------------------------------------------------------------------------

struct qint {
	std::map<qint_rational, qint_rational> intervals; // key=lo, value=hi

	// --- factories ---
	static qint bottom();
	static qint top();

	bool is_empty() const noexcept { return intervals.empty(); }
	bool is_full()  const noexcept;

	bool operator==(const qint& o) const noexcept;
	bool operator!=(const qint& o) const noexcept;
	bool operator==(bool b)        const noexcept;
	bool operator!=(bool b)        const noexcept;

	// Ordering for use in ordered containers (lexicographic on map entries)
	bool operator<(const qint& o) const noexcept;
	std::strong_ordering operator<=>(const qint& o) const noexcept;

	qint operator|(const qint& o) const;
	qint operator&(const qint& o) const;
	qint operator~()              const;
	qint operator^(const qint& o) const;

	std::string to_string() const;

private:
	// Merge overlapping/adjacent intervals in an already-sorted map
	static qint normalize_map(std::map<qint_rational, qint_rational> m);
};

// --- stream output ---
std::ostream& operator<<(std::ostream& os, const qint& d);

// --- free functions expected by the dispatcher ---

bool is_qint_zero(const qint& x);
bool is_qint_one (const qint& x);
qint normalize_qint(const qint& x);
tref simplify_qint_symbol(tref sym);
tref simplify_qint_term(tref t);
/// BA1-5 contract note: unlike sbf_splitter (which honors every
/// splitter_type and always makes progress), this splitter ignores @p st
/// and MAY RETURN @p x UNCHANGED when the element is atomic/degenerate
/// (e.g. a singleton piece). Callers looping "split until proper subset"
/// must guard against a fixpoint.
qint qint_splitter(const qint& x, splitter_type st);
qint qint_splitter_one();

// --- parsing helpers (declared here, defined in qint.cpp) ---

std::optional<qint> qint_eval_interval(
	const qint_parser::tree::traverser& interval_node);

std::optional<qint> qint_eval_parse_tree(
	const qint_parser::tree::traverser& t);

} // namespace idni::tau_lang

// --- Hash specialization for qint ---
template<>
struct std::hash<idni::tau_lang::qint> {
	size_t operator()(const idni::tau_lang::qint& d) const noexcept;
};

#include "boolean_algebras/qint/qint.tmpl.h"
#include "boolean_algebras/qint/qint_descriptor.tmpl.h"
#include "boolean_algebras/qint/qint_types.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_H__
