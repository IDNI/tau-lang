/**
 * @file bool_ba.h
 * @brief Classical two-element Boolean algebra (`Bool`).
 *
 * Implements the standard `{0, 1}` BA with bitwise operators, used as the
 * base coefficient type for BDD-backed algebras.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_BA_H__

#include "defs.h"

namespace idni::tau_lang {

/**
 * @brief Classical two-element Boolean algebra element.
 *
 * Wraps a single `bool` and provides the full BA interface (AND, OR, XOR,
 * NOT, comparisons) so that `Bool` can be used as a base type for
 * BDD-backed algebras such as `sbf_ba`.
 */
struct Bool {
	/** @brief Default-construct as `false` (zero). */
	Bool();
	/** @brief Construct from raw `bool` @p b (`true` is one). */
	Bool(bool b);

	/** @brief Return the zero element (static singleton). */
	static const Bool& zero();
	/** @brief Return the one element (static singleton). */
	static const Bool& one();

	/** @brief Bitwise AND with @p x. */
	Bool operator&(const Bool& x) const;
	/** @brief Bitwise OR with @p x. */
	Bool operator|(const Bool& x) const;
	/** @brief Bitwise XOR with @p x. */
	Bool operator^(const Bool& x) const;
	/** @brief Addition of @p x (synonym for XOR in GF(2)). */
	Bool operator+(const Bool& x) const;
	/** @brief Bitwise complement. */
	Bool operator~() const;
	/** @brief Three-way comparison (defaulted). */
	auto operator<=>(const Bool& x) const = default;
	/**
	 * @brief Equality against another `Bool`.
	 *
	 * Declared explicitly because declaring `operator==(bool)` below
	 * suppresses the one `<=>` would otherwise generate.
	 */
	bool operator==(const Bool& x) const = default;
	/** @brief Return `true` if this element's value equals @p x. */
	bool operator==(bool x) const { return b == x; }

	/** @brief Return `true` if this element is zero. */
	bool is_zero() const;
	/** @brief Return `true` if this element is one. */
	bool is_one() const;

	bool b; ///< Underlying boolean value.
};

/**
 * @brief Normalise a `Bool` (identity — already normalised).
 * @param b Element to normalise.
 * @return @p b unchanged.
 */
Bool normalize_bool(const Bool& b);

/**
 * @brief Stream-print a `Bool` as `0` or `1`.
 * @param os Output stream.
 * @param b Element to print.
 * @return @p os.
 */
std::ostream& operator<<(std::ostream& os, const Bool& b);

} // namespace idni::tau_lang

/** @brief Hash of a `Bool`: 1 for one, 0 for zero. */
template<>
struct std::hash<idni::tau_lang::Bool> {
	size_t operator()(const idni::tau_lang::Bool& b) const noexcept {
		return b.b ? 1 : 0;
	}
};

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_BA_H__
