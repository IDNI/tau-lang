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

/**
 * @brief Compare two qlt singleton constants: -1/0/+1, or nullopt if either
 * side is not a finite qlt singleton.
 */
template<NodeType node>
static std::optional<int> qlt_singleton_cmp(
	const tree<node>& c1, const tree<node>& c2)
{
	if (!c1.is_ba_constant() || !c2.is_ba_constant()) return {};
	auto v1 = c1.get_ba_constant();
	auto v2 = c2.get_ba_constant();
	if (!std::holds_alternative<qlt>(v1) || !std::holds_alternative<qlt>(v2))
		return {};
	const auto& q1 = std::get<qlt>(v1);
	const auto& q2 = std::get<qlt>(v2);
	if (q1.pieces.size() != 1 || q2.pieces.size() != 1) return {};
	const auto& p1 = q1.pieces[0];
	const auto& p2 = q2.pieces[0];
	if (p1.lo.val != p1.hi.val || p2.lo.val != p2.hi.val) return {};
	if (p1.lo.bound != qlt_bound::CLOSED || p1.hi.bound != qlt_bound::CLOSED)
		return {};
	if (p2.lo.bound != qlt_bound::CLOSED || p2.hi.bound != qlt_bound::CLOSED)
		return {};
	if (!p1.lo.val.is_finite() || !p2.lo.val.is_finite()) return {};
	if (p1.lo.val < p2.lo.val) return -1;
	if (p1.lo.val > p2.lo.val) return +1;
	return 0;
}

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
	 * @brief The order of two qlt singleton constants. `0` and `1` are the
	 * order's sentinels below and above every point, not points, so they
	 * compare as nothing here.
	 */
	static std::optional<int> dense_order_compare(size_t, tref a, tref b) {
		auto operand = [](tref c) -> const tau& {
			const auto& t = tau::get(c);
			return t.is(tau::bf) && t.has_child() ? t[0] : t;
		};
		const auto& x = operand(a);
		const auto& y = operand(b);
		if (!x.is_ba_constant() || !y.is_ba_constant()) return std::nullopt;
		return qlt_singleton_cmp<node_t>(x, y);
	}

	/**
	 * @brief The rational halfway between @p lo and @p hi, one above
	 * @p lo or below @p hi when only one is given, 0 when neither is.
	 */
	static tref dense_order_between(size_t ba_type, tref lo, tref hi) {
		auto point = [](tref c) -> std::optional<qlt_rational> {
			const auto& t = tau::get(c);
			const auto& x = t.is(tau::bf) && t.has_child() ? t[0] : t;
			if (!x.is_ba_constant()) return std::nullopt;
			auto v = x.get_ba_constant();
			if (!std::holds_alternative<qlt>(v)) return std::nullopt;
			const auto& q = std::get<qlt>(v);
			if (q.pieces.size() != 1) return std::nullopt;
			const auto& p = q.pieces[0];
			if (!p.lo.val.is_finite() || p.lo.val != p.hi.val
				|| p.lo.bound != qlt_bound::CLOSED
				|| p.hi.bound != qlt_bound::CLOSED) return std::nullopt;
			return p.lo.val;
		};
		qlt_rational v(0, 1);
		if (lo && hi) {
			auto a = point(lo), b = point(hi);
			if (!a || !b) return nullptr;
			v = a->midpoint(*b);
		} else if (lo) {
			auto a = point(lo);
			if (!a) return nullptr;
			v = *a + qlt_rational(1, 1);
		} else if (hi) {
			auto b = point(hi);
			if (!b) return nullptr;
			v = *b + qlt_rational(-1, 1);
		}
		qlt z;
		qlt_piece p;
		p.lo = qlt_endpoint{ v, qlt_bound::CLOSED };
		p.hi = qlt_endpoint{ v, qlt_bound::CLOSED };
		z.pieces.push_back(p);
		return tau::get(tau::bf, { tau::get_ba_constant(
			typename node_t::constant(z), ba_type) });
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

	/**
	 * @brief Synthesise an LTL formula over qlt atoms propositionally.
	 *
	 * Algorithms A, B and D encode the (Q,<) order types exactly, so no ABA
	 * oracle is needed when they apply. Declining and proving unrealizable
	 * are separate answers -- see @ref propositional_synthesis.
	 */
	static result<propositional_synthesis<node_t>> try_propositional_synthesis(
		tref fm, const std::vector<std::pair<tref, std::string>>& atoms)
	{ return qlt_try_propositional_synthesis<node_t>(fm, atoms); }

	/** @brief Revise @p clause by the winning region of its product game. */
	static result<tref> semantic_pwr_optimal(tref clause, tref update) {
		return qlt_semantic_pwr_optimal<node_t>(clause, update);
	}
};

} // namespace idni::tau_lang

#include "boolean_algebras/qlt/qlt_qe.tmpl.h"
#include "boolean_algebras/qlt/qlt_codegen.tmpl.h"
#include "boolean_algebras/qlt/qlt_ltl_synthesis.tmpl.h"
#include "boolean_algebras/qlt/qlt_semantic_pwr.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_DESCRIPTOR_TMPL_H__
