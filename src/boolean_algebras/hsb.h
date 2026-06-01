// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_H__

#include <algorithm>
#include <cassert>
#include <cmath>
#include <compare>
#include <functional>
#include <map>
#include <numeric>
#include <optional>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>

#include "tau_tree.h"
#include "splitter_types.h"
#include "boolean_algebras/cvc5/cvc5.h"
#include "boolean_algebras/hsb_node.h"

namespace idni::tau_lang {

/// @cond INTERNAL
namespace hsb_detail {
	/// @brief Returns true iff @p a and @p b are equal (NaN-safe: NaN ≠ NaN).
	inline bool feq(double a, double b) noexcept {
		return !std::isnan(a) && !std::isnan(b) && !(a < b) && !(b < a);
	}
}
/// @endcond

// =============================================================================
/**
 * @brief A single canonical halfspace constraint H_{w,b} in R^d.
 *
 * The halfspace is defined by a normal vector `w ∈ R^d \ {0}` and bias `b`:
 * @code
 *   H_{w,b} = { x ∈ R^d : ⟨w,x⟩ + b ≤ 0 }   if s(w) = -1  (closed)
 *   H_{w,b} = { x ∈ R^d : ⟨w,x⟩ + b <  0 }   if s(w) = +1  (open/strict)
 * @endcode
 * where the **lex-leading sign** `s(w) = sign(w_{L(w)})` is the sign of the
 * first non-zero component of `w`.
 *
 * Strictness is canonical: it is determined by `lex_leading_sign()`, not by
 * the comparison operator that appeared in the source text.  The complement of
 * H_{w,b} is H_{-w,-b} (sign flip ⟺ open↔closed).
 */
// =============================================================================
struct hsb_halfspace {
	std::vector<double> w;  ///< Normal vector (d dimensions).
	double b = 0.0;         ///< Bias: ⟨w,x⟩ + b {≤,<} 0.

	/// @brief Returns the ambient dimension d.
	size_t dimension() const noexcept;

	/**
	 * @brief Returns L(w): the index of the first non-zero component of w.
	 * @returns Index in [0, d); returns d for the degenerate zero vector.
	 */
	size_t lex_leading_index() const noexcept;

	/**
	 * @brief Returns s(w) = sign(w_{L(w)}): +1 (strict), -1 (non-strict),
	 *        or 0 for the degenerate zero vector.
	 */
	int lex_leading_sign() const noexcept;

	/// @brief Returns true iff this halfspace uses strict inequality (s(w) = +1).
	bool is_strict() const noexcept;

	/// @brief Evaluates ⟨w,x⟩ + b. If the point dimension is less than w.size(),
	/// only the first x.size() components are used and the rest are treated as 0.
	double eval(const std::vector<double>& x) const noexcept;

	/// @brief Returns true iff x satisfies the halfspace constraint.
	bool contains(const std::vector<double>& x) const noexcept;

	/**
	 * @brief Returns the complement halfspace H_{-w,-b}.
	 *
	 * The complement satisfies R^d ∖ H_{w,b} = H_{-w,-b}, with the sign of
	 * the leading component flipped (strict ↔ non-strict).
	 */
	hsb_halfspace negate() const;

	bool operator==(const hsb_halfspace& o) const noexcept;
	bool operator!=(const hsb_halfspace& o) const noexcept;

	/**
	 * @brief Phase-1 normalization: divides w and b by |w[highest-indexed non-zero]|
	 *        so that the coefficient of the highest-indexed variable is exactly ±1.
	 *
	 * Preserves `is_strict()` (scaling by a positive divisor keeps all signs).
	 * Returns `*this` unchanged if w is the zero vector.
	 */
	hsb_halfspace normalize() const;

	/// @brief Lexicographic ordering on (w, b) — used to sort boundary expressions.
	bool operator<(const hsb_halfspace& o) const noexcept;

	/// @brief Returns a string like "2*x[0] - x[1] + 3 < 0".
	std::string to_string() const;
};

// =============================================================================
// hsb_halfspace_pool — append-only interned pool of hsb_halfspace values
// =============================================================================

/// @cond INTERNAL
struct hsb_halfspace_pool {
	/// Insert h (after normalizing) and return its pool index.
	/// Returns an existing index if an equal normalized halfspace is found.
	static size_t insert(const hsb_halfspace& h);

	/// Retrieve a halfspace by pool index.
	static const hsb_halfspace& get(size_t idx);

	/// Return the pool index of the complement (~h), inserting if needed.
	static size_t complement_index(size_t idx);

	static size_t size();

private:
	inline static std::vector<hsb_halfspace> pool_;
	inline static std::map<hsb_halfspace, size_t> index_;
};
/// @endcond

// =============================================================================
/**
 * @brief Atomless Boolean algebra of lex-half-open polyhedra in R^d (LP_d^Q).
 *
 * Generalises `qint` (1D half-open intervals) to d dimensions.  Elements are
 * Boolean combinations of canonical halfspaces, stored as interned formula
 * trees (lcrs_tree<hsb_node>).
 *
 * **Boolean operations carry structural short-circuit simplifications:**
 *   - `A & bot = bot`,  `A & top = A`,  `A & A = A`,  `A & ~A = bot`
 *   - `A | bot = A`,   `A | top = top`, `A | A = A`,  `A | ~A = top`
 *   - `~~A = A`,  `~H_{w,b} = H_{-w,-b}`
 */
// =============================================================================
struct hsb {
	// ── Kind ─────────────────────────────────────────────────────────────────

	/// @brief Tag identifying the kind of a formula-tree node.
	/// Values match hsb_parser::nonterminal for seamless conversion.
	enum class kind : uint8_t {
		bot       = hsb_parser::hsb_bot,
		top       = hsb_parser::hsb_top,
		halfspace = hsb_parser::hsb_hs,
		and_      = hsb_parser::hsb_and,
		or_       = hsb_parser::hsb_or,
		not_      = hsb_parser::hsb_not
	};

	// ── Root ─────────────────────────────────────────────────────────────────

	htref root;  ///< GC-safe handle into the lcrs_tree<hsb_node> store.

	hsb();
	explicit hsb(tref r);
	explicit hsb(htref h);

	// ── Internal tref access ─────────────────────────────────────────────────

	/// Returns the raw tref of the root node.
	tref root_ref() const noexcept { return root ? root->get() : nullptr; }

	// ── Node factories (return tref for use in tree construction) ────────────

	/// @cond INTERNAL
	static tref mk_bot();
	static tref mk_top();
	static tref mk_hs(const hsb_halfspace& h);
	static tref mk_hs_by_index(size_t pool_idx);
	static tref mk_and(tref l, tref r);
	static tref mk_or(tref l, tref r);
	static tref mk_not(tref inner);
	/// @endcond

	// ── Element factories ────────────────────────────────────────────────────

	/// @brief Returns the bottom element (empty set ∅).
	static hsb bottom();

	/// @brief Returns the top element (full space R^d).
	static hsb top();

	/**
	 * @brief Constructs an hsb from a single halfspace constraint.
	 * @param h  The halfspace H_{w,b} to wrap.
	 */
	static hsb from_halfspace(const hsb_halfspace& h);

	// ── Tree accessor helpers ─────────────────────────────────────────────────

	/// Returns the kind of the root node.
	kind root_kind() const noexcept;

	/// Returns the halfspace stored at the root (valid only if root_kind() == kind::halfspace).
	const hsb_halfspace& root_halfspace() const;

	/// Returns the left (first) child as an hsb (valid for and_/or_ nodes).
	hsb lhs() const;

	/// Returns the right (second) child as an hsb (valid for and_/or_ nodes).
	hsb rhs() const;

	/// Returns the inner (first) child as an hsb (valid for not_ nodes).
	hsb inner() const;

	// ── Boolean operations ───────────────────────────────────────────────────

	/**
	 * @brief Conjunction with structural short-circuit simplifications.
	 *
	 * Applies: `bot & _ = bot`, `top & A = A`, `A & A = A`, `A & ~A = bot`.
	 */
	hsb operator&(const hsb& o) const;

	/**
	 * @brief Disjunction with structural short-circuit simplifications.
	 *
	 * Applies: `bot | A = A`, `top | _ = top`, `A | A = A`, `A | ~A = top`.
	 */
	hsb operator|(const hsb& o) const;

	/**
	 * @brief Complement with structural simplifications.
	 *
	 * Applies: `~~A = A`, `~H_{w,b} = H_{-w,-b}` (single halfspace).
	 */
	hsb operator~() const;

	/**
	 * @brief Symmetric difference: `(A | B) & ~(A & B)`.
	 * @param o  Right operand.
	 */
	hsb operator^(const hsb& o) const;

	// ── Comparisons ──────────────────────────────────────────────────────────

	/// @brief Structural equality (O(1) tref pointer comparison after interning).
	bool operator==(const hsb& o) const noexcept;
	bool operator!=(const hsb& o) const noexcept;

	/// @brief Compares with a bool: `true` iff equal to top/bot respectively.
	bool operator==(bool b) const;
	bool operator!=(bool b) const;

	/// @brief Structural ordering via `to_string()` output.
	bool operator<(const hsb& o) const;
	std::strong_ordering operator<=>(const hsb& o) const;

	// ── Serialisation ────────────────────────────────────────────────────────

	/// @brief Returns a human-readable string, e.g. `"(x[0] < 0 & ~(x[1] < 0))"`.
	std::string to_string() const;

	/**
	 * @brief Recognises the three keywords `"top"`, `"bot"`, `"bottom"`;
	 *        returns `bottom()` for any other input.
	 */
	static hsb from_string(const std::string& s);
};

/// @brief Stream output operator for hsb.
std::ostream& operator<<(std::ostream& os, const hsb& h);

} // namespace idni::tau_lang

#include "boolean_algebras/hsb.tmpl.h"
#include "boolean_algebras/hsb_splitter.tmpl.h"
#include "boolean_algebras/hsb_normalizer.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_H__
