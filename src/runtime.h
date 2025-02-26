// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#include "nso_rr.h"

namespace idni::tau_lang {

/**
 * Factory for creating and manipulating tau objects.
 */
template <typename... BAs>
struct nso_factory {

	/**
	 * Parses the given string into a tau object.
	 * @param input The string to parse.
	 * @param options Optional parse options.
	 * @return The parsed tau object, or std::nullopt if parsing fails.
	 */
	std::optional<tau<BAs...>> parse(const std::string& input,
		const std::string& options = "") const;

	/**
	 * Binds the given tau object with the specified type.
	 * @param n The tau object to bind.
	 * @param type The type to bind with.
	 * @return The bound tau object.
	 */
	tau<BAs...> binding(const tau<BAs...>& n, const std::string& type = "") const;

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
	std::string one(const std::string& type_name) const;

	/**
	 * Returns a string representation of zero for the given type.
	 * @param type_name The type name.
	 * @return The string representation of zero.
	 */
	std::string zero(const std::string& type_name) const;

	/**
	 * Returns a tau object representing a splitter with one.
	 * @param type Optional type name.
	 * @return The splitter tau object.
	 */
	tau<BAs...> splitter_one(const std::string& type = "") const;


	/**
	 * Returns the tau formula stored in the variant, if present
	 * @param v Variant for formula extraction
	 * @return Extracted formula if present
	 */
	std::optional<tau<BAs...>> unpack_tau_ba (const std::variant<BAs...>& v);

	/**
	 * Returns the singleton instance of the factory.
	 * @return The singleton instance.
	 */
	static nso_factory<BAs...>& instance();
};

} // namespace idni::tau_lang

#endif // __RUNTIME_H__