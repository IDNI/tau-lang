// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU_BA_H__

#include "tau_tree.h"
#include "splitter_types.h"
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

template <typename... BAs>
requires BAsPack<BAs...>
struct tau_ba {
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	using tau = tau_lang::tree<node>;

	/**
	 * @brief Constructor initializing tau_ba with recursive relations and main tau_nso_t.
	 *
	 * @param rec_relations Reference to rules of tau_nso_t.
	 * @param main Reference to main tau_nso_t.
	 */
	tau_ba(rewriter::rules& rec_relations, htree::sp main);

	/**
	 * @brief Constructor initializing tau_ba with recursive relations and main tau_nso_t.
	 *
	 * @param rec_relations Reference to rules of tau_nso_t.
	 * @param main Reference to main tau_nso_t.
	 */
	tau_ba(rewriter::rules& rec_relations, tref main);

	/**
	 * @brief Constructor initializing tau_ba with main tau_nso_t.
	 *
	 * @param main Reference to main tau_nso_t.
	 */
	tau_ba(htree::sp main);

	/**
	 * @brief Constructor initializing tau_ba with main tau_nso_t.
	 *
	 * @param main Reference to main tau_nso_t.
	 */
	tau_ba(tref main);

	/**
	 * @brief Three-way comparison operator.
	 *
	 * @param other Reference to another tau_ba<BAs...>.
	 * @return Result of the comparison.
	 */
	auto operator<=>(const tau_ba<BAs...>&) const;

	/**
	 * @brief Bitwise NOT operator.
	 *
	 * @return Result of the bitwise NOT operation.
	 */
	tau_ba<BAs...> operator~() const;

	/**
	 * @brief Bitwise AND operator.
	 *
	 * @param other Reference to another tau_ba<BAs...>.
	 * @return Result of the bitwise AND operation.
	 */
	tau_ba<BAs...> operator&(const tau_ba<BAs...>& other) const;

	/**
	 * @brief Bitwise OR operator.
	 *
	 * @param other Reference to another tau_ba<BAs...>.
	 * @return Result of the bitwise OR operation.
	 */
	tau_ba<BAs...> operator|(const tau_ba<BAs...>& other) const;

	/**
	 * @brief Addition operator.
	 *
	 * @param other Reference to another tau_ba<BAs...>.
	 * @return Result of the addition operation.
	 */
	tau_ba<BAs...> operator+(const tau_ba<BAs...>& other) const;

	/**
	 * @brief Bitwise XOR operator.
	 *
	 * @param other Reference to another tau_ba<BAs...>.
	 * @return Result of the bitwise XOR operation.
	 */
	tau_ba<BAs...> operator^(const tau_ba<BAs...>& other) const;

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
	const rr<node> nso_rr;

private:
	// /**
	//  * @brief Renames the given tau_nso_t form.
	//  *
	//  * @param form Reference to the tau_nso_t form.
	//  * @return Renamed tau_nso_t.
	//  */
	// tref rename(tref form) const;

	// /**
	//  * @brief Renames the given rewriter rule.
	//  *
	//  * @param rule Reference to the rewriter rule of tau_nso_t.
	//  * @return Renamed rewriter rule.
	//  */
	// rewriter::rule rename(const rewriter::rule<tau_nso_t>& rule) const;

};

/**
 * @brief Equality operator for tau_ba and bool.
 *
 * @tparam BAs Variadic template parameters.
 * @param other Reference to tau_ba.
 * @param b Reference to bool.
 * @return True if equal, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tau_ba<BAs...>& other, const bool& b);

/**
 * @brief Equality operator for bool and tau_ba.
 *
 * @tparam BAs Variadic template parameters.
 * @param b Reference to bool.
 * @param other Reference to tau_ba.
 * @return True if equal, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const bool& b, const tau_ba<BAs...>& other);

/**
 * @brief Inequality operator for tau_ba and bool.
 *
 * @tparam BAs Variadic template parameters.
 * @param other Reference to tau_ba.
 * @param b Reference to bool.
 * @return True if not equal, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const tau_ba<BAs...>& other, const bool& b);

/**
 * @brief Inequality operator for bool and tau_ba.
 *
 * @tparam BAs Variadic template parameters.
 * @param b Reference to bool.
 * @param other Reference to tau_ba.
 * @return True if not equal, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const bool& b, const tau_ba<BAs...>& other);

/**
 * @brief Normalizes the given tau_ba.
 *
 * @tparam BAs Variadic template parameters.
 * @param fm Reference to tau_ba.
 * @return Normalized tau_ba.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> normalize(const tau_ba<BAs...>& fm);

/**
 * @brief Splits the given tau_ba based on splitter type.
 *
 * @tparam BAs Variadic template parameters.
 * @param fm Reference to tau_ba.
 * @param st Splitter type.
 * @return Split tau_ba.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> splitter(const tau_ba<BAs...>& fm, splitter_type st);

/**
 * @brief Splits tau_ba into one.
 *
 * @tparam BAs Variadic template parameters.
 * @return Split tau_ba.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_splitter_one();

/**
 * @brief Checks if the tau_ba is closed.
 *
 * @tparam BAs Variadic template parameters.
 * @param fm Reference to tau_ba.
 * @return True if closed, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool is_closed(const tau_ba<BAs...>& fm);

/**
 * @brief Template struct representing a tau_ba_factory.
 *
 * @tparam BAs Variadic template parameters.
 */
template <typename... BAs>
requires BAsPack<BAs...>
struct tau_ba_factory {
	/**
	 * @brief Parses the given source string into tau_nso_t.
	 *
	 * @param src Reference to the source string.
	 * @return Parsed tau_nso_t or std::nullopt if parsing fails.
	 */
	std::optional<constant_with_type<tau_ba<BAs...>, BAs...>> parse(
		const std::string& constant_source);

	/**
	 * @brief Splits one.
	 *
	 * @return Variant containing the splitter of one in the given BA.
	 */
	constant_with_type<tau_ba<BAs...>, BAs...> splitter_one() const;

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

	/**
	 * @brief Returns the instance of the tau_ba_factory.
	 *
	 * @return Instance of the tau_ba_factory.
	 */
	static tau_ba_factory<BAs...>& instance();

};

// << for printing tau_ba's nso rr
template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const tau_ba<BAs...>& rs);

} // namespace idni::tau_lang

// Hash for tau_ba using specialization to std::hash
template <typename... BAs>
requires idni::tau_lang::BAsPack<BAs...>
struct std::hash<idni::tau_lang::tau_ba<BAs...>> {
	size_t operator()(const idni::tau_lang::tau_ba<BAs...>& f) const
								noexcept;
};

#include "boolean_algebras/tau_ba.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU_BA_H__