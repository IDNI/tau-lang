// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the qlt Boolean algebra.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_DESCRIPTOR_TMPL_H__

#include "boolean_algebras/qlt/parser/qlt_parser.generated.h"
#include "boolean_algebras/ba_descriptor.h"
#include <array>
#include "ba_types.h"
// Reaches nothing beyond the standard library itself, unlike normalizer.h,
// so it is safe here; needed for `result<...>` before ltl_aba_result.h.
#include "tau_diagnostics.h"
#include "ltl_aba_result.h"

namespace idni::tau_lang {

// Defined in the templates this file includes at its end;
// declared here so the descriptor's own definition context can name them.
template <NodeType node>
static std::optional<bool> qlt_omcat_qe(tref var, tref body);

template <NodeType node>
static std::optional<std::string> qlt_codegen_constant_expr(tref cst);

template <NodeType node>
static result<propositional_synthesis<node>> qlt_try_propositional_synthesis(
	tref fm, const std::vector<std::pair<tref, std::string>>& atoms);

template <NodeType node>
result<tref> qlt_semantic_pwr_optimal(tref clause, tref update);

template <typename... PackBAs>
struct ba_descriptor<qlt, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	static constexpr const char* type_name = "qlt";
	static constexpr int default_type_priority = 50;

	/** @brief The 64-bit content hash: std::hash<qlt> keeps 32 bits on wasm32. */
	static std::uint64_t hash_constant(const qlt& x) { return qlt_hash(x); }

	/**
	 * @brief qlt values are sets of rationals: every nonzero element lies
	 * above an atom (any point it holds), so the algebra is not atomless.
	 */
	static constexpr bool atomless = false;
	static constexpr bool non_aba_omcat = false;

	static bool matches_type(tref type_tree) {
		return ba_types_detail::type_tree_name_is<qlt, node_t>(
			type_tree, type_name);
	}

	/// @name qlt-declared CLI/REPL options
	/// Backing getter/setter for @ref options; plain free functions so they
	/// decay to the function pointers `ba_option` holds.
	/// @{
	static size_t get_t3_cap_option() { return qlt_t3_encoding_cap(); }
	static void set_t3_cap_option(size_t n) {
		qlt_t3_encoding_cap_param = (long) n;
	}
	static size_t get_const_output_max_option() {
		return qlt_const_output_max();
	}
	static void set_const_output_max_option(size_t n) {
		qlt_const_output_max_param = (long) n;
	}
	/// @}

	/**
	 * @brief The options qlt declares about itself: `qlt-t3-cap`, the
	 * data-atom cap of the T3 encodings (Algorithms A/B/D and the semantic
	 * PWR); above it the default ABA-oracle path decides. Clamped to 30
	 * (the encodings shift `1 << K`); 0 = that bound.
	 */
	static std::array<ba_option, 2> options() {
		return {{
			{ "t3-cap", ba_option_kind::count,
				nullptr, nullptr,
				get_t3_cap_option, set_t3_cap_option,
				"cap the data atoms the qlt T3 synthesis encodings "
				"accept before the ABA-oracle path decides instead "
				"(default: TAU_QLT_T3_CAP or 20, at most 30; "
				"0 = 30)" },
			{ "const-output-max", ba_option_kind::count,
				nullptr, nullptr,
				get_const_output_max_option,
				set_const_output_max_option,
				"cap the constant-output assignments the fast path "
				"in front of Algorithm B enumerates (default: "
				"TAU_QLT_CONST_OUTPUT_MAX or 100; "
				"0 = unlimited)" },
		}};
	}

	static tref type_tree() {
		return ba_types_detail::make_syntactic_type_tree<node_t>(
			type_name);
	}

	static bool owns_type(size_t ba_type_id) {
		return ba_types_detail::type_tree_name_is<qlt, node_t>(
			ba_type_id, type_name);
	}


	static bool is_syntactic_one(const qlt& x) { return is_qlt_one(x); }

	static bool is_syntactic_zero(const qlt& x) { return is_qlt_zero(x); }

	static result<bool> is_one(const qlt& x) { return result<bool>{is_qlt_one(x)}; }

	static result<bool> is_zero(const qlt& x) { return result<bool>{is_qlt_zero(x)}; }

	static result<bool> is_closed(const qlt&) { return result<bool>{true}; }

	static std::string literal_one(tref) { return "top"; }

	static std::string literal_zero(tref) { return "bot"; }

	static result<qlt> normalize(const qlt& x) {
		return result<qlt>{normalize_qlt(x)};
	}

	static result<qlt> splitter(const qlt& x, splitter_type st) {
		return result<qlt>{qlt_splitter(x, st)};
	}

	static tref splitter_one(tref) {
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(qlt_splitter_one()),
			type_tree()));
	}

	static tref simplify_symbol(tref sym) { return simplify_qlt_symbol(sym); }

	static result<tref> simplify_term(tref term) { return result<tref>{simplify_qlt_term(term)}; }

	static result<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_qlt<PackBAs...>(src);
	}

	/**
	 * @brief `true` when @p src is a truncated qlt literal, not a bad one.
	 *
	 * Distinct from `parse` failing, which cannot tell the two apart; the REPL
	 * keeps reading on truncation and stops on a genuine syntax error.
	 */
	static bool literal_incomplete(const std::string& src) {
		auto result = qlt_parser::instance()
			.parse(src.c_str(), src.size());
		return !result.found && result.parse_error.at_eof();
	}

	/**
	 * @brief Decide a quantifier over a qlt variable: exactly when the
	 * quantified formula is closed (see qlt_decide_closed), and `ex` of a
	 * body that only excludes values; nullopt otherwise, and core falls
	 * through to its generic path.
	 */
	static std::optional<bool> omcat_qe(tref var, tref body) {
		return qlt_omcat_qe<node_t>(var, body);
	}

	/** @brief @p cst's own set, spelled for generated C++. */
	static std::optional<std::string> codegen_constant_expr(tref cst) {
		return qlt_codegen_constant_expr<node_t>(cst);
	}
};

} // namespace idni::tau_lang

#include "boolean_algebras/qlt/qlt_qe.tmpl.h"
#include "boolean_algebras/qlt/qlt_codegen.tmpl.h"
#include "boolean_algebras/qlt/qlt_ltl_synthesis.tmpl.h"
#include "boolean_algebras/qlt/qlt_semantic_pwr.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_DESCRIPTOR_TMPL_H__
