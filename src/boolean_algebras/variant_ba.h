/**
 * @file variant_ba.h
 * @brief Boolean algebra operators for `std::variant<BAs...>`.
 *
 * Lifts the BA operators (AND, OR, XOR, NOT, bool comparisons) to
 * `std::variant<BAs...>` by dispatching to the active alternative.
 * If the two operands hold different alternatives, every build prints a
 * message and aborts: no value of either algebra is a correct result.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__VARIANT_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__VARIANT_BA_H__

#include <cstdlib>
#include <iostream>
#include <variant>

#include "tau_tree.h"
#include "splitter_types.h"

namespace idni::tau_lang {

/// @brief Stop on a binary operator applied to two different algebras.
/// Typing gives both operands of every in-tree call one type, so reaching
/// this is a broken invariant; a default value would be a silent wrong
/// constant.
/// @param op Spelling of the operator, printed before aborting.
[[noreturn]] inline void variant_ba_mismatch(const char* op) {
	std::cerr << "variant operator" << op
		<< ": mismatched alternatives\n";
	std::abort();
}

/// @brief AND of the active alternatives of @p l and @p r.
/// @pre @p l and @p r hold the same alternative; otherwise the process aborts.
template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator&(const std::variant<BAs...>& l,
	const std::variant<BAs...>& r)
{
	return std::visit(overloaded(
		[]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l & r;},
		[](const auto&, const auto&) -> std::variant<BAs...> {
			variant_ba_mismatch("&");}
	), l, r);
}

/// @brief OR of the active alternatives of @p l and @p r.
/// @pre @p l and @p r hold the same alternative; otherwise the process aborts.
template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator|(const std::variant<BAs...>& l,
	const std::variant<BAs...>& r)
{
	return std::visit(overloaded(
		[]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l | r;},
		[](const auto&, const auto&) -> std::variant<BAs...> {
			variant_ba_mismatch("|");}
	), l, r);
}

/// @brief XOR of the active alternatives of @p l and @p r.
/// @pre @p l and @p r hold the same alternative; otherwise the process aborts.
template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator^(const std::variant<BAs...>& l,
	const std::variant<BAs...>& r)
{
	return std::visit(overloaded(
		[]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l ^ r;},
		[](const auto&, const auto&) -> std::variant<BAs...> {
			variant_ba_mismatch("^");}
	), l, r);
}

/// @brief Addition (synonym for XOR) of @p l and @p r.
/// @pre @p l and @p r hold the same alternative; otherwise the process aborts.
template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator+(const std::variant<BAs...>& l,
	const std::variant<BAs...>& r)
{
	return l ^ r;
}

/// @brief Complement of the active alternative of @p l.
template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator~(const std::variant<BAs...>& l) {
	return std::visit(overloaded(
		[](const auto& l) -> std::variant<BAs...> {
			return ~l;
	}), l);
}

/// @brief Compare the active alternative of @p l with the truth value @p r.
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const std::variant<BAs...>& l, const bool& r) {
	return std::visit(overloaded(
			[&r](const auto& l) -> bool {
				return l == r;
			}
		), l);
}

/// @brief Compare the truth value @p l with the active alternative of @p r.
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const bool& l, const std::variant<BAs...>& r) {
	return r == l;
}

/// @brief Negation of `l == r` for a variant @p l and a truth value @p r.
template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const std::variant<BAs...>& l, const bool& r) {
	return !(l == r);
}

/// @brief Negation of `l == r` for a truth value @p l and a variant @p r.
template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const bool& l, const std::variant<BAs...>& r) {
	return r != l;
}

} // namespace idni::tau_lang

//TODO (MEDIUM) add << for variant_ba

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__VARIANT_BA_H__