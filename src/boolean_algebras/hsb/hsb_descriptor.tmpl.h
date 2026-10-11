// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file hsb_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the hsb Boolean algebra.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_DESCRIPTOR_TMPL_H__

#include "boolean_algebras/hsb/parser/hsb_parser.generated.h"
#include "boolean_algebras/ba_descriptor.h"
#include "ba_types.h"

namespace idni::tau_lang {

/**
 * @brief The hsb descriptor: half-open polyhedra in R^d. Each member answers
 * the contract `ba_descriptor_complete` (ba_descriptor.h) names for it.
 */
template <typename... PackBAs>
struct ba_descriptor<hsb, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	/// Type name the pack registers for hsb.
	static constexpr const char* type_name = "hsb";
	/// Default-type priority (lower wins); 50 like every non-core BA.
	static constexpr int default_type_priority = 50;

	/// The platform-independent 64-bit content hash of @p x.
	static std::uint64_t hash_constant(const hsb& x) { return x.content_hash(); }

	/** @brief hsb is atomless: it generalizes qint's intervals to polyhedra
	 * in R^d, so every nonzero element splits. */
	static constexpr bool atomless = true;
	/// hsb is an ABA, not a non-ABA omega-categorical type.
	static constexpr bool non_aba_omcat = false;

	/// `true` when @p type_tree names the hsb type.
	static bool matches_type(tref type_tree) {
		return ba_types_detail::type_tree_name_is<hsb, node_t>(
			type_tree, type_name);
	}

	/// The syntactic type tree of hsb.
	static tref type_tree() {
		return ba_types_detail::make_syntactic_type_tree<node_t>(
			type_name);
	}

	/// `true` when the type with id @p ba_type_id is hsb.
	static bool owns_type(size_t ba_type_id) {
		return ba_types_detail::type_tree_name_is<hsb, node_t>(
			ba_type_id, type_name);
	}


	/// `true` when @p x is the whole space, decided by LRA feasibility.
	static bool is_syntactic_one(const hsb& x) { return is_hsb_one(x); }

	/// `true` when @p x is empty, decided by LRA feasibility.
	static bool is_syntactic_zero(const hsb& x) { return is_hsb_zero(x); }

	/// Semantic top test; never fails for hsb.
	static result<bool> is_one(const hsb& x) { return result<bool>{is_hsb_one(x)}; }

	/// Semantic bottom test; never fails for hsb.
	static result<bool> is_zero(const hsb& x) { return result<bool>{is_hsb_zero(x)}; }

	/// hsb constants carry no free variables, so always `true`.
	static result<bool> is_closed(const hsb&) { return result<bool>{true}; }

	/// Source spelling of the top element.
	static std::string literal_one(tref) { return "top"; }

	/// Source spelling of the bottom element.
	static std::string literal_zero(tref) { return "bot"; }

	/// The canonical form of @p x (normalize_hsb); never fails.
	static result<hsb> normalize(const hsb& x) {
		return result<hsb>{normalize_hsb(x)};
	}

	/// A part of @p x below it (hsb_splitter); @p st is ignored, and @p x
	/// comes back unchanged when no proper split is found.
	static result<hsb> splitter(const hsb& x, splitter_type st) {
		return result<hsb>{hsb_splitter(x, st)};
	}

	/// The fixed seed `{x[0] < 0}`, a proper part of top, as an hsb bf constant.
	static tref splitter_one(tref) {
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(hsb_splitter_one()),
			type_tree()));
	}

	/// Returns @p sym unchanged: hsb has no symbol simplification.
	static tref simplify_symbol(tref sym) { return simplify_hsb_symbol(sym); }

	/// Returns @p term unchanged: hsb has no term simplification.
	static result<tref> simplify_term(tref term) { return result<tref>{simplify_hsb_term(term)}; }

	/// Parses an hsb literal @p src into a constant with its type; the type
	/// argument is ignored. An error carries the parse failure.
	static result<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_hsb<PackBAs...>(src);
	}
	/**
	 * @brief `true` when @p src is a truncated hsb literal, not a bad one.
	 *
	 * Distinct from `parse` failing, which cannot tell the two apart; the REPL
	 * keeps reading on truncation and stops on a genuine syntax error.
	 */
	static bool literal_incomplete(const std::string& src) {
		auto result = hsb_parser::instance()
			.parse(src.c_str(), src.size());
		return !result.found && result.parse_error.at_eof();
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_DESCRIPTOR_TMPL_H__
