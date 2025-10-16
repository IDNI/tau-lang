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
	 * Returns a vector of available base type names.
	 * @return A vector of base type names.
	 */
	static std::vector<std::string> types();

	/**
	 * Returns the default type.
	 * @return The default type name.
	 */
	static tref default_type();

	/**
	 * Returns a string representation of one for the given type.
	 * @param type_tree The type tree.
	 * @return The string representation of one.
	 */
	static std::string one(tref type_tree);

	/**
	 * Returns a string representation of zero for the given type.
	 * @param type_tree The type tree.
	 * @return The string representation of zero.
	 */
	static std::string zero(tref type_tree);

	/**
	 * Returns a tau object representing a splitter with one.
	 * @param type_tree The type tree.
	 * @return The splitter constant.
	 */
	static tref splitter_one(tref type_tree);

	/**
	 * Returns the tau formula stored in the variant, if present
	 * @param constant Variant for formula extraction
	 * @return Extracted tau_ba spec if present
	 */
	static tref unpack_tau_ba(const std::variant<BAs...>& constant);

	/**
	 * Returns a std::variant holding the tau_ba of the given formula
	 * @param c Formula to wrap inside tau_ba
	 * @return std::variant holding a tau_ba
	 */
	static std::variant<BAs...> pack_tau_ba (tref c);
};


} // namespace idni::tau_lang

#endif // __IDNI__TAU__NSO_FACTORY_H__
