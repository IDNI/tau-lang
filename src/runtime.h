// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file runtime.h
 * @brief Concrete `nso_factory` specializations and `ba_constants::get` overloads
 *        for the runtime BA type combinations used by Tau.
 *
 * Provides `nso_factory<bv, sbf_ba>`, `nso_factory<tau_ba<bv,sbf_ba>, bv, sbf_ba>`,
 * and `nso_factory<bv, Bool>`, together with the matching `ba_constants::get`
 * specializations that parse constants from source strings.
 */

#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/tau_ba.h"
#include "boolean_algebras/bv_ba.h"
#include "nso_rr.h"

namespace idni::tau_lang {


/**
 * @brief NSO factory for the `<bv, sbf_ba>` BA combination.
 *
 * Used when executing Tau specifications without the Tau BA wrapper.
 */
template <>
struct nso_factory<bv, sbf_ba> {
	using node_t = node<bv, sbf_ba>;
	using tau = tree<node_t>;

	/** @brief Return `true` if @p elem is syntactically one (top element). */
	static bool is_syntactic_one(const std::variant<bv, sbf_ba>& elem);
	/** @brief Return `true` if @p elem is syntactically zero (bottom element). */
	static bool is_syntactic_zero(const std::variant<bv, sbf_ba>& elem);
	/** @brief Return `true` if @p elem evaluates to one. */
	static bool is_one(const std::variant<bv, sbf_ba>& elem);
	/** @brief Return `true` if @p elem evaluates to zero. */
	static bool is_zero(const std::variant<bv, sbf_ba>& elem);
	/** @brief Return `true` if @p elem is a closed (ground) term. */
	static bool is_closed(const std::variant<bv, sbf_ba>& elem);
	/** @brief Return the list of supported BA type name strings. */
	static std::vector<std::string> types();
	/** @brief Return the default BA type tree node. */
	static tref default_type();
	/** @brief Return the source string for the one constant of @p type_tree. */
	static std::string one(const tref type_tree);
	/** @brief Return the source string for the zero constant of @p type_tree. */
	static std::string zero(const tref type_tree);
	/** @brief Return the tree node for the splitter's one element. */
	static tref splitter_one();
	/** @brief Return the splitter of @p elem with type @p st. */
	static std::variant<bv, sbf_ba> splitter(const std::variant<bv, sbf_ba>& elem, splitter_type st);
	/** @brief Return the default (upper) splitter of @p elem. */
	static std::variant<bv, sbf_ba> splitter(const std::variant<bv, sbf_ba>& elem);
	/** @brief Unpack a Tau-BA wrapper and return the inner tree (always `nullptr` here). */
	static tref unpack_tau_ba(const std::variant<bv, sbf_ba>&);
	/** @brief Pack a tree node into a BA variant (returns empty). */
	static std::variant<bv, sbf_ba> pack_tau_ba(tref);
	/** @brief Normalize @p v and return the result. */
	static std::variant<bv, sbf_ba> normalize(const std::variant<bv, sbf_ba>& v);
	/** @brief Simplify a BA-typed symbol tree node. */
	static tref simplify_symbol(tref symbol);
	/** @brief Simplify a BA-typed term tree node. */
	static tref simplify_term(tref term);
};

/**
 * @brief NSO factory for the `<tau_ba<bv,sbf_ba>, bv, sbf_ba>` BA combination.
 *
 * Used in the REPL where the full Tau BA wrapper is required.
 */
template<>
struct nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba> {
	using node_t = node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
	using tau = tree<node_t>;

	/** @brief Return `true` if @p elem is syntactically one. */
	static 	bool is_syntactic_one(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem);
	/** @brief Return `true` if @p elem is syntactically zero. */
	static bool is_syntactic_zero(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem);
	/** @brief Return `true` if @p elem evaluates to one. */
	static bool is_one(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem);
	/** @brief Return `true` if @p elem evaluates to zero. */
	static bool is_zero(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem);
	/** @brief Return `true` if @p elem is a closed (ground) term. */
	static bool is_closed(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem);
	/** @brief Return the list of supported BA type name strings. */
	static std::vector<std::string> types();
	/** @brief Return the default BA type tree node. */
	static tref default_type();
	/** @brief Return the source string for the one constant of @p type_tree. */
	static std::string one(tref type_tree = tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>());
	/** @brief Return the source string for the zero constant of @p type_tree. */
	static std::string zero(tref type_tree = tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>());
	/** @brief Return the tree node for the splitter's one element of @p type_tree. */
	static tref splitter_one(tref type_tree);
	/** @brief Return the splitter of @p elem with type @p st. */
	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> splitter(
		const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem, splitter_type st);
	/** @brief Return the default splitter of @p elem. */
	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> splitter(
		const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem);
	/** @brief Unpack the Tau-BA wrapper from @p v and return the inner tree. */
	static tref unpack_tau_ba(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& v);
	/** @brief Pack tree node @p c into a BA variant. */
	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> pack_tau_ba(tref c);
	/** @brief Normalize @p v and return the result. */
	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> normalize(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& v);
	/** @brief Simplify a BA-typed symbol tree node. */
	static tref simplify_symbol(tref symbol);
	/** @brief Simplify a BA-typed term tree node. */
	static tref simplify_term(tref term);
};


/**
 * @brief NSO factory for the `<bv, Bool>` BA combination.
 *
 * Used for bitvector predicate blasting where the Boolean base type is the
 * classical two-element BA rather than an SBF.
 */
template<>
struct nso_factory<bv, Bool> {
	using node_t = node<bv, Bool>;
	using tau = tree<node_t>;

	/** @brief Return `true` if @p elem is syntactically one. */
	static bool is_syntactic_one(const std::variant<bv, Bool>& elem) {
		return std::holds_alternative<bv>(elem)
			? is_bv_syntactic_one(std::get<bv>(elem))
			: std::get<Bool>(elem).is_one();
	}

	/** @brief Return `true` if @p elem is syntactically zero. */
	static bool is_syntactic_zero(const std::variant<bv, Bool>& elem) {
		return std::holds_alternative<bv>(elem)
			? is_bv_syntactic_zero(std::get<bv>(elem))
			: std::get<Bool>(elem).is_zero();
	}

	/** @brief Return `true` if @p elem evaluates to one (syntactic check for BV). */
	static bool is_one(const std::variant<bv, Bool>& elem) {
		return std::holds_alternative<bv>(elem)
			? is_bv_syntactic_one(std::get<bv>(elem))
			: std::get<Bool>(elem).is_one();
	}

	/** @brief Return `true` if @p elem evaluates to zero (syntactic check for BV). */
	static bool is_zero(const std::variant<bv, Bool>& elem) {
		return std::holds_alternative<bv>(elem)
			? is_bv_syntactic_zero(std::get<bv>(elem))
			: std::get<Bool>(elem).is_zero();
	}

	/** @brief Return `true` — both `bv` and `Bool` are always considered closed. */
	static bool is_closed(const std::variant<bv, Bool>&) {
		// We return true as bv and Bool are closed (for our pourposes)
		return true;
	}

	/** @brief Return the list of supported BA type name strings. */
	static std::vector<std::string> types() { return { "bool" }; }

	/** @brief Return the default BA type tree node. */
	static tref default_type() { return bool_type<node_t>(); }

	/** @brief Return the source string for the one constant. */
	static std::string one(const tref) { return "1"; }

	/** @brief Return the source string for the zero constant. */
	static std::string zero(const tref) { return "0"; }

	/** @brief Return `Bool(true)` as the splitter one element. */
	static std::variant<bv, Bool> splitter_one(const std::string) { return Bool(true);}

	/** @brief Unpack a Tau-BA wrapper (always `nullptr` for this specialisation). */
	static tref unpack_tau_ba(const std::variant<bv, Bool>&) { return nullptr; }

	/** @brief Pack a tree node into a BA variant (returns empty). */
	static std::variant<bv, Bool> pack_tau_ba(tref) { return {}; }

	/** @brief Normalise @p v and return the result. */
	static std::variant<bv, Bool> normalize(const std::variant<bv, Bool>& v) {
		return std::holds_alternative<bv>(v)
			? std::variant<bv, Bool>(normalize_bv(std::get<bv>(v)))
			: std::variant<bv, Bool>(normalize_bool(std::get<Bool>(v)));
	}

	/** @brief Simplify a BA-typed symbol tree node. */
	static tref simplify_symbol(tref symbol) {
		auto ba_type = tau::get(symbol).get_ba_type();
		return is_bv_type_family<node_t>(ba_type) ? simplify_bv_symbol<node_t>(symbol) : symbol;
	}

	/** @brief Simplify a BA-typed term tree node. */
	static tref simplify_term(tref term) {
		auto ba_type = tau::get(term).get_ba_type();
		return is_bv_type_family<node_t>(ba_type) ? simplify_bv_term<node_t>(term) : term;
	}
};


/**
 * @brief Parse a constant for the `<bv, sbf_ba>` node type.
 *
 * Dispatches to `parse_bv` for bitvector types or `parse_sbf` otherwise.
 */
template <>
inline std::optional<ba_constants<node<bv, sbf_ba>>::constant_with_type> ba_constants<node<bv, sbf_ba>>::get(
		const std::string& constant_source,	tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (is_bv_type_family<node<bv, sbf_ba>>(type_tree))
		return parse_bv<bv, sbf_ba>(constant_source, type_tree);
	return parse_sbf<bv, sbf_ba>(constant_source);
}

/**
 * @brief Parse a constant for the `<tau_ba<bv,sbf_ba>, bv, sbf_ba>` node type.
 *
 * Dispatches to `parse_sbf` for sbf, `parse_bv` for BV, and `parse_tau` otherwise.
 */
template <>
inline std::optional<typename ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::constant_with_type> ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::get(
		const std::string& constant_source,	tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (is_sbf_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>(type_tree))
		return parse_sbf<tau_ba<bv, sbf_ba>, bv, sbf_ba>(constant_source);
	if (is_bv_type_family<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>(type_tree))
		return parse_bv<tau_ba<bv, sbf_ba>, bv, sbf_ba>(constant_source, type_tree);
	return parse_tau<bv, sbf_ba>(constant_source);
}

/**
 * @brief Parse a constant for the `<bv, Bool>` node type.
 *
 * Accepts `"1"` / `"true"` → `Bool(true)` and `"0"` / `"false"` → `Bool(false)`.
 */
template <>
std::optional<typename ba_constants<node<bv, Bool>>::constant_with_type> ba_constants<node<bv, Bool>>::get(
		const std::string& constant_source,
		[[maybe_unused]] tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (constant_source == "1" || constant_source == "true")
		return ba_constants<node<bv, Bool>>::constant_with_type{ Bool(true), bool_type<node<bv, Bool>>() };
	if (constant_source == "0" || constant_source == "false")
		return ba_constants<node<bv, Bool>>::constant_with_type{ Bool(false), bool_type<node<bv, Bool>>() };
	return {};
}

} // namespace idni::tau_lang

#include "runtime.tmpl.h"

#endif // __RUNTIME_H__