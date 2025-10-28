// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <optional>
#include <variant>
#include <string>
#include <vector>

#include "tau_tree.h"

#ifndef __IDNI__TAU__NSO_FACTORY_H__
#define __IDNI__TAU__NSO_FACTORY_H__

namespace idni::tau_lang {

template <typename... BAs>
requires BAsPack<BAs...>
using constant_with_type = std::pair<std::variant<BAs...>, std::string>;

/**
 * Factory for creating and manipulating tau objects.
 * - interface" for creating custom BA types by adding nso_factory<YourBA>
 *   template specialization.
 */
template <typename... BAs>
requires BAsPack<BAs...>
struct nso_factory {

	/**
	 * Parses the given string into a tau object.
	 * @param constant_source The string to parse.
	 * @param options Optional parse options.
	 * @return A pair with the parsed constant and the string type name
	 */
	std::optional<constant_with_type<BAs...>> parse(
		const std::string& constant_source,
		const std::string type_name,
		const std::string options = "") const;

	/**
	 * Returns a vector of available types.
	 * @return A vector of type names.
	 */
	std::vector<std::string> types() const;

	/**
	 * Returns the default type.
	 * @return The default type name.
	 */
	std::string default_type() const;

	/**
	 * Returns a string representation of one for the given type.
	 * @param type_name The type name.
	 * @return The string representation of one.
	 */
	std::string one(const std::string type_name) const;

	/**
	 * Returns a string representation of zero for the given type.
	 * @param type_name The type name.
	 * @return The string representation of zero.
	 */
	std::string zero(const std::string type_name) const;

	/**
	 * Returns a tau object representing a splitter with one.
	 * @param ba_type Optional type name.
	 * @return The splitter constant.
	 */
	tref splitter_one(const std::string type_name = "") const;

	/**
	 * Returns the tau formula stored in the variant, if present
	 * @param constant Variant for formula extraction
	 * @return Extracted tau_ba spec if present
	 */
	tref unpack_tau_ba(const std::variant<BAs...>& constant);

	/**
	 * Returns a std::variant holding the tau_ba of the given formula
	 * @param c Formula to wrap inside tau_ba
	 * @return std::variant holding a tau_ba
	 */
	std::variant<BAs...> pack_tau_ba (tref c) const;

	/**
	 * Returns the singleton instance of the factory.
	 * @return The singleton instance.
	 */
	static nso_factory<BAs...>& instance();
};


} // namespace idni::tau_lang

#endif // __IDNI__TAU__NSO_FACTORY_H__
