// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file bv_ba.h
 * @brief Bit-vector Boolean Algebra interface for tau-lang using cvc5.
 *
 * This header provides type aliases, helper functions, and advanced methods for
 * manipulating and solving bit-vector formulas using the cvc5 SMT solver within
 * the tau-lang framework.
 *
 * Type Aliases:
 * - bv: Alias for cvc5::Term representing a bit-vector term.
 * - sort: Alias for cvc5::Sort representing a sort/type.
 * - bvs: Alias for std::vector<bv>, a vector of bit-vector terms.
 * - solver: Alias for cvc5::Solver, the SMT solver instance.
 * - term_manager: Alias for cvc5::TermManager, manages terms in cvc5.
 * - solution<node>: Alias for subtree_map<node, tref>, representing a solution mapping.
 *
 * Helper Methods:
 * - get_bv_size: Returns the bit-width of a bit-vector term.
 * - config_cvc5_solver: Configures a cvc5::Solver for bit-vector logic and model production.
 *
 * Advanced Methods:
 * - bv_eval_node: Evaluates a (bv) tau tree to a bit-vector formula.
 * - is_bv_formula_sat: Checks satisfiability of a bit-vector formula.
 * - is_bv_formula_valid: Checks validity of a bit-vector formula.
 * - is_bv_formula_unsat: Checks unsatisfiability of a bit-vector formula.
 * - solve_bv: Attempts to solve a bit-vector formula, returning an optional solution.
 * - splitter: Dummy method included for completeness (variant ba). In the case
 * of atomless BAs, it computes the splitter of a given BA element.
 * - parse_bv: Parses a string into a bit-vector constant with type information.
 *
 * @note Implementation details are provided in "bv_ba.tmpl.h".
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_BA_H__

#include <cvc5/cvc5.h>

#include "backends/cvc5/cvc5.h"
#include "backends/cvc5/cvc5_options.h"
#include "boolean_algebras/ba_pack_traits.h"
#include "tau_tree.h"
#include "splitter_types.h"

namespace idni::tau_lang {

using bv = cvc5::Term;
using sort = cvc5::Sort;
using bvs = std::vector<bv>;
using solver = cvc5::Solver;
using term_manager = cvc5::TermManager;

/** @brief bv carries bitvector arithmetic and its own decision procedure. */
template <>
struct ba_has_arithmetic_theory<bv> : std::true_type {};

template<NodeType node>
using solution = subtree_map<node, tref>;

/**
 * @brief Returns the bit-vector size associated with the given tref.
 *
 * This function template retrieves the size (in bits) of the bit-vector
 * represented by the provided tref object, depending on the specified NodeType.
 *
 * @tparam node The type of node for which the bit-vector size is queried.
 * @param b The tref object representing the bit-vector.
 * @return The size of the bit-vector in bits.
 */
template<NodeType node>
size_t get_bv_size(const tref t);

/**
 * @brief Configures the given cvc5 solver instance for bit-vector logic.
 *
 * Applies the option set selected by `cvc5_options`
 * (backends/cvc5/cvc5_options.h -- the considered sets, the measured
 * matrix and the selection rationale live there), then the base
 * configuration: model production, no proofs, logic "BV". Options must be
 * set before `setLogic`, which is where cvc5 resolves its module defaults.
 *
 * @param solver Reference to a cvc5::Solver instance to be configured.
 * @param decision_only The caller will only read the checkSat verdict --
 * never a model (`getValue`) and never a `simplify` result. This admits
 * satisfiability-preserving-only preprocessing (see
 * `cvc5_option_set::decision_no_models`); passing it from a call site that
 * later extracts a model throws in cvc5, and from a `simplify` site would
 * silently corrupt the rewritten term, so it defaults to false.
 */
inline void config_cvc5_solver(cvc5::Solver& solver, bool decision_only = false) {
	switch (cvc5_options) {
	case cvc5_option_set::baseline: break;
	case cvc5_option_set::decision_no_models: break; // handled below
	case cvc5_option_set::miniscope_agg:
		solver.setOption("miniscope-quant", "agg"); break;
	case cvc5_option_set::ext_rewrite_quant:
		solver.setOption("ext-rewrite-quant", "true"); break;
	case cvc5_option_set::pre_skolem_agg:
		solver.setOption("pre-skolem-quant", "agg"); break;
	case cvc5_option_set::sygus_inst:
		// sygus-inst refuses incremental solving (cvc5's API default);
		// single-checkSat usage makes non-incremental safe, see below
		solver.setOption("incremental", "false");
		solver.setOption("sygus-inst", "true"); break;
	case cvc5_option_set::mbqi:
		solver.setOption("mbqi", "true"); break;
	case cvc5_option_set::enum_inst:
		solver.setOption("enum-inst", "true"); break;
	case cvc5_option_set::cegqi_bv_ineq_keep:
		solver.setOption("cegqi-bv-ineq", "keep"); break;
	case cvc5_option_set::non_incremental:
		// every solver instance here performs exactly ONE checkSat and
		// no push/pop, so cvc5's incremental API default buys nothing
		solver.setOption("incremental", "false"); break;
	case cvc5_option_set::ext_rewrite_no_models:
		solver.setOption("incremental", "false");
		solver.setOption("ext-rewrite-quant", "true"); break;
	case cvc5_option_set::combined_best:
		solver.setOption("incremental", "false");
		solver.setOption("ext-rewrite-quant", "true");
		solver.setOption("cegqi-bv-ineq", "keep"); break;
	}
	const bool drop_models = decision_only
		&& (cvc5_options == cvc5_option_set::decision_no_models
		|| cvc5_options == cvc5_option_set::ext_rewrite_no_models
		|| cvc5_options == cvc5_option_set::combined_best);
	solver.setOption("produce-models", drop_models ? "false" : "true");
	// NOTE: unconstrained-simp looked like the natural companion here but
	// cvc5 rejects it outright in any logic admitting quantifiers ("Cannot
	// use unconstrained simplification in this logic"), and the
	// decision-only path is exactly the quantified one -- so the no-models
	// sets reduce to dropping models and incrementality.
	if (drop_models) solver.setOption("incremental", "false");
	solver.setOption("produce-proofs", "false");
	//solver.setOption("incremental", "true");
	solver.setLogic("BV");
}

/**
 * @brief Let counterexample-guided instantiation consider outer quantifiers.
 *
 * By default cvc5 only instantiates the *innermost* quantified subformula
 * (`cegqi-innermost`), which collapses on interleaved `all`/`ex` over
 * bitvectors: a 4-level alternating chain with a disjunctive body over
 * `bv[8]` takes ~14s, and the `bv[16]`, `bv[32]`, `bv[64]` and 6-level
 * variants do not finish at all. Turning it off decides every one of them
 * in 13-194ms.
 *
 * The cost is confined and small: quantified multiplication is unchanged
 * (1/4/16ms at `bv[8]`/`bv[16]`/`bv[32]`) and quantified division is the
 * worst case at roughly 1.8x (25ms to 44ms at `bv[16]`, 133ms to 237ms at
 * `bv[32]`) -- which is why `bv_formula_sat_status` applies this whenever
 * the quantification actually alternates, rather than trying to also
 * exclude arithmetic.
 *
 * @note `cegqi-nested-qe` looks like the option for this and is what cvc5's
 * own command line derives a fast configuration from, but it does nothing
 * on its own through the C++ API: setting it leaves the search unchanged
 * (~13s) because the CLI's speedup actually comes from the `pre-skolem-quant`
 * / `prenex-quant-user` defaults cvc5 derives *from* it, and deriving those
 * by hand still only reaches ~200ms and does not scale past `bv[8]`.
 *
 * @warning Do not pair quantifier-strategy changes with a resource limit
 * (`rlimit`/`rlimit-per`). A truncated instantiation search can report a
 * plain `sat` instead of `unknown`: `--cegqi-nested-qe --rlimit-per=2000`
 * calls the 4-level `bv[16]` chain **sat** when it is unsat (reproducible;
 * two unbounded strategies agree on unsat). Callers treat a definite answer
 * as truth, so a bounded run here would silently corrupt results.
 *
 * @param solver Reference to a cvc5::Solver instance to be reconfigured.
 */
inline void config_cvc5_solver_alternating_quantifiers(cvc5::Solver& solver) {
	solver.setOption("cegqi-innermost", "false");
}

/**
 * @brief Evaluates a (bv) tau tree to a bitvector formula.
 *
 * This function traverses the given tau tree node and computes its
 * bitvector value based on the provided variable mappings. It supports evaluation
 * with both bound and free variables, and can optionally perform additional checks.
 *
 * @param form Traverser for the boolean algebra tree node to be evaluated.
 * @param vars Mapping from tree nodes to bitvector values for bound variables. Passed
 * by reference for performance; quantifier cases save and restore any shadowed
 * outer binding around their recursive call, so it is left unchanged on return.
 * @param free_vars Mapping from tree nodes to bitvector values for free variables (may be updated).
 * @return An optional with the the evaluated bitvector value of the node if possible and
 * an empty optional if not.
 *
 */
template <NodeType node>
std::optional<bv> bv_eval_node(const typename tree<node>::traverser& form,
	subtree_map<node, bv>& vars, subtree_map<node, bv>& free_vars);

/**
 * @brief Evaluate a `tref` BV formula node; wrapper overload of the traverser version.
 * @param form Root formula node.
 * @param vars Bound variable assignments.
 * @param free_vars Free variable assignments (updated in place).
 * @return Evaluated BV term, or `nullopt` on failure.
 */
template <NodeType node>
std::optional<bv> bv_eval_node(tref form, subtree_map<node, bv>& vars,
	subtree_map<node, bv>& free_vars);

/** @brief Convert a cvc5 term tree @p n back into a Tau tree reference. */
template <NodeType node>
tref cvc5_tree_to_tau_tree (bv n,
	const std::map<std::string, tref>& var_map = {});

/**
 * @brief Tri-state result of deciding a bit-vector formula's satisfiability,
 * distinct from the translation-failure case (see bv_formula_sat_status).
 */
enum class bv_sat_status { sat, unsat, unknown };

/**
 * @brief Decides satisfiability of a bit-vector formula, distinguishing a
 * definite answer from cvc5 giving up (unknown, e.g. on a resource limit)
 * and from translation failure (the formula could not be turned into a
 * cvc5 term at all -- returned as nullopt, never as bv_sat_status::unknown).
 *
 * is_bv_formula_sat below collapses this to a bool for callers that only
 * care about "is it definitely sat", treating unknown and translation
 * failure the same as unsat; callers that would otherwise assert a formula
 * is definitely false based on "not sat" should use this instead and treat
 * unknown/nullopt as "cannot decide", not as unsat.
 *
 * @param form The bit-vector formula to be checked for satisfiability.
 * @return The tri-state result, or nullopt if translation to cvc5 failed.
 */
template <NodeType node>
std::optional<bv_sat_status> bv_formula_sat_status(tref form);

/**
 * @brief Checks if a given bit-vector formula is satisfiable.
 *
 * This function determines whether the provided bit-vector formula,
 * represented by the parameter `form`, has at least one assignment
 * that makes the formula true (i.e., is satisfiable).
 *
 * @param form The bit-vector formula to be checked for satisfiability.
 * @return true if the formula is satisfiable, false otherwise (including
 * when cvc5 returns unknown, or when translation to cvc5 fails).
 */
template <NodeType node>
bool is_bv_formula_sat(tref form);

/**
 * @brief Checks that the formula can be decided by the bitvector solver:
 * every variable must have an explicitly sized bitvector type. Mixed-type
 * formulas (e.g. with sbf or tau variables) cannot be translated to cvc5.
 *
 * @note Also rejects `ref` nodes, formulas whose variables lack an explicit
 * bitwidth, and formulas carrying a non-bv-typed ba_constant (e.g. a `qlt`
 * constant like `{1/3}:qlt`): such a constant can appear in an otherwise
 * bv-only clause once its variable has already been substituted by a
 * concrete value (e.g. during interpretation), so checking only `variable`
 * nodes is not enough to catch the mixed-type case.
 *
 * @param form The formula to check
 * @return true if all variables are explicitly sized bitvectors
 */
template <NodeType node>
bool is_bv_solvable_formula(tref form);

/**
 * @brief Does @p form carry a constant (or typed `T`/`F`) of a Boolean algebra
 * other than the bitvector family?
 *
 * Such content is invisible to `bv_eval_node`, so no bv scope sharing a
 * formula with it can be decided by cvc5 -- callers use this to tell "the
 * solver owns this bv content" apart from "nothing here will ever decide it".
 * Untyped `T`/`F` carries no algebra of its own and does not count.
 *
 * @param form The formula to scan
 * @return true if a foreign-algebra constant occurs in @p form
 */
template <NodeType node>
bool has_foreign_ba_constant(tref form);

/**
 * @brief Does @p form contain the residue of a blasted bitvector predicate?
 *
 * `bit` (bv_predicate_blasting_logic.tmpl.h) extracts bit @e i of an operand as
 * `operand & bit_mask_cte(i)`, a conjunction with a bitvector constant having
 * exactly one bit set. Every blasted comparison and every blasted arithmetic
 * constraint is built out of those bit extractions, so a one-hot masking
 * conjunction is what blasting leaves behind and nothing else in the pipeline
 * produces in bulk.
 *
 * `resolve_quantifiers` uses this to keep its "ask the solver before blasting"
 * rule true across passes: it queries cvc5 for a bv scope because cvc5 handles
 * bitvector arithmetic natively, but `eliminate_bv_and_quantifiers` runs
 * `resolve_quantifiers` three times and is itself re-entered from the
 * interpreter's fixpoint loops, so a later pass can meet a scope an earlier one
 * already blasted. Only its *open*-scope branch screens on this, because only
 * that branch synthesises the universal block which -- wrapped around the
 * auxiliary quantifiers blasting introduced -- gives cvc5's
 * counterexample-guided instantiation the alternation it does not terminate on.
 * See that branch for the measurements and the reproducing spec.
 *
 * A hand-written `x & { 1 }:bv[N]` matches too. That costs nothing beyond the
 * solver shortcut for that one open scope -- blasting, the same fallback taken
 * for any scope the solver cannot own, still applies, and by the caller's own
 * reasoning blasting "neither closes a formula nor makes this check succeed
 * later", so no scope that cvc5 would have decided is lost.
 *
 * @param form The formula to scan
 * @return true if a one-hot bitvector masking conjunction occurs in @p form
 */
template <NodeType node>
bool has_blasting_residue(tref form);

/**
 * @brief Checks whether a given bit-vector formula is valid.
 *
 * This function analyzes the provided formula and determines if it is valid
 * according to the semantics of bit-vector boolean algebra.
 *
 * @param form The formula to be checked for validity.
 * @return true if the formula is valid, false otherwise.
 */
template <NodeType node>
bool is_bv_formula_valid(tref form);

/**
 * @brief Checks whether a given bit-vector formula is unsatisfiable.
 *
 * This function analyzes the provided formula and determines if there is no possible assignment
 * to its variables that would make the formula evaluate to true.
 *
 * @param form The bit-vector formula to be checked for unsatisfiability.
 * @return true if the formula is unsatisfiable; false otherwise.
 */
template <NodeType node>
bool is_bv_formula_unsat(tref form);

// (BA1-16: never-defined solve_bv(tref, cvc5::Solver&) declaration removed.)

/**
 * @brief Solves a boolean algebra problem over bit-vectors.
 *
 * Given a term reference representing a formula, attempts to find a solution
 * that satisfies the formula within the context of bit-vector boolean algebras.
 *
 * @param form The term reference representing the formula to solve.
 * @return An optional solution containing a mapping from nodes to values if a solution exists,
 *         or std::nullopt if no solution is found.
 */
template <NodeType node>
std::optional<solution<node>> solve_bv(tref form);

/**
 * @brief Solves a Boolean algebra problem over bit-vectors.
 *
 * This function attempts to find a solution to the given Boolean formula
 * represented by the parameter `form`, which is expressed in terms of bit-vectors.
 *
 * @param form The Boolean formula to solve, represented as a collection of term references.
 * @return An optional solution containing a mapping of nodes if a solution exists;
 *         std::nullopt otherwise.
 */
template <NodeType node>
std::optional<solution<node>> solve_bv(const trefs& form);

/** @brief Extract a BV constant from a parse-tree node @p parse_tree with type @p type_tree. */
template<typename...BAs>
requires BAsPack<BAs...>
std::optional<bv> bv_constant_from_parse_tree(tref parse_tree, tref type_tree);

/**
 * @brief Parses a bit-vector constant from a string representation.
 *
 * This function attempts to parse the given string `src` as a bit-vector constant,
 * interpreting it according to the specified `size` (number of bits) and `base` (numerical base).
 * If parsing is successful, it returns an optional containing the parsed constant with its type;
 * otherwise, it returns an empty optional.
 *
 * @tparam BAs... Variadic template parameters representing Boolean Algebra types.
 * @param src The string representation of the bit-vector constant to parse.
 * @param type_tree The type of the bit-vector.
 * @param base The numerical base to use for parsing (e.g., 2 for binary, 10 for decimal, 16 for hexadecimal). Defaults to 10.
 * @return std::optional<constant_with_type<BAs...>> The parsed bit-vector constant with type, or std::nullopt if parsing fails.
 */
template<typename...BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type> parse_bv(const std::string& src,
	tref type_tree);

// -----------------------------------------------------------------------------
// Basic Boolean algebra infrastructure
// (BA1-16: four never-defined declarations removed: solve_bv(tref,Solver&),
// canonize_associative_commutative_symbol, is_associative_and_commutative,
// get_inv_sym.)

/** @brief Normalise a BV term via cvc5's simplifier.
 *
 * One long-lived solver plus a result cache: constructing a solver per
 * call pays full engine initialization (theory stack + statistics
 * registry) on its first simplify -- sampled as the dominant cost of a
 * bv[64] interpreter step once the spec grows past a few thousand printed
 * chars, since the constant-folding term hooks route every rebuilt bv
 * operation through here. simplify() adds no assertions, so solver reuse
 * is state-safe, and it is deterministic for a fixed option set (options
 * are fixed before the first query, see cvc5_options.h), so the cache is
 * sound. The solver is deliberately leaked: a static Solver object could
 * destruct after the global cvc5_term_manager it references (see at_exit
 * in main.cpp for the cleanup-order minefield).
 *
 * NOTE (BA1-17): an earlier attempt at a plain static Solver here (owned,
 * not leaked) SIGSEGVed Release LTL execution mid-run (test_ltl_correctness
 * LT2-EXEC-03, test_ltl_qlt_bv); the deliberate leak above avoids the
 * destruction-order interaction. Do not convert it back to an owned
 * static object. */
inline cvc5::Term normalize_bv(const cvc5::Term& fm) {
#ifdef TAU_CACHE
	static std::unordered_map<cvc5::Term, cvc5::Term> cache;
	if (auto it = cache.find(fm); it != cache.end()) return it->second;
#endif // TAU_CACHE
	static cvc5::Solver* solver = [] {
		auto* s = new cvc5::Solver(cvc5_term_manager);
		config_cvc5_solver(*s);
		return s;
	}();
	// Use general simplification procedure
	cvc5::Term res = solver->simplify(fm);
#ifdef TAU_CACHE
	cache.emplace(fm, res);
#endif // TAU_CACHE
	return res;
}

/** @brief Return `true` if @p fm is the all-zeros bitvector constant. */
inline bool is_bv_syntactic_zero(const cvc5::Term& fm) {
	// Check if represented bitvector is just bottom element in Boolean algebra
	if (!fm.isBitVectorValue()) return false;
	return fm.getBitVectorValue(2) ==
		std::string(fm.getSort().getBitVectorSize(), '0');
}

/** @brief Return `true` if @p fm is the all-ones bitvector constant. */
inline bool is_bv_syntactic_one(const cvc5::Term& fm) {
	// Check if represented bitvector is just top element in Boolean algebra
	if (!fm.isBitVectorValue()) return false;
	return fm.getBitVectorValue(2) ==
		std::string(fm.getSort().getBitVectorSize(), '1');
}

} // namespace idni::tau_lang

// Bool comparisons.  Declared in namespace cvc5, not idni::tau_lang, so that
// ADL finds them: `bv` is an alias for cvc5::Term, whose only associated
// namespace is cvc5.  ba_descriptor_complete checks `x == b` from a definition
// context (ba_descriptor.h) that predates this header, so an operator visible
// only to unqualified lookup inside idni::tau_lang would not be found there.
namespace cvc5 {

/** @brief Return `true` if BV term @p lhs equals @p rhs (one/zero). */
inline bool operator==(const Term& lhs, const bool& rhs);
/** @brief Return `true` if @p lhs (bool) equals BV term @p rhs. */
inline bool operator==(const bool& lhs, const Term& rhs);
/** @brief Return `true` if BV term @p lhs is not equal to @p rhs. */
inline bool operator!=(const Term& lhs, const bool& rhs);
/** @brief Return `true` if @p lhs (bool) is not equal to BV term @p rhs. */
inline bool operator!=(const bool& lhs, const Term& rhs);

} // namespace cvc5

namespace idni::tau_lang {

/** @brief Create the type tree for a bitvector of @p bitwidth bits. */
template <NodeType node> tref bv_type(unsigned short bitwidth
	= default_bv_size);
/** @brief Return the type id for a bitvector of @p bitwidth bits. */
template <NodeType node> size_t bv_type_id(unsigned short bitwidth
	= default_bv_size);
/** @brief Return `true` if type tree @p t is a bitvector type. */
template <NodeType node> bool is_bv_type_family(tref t);
/** @brief Return `true` if type id @p ba_type_id is a bitvector type. */
template <NodeType node> bool is_bv_type_family(size_t ba_type_id);
/** @brief Return `true` if node @p t carries a bitvector type. */
template <NodeType node> bool is_tref_bv_type_family(tref t);
/** @brief Return the bitwidth of the bitvector type tree @p t. */
template <NodeType node> size_t get_bv_width(tref t);
/** @brief Return the bitwidth encoded in type id @p ba_type_id. */
template <NodeType node> size_t get_bv_width(size_t ba_type_id);

// Bitvector specific symbol simplification
/** @brief Simplify an `add` bitvector symbol node @p symbol. */
template<NodeType node> tref term_add(tref symbol);
/** @brief Simplify a `sub` bitvector symbol node @p symbol. */
template<NodeType node> tref term_sub(tref symbol);
/** @brief Simplify a `mul` bitvector symbol node @p symbol. */
template<NodeType node> tref term_mul(tref symbol);
/** @brief Simplify a `div` bitvector symbol node @p symbol. */
template<NodeType node> tref term_div(tref symbol);
/** @brief Simplify a `mod` bitvector symbol node @p symbol. */
template<NodeType node> tref term_mod(tref symbol);
/** @brief Simplify a `shr` bitvector symbol node @p symbol. */
template<NodeType node> tref term_shr(tref symbol);
/** @brief Simplify a `shl` bitvector symbol node @p symbol. */
template<NodeType node> tref term_shl(tref symbol);
/** @brief Simplify a `nor` bitvector symbol node @p symbol. */
template<NodeType node> tref term_nor(tref symbol);
/** @brief Simplify an `xnor` bitvector symbol node @p symbol. */
template<NodeType node> tref term_xnor(tref symbol);
/** @brief Simplify a `nand` bitvector symbol node @p symbol. */
template<NodeType node> tref term_nand(tref symbol);
/** @brief Cast the constant in `cast` node @p symbol to @p target_type_id. */
template<NodeType node> tref term_cast(tref symbol, size_t target_type_id);

// Constant comparison folds; nullptr when an operand is not a bv constant.
/** @brief Fold `<` over the constants in @p ch. */
template<NodeType node> tref wff_bv_lt(const tref* ch, tref r);
/** @brief Fold `!<` over the constants in @p ch. */
template<NodeType node> tref wff_bv_nlt(const tref* ch, tref r);
/** @brief Fold `<=` over the constants in @p ch. */
template<NodeType node> tref wff_bv_lteq(const tref* ch, tref r);
/** @brief Fold `!<=` over the constants in @p ch. */
template<NodeType node> tref wff_bv_nlteq(const tref* ch, tref r);
/** @brief Fold `>` over the constants in @p ch. */
template<NodeType node> tref wff_bv_gt(const tref* ch, tref r);
/** @brief Fold `!>` over the constants in @p ch. */
template<NodeType node> tref wff_bv_ngt(const tref* ch, tref r);
/** @brief Fold `>=` over the constants in @p ch. */
template<NodeType node> tref wff_bv_gteq(const tref* ch, tref r);
/** @brief Fold `!>=` over the constants in @p ch. */
template<NodeType node> tref wff_bv_ngteq(const tref* ch, tref r);

/** @brief Apply all BV symbol-level simplifications to @p symbol. */
template<NodeType node> tref simplify_bv_symbol(tref symbol);

/** @brief Apply all BV term-level simplifications to @p term. */
template<NodeType node> tref simplify_bv_term(tref term);

// (BA1-16: three never-defined declarations removed here:
// canonize_associative_commutative_symbol, is_associative_and_commutative,
// get_inv_sym.)


// -----------------------------------------------------------------------------

} // namespace idni::tau_lang

// This is the proper way to include heuristics as the header must be independent
// of the heuristics themselves and also they could need definitions from the
// header (as is the case in 'boolean_algebras/bv/heuristics/bv_ba_simplification.h'. Also, they
// need to be included before the definitions as they can be used in there.
#include "boolean_algebras/bv/heuristics/bv_ba_simplification.h"
#include "boolean_algebras/bv/bv_types.tmpl.h"
#include "boolean_algebras/bv/bv_ba.tmpl.h"
#include "boolean_algebras/bv/bv_ba_solver.tmpl.h"
#include "boolean_algebras/bv/bv_ba_helpers.tmpl.h"
#include "boolean_algebras/bv/bv_ba_hooks.tmpl.h"
#include "boolean_algebras/bv/heuristics/bv_predicate_blasting.h"
#include "boolean_algebras/bv/bv_descriptor.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_BA_H__
