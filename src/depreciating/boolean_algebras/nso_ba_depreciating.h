// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __NSO_BA_DEPRECATING_H__
#define __NSO_BA_DEPRECATING_H__

#include "depreciating/hooks_depreciating.h"

namespace idni::tau_lang_depreciating {

/**
 * @brief Bitwise AND operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return Result of bitwise AND operation.
 */
template <typename... BAs>
tau_<BAs...> operator&(const tau_<BAs...>& l, const tau_<BAs...>& r);

/**
 * @brief Bitwise OR operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return Result of bitwise OR operation.
 */
template <typename... BAs>
tau_<BAs...> operator|(const tau_<BAs...>& l, const tau_<BAs...>& r);

/**
 * @brief Bitwise NOT operator for tau.
 * @param l Operand.
 * @return Result of bitwise NOT operation.
 */
template <typename... BAs>
tau_<BAs...> operator~(const tau_<BAs...>& l);

/**
 * @brief Bitwise XOR operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return Result of bitwise XOR operation.
 */
template <typename... BAs>
tau_<BAs...> operator^(const tau_<BAs...>& l, const tau_<BAs...>& r);

/**
 * @brief Addition operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return Result of addition operation.
 */
template <typename... BAs>
tau_<BAs...> operator+(const tau_<BAs...>& l, const tau_<BAs...>& r);

/**
 * @brief Checks if the tau is zero.
 * @param l Operand.
 * @return True if the tau is zero, false otherwise.
 */
template <typename... BAs>
bool is_zero(const tau_<BAs...>& l);

/**
 * @brief Checks if the tau is one.
 * @param l Operand.
 * @return True if the tau is one, false otherwise.
 */
template <typename... BAs>
bool is_one(const tau_<BAs...>& l);

/**
 * @brief Equality operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if both tau are equal, false otherwise.
 */
template <typename... BAs>
bool operator==(const tau_<BAs...>& l, const tau_<BAs...>& r);

/**
 * @brief Inequality operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if both tau are not equal, false otherwise.
 */
template <typename... BAs>
bool operator!=(const tau_<BAs...>& l, const tau_<BAs...>& r);

/**
 * @brief Three-way comparison operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return Result of the three-way comparison.
 */
template <typename... BAs>
std::weak_ordering operator<=>(const tau_<BAs...>& l, const tau_<BAs...>& r);

/**
 * @brief Less-than operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if l is less than r, false otherwise.
 */
template<typename... BAs>
bool operator<(const tau_<BAs...>& l, const tau_<BAs...>& r);

/**
 * @brief Less-than or equal-to operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if l is less than or equal to r, false otherwise.
 */
template<typename... BAs>
bool operator<=(const tau_<BAs...>& l, const tau_<BAs...>& r);

/**
 * @brief Greater-than operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if l is greater than r, false otherwise.
 */
template<typename... BAs>
bool operator>(const tau_<BAs...>& l, const tau_<BAs...>& r);

/**
 * @brief Greater-than or equal-to operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if l is greater than or equal to r, false otherwise.
 */
template<typename... BAs>
bool operator>=(const tau_<BAs...>& l, const tau_<BAs...>& r);

/**
 * @brief Equality operator for tau and bool.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if tau is equal to the boolean value, false otherwise.
 */
template <typename... BAs>
bool operator==(const tau_<BAs...>& l, const bool& r);

/**
 * @brief Equality operator for bool and tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if boolean value is equal to tau, false otherwise.
 */
template <typename... BAs>
bool operator==(const bool l, const tau_<BAs...>& r);

/**
 * @brief Splitter function for a nso tau_parser::bf_constant node holding a BA constant.
 * @param n Operand.
 * @param st Splitter type (default is splitter_type::upper).
 * @return Result of the splitter operation.
 */
template <typename... BAs>
tau_<BAs...> splitter(const tau_<BAs...>& n, splitter_type st = splitter_type::upper);

} // namespace idni::tau_lang_depreciating

#include "nso_ba_depreciating.tmpl.h"

#endif // __NSO_BA_DEPRECATING_H__