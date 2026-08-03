// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the qlt Boolean algebra.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_DESCRIPTOR_TMPL_H__

#include "boolean_algebras/qlt/parser/qlt_parser.generated.h"
#include "boolean_algebras/ba_descriptor.h"
#include "ba_types.h"
#include "ltl_aba_result.h"
#include "solver_types.h"

namespace idni::tau_lang {

// Defined in the qlt_qe / qlt_solver templates this file includes at its end;
// declared here so the descriptor's own definition context can name them.
template <NodeType node>
static std::optional<bool> qlt_omcat_qe(tref var, tref body);

template <NodeType node>
static std::optional<solution<node>> qlt_omcat_solve_inequality_system(
	const inequality_system<node>& sys, const solver_options& options);

template <NodeType node>
static std::optional<std::string> qlt_codegen_witness(tref var, tref conj);

template <NodeType node>
static propositional_synthesis<node> qlt_try_propositional_synthesis(tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms);

template <NodeType node>
tref qlt_semantic_pwr_optimal(tref clause, tref update);

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
		return ba_types_detail::type_tree_name_is<qlt, node_t>(
			type_tree, type_name);
	}

	static tref type_tree() {
		return ba_types_detail::make_syntactic_type_tree<node_t>(
			type_name);
	}

	static bool owns_type(tref type_tree) { return matches_type(type_tree); }

	static bool owns_type(size_t ba_type_id) {
		return ba_types_detail::type_tree_name_is<qlt, node_t>(
			ba_type_id, type_name);
	}

	/** @brief qlt takes no subtype, so a type never carries a parameter. */
	static std::optional<unsigned short> type_param(tref) {
		return std::nullopt;
	}

	static size_t type_id_for(unsigned short) {
		static const size_t id = ba_types<node_t>::id(type_tree());
		return id;
	}

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
			type_tree()));
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

	/**
	 * @brief `true` when @p src is a truncated qlt literal, not a bad one.
	 *
	 * Distinct from `parse` failing, which cannot tell the two apart; the REPL
	 * keeps reading on truncation and stops on a genuine syntax error.
	 */
	static bool literal_incomplete(const std::string& src) {
		auto result = qlt_parser::instance()
			.parse(src.c_str(), src.size());
		return !result.found && result.parse_error
			.to_str(qlt_parser::error::info_lvl::INFO_BASIC)
			.find("Unexpected end of file") != std::string::npos;
	}

	/**
	 * @brief The singleton `{0}`, wrapped as a bf constant.
	 *
	 * A dense linear order has no bottom element, so qlt's default is the
	 * finite rational 0 rather than bf_f.
	 */
	static tref zero_constant(size_t ba_type) {
		qlt z;
		qlt_piece p;
		p.lo = qlt_endpoint{qlt_rational(0, 1), qlt_bound::CLOSED};
		p.hi = qlt_endpoint{qlt_rational(0, 1), qlt_bound::CLOSED};
		z.pieces.push_back(p);
		return tau::get(tau::bf, { tau::get_ba_constant(
			typename node_t::constant(z), ba_type) });
	}

	/**
	 * @brief Decide a quantifier over a qlt variable by DLO elimination.
	 *
	 * Answers satisfiability rather than the satisfying interval, which stays
	 * qlt's own: nullopt says the body is not a DLO conjunction this can
	 * resolve, and core falls through to the atomless-BA path.
	 */
	static std::optional<bool> omcat_qe(tref var, tref body) {
		return qlt_omcat_qe<node_t>(var, body);
	}

	/** @brief A rational witness for @p var, spelled for generated C++. */
	static std::optional<std::string> codegen_witness(tref var, tref conj) {
		return qlt_codegen_witness<node_t>(var, conj);
	}

	/**
	 * @brief Synthesise an LTL formula over qlt atoms propositionally.
	 *
	 * Algorithms A, B and D encode the (Q,<) order types exactly, so no ABA
	 * oracle is needed when they apply. Declining and proving unrealizable
	 * are separate answers -- see @ref propositional_synthesis.
	 */
	static propositional_synthesis<node_t> try_propositional_synthesis(
		tref fm, const std::vector<std::pair<tref, std::string>>& atoms)
	{
		return qlt_try_propositional_synthesis<node_t>(fm, atoms);
	}

	/** @brief Revise @p clause by the winning region of its product game. */
	static tref semantic_pwr_optimal(tref clause, tref update) {
		return qlt_semantic_pwr_optimal<node_t>(clause, update);
	}

	/** @brief Solve a pure ordering system, which a BA-level solve cannot. */
	static std::optional<solution<node_t>> omcat_solve_inequality_system(
		const inequality_system<node_t>& sys,
		const solver_options& options)
	{
		return qlt_omcat_solve_inequality_system<node_t>(sys, options);
	}
};

} // namespace idni::tau_lang

#include "boolean_algebras/qlt/qlt_ba_hooks_ext.tmpl.h"
#include "boolean_algebras/qlt/qlt_qe.tmpl.h"
#include "boolean_algebras/qlt/qlt_solver.tmpl.h"
#include "boolean_algebras/qlt/qlt_codegen.tmpl.h"
#include "boolean_algebras/qlt/qlt_ltl_synthesis.tmpl.h"
#include "boolean_algebras/qlt/qlt_semantic_pwr.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_DESCRIPTOR_TMPL_H__
