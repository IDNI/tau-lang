// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file base_ba_dispatcher.h
 * @brief Generic dispatcher struct for base Boolean algebra (BA) type packs.
 *
 * `base_ba_dispatcher<BAs...>` forwards `nso_factory` operations (is_one, splitter,
 * normalize, etc.) to the appropriate BA implementation based on the active
 * `std::variant<BAs...>` alternative. A fallible operation returns a `result`
 * carrying the owner's report.
 */

#ifndef __BASE_BA_DISPATCHER_H__
#define __BASE_BA_DISPATCHER_H__

// bool_ba.h stays: bool_descriptor.tmpl.h, which base_ba_dispatcher.tmpl.h
// includes because its parse() names parse_bool defined there, needs Bool. The
// concrete BAs of a pack arrive with whoever spells the pack -- tau_pack.h
// includes each configured BA's header -- so this file names none of them.
#include <array>
#include <string_view>

#include "boolean_algebras/ba_descriptor.h"
#include "boolean_algebras/ba_pack_traits.h"
#include "boolean_algebras/bool_ba.h"
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
	/// The node type of the pack.
	using node_t = node<BAs...>;
	/// The tree type of the pack.
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
	 * @brief Checks if the element is the semantically one (true) value;
	 * the result reports why on a decision failure.
	 * @param elem The Boolean algebra element.
	 * @return Whether @p elem is one, or the owning BA's error.
	 */
	static result<bool> is_one(const std::variant<BAs...>& elem);

	/**
	 * @brief Checks if the element is semantically zero (false) value;
	 * the result reports why on a decision failure.
	 * @param elem The Boolean algebra element.
	 * @return Whether @p elem is zero, or the owning BA's error.
	 */
	static result<bool> is_zero(const std::variant<BAs...>& elem);

	/**
	 * @brief Checks if the Boolean algebra element is closed; the result
	 * reports why on a decision failure.
	 * @param elem The Boolean algebra element.
	 * @return Whether @p elem is closed, or the owning BA's error.
	 */
	static result<bool> is_closed(const std::variant<BAs...>& elem);

	/**
	 * @brief Returns the supported Boolean algebra type names, in pack order.
	 * @return Vector of type names as strings.
	 */
	static std::vector<std::string> types();

	/**
	 * @brief Returns the supported Boolean algebra type names as compile-time
	 * string views, in pack order.
	 * @return One descriptor `type_name` per BA of the pack.
	 */
	static constexpr std::array<std::string_view, sizeof...(BAs)> type_names();

	/**
	 * @brief Total length of the comma-joined type name list (no separator
	 * after the last name, no null terminator).
	 * @return The length of `types_joined()`.
	 */
	static consteval std::size_t types_joined_length();

	/**
	 * @brief Returns the supported Boolean algebra type names as one
	 * comma-joined string, e.g. "tau, qint, qlt, nlang, bv, sbf, hsb".
	 * @return A view over a static constexpr character array.
	 */
	static constexpr std::string_view types_joined();

	/**
	 * @brief Returns the default type of the pack: the type tree of the BA
	 * with the lowest `default_type_priority`, ties broken by pack order.
	 * @return The default type tree.
	 */
	static tref default_type();

	/**
	 * @brief Returns the string representation of the one (true) value for a given type.
	 *
	 * A type no BA of the pack owns falls back to the pack's Boolean carrier
	 * type, when the pack has one.
	 * @param type_tree The type reference.
	 * @return String representation of one, or an `unsupported_operation`
	 * error when no BA owns the type and no carrier applies.
	 */
	static result<std::string> one(const tref type_tree);

	/**
	 * @brief Returns the string representation of the zero (false) value for a given type.
	 *
	 * Falls back to the Boolean carrier type as `one()` does.
	 * @param type_tree The type reference.
	 * @return String representation of zero, or an `unsupported_operation`
	 * error when no BA owns the type and no carrier applies.
	 */
	static result<std::string> zero(const tref type_tree);

	/**
	 * @brief Splits the element using the specified splitter type.
	 * @param elem The Boolean algebra element.
	 * @param st The splitter type.
	 * @return The split element as a variant (@p elem itself when its BA
	 * declares no splitter), or the report of a failed split.
	 */
	static result<std::variant<BAs...>> splitter(const std::variant<BAs...>& elem, splitter_type st = splitter_type::upper);

	/**
	 * @brief Returns a splitter of the one element of the given type,
	 * wrapped as a bf constant tree.
	 * @param type_tree type of the splitter one element to return.
	 * @return The tree for the split one, or nullptr when no BA owns the
	 * type or its algebra has no splitter one (e.g. bv).
	 */
	static tref splitter_one(tref type_tree);

	/**
	 * @brief Unpacks a tau_ba from the element if present.
	 * @param elem The Boolean algebra element.
	 * @return The tref for the unpacked tau_ba, or nullptr when @p elem is
	 * not a wrapper (tau_ba) element.
	 */
	static tref unpack_tau_ba(const std::variant<BAs...>& elem);

	/**
	 * @brief Packs a tref into a Boolean algebra variant.
	 *
	 * The first wrapper (tau_ba) BA of the pack whose descriptor accepts
	 * @p t packs it.
	 * @param t The tref to pack.
	 * @return The packed variant, or nullopt when the pack has no wrapper BA
	 * or every wrapper declined.
	 */
	static std::optional<std::variant<BAs...>> pack_tau_ba(tref t);

	/**
	 * @brief Normalizes the Boolean algebra element.
	 * @param v The element to normalize.
	 * @return The normalized variant, or the report of a failed
	 * normalization.
	 */
	static result<std::variant<BAs...>> normalize(const std::variant<BAs...>& v);

	/**
	 * @brief Simplifies a symbol tref through the BA owning its type.
	 * @param symbol The symbol tref.
	 * @return The simplified tref, or @p symbol when no BA owns its type.
	 */
	static tref simplify_symbol(tref symbol);

	/**
	 * @brief Simplifies a term tref through the BA owning its type; the
	 * result reports why on a failure to simplify.
	 * @param term The term tref.
	 * @return The simplified term (@p term when no BA owns its type), or the
	 * owning BA's error.
	 */
	static result<tref> simplify_term(tref term);
};


} // namespace idni::tau_lang

#include "base_ba_dispatcher.tmpl.h"

#endif // __BASE_BA_DISPATCHER_H__