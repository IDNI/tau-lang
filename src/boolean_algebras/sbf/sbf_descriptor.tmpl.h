// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file sbf_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the SBF Boolean algebra.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_DESCRIPTOR_TMPL_H__

#include <set>

#include "boolean_algebras/sbf/parser/sbf_parser.generated.h"
#include "boolean_algebras/ba_descriptor.h"

namespace idni::tau_lang {

// Defined in sbf_codegen.tmpl.h, included at this file's end; declared here
// so the descriptor's own definition context can name it.
template <NodeType node>
static std::optional<std::string> sbf_codegen_constant_expr(tref cst);

template <typename... PackBAs>
struct ba_descriptor<sbf_ba, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	static constexpr const char* type_name = "sbf";
	static constexpr int default_type_priority = 1;
	static constexpr bool atomless = true;
	static constexpr bool non_aba_omcat = false;

	/** @brief sbf can carry a plain 0 or 1. */
	static constexpr bool can_host_bool = true;

	/** @brief Keep sbf's own grammar in step with core's var/charvar mode. */
	static void set_charvar(bool charvar) {
		std::set<std::string> guards{ charvar ? "charvar" : "var" };
		sbf_parser::instance().get_grammar()
			.set_enabled_productions(guards);
	}

	static bool matches_type(tref type_tree) {
		return ba_types_detail::type_tree_name_is<sbf_ba, node_t>(
			type_tree, type_name);
	}

	static tref type_tree() {
		return ba_types_detail::make_syntactic_type_tree<node_t>(
			type_name);
	}

	static bool owns_type(tref type_tree) { return matches_type(type_tree); }

	static bool owns_type(size_t ba_type_id) {
		return ba_types_detail::type_tree_name_is<sbf_ba, node_t>(
			ba_type_id, type_name);
	}

	/** @brief sbf takes no subtype, so a type never carries a parameter. */
	static std::optional<unsigned short> type_param(tref) {
		return std::nullopt;
	}

	static size_t type_id_for(unsigned short) {
		static const size_t id = ba_types<node_t>::id(type_tree());
		return id;
	}

	static tref type_tree_for(unsigned short) { return type_tree(); }

	static bool is_syntactic_one(const sbf_ba& x) { return is_sbf_one(x); }

	static bool is_syntactic_zero(const sbf_ba& x) { return is_sbf_zero(x); }

	static bool is_one(const sbf_ba& x) { return is_sbf_one(x); }

	static bool is_zero(const sbf_ba& x) { return is_sbf_zero(x); }

	static bool is_closed(const sbf_ba&) { return true; }

	static std::string literal_one(tref) { return "1"; }

	static std::string literal_zero(tref) { return "0"; }

	/** @brief The sbf constant holding @p value (0 or 1), wrapped as a bf constant. */
	static tref value_constant(size_t, size_t value) {
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(value ? bdd_handle<Bool>::htrue
			                              : bdd_handle<Bool>::hfalse),
			type_tree()));
	}

	static sbf_ba normalize(const sbf_ba& x) { return normalize_sbf(x); }

	static sbf_ba splitter(const sbf_ba& x, splitter_type st) {
		return sbf_splitter(x, st);
	}

	static tref splitter_one(tref) {
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(sbf_splitter_one()),
			type_tree()));
	}

	static tref simplify_symbol(tref sym) { return simplify_sbf_symbol(sym); }

	static tref simplify_term(tref term) { return simplify_sbf_term(term); }

	static std::optional<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_sbf<PackBAs...>(src);
	}

	/**
	 * @brief `true` when @p src is a truncated sbf literal, not a bad one.
	 *
	 * Distinct from `parse` failing, which cannot tell the two apart; the REPL
	 * keeps reading on truncation and stops on a genuine syntax error.
	 */
	static bool literal_incomplete(const std::string& src) {
		auto result = sbf_parser::instance().parse(src.c_str(), src.size());
		return !result.found && result.parse_error
			.to_str(sbf_parser::error::info_lvl::INFO_BASIC)
			.find("Unexpected end of file") != std::string::npos;
	}

	/** @brief @p cst's own sbf BDD value, spelled for generated C++. */
	static std::optional<std::string> codegen_constant_expr(tref cst) {
		return sbf_codegen_constant_expr<node_t>(cst);
	}
};

} // namespace idni::tau_lang

#include "boolean_algebras/sbf/sbf_codegen.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_DESCRIPTOR_TMPL_H__
