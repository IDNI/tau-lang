// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <optional>
#include <variant>
#include <string>
#include <vector>

#include "tau_tree.h"

#ifndef __IDNI__TAU__NSO_FACTORY_H__
#define __IDNI__TAU__NSO_FACTORY_H__

namespace idni::tau_lang {

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
	 * @param input The string to parse.
	 * @param options Optional parse options.
	 * @return The parsed tau object, or std::nullopt if parsing fails.
	 */
	std::optional<std::variant<BAs...>> parse(const std::string& input,
					const std::string options = "") const;

	/**
	 * Binds the given tau object with the specified type.
	 * @param n The tau object source to bind.
	 * @param type_name The type to bind with (string_id(type_name)).
	 * @return The bound tau object.
	 */
	tref binding(const std::string& n, const std::string& type_name) const;

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
	 * @param type Optional type name.
	 * @return The splitter tau object.
	 */
	tref splitter_one(const std::string type = "") const;

	/**
	 * Returns the tau formula stored in the variant, if present
	 * @param v Variant for formula extraction
	 * @return Extracted formula if present
	 */
	tref unpack_tau_ba(const std::variant<BAs...>& v);

	/**
	 * Returns the singleton instance of the factory.
	 * @return The singleton instance.
	 */
	static nso_factory<BAs...>& instance();
};


} // namespace idni::tau_lang

#endif // __IDNI__TAU__NSO_FACTORY_H__
