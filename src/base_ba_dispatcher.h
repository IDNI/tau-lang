// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __BASE_BA_DISPATCHER_H__
#define __BASE_BA_DISPATCHER_H__

#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/tau_ba.h"
#include "boolean_algebras/bv_ba.h"
#include "nso_rr.h"



/**
 * @namespace idni::tau_lang
 * Tau language namespace containing Boolean algebra dispatchers and related utilities.
 */
namespace idni::tau_lang {

/**
 * @brief Dispatcher for base Boolean algebras (BAs...).
 *
 * This specialization provides static methods to operate on elements of the
 * base Boolean algebra types BAs..., delegating to the appropriate
 * implementation depending on the variant type.
 */
template <typename... BAs> requires BAsPack<BAs...>
struct base_ba_dispatcher {
	using node_t = node<BAs...>;
	using tau = tree<node_t>;

	/**
	 * @brief Checks if the element is the syntactic one (true) value.
	 * @param elem The Boolean algebra element.
	 * @return True if the element is syntactic one, false otherwise.
	 */
	static bool is_syntactic_one(const std::variant<BAs...>& elem);

	/**
	 * @brief Checks if the element is syntactic zero (false) value.
	 * @param elem The Boolean algebra element.
	 * @return True if the element is syntactic zero, false otherwise.
	 */
	static bool is_syntactic_zero(const std::variant<BAs...>& elem);

	/**
	 * @brief Checks if the element is the semantically one (true) value.
	 * @param elem The Boolean algebra element.
	 * @return True if the element is semantic one, false otherwise.
	 */
	static bool is_one(const std::variant<BAs...>& elem);

	/**
	 * @brief Checks if the element is semantically zero (false) value.
	 * @param elem The Boolean algebra element.
	 * @return True if the element is semantically zero, false otherwise.
	 */
	static bool is_zero(const std::variant<BAs...>& elem);

	/**
	 * @brief Checks if the Boolean algebra element is closed.
	 * @param elem The Boolean algebra element.
	 * @return True if closed, false otherwise.
	 */
	static bool is_closed(const std::variant<BAs...>& elem);

	/**
	 * @brief Returns the supported Boolean algebra type names.
	 * @return Vector of type names as strings.
	 */
	static std::vector<std::string> types();

	/**
	 * @brief Returns the default type reference for the Boolean algebra.
	 * @return The default tref.
	 */
	static tref default_type();

	/**
	 * @brief Returns the string representation of the one (true) value for a given type.
	 * @param type_tree The type reference.
	 * @return String representation of one.
	 */
	static std::string one(const tref type_tree);

	/**
	 * @brief Returns the string representation of the zero (false) value for a given type.
	 * @param type_tree The type reference.
	 * @return String representation of zero.
	 */
	static std::string zero(const tref type_tree);

	/**
	 * @brief Splits the element using the specified splitter type.
	 * @param elem The Boolean algebra element.
	 * @param st The splitter type.
	 * @return The split element as a variant.
	 */
	static std::variant<BAs...> splitter(const std::variant<BAs...>& elem, splitter_type st = splitter_type::upper);

	/**
	 * @brief Splits the element using the default splitter type.
	 * @param type_tree type of the splitter one element to return.
	 * @return The split one element as a variant.
	 */
	static tref splitter_one(tref type_tree);

	/**
	 * @brief Unpacks a tau_ba from the element if present.
	 * @param elem The Boolean algebra element.
	 * @return The tref for the unpacked tau_ba, or nullptr if not present.
	 */
	static tref unpack_tau_ba(const std::variant<BAs...>& elem);

	/**
	 * @brief Packs a tref into a Boolean algebra variant.
	 * @param t The tref to pack.
	 * @return The packed variant.
	 */
	static std::variant<BAs...> pack_tau_ba(tref t);

	/**
	 * @brief Normalizes the Boolean algebra element.
	 * @param v The element to normalize.
	 * @return The normalized variant.
	 */
	static std::variant<BAs...> normalize(const std::variant<BAs...>& v);

	/**
	 * @brief Simplifies a symbol tref for the Boolean algebra.
	 * @param symbol The symbol tref.
	 * @return The simplified tref.
	 */
	static tref simplify_symbol(tref symbol);

	/**
	 * @brief Simplifies a term tref for the Boolean algebra.
	 * @param term The term tref.
	 * @return The simplified tref.
	 */
	static tref simplify_term(tref term);
};


} // namespace idni::tau_lang

#include "base_ba_dispatcher.tmpl.h"

#endif // __BASE_BA_DISPATCHER_H__