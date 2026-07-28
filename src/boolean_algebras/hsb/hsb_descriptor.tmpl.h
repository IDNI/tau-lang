// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file hsb_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the hsb Boolean algebra.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_DESCRIPTOR_TMPL_H__

#include "boolean_algebras/ba_descriptor.h"
#include "ba_types.h"

namespace idni::tau_lang {

template <typename... PackBAs>
struct ba_descriptor<hsb, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	static constexpr const char* type_name = "hsb";
	static constexpr int default_type_priority = 50;

	/** @brief hsb generalizes qint's intervals to polyhedra in R^d. */
	static constexpr bool atomless = true;
	static constexpr bool non_aba_omcat = false;

	static bool matches_type(tref type_tree) {
		return is_hsb_type<node_t>(type_tree);
	}

	static tref type_tree() { return hsb_type<node_t>(); }

	static bool owns_type(tref type_tree) { return matches_type(type_tree); }

	static bool owns_type(size_t ba_type_id) {
		return is_hsb_type<node_t>(ba_type_id);
	}

	/** @brief hsb takes no subtype, so a type never carries a parameter. */
	static std::optional<unsigned short> type_param(tref) {
		return std::nullopt;
	}

	static size_t type_id_for(unsigned short) { return hsb_type_id<node_t>(); }

	static tref type_tree_for(unsigned short) { return type_tree(); }

	static bool is_syntactic_one(const hsb& x) { return is_hsb_one(x); }

	static bool is_syntactic_zero(const hsb& x) { return is_hsb_zero(x); }

	static bool is_one(const hsb& x) { return is_hsb_one(x); }

	static bool is_zero(const hsb& x) { return is_hsb_zero(x); }

	static bool is_closed(const hsb&) { return true; }

	static std::string literal_one(tref) { return "top"; }

	static std::string literal_zero(tref) { return "bot"; }

	static hsb normalize(const hsb& x) { return normalize_hsb(x); }

	static hsb splitter(const hsb& x, splitter_type st) {
		return hsb_splitter(x, st);
	}

	static tref splitter_one(tref) {
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(hsb_splitter_one()),
			hsb_type<node_t>()));
	}

	/** @brief hsb holds no Tau spec, so there is nothing to unpack. */
	static tref unpack(const hsb&) { return nullptr; }

	static std::optional<hsb> pack(tref) { return std::nullopt; }

	static tref simplify_symbol(tref sym) { return simplify_hsb_symbol(sym); }

	static tref simplify_term(tref term) { return simplify_hsb_term(term); }

	static std::optional<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_hsb<PackBAs...>(src);
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_DESCRIPTOR_TMPL_H__
