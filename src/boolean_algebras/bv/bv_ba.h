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
 * - get_bv_size: Returns the bit-width of a bit-vector type.
 * - config_cvc5_solver: Configures a cvc5::Solver for bit-vector logic and model production.
 *
 * Advanced Methods:
 * - bv_eval_node: Translates a (bv) tau tree to a cvc5 term.
 * - bv_formula_sat_status: Decides satisfiability, keeping unknown apart.
 * - is_bv_formula_sat / is_bv_formula_unsat / is_bv_formula_valid: bool
 *   collapses of bv_formula_sat_status (see each for how unknown answers).
 * - solve_bv: Attempts to solve a bit-vector formula, returning an optional solution.
 * - parse_bv: Parses a string into a bit-vector constant with type information.
 *
 * @note Definitions live in bv_ba.tmpl.h, bv_types.tmpl.h,
 * bv_ba_solver.tmpl.h and bv_ba_hooks.tmpl.h.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_BA_H__

#include <chrono>
#include <cstdlib>
#include <unordered_set>
#include <cvc5/cvc5.h>

#include "backends/cvc5/cvc5.h"
#include "env_limits.h"
#include "backends/cvc5/cvc5_options.h"
#include "backends/cvc5/cvc5_bitblast.h"
#include "bounded_call.h"
#include "boolean_algebras/ba_pack_traits.h"
#include "tau_tree.h"
#include "tau_diagnostics.h"
#include "splitter_types.h"

namespace idni::tau_lang {

using bv = cvc5::Term;
/// A cvc5 sort.
using sort = cvc5::Sort;
/// A list of bv terms.
using bvs = std::vector<bv>;
/// The cvc5 solver.
using solver = cvc5::Solver;
/// The cvc5 term manager.
using term_manager = cvc5::TermManager;

/// A satisfying assignment: each free variable (as a `bf` node) to its value.
template<NodeType node>
using solution = subtree_map<node, tref>;

/**
 * @brief Returns the bit width of a bitvector type tree; same as get_bv_width.
 *
 * @param t A bitvector type tree (`typed > type > subtype > num`).
 * @return The width in bits, or an error when the type has no explicit
 * width or the width is outside 1..65535.
 */
template<NodeType node>
result<size_t> get_bv_size(const tref t);

/// Opt-in: decide a closed bitvector formula whose binders are all of one
/// kind quantifier-free (see `bv_formula_sat_status`). A formula with only
/// existential binders in positive polarity is satisfiable exactly when its
/// matrix is (`sat(ex x phi) == sat(phi)` with `x` free); one with only
/// universal binders is satisfiable exactly when the negated matrix is not
/// (`sat(all x phi) == !sat(!phi)`). Such a formula is then handed to cvc5 in
/// `QF_BV` with eager bitblasting instead of the quantified `BV` logic.
/// Off by default; enabled via `--bv-quantifier-free-decision`, the REPL
/// option `bv-quantifier-free-decision`, or the environment variable
/// TAU_BV_QF_DECISION (a value of "0" disables).
inline bool bv_quantifier_free_decision = false;

/**
 * @brief Whether the quantifier-free decision of `bv_formula_sat_status` is on.
 * @return `bv_quantifier_free_decision`, or true when TAU_BV_QF_DECISION is
 * set to a non-empty value other than "0" (read once per process).
 */
inline bool bv_quantifier_free_decision_enabled() {
	static const bool env = [] {
		const char* v = std::getenv("TAU_BV_QF_DECISION");
		return v && *v && !(v[0] == '0' && v[1] == '\0');
	}();
	return bv_quantifier_free_decision || env;
}

/// Budget of `bv_formula_sat_status`'s BDD decision (cvc5_bitblast_sat):
/// the nodes it may keep in use at once before it leaves the formula to
/// cvc5; default 2^20; 0 leaves every formula to cvc5. The option
/// `bv-bitblast-max-nodes`.
/// Environment fallback `TAU_BV_BITBLAST_MAX_NODES`.
inline env_limit<size_t> bv_bitblast_max_nodes{ "TAU_BV_BITBLAST_MAX_NODES",
	size_t{1} << 20 };

/// Widest bit-vector the BDD decision takes; a formula with a wider one goes
/// to cvc5. Default 16; 0 sends every formula there. A product of two wider
/// values outgrows any useful budget. The option `bv-bitblast-max-width`,
/// environment fallback `TAU_BV_BITBLAST_MAX_WIDTH`.
inline env_limit<size_t> bv_bitblast_max_width{ "TAU_BV_BITBLAST_MAX_WIDTH",
	16 };

/// Wall-clock budget, in seconds, of one quantified cvc5 decision of
/// `bv_formula_sat_status` or `solve_bv`: the query runs in a child process killed at the
/// bound (bounded_call.h), and a query killed there has no answer, which
/// makes the command asking it UNKNOWN. Default 60; 0 runs every query in the
/// process, unbounded. The option `bv-solve-timeout`, whose setter clears
/// the caches of decided verdicts when the value changes.
/// Environment fallback `TAU_BV_SOLVE_TIMEOUT`.
inline env_limit<size_t> bv_solve_timeout{ "TAU_BV_SOLVE_TIMEOUT", 60 };

/**
 * @brief Configure a solver for a quantifier-free decision-only query.
 *
 * `QF_BV` with eager bitblasting: the whole formula goes to the SAT solver
 * at once, which is what a closed, binder-free bitvector query wants. Models
 * and proofs are never read by the callers of `bv_formula_sat_status`, and
 * every instance performs exactly one checkSat, so incrementality is off as
 * well. Only reachable through `bv_quantifier_free_decision`.
 *
 * @param solver A fresh solver, before its logic is set.
 */
inline void config_cvc5_solver_quantifier_free(cvc5::Solver& solver) {
	solver.setOption("incremental", "false");
	solver.setOption("produce-models", "false");
	solver.setOption("produce-proofs", "false");
	solver.setOption("bitblast", "eager");
	solver.setLogic("QF_BV");
}

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
 * @return The evaluated bitvector value; a value-less, error-less result
 * when the node is not bv-translatable (an ordinary decline), or a report
 * on a genuine internal failure.
 *
 */
template <NodeType node>
result<bv> bv_eval_node(const typename tree<node>::traverser& form,
	subtree_map<node, bv>& vars, subtree_map<node, bv>& free_vars);

/**
 * @brief bv_eval_node's memo: tref -> (context id -> already-translated
 * term). See the memo-taking overload's doc for what a context id
 * identifies.
 */
template <NodeType node>
using bv_eval_memo = subtree_unordered_map<node, std::unordered_map<size_t, bv>>;

/**
 * @brief Memoised worker behind the overload above; caches tref -> term
 * keyed by (tref, ctx). @p ctx identifies the enclosing wff_all/wff_ex
 * instance (0 at top level, else a fresh value from @p ctx_counter minted
 * per quantifier entry) so memo entries don't leak across quantifier scopes.
 *
 * @param form Node to translate.
 * @param vars Bound variable terms, as in the overload above.
 * @param free_vars Free variable terms; a new free variable is added.
 * @param memo Translation cache shared by one top-level call.
 * @param ctx_counter Last context id handed out; incremented per binder.
 * @param ctx Context id of the enclosing binder instance.
 * @return As the overload above.
 */
template <NodeType node>
result<bv> bv_eval_node(const typename tree<node>::traverser& form,
	subtree_map<node, bv>& vars, subtree_map<node, bv>& free_vars,
	bv_eval_memo<node>& memo, size_t& ctx_counter, size_t ctx);

/**
 * @brief Evaluate a `tref` BV formula node; wrapper overload of the traverser version.
 * @param form Root formula node.
 * @param vars Bound variable assignments.
 * @param free_vars Free variable assignments (updated in place).
 * @return Evaluated BV term; value-less and error-less on an ordinary
 * decline, or a report on a genuine internal failure.
 */
template <NodeType node>
result<bv> bv_eval_node(tref form, subtree_map<node, bv>& vars,
	subtree_map<node, bv>& free_vars);

/**
 * @brief Convert a cvc5 term back into a Tau tree.
 *
 * Handles the Boolean connectives, quantifiers over one variable, the
 * unsigned comparisons, the bitvector operations of the bv BA, zero-extend,
 * extract and concat (as casts and shifts), negation (as `~x + 1`) and the
 * min/max ITE shapes make_bitvector_min/max build.
 *
 * @param n The cvc5 term.
 * @param var_map cvc5 variable name (without `|` quotes) to the Tau variable
 * it stands for; a variable not in it becomes a new bf variable of that name.
 * @return The Tau tree, or nullptr when some subterm has no Tau counterpart.
 */
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
 * Obeys `bv_quantifier_free_decision`, `bv_bitblast_max_nodes` and
 * `bv_solve_timeout`; a query killed at the timeout answers unknown.
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
 * @brief Why `is_bv_solvable_formula` declined a formula, or `ok` if it
 * didn't. Lets callers that can act differently on different rejections
 * (see the reason-aware overload below) tell them apart instead of getting
 * a single collapsed `false`.
 */
enum class bv_unsolvable_reason {
	ok,                 // solvable (or n/a: the bool-only overload doesn't ask)
	has_unresolved_ref,  // formula still contains an unresolved wff_ref/bf_ref
	non_bv_variable,     // a variable's ba_type is outside the bv family
	missing_bitwidth,    // bv-typed variable with no explicit bitwidth subtype
	no_bv_content,        // solvable checks all pass vacuously, but no bv content at all
};

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
 * @param reason Set to why the formula was rejected (`ok` if it wasn't).
 * @return true if all variables are explicitly sized bitvectors
 */
template <NodeType node>
bool is_bv_solvable_formula(tref form, bv_unsolvable_reason& reason);

/** @copydoc is_bv_solvable_formula(tref,bv_unsolvable_reason&)
 * Adapter for callers that only need the yes/no answer. */
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
 * bitvector arithmetic natively, but `eliminate_arithmetic_and_quantifiers` runs
 * `resolve_quantifiers` three times and is itself re-entered from the
 * interpreter's fixpoint loops, so a later pass can meet a scope an earlier one
 * already blasted. Both of its solver branches screen on this, through
 * the descriptor's `has_preprocessing_residue`: the open-scope branch because
 * the universal block it synthesises around the auxiliary quantifiers
 * blasting introduced gives cvc5's counterexample-guided instantiation the
 * alternation it does not terminate on, and the closed-scope branch because
 * the scope may already carry such an alternation from a prior pass. See the
 * open-scope branch for the measurements and the reproducing spec.
 *
 * A hand-written `x & { 1 }:bv[N]` matches too. That costs nothing beyond the
 * solver shortcut for that one scope -- blasting, the same fallback taken
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
 * @brief Checks whether a given bit-vector formula is valid, as
 * `is_bv_formula_unsat` of its negation.
 *
 * A false return is not a definite answer: when cvc5 answers unknown for
 * the negation (a timeout included), or the negation cannot be translated,
 * this returns false as well. A caller that must tell "not valid" from
 * "cannot decide" uses `bv_formula_sat_status` on the negation instead.
 *
 * @param form The formula to be checked for validity.
 * @return true iff the negation of @p form is definitely unsatisfiable.
 */
template <NodeType node>
bool is_bv_formula_valid(tref form);

/**
 * @brief Checks whether a given bit-vector formula is unsatisfiable.
 *
 * A false return is not a definite answer: a cvc5 unknown (a timeout
 * included) and a translation failure return false as well. A caller that
 * must tell "satisfiable" from "cannot decide" uses `bv_formula_sat_status`
 * instead.
 *
 * @param form The bit-vector formula to be checked for unsatisfiability.
 * @return true iff @p form is definitely unsatisfiable.
 */
template <NodeType node>
bool is_bv_formula_unsat(tref form);

/**
 * @brief Solves a boolean algebra problem over bit-vectors.
 *
 * Given a term reference representing a formula, attempts to find a solution
 * that satisfies the formula within the context of bit-vector boolean algebras.
 *
 * Runs one cvc5 query with models on a fresh solver. A question
 * `bv_formula_sat_status` would bound by `bv_solve_timeout` is first decided
 * in a child process under that budget, and solved for its model only when
 * the child answers sat. Once a time budget of the unit of work ran out
 * (`time_budget_exhausted`), nothing is solved.
 *
 * @param form The term reference representing the formula to solve.
 * @return The value of every free variable when cvc5 answers sat; a value-less
 * optional when it answers unsat or unknown, when the budget runs out, or when
 * @p form cannot be translated; the error of the translation when it fails.
 */
template <NodeType node>
result<std::optional<solution<node>>> solve_bv(tref form);

/**
 * @brief Solves a Boolean algebra problem over bit-vectors.
 *
 * This function attempts to find a solution to the given Boolean formula
 * represented by the parameter `form`, which is expressed in terms of bit-vectors.
 *
 * @param form The literals of the formula, solved as their conjunction.
 * @return As solve_bv(tref) on that conjunction.
 */
template <NodeType node>
result<std::optional<solution<node>>> solve_bv(const trefs& form);

/**
 * @brief Build a bv constant from a bitvector-grammar parse tree.
 * @param parse_tree The parse tree of a decimal, binary or hexadecimal
 * literal; may be null.
 * @param type_tree The bitvector type giving the width.
 * @return The constant; no value and no error when @p parse_tree is null or
 * not a literal; an error when the width is missing or cvc5 rejects the
 * literal (for example a value wider than the type).
 */
template<typename...BAs>
requires BAsPack<BAs...>
result<bv> bv_constant_from_parse_tree(tref parse_tree, tref type_tree);

/**
 * @brief Parses a bit-vector constant from a string representation.
 *
 * The base comes from the literal itself (bitvector grammar: decimal,
 * `#b`/`#x`); a leading `0b`/`0x` is accepted as `#b`/`#x`. The width
 * comes from @p type_tree.
 *
 * @tparam BAs The Boolean algebras of the node pack.
 * @param src The string representation of the bit-vector constant to parse.
 * @param type_tree The bitvector type of the constant.
 * @return The constant with @p type_tree, or a parse error.
 */
template<typename...BAs>
requires BAsPack<BAs...>
result<typename node<BAs...>::constant_with_type> parse_bv(const std::string& src,
	tref type_tree);

// -----------------------------------------------------------------------------
// Basic Boolean algebra infrastructure

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
 * sound (the cache exists only with TAU_CACHE). The solver is deliberately
 * leaked: do not make it an owned static object, which could be destroyed
 * after the global cvc5_term_manager it references and crashes at exit.
 *
 * @param fm A bv term.
 * @return The simplified term, equivalent to @p fm. */
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

/** @brief `true` if BV term @p lhs simplifies to all ones (@p rhs true) or
 *  to all zeros (@p rhs false). */
inline bool operator==(const Term& lhs, const bool& rhs);
/** @brief Same as `rhs == lhs`. */
inline bool operator==(const bool& lhs, const Term& rhs);
/** @brief Negation of `lhs == rhs`. */
inline bool operator!=(const Term& lhs, const bool& rhs);
/** @brief Negation of `rhs == lhs`. */
inline bool operator!=(const bool& lhs, const Term& rhs);

} // namespace cvc5

namespace idni::tau_lang {

/** @brief Create the type tree for a bitvector of @p bitwidth bits. */
template <NodeType node> tref bv_type(size_t bitwidth);
/** @brief Return the type id for a bitvector of @p bitwidth bits. */
template <NodeType node> size_t bv_type_id(size_t bitwidth);
/** @brief Return `true` if type tree @p t is a bitvector type. */
template <NodeType node> bool is_bv_type_family(tref t);
/** @brief Return `true` if type id @p ba_type_id is a bitvector type. */
template <NodeType node> bool is_bv_type_family(size_t ba_type_id);
/** @brief Return `true` if node @p t carries a bitvector type. */
template <NodeType node> bool is_tref_bv_type_family(tref t);
/** @brief Checked bitwidth lookup: a type without an explicit bitwidth, or
 *  a width outside 1..65535, is an error. */
template <NodeType node> result<size_t> get_bv_width(tref t);
/** @brief Checked bitwidth lookup by type id; an unknown id is an error too. */
template <NodeType node> result<size_t> get_bv_width(size_t ba_type_id);

// Bitvector specific symbol simplification
// term_add, term_sub, term_mul and term_shl fold a constant pair at the
// operands' width; under the opt-in `bv_widening` mode (heuristics/bv_widening.h)
// they decline the fold -- leaving the node symbolic -- whenever the exact
// result would not fit, so the later widening pass can compute it at a wider
// width instead of wrapping it here. div, mod and shr cannot overflow and
// always fold. Each returns the folded node, or @p symbol unchanged when
// there is nothing to fold or a constant's width cannot be read.
/** @brief Simplify an `add` bitvector symbol node @p symbol (fit-gated under `bv_widening`). */
template<NodeType node> tref term_add(tref symbol);
/** @brief Simplify a `sub` bitvector symbol node @p symbol (fit-gated under `bv_widening`). */
template<NodeType node> tref term_sub(tref symbol);
/** @brief Simplify a `mul` bitvector symbol node @p symbol (fit-gated under `bv_widening`). */
template<NodeType node> tref term_mul(tref symbol);
/** @brief Simplify a `div` bitvector symbol node @p symbol. */
template<NodeType node> tref term_div(tref symbol);
/** @brief Simplify a `mod` bitvector symbol node @p symbol. */
template<NodeType node> tref term_mod(tref symbol);
/** @brief Simplify a `shr` bitvector symbol node @p symbol. */
template<NodeType node> tref term_shr(tref symbol);
/** @brief Simplify a `shl` bitvector symbol node @p symbol (fit-gated under `bv_widening`). */
template<NodeType node> tref term_shl(tref symbol);
/** @brief Simplify a `nor` bitvector symbol node @p symbol. */
template<NodeType node> tref term_nor(tref symbol);
/** @brief Simplify an `xnor` bitvector symbol node @p symbol. */
template<NodeType node> tref term_xnor(tref symbol);
/** @brief Simplify a `nand` bitvector symbol node @p symbol. */
template<NodeType node> tref term_nand(tref symbol);
/** @brief Cast the constant in `cast` node @p symbol to @p target_type_id. */
template<NodeType node> tref term_cast(tref symbol, size_t target_type_id);

// Constant comparison folds (unsigned). @p ch is the children array of the
// `wff` being interned, @p r its right sibling, kept on the result. Each
// returns T or F, or nullptr when the operands are not bv constants of the
// same type.
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
/** @brief Simplify a `min` bitvector symbol node @p symbol (unsigned). */
template<NodeType node> tref term_min(tref symbol);
/** @brief Simplify a `max` bitvector symbol node @p symbol (unsigned). */
template<NodeType node> tref term_max(tref symbol);

/** @brief Dispatch @p symbol to the term_* fold of its operator; @p symbol
 *  unchanged for any other operator. */
template<NodeType node> tref simplify_bv_symbol(tref symbol);

/** @brief Simplify the bv term @p term with bv_ba_cvc5_simplification, or
 *  with bv_ba_custom_simplification when that declines.
 *  @return The simplified term, with a warning when the custom pass hit
 *  `max_simplify_rounds`; an error when either pass fails. */
template<NodeType node> result<tref> simplify_bv_term(tref term);



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
#include "boolean_algebras/bv/heuristics/bv_case_split.h"
#include "boolean_algebras/bv/heuristics/bv_definitional_elimination.h"
#include "boolean_algebras/bv/heuristics/bv_widening.h"
#include "boolean_algebras/bv/bv_descriptor.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BV__BV_BA_H__
