// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the qlt Boolean algebra.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_DESCRIPTOR_TMPL_H__

#include "boolean_algebras/ba_descriptor.h"
#include "ba_types.h"

namespace idni::tau_lang {

template <typename... PackBAs>
struct ba_descriptor<qlt, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	static constexpr const char* type_name = "qlt";
	static constexpr int default_type_priority = 50;

	/** @brief qlt is ω-categorical but not a Boolean algebra. */
	static constexpr bool atomless = false;
	static constexpr bool non_aba_omcat = true;

	static bool matches_type(tref type_tree) {
		return is_qlt_type<node_t>(type_tree);
	}

	static tref type_tree() { return qlt_type<node_t>(); }

	static bool owns_type(tref type_tree) { return matches_type(type_tree); }

	static bool owns_type(size_t ba_type_id) {
		return is_qlt_type<node_t>(ba_type_id);
	}

	/** @brief qlt takes no subtype, so a type never carries a parameter. */
	static std::optional<unsigned short> type_param(tref) {
		return std::nullopt;
	}

	static size_t type_id_for(unsigned short) { return qlt_type_id<node_t>(); }

	static tref type_tree_for(unsigned short) { return type_tree(); }

	static bool is_syntactic_one(const qlt& x) { return is_qlt_one(x); }

	static bool is_syntactic_zero(const qlt& x) { return is_qlt_zero(x); }

	static bool is_one(const qlt& x) { return is_qlt_one(x); }

	static bool is_zero(const qlt& x) { return is_qlt_zero(x); }

	static bool is_closed(const qlt&) { return true; }

	static std::string literal_one(tref) { return "top"; }

	static std::string literal_zero(tref) { return "bot"; }

	static qlt normalize(const qlt& x) { return normalize_qlt(x); }

	static qlt splitter(const qlt& x, splitter_type st) {
		return qlt_splitter(x, st);
	}

	static tref splitter_one(tref) {
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(qlt_splitter_one()),
			qlt_type<node_t>()));
	}

	/** @brief qlt holds no Tau spec, so there is nothing to unpack. */
	static tref unpack(const qlt&) { return nullptr; }

	static std::optional<qlt> pack(tref) { return std::nullopt; }

	static tref simplify_symbol(tref sym) { return simplify_qlt_symbol(sym); }

	static tref simplify_term(tref term) { return simplify_qlt_term(term); }

	static std::optional<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_qlt<PackBAs...>(src);
	}
};

} // namespace idni::tau_lang

#include "boolean_algebras/qlt/qlt_ba_hooks_ext.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_DESCRIPTOR_TMPL_H__
