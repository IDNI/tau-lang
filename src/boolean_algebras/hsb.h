// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_H__

#include <algorithm>
#include <cassert>
#include <cmath>
#include <compare>
#include <functional>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>

#include "tau_tree.h"
#include "splitter_types.h"
#include "boolean_algebras/cvc5/cvc5.h"
#include "../parser/hsb_parser.generated.h"

namespace idni::tau_lang {

/// @cond INTERNAL
namespace hsb_detail {
	inline bool feq(double a, double b) noexcept {
		return !(a < b) && !(b < a);
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

	/// @brief Evaluates ⟨w,x⟩ + b.
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

	/// @brief Returns a string like "2*x[0] - x[1] + 3 < 0".
	std::string to_string() const;
};

// =============================================================================
/**
 * @brief Atomless Boolean algebra of lex-half-open polyhedra in R^d (LP_d^Q).
 *
 * Generalises `qint` (1D half-open intervals) to d dimensions.  Elements are
 * Boolean combinations of canonical halfspaces, represented as formula trees.
 *
 * For w ∈ R^d \ {0}:
 * @code
 *   L(w) = min { i : w_i ≠ 0 }         (lex-leading index)
 *   s(w) = sign(w_{L(w)}) ∈ {+1, -1}   (lex-leading sign)
 * @endcode
 *
 * **Key algebraic properties:**
 *   - Complement: R^d ∖ H_{w,b} = H_{-w,-b}  (sign flip ⟺ open↔closed)
 *   - Atomless: no equality set {⟨v,x⟩ = c} is expressible
 *   - 1D reduction: d = 1 gives exactly the `qint` half-open intervals
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
	enum class kind : uint8_t {
		bot,        ///< Bottom (empty set ∅).
		top,        ///< Top (full space R^d).
		halfspace,  ///< A single canonical halfspace H_{w,b}.
		and_,       ///< Boolean conjunction (A ∧ B).
		or_,        ///< Boolean disjunction (A ∨ B).
		not_        ///< Boolean negation (¬A).
	};

	// ── Node ─────────────────────────────────────────────────────────────────

	/// @brief Internal formula-tree node.  Not part of the public API.
	struct node {
		kind k = kind::bot;
		hsb_halfspace hs;            ///< Payload for `kind::halfspace`.
		std::shared_ptr<node> lhs;   ///< Left child for `kind::and_` / `kind::or_`.
		std::shared_ptr<node> rhs;   ///< Right child for `kind::and_` / `kind::or_`.
		std::shared_ptr<node> inner; ///< Child for `kind::not_`.

		/// @brief Structural (recursive) equality.
		bool operator==(const node& o) const;
		/// @brief Returns true iff this node and `o` are semantic complements.
		bool is_complement_of(const node& o) const;
		/// @brief Returns a string representation of this node.
		std::string to_string() const;
	};
	using node_ptr = std::shared_ptr<node>;

	node_ptr root;

	hsb();
	explicit hsb(node_ptr r);

	// ── Node factories ───────────────────────────────────────────────────────

	/// @cond INTERNAL
	static node_ptr mk_bot();
	static node_ptr mk_top();
	static node_ptr mk_hs(hsb_halfspace h);
	static node_ptr mk_and(node_ptr l, node_ptr r);
	static node_ptr mk_or(node_ptr l, node_ptr r);
	static node_ptr mk_not(node_ptr i);
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
	static hsb from_halfspace(hsb_halfspace h);

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

	/// @brief Structural equality (two hsb values are equal iff their trees match).
	bool operator==(const hsb& o) const;
	bool operator!=(const hsb& o) const;

	/// @brief Compares with a bool: `true` iff equal to top/bot respectively.
	bool operator==(bool b) const;
	bool operator!=(bool b) const;

	bool operator<(const hsb& o) const;
	std::strong_ordering operator<=>(const hsb& o) const;

	// ── Serialisation ────────────────────────────────────────────────────────

	/// @brief Returns a human-readable string, e.g. `"(x[0] < 0 & ~(x[1] < 0))"`.
	std::string to_string() const;

	/**
	 * @brief Parses `"top"`, `"bot"`, or `"bottom"`; returns `bottom()` on failure.
	 * @param s  Input string (leading/trailing whitespace is stripped).
	 */
	static hsb from_string(const std::string& s);
};

/// @brief Stream output operator for hsb.
inline std::ostream& operator<<(std::ostream& os, const hsb& h);

} // namespace idni::tau_lang

#include "boolean_algebras/hsb.tmpl.h"
#include "boolean_algebras/hsb_splitter.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_H__
