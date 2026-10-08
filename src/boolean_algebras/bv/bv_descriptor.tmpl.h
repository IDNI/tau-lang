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

#include "boolean_algebras/bv/parser/bitvector_parser.generated.h"
#include "boolean_algebras/ba_descriptor.h"
#include "ba_types.h"
// For `max_blast_reentry_depth`, which bv_options.h binds; see its
// declaration comment for why the storage stays in core.
#include "antiprenexing/antiprenexing.h"
#include "boolean_algebras/bv/bv_options.h"

namespace idni::tau_lang {

// Defined in bv_codegen.tmpl.h, included at this file's end; declared here so
// the descriptor's own definition context can name it.
template <NodeType node>
static std::optional<std::string> bv_codegen_witness(tref var, tref conj);

/// C++ spelling of a bv constant; defined in bv_codegen.tmpl.h.
template <NodeType node>
static result<std::optional<std::string>> bv_codegen_constant_expr(tref cst);

/**
 * @brief The descriptor of bv in any pack that holds it: the mandatory
 * ba_descriptor surface plus bv's optional capabilities.
 * @tparam PackBAs The BAs of the pack.
 */
template <typename... PackBAs>
struct ba_descriptor<bv, node<PackBAs...>> {
	/// The node type of the pack.
	using node_t = node<PackBAs...>;
	/// The tree over that node type.
	using tau = tree<node_t>;

	/// The family name of the type, matched by `bv` and every `bv[n]`.
	static constexpr const char* type_name = "bv";
	/// Priority for the pack's default type (lower wins).
	static constexpr int default_type_priority = 50;

	/** @brief bv is an atomic Boolean algebra, and not ω-categorical. */
	static constexpr bool atomless = false;
	/// bv is not a non-ABA ω-categorical type.
	static constexpr bool non_aba_omcat = false;

	/** @brief bv supports the grammar's arithmetic term operators. */
	static constexpr bool arith_ops = true;

	/** @brief A bitvector holds a plain 0 or 1; one bit is enough for it. */
	static constexpr bool can_host_bool = true;
	/// The type core builds a plain 0/1 in when bv carries it: `bv[1]`.
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
	static std::uint64_t hash_constant(const bv& x) { return hash_bv_constant(x); }

	/// Whether @p type_tree is `bv` or some `bv[n]`.
	static bool matches_type(tref type_tree) {
		return is_bv_type_family<node_t>(type_tree);
	}

	/// The default bv type, `bv[default_bv_size]`.
	static tref type_tree() { return bv_type<node_t>(default_bv_size); }

	/// Whether the type id @p ba_type_id is of the bv family; false for an
	/// unknown id.
	static bool owns_type(size_t ba_type_id) {
		return is_bv_type_family<node_t>(ba_type_id);
	}

	/**
	 * @brief The bitwidth of `bv[n]`, or `nullopt` for a bare `bv`.
	 *
	 * Reads the subtype directly rather than through get_bv_width, which
	 * reports an error on absence; a bare type is an ordinary answer here.
	 */
	static std::optional<unsigned short> type_param(tref type_tree) {
		if (!matches_type(type_tree)) return std::nullopt;
		using tt = tau::traverser;
		auto subtype = tt(type_tree) | tau::type | tau::subtype | tt::ref;
		if (!subtype) return std::nullopt;
		const uint64_t width = tau::get(subtype)[0].get_num();
		// An out-of-range width is not a parameter of the bv family.
		if (width < 1 || width > 0xffff) return std::nullopt;
		return static_cast<unsigned short>(width);
	}

	/// The type id of `bv[bitwidth]`, registering the type if needed.
	static size_t type_id_for(unsigned short bitwidth) {
		return bv_type_id<node_t>(bitwidth);
	}

	/// The type tree of `bv[bitwidth]`.
	static tref type_tree_for(unsigned short bitwidth) {
		return bv_type<node_t>(bitwidth);
	}

	/// Whether @p x is literally the all-ones bitvector.
	static bool is_syntactic_one(const bv& x) {
		return is_bv_syntactic_one(x);
	}

	/// Whether @p x is literally the all-zeros bitvector.
	static bool is_syntactic_zero(const bv& x) {
		return is_bv_syntactic_zero(x);
	}

	/// Whether @p x is the top element; decided syntactically, never fails.
	static result<bool> is_one(const bv& x) {
		return result<bool>{is_bv_syntactic_one(x)};
	}

	/// Whether @p x is the bottom element; decided syntactically, never fails.
	static result<bool> is_zero(const bv& x) {
		return result<bool>{is_bv_syntactic_zero(x)};
	}

	/// Every bv constant is closed.
	static result<bool> is_closed(const bv&) { return result<bool>{true}; }

	/**
	 * @brief Width-dependent: the all-ones bitvector of this type's width,
	 * in decimal.
	 * @pre @p type_tree carries a width; empty otherwise.
	 */
	static std::string literal_one(tref type_tree) {
		auto width = get_bv_size<node_t>(type_tree);
		// The type carries a width: inference rejects a widthless `bv`, `bv[0]`
		// and a width past the maximum before any capability sees the type.
		DBG(assert(width.has_value());)
		if (!width.has_value()) return {};
		return make_bitvector_top_elem(width.value()).getBitVectorValue(10);
	}

	/// The all-zeros bitvector of @p type_tree's width, in decimal; the same
	/// precondition as literal_one.
	static std::string literal_zero(tref type_tree) {
		auto width = get_bv_size<node_t>(type_tree);
		DBG(assert(width.has_value());)
		if (!width.has_value()) return {};
		return make_bitvector_bottom_elem(width.value()).getBitVectorValue(10);
	}

	/// @p x simplified by cvc5; never fails.
	static result<bv> normalize(const bv& x) {
		return result<bv>{normalize_bv(x)};
	}

	/// The simplified form of the bv operator node @p sym.
	static tref simplify_symbol(tref sym) {
		return simplify_bv_symbol<node_t>(sym);
	}

	/// The simplified form of the bv term @p term; an error is carried in
	/// the report.
	static result<tref> simplify_term(tref term) {
		return simplify_bv_term<node_t>(term);
	}

	/** @brief The one parse that needs the type tree: it carries the width. */
	static result<typename node_t::constant_with_type>
	parse(const std::string& src, tref type_tree)
	{
		return parse_bv<PackBAs...>(src, type_tree);
	}

	// Optional capabilities: bv brings its own decision procedure, so generic
	// code asks the pack for one rather than naming solve_bv/is_bv_formula_sat.

	/**
	 * @brief Solve @p form with bv's own solver.
	 * @return A satisfying assignment, or nullopt when the solver finds
	 *         none; the error of widening (if on) or of translating @p form
	 *         when it fails, since a failure says nothing about the
	 *         satisfiability of @p form.
	 */
	// Exact arithmetic must see the widened atoms before cvc5 does.
	static result<std::optional<solution<node_t>>> solve(tref form) {
		result<std::optional<solution<node_t>>> r;
		TAU_TRY(form, widen_arithmetic(form));
		TAU_TRY(auto sol, solve_bv<node_t>(form));
		return r.with_value(std::move(sol));
	}

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
	// Exact arithmetic must see the widened atoms before cvc5 does.
	// A probe: every caller of pack_sat_status reads nullopt as "decide it
	// the long way", and that way runs solve(), which reports a widening
	// refusal (bv-max-width) or a translation error. Answering undecided
	// here loses no failure the user can see.
	static std::optional<bool> sat_status(tref form) {
		form = widen_arithmetic(form).value_or(nullptr);
		if (!form) return std::nullopt;
		auto status = bv_formula_sat_status<node_t>(form).value_or(std::nullopt);
		if (status == bv_sat_status::sat) return true;
		if (status == bv_sat_status::unsat) return false;
		return std::nullopt;
	}

	/**
	 * @brief The truth of the closed bitvector formula @p form, decided
	 * whole by its bits or cvc5 whatever its quantifier prefix; nullopt
	 * when the formula is not bv's or the decision is undecided.
	 */
	static std::optional<bool> decide_closed(tref form) {
		if (!can_solve(form)) return std::nullopt;
		return sat_status(form);
	}

	/**
	 * @brief Predicate-blast @p n; returns it unchanged unless BOTH the
	 * core master `preprocessing` switch and bv's own `bv_blasting` switch
	 * are on. A blasting failure carries its own report rather than a
	 * bare null tree.
	 */
	static result<tref> preprocess(tref n) {
		return preprocessing && bv_blasting
			? bv_predicate_blasting<node_t>(n) : result<tref>{n};
	}

	/**
	 * @brief Eliminate a quantified bitvector variable tested only against
	 * constants; returns @p n unchanged when `bv_case_split` is disabled.
	 */
	static tref case_split_quantifiers(tref n) {
		return bv_case_split
			? bv_case_split_quantifiers<node_t>(n) : n;
	}

	/**
	 * @brief Substitute existentially quantified bitvector variables that a
	 * total definition in their scope determines and drop their binders;
	 * returns @p n unchanged when `bv_definitional_elimination` is disabled.
	 */
	static tref eliminate_definitional_existentials(tref n) {
		return bv_definitional_elimination
			? bv_eliminate_definitional_existentials<node_t>(n) : n;
	}

	/**
	 * @brief Elaborate bitvector arithmetic atoms to an overflow-free width;
	 * returns @p fm unchanged when `bv_widening` is disabled.
	 */
	static result<tref> widen_arithmetic(tref fm) {
		return bv_widening ? widen_bv_arithmetic<node_t>(fm) : result<tref>{fm};
	}

	/**
	 * @brief Whether `bv_widening` is currently on -- read (not applied) by
	 * callers that must key a cache on it, since it also gates the
	 * construction-time fit-gated folding hooks (bv_ba_hooks.tmpl.h), not
	 * just @ref widen_arithmetic.
	 */
	static bool widening_state() { return bv_widening; }

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

	/// bv's options, each named `bv-<name>`.
	static const option_set& declared_options() { return bv_option_set; }
	/// Binds bv's options, declared by the pack, to bv's fields.
	static result<void> bind_options(options_repository& repo) {
		return bv_bind_options<node_t>(repo);
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
		return !result.found && result.parse_error.at_eof();
	}

	/**
	 * @brief The bitvector of @p ba_type holding @p value, as a bf constant.
	 *
	 * The width comes from the type, so callers name a value and a type and
	 * never a bitwidth. nullptr when @p value does not fit the width.
	 * @pre @p ba_type carries a width, as for literal_one.
	 */
	static tref value_constant(size_t ba_type, size_t value) {
		auto width = get_bv_size<node_t>(get_ba_type_tree<node_t>(ba_type));
		DBG(assert(width.has_value());)
		if (!width.has_value()) return nullptr;
		if (!bitvector_value_fits(width.value(), value)) return nullptr;
		return tau::get(tau::bf, { tau::get_ba_constant(
			make_bitvector_value(width.value(), value),
			ba_type) });
	}

	/**
	 * @brief The width of `bv[n]`: its values are read as unsigned
	 * integers modulo 2^n. 0 while widening is on, since the operators then
	 * compute at a wider width than the type's.
	 */
	static size_t modular_width(size_t ba_type) {
		if (bv_widening) return 0;
		auto width = get_bv_size<node_t>(get_ba_type_tree<node_t>(ba_type));
		// Advisory drop: a type without a width is not read modularly.
		return width.has_value() ? width.value() : 0;
	}

	/** @brief The unsigned integer the bv constant @p c holds. */
	static std::optional<uint64_t> modular_value(size_t ba_type, tref c) {
		const auto& t = tau::get(c);
		const auto& x = t.is(tau::bf) && t.has_child() ? t[0] : t;
		const size_t n = modular_width(ba_type);
		if (!n || n > 64) return std::nullopt;
		if (x.is(tau::bf_f)) return uint64_t{0};
		if (x.is(tau::bf_t)) return n == 64 ? ~uint64_t{0}
			: (uint64_t{1} << n) - 1;
		if (!x.is_ba_constant()) return std::nullopt;
		auto v = x.get_ba_constant();
		if (!std::holds_alternative<bv>(v)) return std::nullopt;
		const bv& b = std::get<bv>(v);
		if (!b.isBitVectorValue()) return std::nullopt;
		const std::string bits = b.getBitVectorValue(2);
		if (bits.size() > 64) return std::nullopt;
		uint64_t out = 0;
		for (char ch : bits) out = out << 1 | (ch == '1' ? 1 : 0);
		return out;
	}

	/**
	 * @brief The all-zeros bitvector of @p ba_type, wrapped as a bf constant.
	 * @pre @p ba_type carries a width, as for literal_one.
	 */
	static tref zero_constant(size_t ba_type) {
		auto width = get_bv_size<node_t>(get_ba_type_tree<node_t>(ba_type));
		DBG(assert(width.has_value());)
		if (!width.has_value()) return nullptr;
		return tau::get(tau::bf, { tau::get_ba_constant(
			make_bitvector_bottom_elem(width.value()),
			ba_type) });
	}

	/** @brief A bitvector witness for @p var, spelled for generated C++. */
	static std::optional<std::string> codegen_witness(tref var, tref conj) {
		return bv_codegen_witness<node_t>(var, conj);
	}

	/** @brief @p cst's own bitvector value, spelled for generated C++. */
	static result<std::optional<std::string>> codegen_constant_expr(tref cst) {
		return bv_codegen_constant_expr<node_t>(cst);
	}
};

} // namespace idni::tau_lang

#include "boolean_algebras/bv/bv_ba_hooks_ext.tmpl.h"
#include "boolean_algebras/bv/bv_codegen.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_DESCRIPTOR_TMPL_H__
