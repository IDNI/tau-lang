// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT_H__

#include <compare>
#include <map>
#include <string>
#include <optional>
#include <functional>

#include "tau_tree.h"
#include "splitter_types.h"
#include "../parser/qint_parser.generated.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// qint — atomless Boolean algebra of left-closed right-open intervals [a, b)
// on the extended real line.
//
// Represented as std::map<double, double> where each entry {lo -> hi} encodes
// the interval [lo, hi).  The map is kept sorted by lo and normalised
// (no overlapping or adjacent entries).
//
// Special endpoints: ±std::numeric_limits<double>::infinity() represent the
// extended real line boundaries.
// Top element:    single entry { -inf -> +inf }
// Bottom element: empty map
// -----------------------------------------------------------------------------

namespace qint_detail {

static constexpr double POS_INF =  std::numeric_limits<double>::infinity();
static constexpr double NEG_INF = -std::numeric_limits<double>::infinity();

// Parse an endpoint string to double.
// Accepts: +inf/-inf, p/q fractions (all rationals), decimals, integers.
bool parse_endpoint(const std::string& s, double& out);

// Format a double endpoint for display ("+inf"/"-inf" for infinities).
std::string endpoint_to_string(double v);

} // namespace qint_detail

// -----------------------------------------------------------------------------
// qint: finite union of disjoint half-open intervals [lo, hi).
// std::map key = lo, value = hi.  Sorted and normalised by construction.
// -----------------------------------------------------------------------------

struct qint {
	std::map<double, double> intervals; // key=lo, value=hi

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
	static qint normalize_map(std::map<double, double> m);
};

// --- stream output ---
std::ostream& operator<<(std::ostream& os, const qint& d);

// --- free functions expected by the dispatcher ---

bool is_qint_zero(const qint& x);
bool is_qint_one (const qint& x);
qint normalize_qint(const qint& x);
tref simplify_qint_symbol(tref sym);
tref simplify_qint_term(tref t);
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

#include "boolean_algebras/qint.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT_H__
