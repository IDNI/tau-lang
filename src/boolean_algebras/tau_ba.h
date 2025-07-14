// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __TAU_BA_H__
#define __TAU_BA_H__

#include <iostream>

#include "satisfiability.h"
#include "splitter.h"

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of tau,...

namespace idni::tau_lang {

// Check https://gcc.gnu.org/bugzilla/show_bug.cgi?id=102609 to follow up on
// the implementation of "Deducing this" on gcc.
// See also (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0847r7.html)
// and https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/ for how to use
// "Deducing this" on CRTP.

template <typename...BAs>
struct tau_ba {
	using tau_ba_t = tau_ba<BAs...>;
	using tau_nso_t = tau<tau_ba_t, BAs...>;

	/**
	 * @brief Constructor initializing tau_ba with recursive relations and main tau_nso_t.
	 *
	 * @param rec_relations Reference to rules of tau_nso_t.
	 * @param main Reference to main tau_nso_t.
	 */
	tau_ba(const rules<tau_nso_t>& rec_relations, const tau_nso_t& main);

	/**
	 * @brief Constructor initializing tau_ba with main tau_nso_t.
	 *
	 * @param main Reference to main tau_nso_t.
	 */
	tau_ba(const tau_nso_t& main);

	/**
	 * @brief Three-way comparison operator.
	 *
	 * @param other Reference to another tau_ba_t.
	 * @return Result of the comparison.
	 */
	auto operator<=>(const tau_ba_t&) const;

	/**
	 * @brief Bitwise NOT operator.
	 *
	 * @return Result of the bitwise NOT operation.
	 */
	tau_ba_t operator~() const;

	/**
	 * @brief Bitwise AND operator.
	 *
	 * @param other Reference to another tau_ba_t.
	 * @return Result of the bitwise AND operation.
	 */
	tau_ba_t operator&(const tau_ba_t& other) const;

	/**
	 * @brief Bitwise OR operator.
	 *
	 * @param other Reference to another tau_ba_t.
	 * @return Result of the bitwise OR operation.
	 */
	tau_ba_t operator|(const tau_ba_t& other) const;

	/**
	 * @brief Addition operator.
	 *
	 * @param other Reference to another tau_ba_t.
	 * @return Result of the addition operation.
	 */
	tau_ba_t operator+(const tau_ba_t& other) const;

	/**
	 * @brief Bitwise XOR operator.
	 *
	 * @param other Reference to another tau_ba_t.
	 * @return Result of the bitwise XOR operation.
	 */
	tau_ba_t operator^(const tau_ba_t& other) const;

	/**
	 * @brief Checks if the tau_ba is zero.
	 *
	 * @return True if zero, otherwise false.
	 */
	bool is_zero() const;

	/**
	 * @brief Checks if the tau_ba is one.
	 *
	 * @return True if one, otherwise false.
	 */
	bool is_one() const;

	/**
	 * @brief Type equivalent to tau_spec<BAs...>.
	 */
	const rr<tau_nso_t> nso_rr;

private:
	/**
	 * @brief Renames the given tau_nso_t form.
	 *
	 * @param form Reference to the tau_nso_t form.
	 * @return Renamed tau_nso_t.
	 */
	tau_nso_t rename(const tau_nso_t& form) const;

	/**
	 * @brief Renames the given rewriter rule.
	 *
	 * @param rule Reference to the rewriter rule of tau_nso_t.
	 * @return Renamed rewriter rule.
	 */
	rewriter::rule<rr<tau_nso_t>> rename(const rewriter::rule<tau_nso_t>& rule) const;

	/**
	 * @brief Merges two sets of rules.
	 *
	 * @param rs1 Reference to the first set of rules.
	 * @param rs2 Reference to the second set of rules.
	 * @return Merged set of rules.
	 */
	rules<tau_nso_t> merge(const rules<tau_nso_t>& rs1, const rules<tau_nso_t>& rs2) const;
};

/**
 * @brief Equality operator for tau_ba and bool.
 *
 * @tparam BAs Variadic template parameters.
 * @param other Reference to tau_ba.
 * @param b Reference to bool.
 * @return True if equal, otherwise false.
 */
template <typename...BAs>
bool operator==(const tau_ba<BAs...>& other, const bool& b);

/**
 * @brief Equality operator for bool and tau_ba.
 *
 * @tparam BAs Variadic template parameters.
 * @param b Reference to bool.
 * @param other Reference to tau_ba.
 * @return True if equal, otherwise false.
 */
template <typename...BAs>
bool operator==(const bool& b, const tau_ba<BAs...>& other);

/**
 * @brief Inequality operator for tau_ba and bool.
 *
 * @tparam BAs Variadic template parameters.
 * @param other Reference to tau_ba.
 * @param b Reference to bool.
 * @return True if not equal, otherwise false.
 */
template <typename...BAs>
bool operator!=(const tau_ba<BAs...>& other, const bool& b);

/**
 * @brief Inequality operator for bool and tau_ba.
 *
 * @tparam BAs Variadic template parameters.
 * @param b Reference to bool.
 * @param other Reference to tau_ba.
 * @return True if not equal, otherwise false.
 */
template <typename...BAs>
bool operator!=(const bool& b, const tau_ba<BAs...>& other);

/**
 * @brief Normalizes the given tau_ba.
 *
 * @tparam BAs Variadic template parameters.
 * @param fm Reference to tau_ba.
 * @return Normalized tau_ba.
 */
template<typename... BAs>
auto normalize(const tau_ba<BAs...>& fm);

/**
 * @brief Splits the given tau_ba based on splitter type.
 *
 * @tparam BAs Variadic template parameters.
 * @param fm Reference to tau_ba.
 * @param st Splitter type.
 * @return Split tau_ba.
 */
template<typename... BAs>
auto splitter(const tau_ba<BAs...>& fm, splitter_type st);

/**
 * @brief Splits tau_ba into one.
 *
 * @tparam BAs Variadic template parameters.
 * @return Split tau_ba.
 */
template<typename... BAs>
auto tau_splitter_one();

/**
 * @brief Checks if the tau_ba is closed.
 *
 * @tparam BAs Variadic template parameters.
 * @param fm Reference to tau_ba.
 * @return True if closed, otherwise false.
 */
template<typename... BAs>
bool is_closed(const tau_ba<BAs...>& fm);

/**
 * @brief Alias for tau_nso.
 *
 * @tparam BAs Variadic template parameters.
 */
template <typename... BAs>
using tau_nso = tau<tau_ba<BAs...>, BAs...>;

/**
 * @brief Alias for tau_spec.
 *
 * @tparam BAs Variadic template parameters.
 */
template <typename... BAs>
using tau_spec = rr<tau_nso<BAs...>>;

/**
 * @brief Template struct representing a tau_ba_factory.
 *
 * @tparam BAs Variadic template parameters.
 */
template <typename...BAs>
struct tau_ba_factory {
	using tau_ba_t = tau_ba<BAs...>;
	using tau_nso_t = tau_nso<BAs...>;

	/**
	 * @brief Parses the given source string into tau_nso_t.
	 *
	 * @param src Reference to the source string.
	 * @return Parsed tau_nso_t or std::nullopt if parsing fails.
	 */
	std::optional<tau_nso_t> parse(const std::string& src);

	/**
	 * @brief Binds the given tau_nso_t.
	 *
	 * @param n Reference to tau_nso_t.
	 * @return Bound tau_nso_t.
	 */
	tau_nso_t binding(const tau_nso_t& n);

	/**
	 * @brief Splits one.
	 *
	 * @return Variant containing the splitter of one in the given BA.
	 */
	std::variant<tau_ba_t, BAs...> splitter_one() const;

	/**
	 * @brief Converts one to string.
	 *
	 * @param str Reference to the string.
	 * @return Converted string.
	 */
	std::string one(std::string&) const;

	/**
	 * @brief Converts zero to string.
	 *
	 * @param str Reference to the string.
	 * @return Converted string.
	 */
	std::string zero(std::string&) const;
};

} // namespace idni::tau_lang

// Hash for tau_ba using specialization to std::hash
template <typename... BAs>
struct std::hash<idni::tau_lang::tau_ba<BAs...>> {
	size_t operator()(const idni::tau_lang::tau_ba<BAs...>& f) const noexcept;
};

// << for printing tau_ba's form
template <typename... BAs>
std::ostream& operator<<(std::ostream& os, const idni::tau_lang::tau_ba<BAs...>& rs);

#include "tau_ba.tmpl.h"

#endif // __TAU_BA_H__