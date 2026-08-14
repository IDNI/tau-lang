// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file bool_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the classical Boolean algebra.
 *
 * `Bool` is not listed in `TAU_BAS` — it is not a configurable plugin — but it
 * still needs a descriptor, because packs such as `node<bv, Bool>` reach the
 * generic dispatcher like any other.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_DESCRIPTOR_TMPL_H__

#include "boolean_algebras/ba_descriptor.h"
#include "ba_types.h"

namespace idni::tau_lang {

template <typename... PackBAs>
struct ba_descriptor<Bool, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	static constexpr const char* type_name = "bool";

	/** @brief Wins the default type over any base BA that scores higher. */
	static constexpr int default_type_priority = 1;

	/** @brief Two-element: the one algebra that is definitely not atomless. */
	static constexpr bool atomless = false;
	static constexpr bool non_aba_omcat = false;

	/** @brief Bool is a bit already. */
	static constexpr bool can_host_bool = true;

	static bool matches_type(tref type_tree) {
		return ba_types_detail::type_tree_name_is<Bool, node_t>(
			type_tree, type_name);
	}

	static tref type_tree() {
		return ba_types_detail::make_syntactic_type_tree<node_t>(
			type_name);
	}

	static bool owns_type(tref type_tree) { return matches_type(type_tree); }

	static bool owns_type(size_t ba_type_id) {
		return ba_types_detail::type_tree_name_is<Bool, node_t>(
			ba_type_id, type_name);
	}

	/** @brief Bool takes no subtype, so a type never carries a parameter. */
	static std::optional<unsigned short> type_param(tref) {
		return std::nullopt;
	}

	static size_t type_id_for(unsigned short) {
		static const size_t id = ba_types<node_t>::id(type_tree());
		return id;
	}

	static tref type_tree_for(unsigned short) { return type_tree(); }

	static bool is_syntactic_one(const Bool& x) { return x.is_one(); }

	static bool is_syntactic_zero(const Bool& x) { return x.is_zero(); }

	static bool is_one(const Bool& x) { return x.is_one(); }

	static bool is_zero(const Bool& x) { return x.is_zero(); }

	static bool is_closed(const Bool&) { return true; }

	static std::string literal_one(tref) { return "1"; }

	static std::string literal_zero(tref) { return "0"; }

	static Bool normalize(const Bool& x) { return normalize_bool(x); }

	static tref simplify_symbol(tref sym) { return sym; }

	static tref simplify_term(tref term) { return term; }

	static std::optional<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_bool<PackBAs...>(src);
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_DESCRIPTOR_TMPL_H__
