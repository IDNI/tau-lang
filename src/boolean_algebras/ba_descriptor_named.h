// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ba_descriptor_named.h
 * @brief An opt-in base for the descriptor of a BA whose one type is named
 * by its `type_name` alone.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_DESCRIPTOR_NAMED_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_DESCRIPTOR_NAMED_H__

#include "boolean_algebras/ba_descriptor.h"
#include "ba_types.h"

namespace idni::tau_lang {

/**
 * @brief The members a descriptor of an unparameterized BA writes the same
 * way: `matches_type`, `type_tree` and `owns_type` from
 * `Descriptor::type_name`, and `is_closed` answering `true` for an algebra
 * whose constants have no free symbol (declare it in the descriptor
 * otherwise). `bf_constant` wraps a value as a bf constant of the type.
 *
 * Derive the descriptor from it, naming the descriptor itself as
 * @p Descriptor. It must declare no optional capability: a `ba_has_*` probe
 * would then find that capability in every algebra deriving from it.
 */
template <typename Descriptor, typename BA, typename Node>
struct ba_descriptor_named {
	/// Whether type tree @p type_tree names the type.
	static bool matches_type(tref type_tree) {
		return ba_types_detail::type_tree_name_is<BA, Node>(
			type_tree, Descriptor::type_name);
	}

	/// The type tree `:type_name`.
	static tref type_tree() {
		return ba_types_detail::make_syntactic_type_tree<Node>(
			Descriptor::type_name);
	}

	/// Whether type id @p ba_type_id names the type; false for an invalid id.
	static bool owns_type(size_t ba_type_id) {
		return ba_types_detail::type_tree_name_is<BA, Node>(
			ba_type_id, Descriptor::type_name);
	}

	/// Always `true`: a constant has no free symbol.
	static result<bool> is_closed(const BA&) { return result<bool>{true}; }

	/// @p value as a bf constant of the type.
	static tref bf_constant(const BA& value) {
		using tau = tree<Node>;
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(value), type_tree()));
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_DESCRIPTOR_NAMED_H__
