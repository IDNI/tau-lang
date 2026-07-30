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

#include "boolean_algebras/ba_descriptor.h"
#include "ba_types.h"

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

	/** @brief bv is atomic, so it has no splitter; the chains assert this. */
	static bv splitter(const bv&, splitter_type) {
		DBG(assert(false && "bv has no splitter");)
		return {};
	}

	static tref splitter_one(tref) { return nullptr; }

	/** @brief bv holds no Tau spec, so there is nothing to unpack. */
	static tref unpack(const bv&) { return nullptr; }

	static std::optional<bv> pack(tref) { return std::nullopt; }

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

	/** @brief Predicate-blast @p n; returns it unchanged when blasting is off. */
	static tref preprocess(tref n) {
		return bv_blasting ? bv_predicate_blasting<node_t>(n) : n;
	}

	static void set_preprocessing(bool enabled) { bv_blasting = enabled; }

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
