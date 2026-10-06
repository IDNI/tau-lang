/**
 * @file nso_ba.h
 * @brief Boolean algebra operators for `tree<node<BAs...>>` (the NSO / Tau BA).
 *
 * Provides `operator&`, `|`, `~`, `^`, `+`, comparison operators, and
 * `is_zero`/`is_one` predicates so that `tree<node<BAs...>>` satisfies the
 * BA interface expected by the solver infrastructure.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__NSO_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__NSO_BA_H__

#include "tau_tree.h"
#include "splitter_types.h"

namespace idni::tau_lang {

/**
 * @brief Conjunction of two tau trees.
 *
 * Folds 0 and 1, combines two BA constants of one type with the constant's
 * own `&`, builds `bf_and` for two `bf`s and `wff_and` for two `wff`s; a `bf`
 * against an (in)equation is pushed into the normalized equation's left side.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return The conjunction; @p l for any other operand shape.
 */
template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& operator&(const tree<node<BAs...>>& l,
        const tree<node<BAs...>>& r);

/**
 * @brief Disjunction of two tau trees, by the same cases as `operator&`.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return The disjunction; @p l for any other operand shape.
 */
template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& operator|(const tree<node<BAs...>>& l,
        const tree<node<BAs...>>& r);

/**
 * @brief Complement of a tau tree: swaps 0 and 1, complements a BA constant,
 * builds `bf_neg` for a `bf` and `wff_neg` for a `wff`.
 * @param l Operand.
 * @return The complement; @p l for any other operand shape.
 */
template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& operator~(const tree<node<BAs...>>& l);

/**
 * @brief Symmetric difference of two tau trees, by the same cases as
 * `operator&` (0 is the identity).
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return The exclusive or; @p l for any other operand shape.
 */
template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& operator^(const tree<node<BAs...>>& l,
                                                const tree<node<BAs...>>& r);

/**
 * @brief Boolean-ring addition, the same as `operator^`.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return `l ^ r`.
 */
template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& operator+(const tree<node<BAs...>>& l,
                                                const tree<node<BAs...>>& r);

/**
 * @brief Checks if the tau tree is zero: syntactically 0 or F, or a BA
 * constant its algebra decides to be zero.
 * @param l Operand.
 * @return True if zero; false otherwise, including when the algebra cannot
 * decide a constant.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool is_zero(const tree<node<BAs...>>& l);

/**
 * @brief Checks if the tau tree is one: syntactically 1 or T, or a BA
 * constant its algebra decides to be one.
 * @param l Operand.
 * @return True if one; false otherwise, including when the algebra cannot
 * decide a constant.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool is_one(const tree<node<BAs...>>& l);

/**
 * @brief Structural equality of two tau trees (`subtree_equals`), not
 * semantic equivalence; typed and untyped constants differ.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if both trees are structurally equal.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tree<node<BAs...>>& l, const tree<node<BAs...>>& r);

/**
 * @brief Negation of the structural `operator==`.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if the trees are not structurally equal.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const tree<node<BAs...>>& l, const tree<node<BAs...>>& r);

/**
 * @brief Structural three-way comparison (`subtree_less`), deterministic
 * across runs; typed and untyped constants compare different.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return `equivalent` for structurally equal trees, else `less` or `greater`.
 */
template <typename... BAs>
requires BAsPack<BAs...>
std::weak_ordering operator<=>(const tree<node<BAs...>>& l,
                                                const tree<node<BAs...>>& r);

/**
 * @brief Less-than operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if l is less than r, false otherwise.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator<(const tree<node<BAs...>>& l, const tree<node<BAs...>>& r);

/**
 * @brief Less-than or equal-to operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if l is less than or equal to r, false otherwise.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator<=(const tree<node<BAs...>>& l, const tree<node<BAs...>>& r);

/**
 * @brief Greater-than operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if l is greater than r, false otherwise.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator>(const tree<node<BAs...>>& l, const tree<node<BAs...>>& r);

/**
 * @brief Greater-than or equal-to operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if l is greater than or equal to r, false otherwise.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator>=(const tree<node<BAs...>>& l, const tree<node<BAs...>>& r);

/**
 * @brief Compares a tau tree with a Boolean value.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return `is_one(l)` when @p r is true, `is_zero(l)` otherwise.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tree<node<BAs...>>& l, const bool& r);

/**
 * @brief Compares a Boolean value with a tau tree, as `r == l`.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return `is_one(r)` when @p l is true, `is_zero(r)` otherwise.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const bool l, const tree<node<BAs...>>& r);

} // namespace idni::tau_lang

#include "boolean_algebras/nso_ba.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__NSO_BA_H__