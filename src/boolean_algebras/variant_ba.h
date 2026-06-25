/**
 * @file variant_ba.h
 * @brief Boolean algebra operators for `std::variant<BAs...>`.
 *
 * Lifts the BA operators (AND, OR, XOR, NOT, bool comparisons) to
 * `std::variant<BAs...>` by dispatching to the active alternative.
 * Throws `std::logic_error` if the two operands hold different alternatives.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__VARIANT_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__VARIANT_BA_H__

#include <variant>

#include "tau_tree.h"
#include "splitter_types.h"

namespace idni::tau_lang {

/// @brief Component-wise bitwise AND on matching-alternative variants.
template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator&(const std::variant<BAs...>& l,
	const std::variant<BAs...>& r)
{
	return std::visit(overloaded(
		[]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l & r;},
		[](const auto&, const auto&) -> std::variant<BAs...> {
			throw std::logic_error("wrong types");}
	), l, r);
}

/// @brief Component-wise bitwise OR on matching-alternative variants.
template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator|(const std::variant<BAs...>& l,
	const std::variant<BAs...>& r)
{
	return std::visit(overloaded(
		[]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l | r;},
		[](const auto&, const auto&) -> std::variant<BAs...> {
			throw std::logic_error("wrong types");}
	), l, r);
}

/// @brief Component-wise bitwise XOR on matching-alternative variants.
template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator^(const std::variant<BAs...>& l,
	const std::variant<BAs...>& r)
{
	return std::visit(overloaded(
		[]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l ^ r;},
		[](const auto&, const auto&) -> std::variant<BAs...> {
			throw std::logic_error("wrong types");}
	), l, r);
}

/// @brief Addition (synonym for XOR) on matching-alternative variants.
template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator+(const std::variant<BAs...>& l,
	const std::variant<BAs...>& r)
{
	return l ^ r;
}

/// @brief Component-wise bitwise NOT.
template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator~(const std::variant<BAs...>& l) {
	return std::visit(overloaded(
		[](const auto& l) -> std::variant<BAs...> {
			return ~l;
	}), l);
}

/// @brief Equality between a variant BA element and a raw `bool`.
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const std::variant<BAs...>& l, const bool& r) {
	return std::visit(overloaded(
			[&r](const auto& l) -> bool {
				return l == r;
			}
		), l);
}

/// @brief Equality between a raw `bool` and a variant BA element.
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const bool& l, const std::variant<BAs...>& r) {
	return r == l;
}

/// @brief Inequality between a variant BA element and a raw `bool`.
template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const std::variant<BAs...>& l, const bool& r) {
	return !(l == r);
}

/// @brief Inequality between a raw `bool` and a variant BA element.
template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const bool& l, const std::variant<BAs...>& r) {
	return r != l;
}

} // namespace idni::tau_lang

//TODO (MEDIUM) add << for variant_ba

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__VARIANT_BA_H__