// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qint_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the qint Boolean algebra.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_DESCRIPTOR_TMPL_H__

#include "boolean_algebras/qint/parser/qint_parser.generated.h"
#include "boolean_algebras/ba_descriptor.h"
#include "ba_types.h"

namespace idni::tau_lang {

/**
 * @brief The qint descriptor: finite unions of half-open rational intervals,
 * an atomless Boolean algebra written `{...}:qint`.
 * @tparam PackBAs The BAs of the configured pack.
 */
template <typename... PackBAs>
struct ba_descriptor<qint, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	static constexpr const char* type_name = "qint";
	static constexpr int default_type_priority = 50;

	/** @brief The 64-bit content hash: std::hash<qint> keeps 32 bits on wasm32. */
	static std::uint64_t hash_constant(const qint& x) { return qint_hash(x); }
	/// @brief qint has no atoms, so the splitter always has room to cut.
	static constexpr bool atomless = true;
	static constexpr bool non_aba_omcat = false;

	/// @brief `true` when @p type_tree names `qint`.
	static bool matches_type(tref type_tree) {
		return ba_types_detail::type_tree_name_is<qint, node_t>(
			type_tree, type_name);
	}

	/// @brief The type tree of `qint`.
	static tref type_tree() {
		return ba_types_detail::make_syntactic_type_tree<node_t>(
			type_name);
	}

	/// @brief `true` when @p ba_type_id is the id of `qint`.
	static bool owns_type(size_t ba_type_id) {
		return ba_types_detail::type_tree_name_is<qint, node_t>(
			ba_type_id, type_name);
	}


	/// @brief `true` when @p x is the whole line `(-inf, +inf)`.
	static bool is_syntactic_one(const qint& x) { return is_qint_one(x); }

	/// @brief `true` when @p x is the empty union.
	static bool is_syntactic_zero(const qint& x) { return is_qint_zero(x); }

	/// @brief Same as `is_syntactic_one`: a qint is kept normalized, so the
	/// syntactic test decides; never fails.
	static result<bool> is_one(const qint& x) { return result<bool>{is_qint_one(x)}; }

	/// @brief Same as `is_syntactic_zero`; never fails.
	static result<bool> is_zero(const qint& x) { return result<bool>{is_qint_zero(x)}; }

	/// @brief Always `true`: a qint constant has no free symbols.
	static result<bool> is_closed(const qint&) { return result<bool>{true}; }

	/// @brief The literal `top`.
	static std::string literal_one(tref) { return "top"; }

	/// @brief The literal `bot`.
	static std::string literal_zero(tref) { return "bot"; }

	/// @brief Return @p x: a qint is normalized at construction.
	static result<qint> normalize(const qint& x) {
		return result<qint>{normalize_qint(x)};
	}

	/// @brief A nonempty proper part of @p x, or @p x itself when the cut does
	/// not fit 64-bit endpoints; @p st is ignored (see `qint_splitter`).
	static result<qint> splitter(const qint& x, splitter_type st) {
		return result<qint>{qint_splitter(x, st)};
	}

	/// @brief The `bf` constant `[0, 1/2)`, a nonempty proper part of `top`.
	static tref splitter_one(tref) {
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(qint_splitter_one()),
			type_tree()));
	}

	/// @brief Return @p sym unchanged: qint has no symbol simplification.
	static tref simplify_symbol(tref sym) { return simplify_qint_symbol(sym); }

	/// @brief Return @p term unchanged: qint has no term simplification.
	static result<tref> simplify_term(tref term) { return result<tref>{simplify_qint_term(term)}; }

	/**
	 * @brief Parse the qint literal @p src.
	 * @param src The literal text; the type annotation is ignored.
	 * @return The constant typed `qint`, or an error naming why @p src is not
	 * one.
	 */
	static result<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_qint<PackBAs...>(src);
	}
	/**
	 * @brief `true` when @p src is a truncated qint literal, not a bad one.
	 *
	 * Distinct from `parse` failing, which cannot tell the two apart; the REPL
	 * keeps reading on truncation and stops on a genuine syntax error.
	 */
	static bool literal_incomplete(const std::string& src) {
		auto result = qint_parser::instance()
			.parse(src.c_str(), src.size());
		return !result.found && result.parse_error.at_eof();
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_DESCRIPTOR_TMPL_H__
