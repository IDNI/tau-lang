// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __NSO_BA_H__
#define __NSO_BA_H__

#include "hooks.h"

namespace idni::tau_lang {

/**
 * @brief Bitwise AND operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return Result of bitwise AND operation.
 */
template <typename... BAs>
tau<BAs...> operator&(const tau<BAs...>& l, const tau<BAs...>& r);

/**
 * @brief Bitwise OR operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return Result of bitwise OR operation.
 */
template <typename... BAs>
tau<BAs...> operator|(const tau<BAs...>& l, const tau<BAs...>& r);

/**
 * @brief Bitwise NOT operator for tau.
 * @param l Operand.
 * @return Result of bitwise NOT operation.
 */
template <typename... BAs>
tau<BAs...> operator~(const tau<BAs...>& l);

/**
 * @brief Bitwise XOR operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return Result of bitwise XOR operation.
 */
template <typename... BAs>
tau<BAs...> operator^(const tau<BAs...>& l, const tau<BAs...>& r);

/**
 * @brief Addition operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return Result of addition operation.
 */
template <typename... BAs>
tau<BAs...> operator+(const tau<BAs...>& l, const tau<BAs...>& r);

/**
 * @brief Checks if the tau is zero.
 * @param l Operand.
 * @return True if the tau is zero, false otherwise.
 */
template <typename... BAs>
bool is_zero(const tau<BAs...>& l);

/**
 * @brief Checks if the tau is one.
 * @param l Operand.
 * @return True if the tau is one, false otherwise.
 */
template <typename... BAs>
bool is_one(const tau<BAs...>& l);

/**
 * @brief Equality operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if both tau are equal, false otherwise.
 */
template <typename... BAs>
bool operator==(const tau<BAs...>& l, const tau<BAs...>& r);

/**
 * @brief Inequality operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if both tau are not equal, false otherwise.
 */
template <typename... BAs>
bool operator!=(const tau<BAs...>& l, const tau<BAs...>& r);

/**
 * @brief Structural equality for tau nodes
 * @param l Left-hand side of comparison.
 * @param r Right-hand side of comparison.
 * @return True, if the tree structure for l and r is the same, else false.
 */
template <typename... BAs>
struct struc_equal {
	bool operator() (const tau<BAs...>& l, const tau<BAs...>& r) {
		if (r == nullptr && l == nullptr) return true;
		if (r == nullptr || l == nullptr) return false;

		if (std::addressof(*l) == std::addressof(*r)) return true;
		if (l->hash != r->hash) return false;

		if (l->value != r->value) return false;
		if (l->child.size() != r->child.size()) return false;

		//compare children
		for (size_t i = 0; i < l->child.size(); ++i)
			if (!operator()(l->child[i], r->child[i])) return false;
		return true;
	}
};

template <typename value_t, typename... BAs>
using unordered_tau_map = std::unordered_map<tau<BAs...>, value_t,
			std::hash<tau<BAs...>>, struc_equal<BAs...>>;

/**
 * @brief Three-way comparison operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return Result of the three-way comparison.
 */
template <typename... BAs>
std::weak_ordering operator<=>(const tau<BAs...>& l, const tau<BAs...>& r);

/**
 * @brief Less-than operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if l is less than r, false otherwise.
 */
template<typename... BAs>
bool operator<(const tau<BAs...>& l, const tau<BAs...>& r);

/**
 * @brief Less-than or equal-to operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if l is less than or equal to r, false otherwise.
 */
template<typename... BAs>
bool operator<=(const tau<BAs...>& l, const tau<BAs...>& r);

/**
 * @brief Greater-than operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if l is greater than r, false otherwise.
 */
template<typename... BAs>
bool operator>(const tau<BAs...>& l, const tau<BAs...>& r);

/**
 * @brief Greater-than or equal-to operator for tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if l is greater than or equal to r, false otherwise.
 */
template<typename... BAs>
bool operator>=(const tau<BAs...>& l, const tau<BAs...>& r);

/**
 * @brief Equality operator for tau and bool.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if tau is equal to the boolean value, false otherwise.
 */
template <typename... BAs>
bool operator==(const tau<BAs...>& l, const bool& r);

/**
 * @brief Equality operator for bool and tau.
 * @param l Left-hand side operand.
 * @param r Right-hand side operand.
 * @return True if boolean value is equal to tau, false otherwise.
 */
template <typename... BAs>
bool operator==(const bool l, const tau<BAs...>& r);

/**
 * @brief Splitter function for a nso tau_parser::bf_constant node holding a BA constant.
 * @param n Operand.
 * @param st Splitter type (default is splitter_type::upper).
 * @return Result of the splitter operation.
 */
template <typename... BAs>
tau<BAs...> splitter(const tau<BAs...>& n, splitter_type st = splitter_type::upper);

} // namespace idni::tau_lang

namespace idni::rewriter {
template <typename... BAs>
struct make_node_cache_equality<tau_lang::tau_sym<BAs...>> {
	bool operator() (const node<tau_lang::tau_sym<BAs...>>& l,
		const node<tau_lang::tau_sym<BAs...>>& r) const {
		static tau_lang::struc_equal<BAs...> st_eq;
		if (std::addressof(l) == std::addressof(r)) return true;
		if (l.hash != r.hash) return false;

		if (l.value != r.value) return false;
		if (l.child.size() != r.child.size()) return false;

		//compare children
		for (size_t i = 0; i < l.child.size(); ++i)
			if (!st_eq(l.child[i], r.child[i])) return false;
		return true;
	}
};

template <typename... BAs>
struct traverser_cache_equality<tau_lang::tau<BAs...>> {
	bool operator() (const tau_lang::tau<BAs...>& l,
		const tau_lang::tau<BAs...>& r) const {
		static tau_lang::struc_equal<BAs...> st_eq;
		return st_eq(l, r);
	}
};

template <typename... BAs>
struct traverser_pair_cache_equality<tau_lang::tau<BAs...>> {
	using p = std::pair<tau_lang::tau<BAs...>, size_t>;
	bool operator() (const p& l, const p& r) const {
		static tau_lang::struc_equal<BAs...> st_eq;
		return st_eq(l.first, r.first) && l.second == r.second;
	}
};

}

#include "nso_ba.tmpl.h"

#endif // __NSO_BA_H__