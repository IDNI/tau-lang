// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file bv_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the bitvector Boolean algebra.
 *
 * bv is the only parameterized BA: `bv[8]` and `bv[16]` are distinct types of
 * one family, so the type-system members carry a bitwidth instead of being the
 * stubs every other descriptor uses.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_DESCRIPTOR_TMPL_H__

#include <array>

#include "boolean_algebras/bv/parser/bitvector_parser.generated.h"
#include "boolean_algebras/ba_descriptor.h"
#include "ba_types.h"
// For `max_blast_reentry_depth`, which get_blastdepth_option()/
// set_blastdepth_option() below read/write directly; see its declaration
// comment for why the storage stays in core.
#include "antiprenexing/antiprenexing.h"

namespace idni::tau_lang {

template <typename... PackBAs>
struct ba_descriptor<bv, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	static constexpr const char* type_name = "bv";
	static constexpr int default_type_priority = 50;

	/** @brief bv is an atomic Boolean algebra, and not ω-categorical. */
	static constexpr bool atomless = false;
	static constexpr bool non_aba_omcat = false;

	/** @brief bv supports the grammar's arithmetic term operators. */
	static constexpr bool arith_ops = true;

	/** @brief A bitvector holds a plain 0 or 1; one bit is enough for it. */
	static constexpr bool can_host_bool = true;
	static tref bool_carrier_type() { return bv_type<node_t>(1); }

	/** @brief Render a value in decimal; cvc5's own operator<< does not. */
	static std::ostream& print_constant(std::ostream& os, const bv& x) {
		return os << (x.isBitVectorValue()
			? x.getBitVectorValue(10) : x.toString());
	}

	/**
	 * @brief Hash a bv constant by content, not by cvc5 term creation id.
	 * See hash_bv_constant (backends/cvc5/cvc5.h) and GitHub #89.
	 */
	static size_t hash_constant(const bv& x) { return hash_bv_constant(x); }

	static bool matches_type(tref type_tree) {
		return is_bv_type_family<node_t>(type_tree);
	}

	static tref type_tree() { return bv_type<node_t>(default_bv_size); }

	static bool owns_type(tref type_tree) { return matches_type(type_tree); }

	static bool owns_type(size_t ba_type_id) {
		return is_bv_type_family<node_t>(ba_type_id);
	}

	/** @brief The bitwidth of `bv[n]`; the one BA with a real subtype. */
	static std::optional<unsigned short> type_param(tref type_tree) {
		if (!matches_type(type_tree)) return std::nullopt;
		return static_cast<unsigned short>(
			get_bv_width<node_t>(type_tree));
	}

	static size_t type_id_for(unsigned short bitwidth) {
		return bv_type_id<node_t>(bitwidth);
	}

	static tref type_tree_for(unsigned short bitwidth) {
		return bv_type<node_t>(bitwidth);
	}

	static bool is_syntactic_one(const bv& x) {
		return is_bv_syntactic_one(x);
	}

	static bool is_syntactic_zero(const bv& x) {
		return is_bv_syntactic_zero(x);
	}

	static bool is_one(const bv& x) { return is_bv_syntactic_one(x); }

	static bool is_zero(const bv& x) { return is_bv_syntactic_zero(x); }

	static bool is_closed(const bv&) { return true; }

	/** @brief Width-dependent: the all-ones bitvector of this type's width. */
	static std::string literal_one(tref type_tree) {
		return make_bitvector_top_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	}

	static std::string literal_zero(tref type_tree) {
		return make_bitvector_bottom_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	}

	static bv normalize(const bv& x) { return normalize_bv(x); }

	static tref simplify_symbol(tref sym) {
		return simplify_bv_symbol<node_t>(sym);
	}

	static tref simplify_term(tref term) {
		return simplify_bv_term<node_t>(term);
	}

	/** @brief The one parse that needs the type tree: it carries the width. */
	static std::optional<typename node_t::constant_with_type>
	parse(const std::string& src, tref type_tree)
	{
		return parse_bv<PackBAs...>(src, type_tree);
	}

	// Optional capabilities: bv brings its own decision procedure, so generic
	// code asks the pack for one rather than naming solve_bv/is_bv_formula_sat.
	// `auto` return keeps the solution type out of the generic fold.

	/** @brief Solve @p form with bv's own solver. */
	static auto solve(tref form) { return solve_bv<node_t>(form); }

	/** @brief Decide satisfiability of @p form with bv's own solver. */
	static bool is_sat(tref form) { return is_bv_formula_sat<node_t>(form); }

	/**
	 * @brief `true` when bv can solve @p form at all.
	 *
	 * Every variable must carry an explicit bitwidth; mixed-type formulas
	 * cannot be translated to cvc5.
	 */
	static bool can_solve(tref form) {
		return is_bv_solvable_formula<node_t>(form);
	}

	/**
	 * @brief Definite satisfiability of @p form, or nullopt when undecided.
	 *
	 * cvc5 answering unknown, and translation failing, both mean "no definite
	 * answer" -- never "unsatisfiable".
	 */
	static std::optional<bool> sat_status(tref form) {
		auto status = bv_formula_sat_status<node_t>(form);
		if (status == bv_sat_status::sat) return true;
		if (status == bv_sat_status::unsat) return false;
		return std::nullopt;
	}

	/**
	 * @brief Predicate-blast @p n; returns it unchanged unless BOTH the
	 * core master `preprocessing` switch and bv's own `bv_blasting` switch
	 * are on.
	 */
	static tref preprocess(tref n) {
		return preprocessing && bv_blasting
			? bv_predicate_blasting<node_t>(n) : n;
	}

	/**
	 * @brief Set bv's OWN preprocessing switch (`bv_blasting`), not the
	 * core master `preprocessing`.
	 *
	 * This is what @ref pack_set_preprocessing (ba_pack_traits.h) calls for
	 * every BA that declares it -- so "set every BA's own switch" is what
	 * that capability has always meant; the core master is set separately,
	 * by `api::set_preprocessing`.
	 */
	static void set_preprocessing(bool enabled) { bv_blasting = enabled; }

	/// @name bv-declared CLI/REPL options
	/// Backing getters/setters for @ref options; plain free functions so
	/// they decay to the function pointers `ba_option` holds.
	/// @{
	static bool get_blasting_option() { return bv_blasting; }
	static void set_blasting_option(bool enabled) { bv_blasting = enabled; }
	static size_t get_blastdepth_option() { return max_blast_reentry_depth; }
	static void set_blastdepth_option(size_t n) {
		max_blast_reentry_depth = n;
	}
	/// @}

	/**
	 * @brief bv's own CLI/REPL options, addressed as `bv-blasting` and
	 * `bv-blastdepth`.
	 *
	 * `blasting` mirrors bv's own `bv_blasting` switch (see @ref preprocess:
	 * blasting still needs the core master `preprocessing` on as well).
	 * `blastdepth` mirrors core's `max_blast_reentry_depth`, whose storage
	 * stays in core (see the comment at its definition in
	 * antiprenexing/antiprenexing.h) but which only bv's own blasting pass
	 * can ever make progress against.
	 */
	static std::array<ba_option, 2> options() {
		return {{
			{ "blasting", ba_option_kind::flag,
				get_blasting_option, set_blasting_option,
				nullptr, nullptr,
				"enable bv predicate blasting (the global "
				"`preprocessing` switch gates it too -- both must "
				"be on for bv to blast)" },
			{ "blastdepth", ba_option_kind::count,
				nullptr, nullptr,
				get_blastdepth_option, set_blastdepth_option,
				"cap blast-block re-entry nesting in anti-prenexing "
				"(0 = unlimited)" },
		}};
	}

	/**
	 * @brief `true` when @p form still carries a one-hot bit-mask conjunction
	 * left over from an earlier predicate-blasting pass.
	 *
	 * @see idni::tau_lang::has_blasting_residue for what the screen looks for
	 * and why closing an already-blasted scope's free variables must avoid
	 * cvc5's non-terminating alternation on it.
	 */
	static bool has_preprocessing_residue(tref form) {
		return idni::tau_lang::has_blasting_residue<node_t>(form);
	}

	/**
	 * @brief `true` when predicate blasting can make progress on @p form.
	 *
	 * Blasting rewrites embedded bv arithmetic/comparisons into per-bit
	 * predicates; it has nothing to rewrite when `is_bv_solvable_formula`
	 * rejected @p form because some variable's ba_type falls outside the bv
	 * family (`non_bv_variable`) -- every other rejection (an unresolved
	 * ref, a missing bitwidth, no bv content at all) leaves that judgment to
	 * the caller, so this only answers `false` for the one reason blasting
	 * itself can never resolve.
	 */
	static bool formula_is_preprocessable(tref form) {
		bv_unsolvable_reason reason = bv_unsolvable_reason::ok;
		is_bv_solvable_formula<node_t>(form, reason);
		return reason != bv_unsolvable_reason::non_bv_variable;
	}

	/**
	 * @brief `true` when @p term's own arithmetic operator has the constant
	 * argument predicate blasting needs.
	 *
	 * Only `*`, `/`, `%`, `<<` and `>>` are constrained -- each blasts only
	 * with a constant second (`*`: either) argument (bv_predicate_blasting.
	 * tmpl.h). `+`, `-` and a cast blast unconditionally, so anything else
	 * answers `true`.
	 */
	static bool term_is_blasteable(tref term) {
		const auto& t = tau::get(term);
		if (t.is(tau::bf_mul))
			return get_bvmul_arguments<node_t>(term).second != nullptr;
		if (t.is(tau::bf_shl) || t.is(tau::bf_shr)
			|| t.is(tau::bf_div) || t.is(tau::bf_mod))
			return get_arguments<node_t>(term).second != nullptr;
		return true;
	}

	/**
	 * @brief `true` when @p src is a truncated bv literal, not a bad one.
	 *
	 * Distinct from `parse` failing, which cannot tell the two apart; the REPL
	 * keeps reading on truncation and stops on a genuine syntax error.
	 */
	static bool literal_incomplete(const std::string& src) {
		auto result = bitvector_parser::instance()
			.parse(src.c_str(), src.size());
		return !result.found && result.parse_error
			.to_str(bitvector_parser::error::info_lvl::INFO_BASIC)
			.find("Unexpected end of file") != std::string::npos;
	}

	/**
	 * @brief The bitvector of @p ba_type holding @p value, as a bf constant.
	 *
	 * The width comes from the type, so callers name a value and a type and
	 * never a bitwidth.
	 */
	static tref value_constant(size_t ba_type, size_t value) {
		return tau::get(tau::bf, { tau::get_ba_constant(
			make_bitvector_value(
				get_bv_size<node_t>(get_ba_type_tree<node_t>(ba_type)),
				value),
			ba_type) });
	}

	/** @brief The all-zeros bitvector of @p ba_type, wrapped as a bf constant. */
	static tref zero_constant(size_t ba_type) {
		return tau::get(tau::bf, { tau::get_ba_constant(
			make_bitvector_bottom_elem(
				get_bv_size<node_t>(get_ba_type_tree<node_t>(ba_type))),
			ba_type) });
	}
};

} // namespace idni::tau_lang

#include "boolean_algebras/bv/bv_ba_hooks_ext.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_DESCRIPTOR_TMPL_H__
