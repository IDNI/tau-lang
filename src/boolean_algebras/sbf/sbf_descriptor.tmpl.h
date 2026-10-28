// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file sbf_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the SBF Boolean algebra.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_DESCRIPTOR_TMPL_H__

#include <set>

#include "boolean_algebras/sbf/parser/sbf_parser.generated.h"
#include "boolean_algebras/ba_descriptor_named.h"
#include "boolean_algebras/ba_literal_parse.h"

namespace idni::tau_lang {

// Defined in sbf_codegen.tmpl.h, included at this file's end; declared here
// so the descriptor's own definition context can name it.
template <NodeType node>
static result<std::optional<std::string>> sbf_codegen_constant_expr(tref cst);

/// The descriptor of the BDD-backed simple Boolean formula algebra (`sbf`).
template <typename... PackBAs>
struct ba_descriptor<sbf_ba, node<PackBAs...>>
	: ba_descriptor_named<ba_descriptor<sbf_ba, node<PackBAs...>>,
		sbf_ba, node<PackBAs...>>
{
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;
	/// The members derived from type_name (ba_descriptor_named).
	using named = ba_descriptor_named<ba_descriptor, sbf_ba, node_t>;
	using named::bf_constant;

	/// The type name, `sbf`.
	static constexpr const char* type_name = "sbf";
	/// Priority for the pack's default type (lower wins).
	static constexpr int default_type_priority = 1;

	/** @brief The 64-bit BDD reference hash: std::hash keeps 32 bits on wasm32. */
	static std::uint64_t hash_constant(const sbf_ba& x) { return x->hash(); }
	/// sbf is atomless.
	static constexpr bool atomless = true;
	/// sbf is not a non-aba omega-categorical algebra.
	static constexpr bool non_aba_omcat = false;

	/** @brief sbf can carry a plain 0 or 1. */
	static constexpr bool can_host_bool = true;

	/** @brief Keep sbf's own grammar in step with core's var/charvar mode. */
	static void set_charvar(bool charvar) {
		std::set<std::string> guards{ charvar ? "charvar" : "var" };
		sbf_parser::instance().get_grammar()
			.set_enabled_productions(guards);
		sbf_charvar = charvar;
	}

	/// Whether @p x is the BDD true; BDDs are canonical, so this is also
	/// the semantic answer.
	static bool is_syntactic_one(const sbf_ba& x) { return is_sbf_one(x); }

	/// Whether @p x is the BDD false.
	static bool is_syntactic_zero(const sbf_ba& x) { return is_sbf_zero(x); }

	/// Whether @p x is one; always decided, never an error.
	static result<bool> is_one(const sbf_ba& x) { return result<bool>{is_sbf_one(x)}; }

	/// Whether @p x is zero; always decided, never an error.
	static result<bool> is_zero(const sbf_ba& x) { return result<bool>{is_sbf_zero(x)}; }

	/// The spelling of the sbf one, `1`.
	static std::string literal_one(tref) { return "1"; }

	/// The spelling of the sbf zero, `0`.
	static std::string literal_zero(tref) { return "0"; }

	/** @brief The sbf constant holding @p value (0 or 1), wrapped as a bf constant; nullptr for any other value. */
	static tref value_constant(size_t, size_t value) {
		if (value > 1) return nullptr;
		return bf_constant(value ? bdd_handle<Bool>::htrue
		                         : bdd_handle<Bool>::hfalse);
	}

	/// @p x unchanged: a BDD is already in normal form.
	static result<sbf_ba> normalize(const sbf_ba& x) {
		return result<sbf_ba>{normalize_sbf(x)};
	}

	/// A splitter of @p x of kind @p st (sbf_splitter).
	static result<sbf_ba> splitter(const sbf_ba& x, splitter_type st) {
		return result<sbf_ba>{sbf_splitter(x, st)};
	}

	/// A bad splitter of one, wrapped as an sbf bf constant.
	static tref splitter_one(tref) {
		return bf_constant(sbf_splitter_one());
	}

	/// @p sym unchanged: sbf has no symbol simplification.
	static tref simplify_symbol(tref sym) { return simplify_sbf_symbol(sym); }

	/// @p term unchanged: sbf has no term simplification.
	static result<tref> simplify_term(tref term) { return result<tref>{simplify_sbf_term(term)}; }

	/// Parse @p src as an sbf literal; the type tree argument is ignored.
	static result<typename node_t::constant_with_type>
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
		return literal_incomplete_with<sbf_parser>(src);
	}

	/** @brief @p cst's own sbf BDD value, spelled for generated C++. */
	static result<std::optional<std::string>> codegen_constant_expr(tref cst) {
		return sbf_codegen_constant_expr<node_t>(cst);
	}
};

} // namespace idni::tau_lang

#include "boolean_algebras/sbf/sbf_codegen.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_DESCRIPTOR_TMPL_H__
