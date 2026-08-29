// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

/**
 * @file api.h
 * @brief Public C++ API for Tau: parsing, normalisation, satisfiability, solving, and execution.
 *
 * All operations are exposed as static methods on `api<node>`. Callers typically
 * create a type alias `using tau_api = api<node<BAs...>>` and call through it.
 * Every operation returns a `result<T>`: a value on success, or a structured
 * error/diagnostics report on failure (see `tau_diagnostics.h`).
 *
 * Threading contract: the API mutates shared parser settings, pretty-printer
 * flags, definition registries, and other caches without synchronization.
 * Treat the public API, interpreter, and REPL integration as single-threaded
 * unless callers provide their own external serialization.
 *
 * **Status:** Highly unstable — contains bugs, subject to change.
 */

// TODO (LOW) add non string api for execution?
// TODO (HIGH) tests for the htref surface (the tref/string surfaces are
//             covered by tests/api)
// TODO (HIGH) decide which parsing get_* methods or other methods will go private if any
// TODO (MEDIUM) parsing with `bool simplify = true` argument

#ifndef __IDNI__TAU__API_H__
#define __IDNI__TAU__API_H__

#include "heuristics/simplify_options.h"
#include "interpreter.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

/**
 * @brief Identifies a named I/O stream at a specific discrete time point.
 */
struct stream_at {
	std::string name;    ///< Stream identifier.
	size_t time_point;   ///< Discrete time index.

	auto operator<=>(const stream_at& other) const = default;
};

/**
 * @brief I/O stream remapping options passed to `api::get_interpreter`.
 */
struct interpreter_options {
	input_streams_remap  input_remaps;  ///< Redirect named input streams.
	output_streams_remap output_remaps; ///< Redirect named output streams.
};

/**
 * @brief One-time setup for @p node: the BDD backend and the grammar's
 * dynamic type names. Call once before the first parse, the way callers
 * already call `bdd_init<Bool>()` -- this replaces that call.
 */
template <NodeType node>
void tau_init();

/// Main public API for the Tau language engine.
///
/// All operations are exposed as static methods on a class template
/// parameterized by the node type.  This allows callers to alias the API
/// with their chosen Boolean-algebra pack:
/// @code
///   using tau_api = api<node<BAs...>>;
///   auto interp = tau_api::get_interpreter("G (o1[t] = 0).");
/// @endcode
///
/// The API is organized into the following groups:
///   - **Settings** — configure parser/printer behavior (charvar, indenting, …)
///   - **Parsing** — convert strings into internal tree representations (tref / htref)
///   - **Querying** — inspect parsed trees (is_term, is_formula, contains)
///   - **Definitions** — register and apply recursive definitions
///   - **Printing** — serialize trees back to strings
///   - **Substitution** — replace sub-expressions
///   - **Normal forms** — DNF, CNF, NNF, Boole normal form
///   - **Procedures** — simplification, normalization, quantifier elimination
///   - **Realizability** — LTL realizability and satisfiability checks
///   - **Solving** — find variable assignments satisfying a formula
///   - **Execution** — construct an interpreter and step through it
///
/// Every group provides three overload families:
///   1. **string** — accepts/returns `std::string` (convenience layer)
///   2. **tref** — operates on raw tree pointers (zero-copy, not GC-safe)
///   3. **htref** — operates on shared-pointer handles (GC-safe)
///
/// Every method returns a `result<T>` carrying either the value or a
/// structured diagnostics report (see `tau_diagnostics.h`).
template <NodeType node>
struct api {
	using tau = tree<node>;
	using severity_level = boost::log::trivial::severity_level;

	friend tau;

	// -----------------------------------------------------------------------
	// Global options
	// -----------------------------------------------------------------------
	/// Switch between single-char variable names ("charvar" mode, e.g. x, y)
	/// and multi-char variable names ("var" mode, e.g. foo, bar).
	/// Affects both the tau parser and the SBF parser.
	static void set_charvar(bool state);
	/**
	 * @brief Enable/disable the core master preprocessing switch
	 * (`preprocessing` in heuristics/preprocess_placement.h).
	 *
	 * This is the pipeline-scheduling gate core itself reads; a BA's own
	 * preprocessing pass (bv predicate blasting today) also needs its own
	 * per-BA switch on -- see `bv-blasting` (bv_descriptor.tmpl.h's
	 * options()) -- so flipping this alone does not by itself turn a BA's
	 * pass on if that BA's own switch is off. To set every BA's own switch
	 * instead, without touching this master, use `pack_set_preprocessing`
	 * (ba_pack_traits.h).
	 */
	static void set_preprocessing(bool state);
	/**
	 * @brief Select where preprocessing may run (see `preprocess_site`).
	 *
	 * 0 = `per_leaf` (default, today's behaviour), 1 = `per_block`,
	 * 2 = `per_formula`. An out-of-range value clamps to the default.
	 */
	static void set_preprocess_placement(int site);
	/**
	 * @brief Select what happens to a preprocessed formula (see `preprocess_mode`).
	 *
	 * 0 = `anti_prenex_result` (default, today's behaviour: re-enter
	 * `anti_prenex` on the preprocessed formula), 1 = `defer` (rewrite only
	 * and leave the introduced quantifiers to the next resolve pass). An
	 * out-of-range value clamps to the default.
	 */
	static void set_preprocess_method(int mode);
	/**
	 * @brief Select where the cvc5 solver may be queried (see `solver_site`).
	 *
	 * 0 = `eager` (default, today's behaviour), 1 = `per_closed_block`,
	 * 2 = `per_formula`. An out-of-range value clamps to the default. The
	 * final closed-formula check of `eliminate_arithmetic_and_quantifiers` runs
	 * under every setting -- it is the "final" site the other two rely on.
	 */
	static void set_solver_placement(int site);
	/**
	 * @brief Select the cvc5 option set (see `cvc5_option_set`).
	 *
	 * Values follow the `cvc5_option_set` enumerators; an out-of-range
	 * value clamps to the default. Must be set before the first solver
	 * query: `bv_formula_sat_status` memoizes verdicts keyed on the
	 * formula alone, so a mid-process flip would serve answers computed
	 * under the previous option set.
	 */
	static void set_cvc5_options(int set);
	/**
	 * @brief Set the per-block Boole-decomposition split budget.
	 *
	 * Bounds the Shannon splits ONE block elimination may charge, across its
	 * whole recursion rather than along one path. Exhausting it costs
	 * precision, not soundness: the elimination takes its graceful re-wrap
	 * path and leaves the quantifier in place. 0 = unlimited (the default).
	 */
	static void set_block_max_splits(size_t n);
	/**
	 * @brief Set the anti-prenex driver's maximum round count.
	 *
	 * Bounds how many times the driver re-collects innermost blocks before
	 * giving up and returning the formula unprocessed, with a log line.
	 * 0 = unlimited (the default); convergence is normally reached in well
	 * under 20 rounds.
	 */
	static void set_block_max_rounds(size_t n);
	/**
	 * @brief Cap the DNF clauses `complete_quantifier_elimination` may
	 * distribute one quantifier scope into (estimated as the product of
	 * the scope's CNF factors' disjunct counts). Above the cap the
	 * quantifier is kept verbatim, with a log line. 0 = unlimited (the
	 * default).
	 */
	static void set_cqe_max_clauses(size_t n);
	/**
	 * @brief Cap `blast_block`'s blast-then-re-enter nesting in
	 * anti-prenexing; 0 = unlimited (default). Real formulas use one level.
	 */
	static void set_max_blast_reentry_depth(size_t n);
	/**
	 * @brief Operand-set size above which block squeezing declines and the
	 * general Boole decomposition runs instead; 0 = unlimited (default:
	 * always squeeze).
	 */
	static void set_block_squeeze_cap(size_t n);
	/**
	 * @brief Cap the temporal-normalization fixpoint searches
	 * (`find_fixpoint_phi`/`find_fixpoint_chi`); 0 = unlimited (default).
	 *
	 * On overrun the search gives up with a log line and returns the
	 * current (non-fixpoint) iterate.
	 */
	static void set_max_fixpoint_steps(size_t n);
	/**
	 * @brief Cap the eventual-flag search past the flag boundary in
	 * `to_unbounded_continuation`; 0 = unlimited (default).
	 *
	 * A bounded give-up reports unsatisfiable — wrong but bounded and
	 * loud (SO-1).
	 */
	static void set_max_flag_search_steps(size_t n);
	/**
	 * @brief Cap definition-expansion passes in
	 * `expand_defs_until_settled`; 0 = unlimited (default). On overrun the
	 * expansion reports failure (`nullptr`) with a log line.
	 */
	static void set_max_def_passes(size_t n);
	/**
	 * @brief Cap recurrence-relation enumeration steps in
	 * `calculate_fixed_point`; 0 = unlimited (default). A bounded give-up
	 * is a bound on the search, not a proof that no fixed point exists.
	 */
	static void set_max_enum_steps(size_t n);
	/**
	 * @brief Cap `repeat_all`'s rewrite-to-fixpoint rounds; 0 = unlimited
	 * (default). Oscillation is detected regardless; this bounds only
	 * ever-growing rewrites.
	 */
	static void set_max_rewrite_rounds(size_t n);
	/**
	 * @brief Cap `bv_ba_custom_simplification` rewrite rounds; 0 =
	 * unlimited (default). Oscillation is detected regardless.
	 */
	static void set_max_simplify_rounds(size_t n);
	/**
	 * @brief Tree-node count floor before the interpreter's gc may
	 * trigger. Default 256 (kept — a tuned value, not a cap).
	 */
	static void set_gc_min_size(size_t n);
	/**
	 * @brief Growth factor of the interpreter's adaptive gc trigger; a
	 * sweep fires when the node count grew by this factor since the last
	 * sweep. Default 1.5 (kept); <= 0 disables gc.
	 */
	static void set_gc_growth_factor(double f);
	/**
	 * @brief Warn when an updated specification exceeds this many printed
	 * characters (the I7 size guard); 0 = off (default).
	 */
	static void set_spec_size_warn(size_t n);
	/**
	 * @brief Cap the revision alternatives kept per specification part,
	 * dropping middle preference tiers with a warning; 0 = unlimited
	 * (default).
	 */
	static void set_max_revision_alts(size_t n);
	/**
	 * @brief Cap the ∀∃-synthesis checks the k-ary consistency-subset
	 * walk may spend per atom group (LT-17); on the cap the remaining
	 * subsets are skipped with a warning (sound, at worst a false
	 * UNREALIZABLE). Default 4096; 0 = unlimited.
	 */
	static void set_max_consistency_subsets(size_t n);
	/**
	 * @brief Bound the string-keyed synthesis caches (FIFO eviction,
	 * LG-27); default 4096 entries, 0 = unbounded.
	 */
	static void set_cache_bound(size_t n);
	/**
	 * @brief Cap the ABA oracle's mixed-type coverage expansion (§13);
	 * beyond it the weaker syntactic verdict stands, logged. Default
	 * 256; 0 = unlimited.
	 */
	static void set_max_cover_products(size_t n);
	/**
	 * @brief Enable the semantic (winning-region) fallback of the temporal
	 * pointwise revision; OFF by default (see `pwr_semantic_fallback`).
	 */
	static void set_pwr_semantic_fallback(bool on);
	/// Enable or disable indented pretty-printing of tree output.
	static void set_indenting(bool state);
	/// Enable or disable support-component factoring of the Tau-BA
	/// constant/valid tests (tau_ba.tmpl.h). Off by default.
	static void set_ba_component_factoring(bool state);
	/// Enable or disable ANSI color highlighting in pretty-printed output.
	static void set_highlighting(bool state);
	/// Enable or disable JSON output format for printing.
	static void set_json(bool state);
	/// Set the Boost.Log severity threshold. Messages below this level
	/// are suppressed.
	static void set_severity(severity_level level);

	// -----------------------------------------------------------------------
	// Parsing
	// ------------------------------------------------------------

	/// Parse a Boolean function (bf) term from a string.
	/// @param term   Source string in Tau syntax (e.g. "x & y").
	/// @param simplified  When true (default), infer BA types and apply
	///                    rewriting hooks after parsing.
	/// @return Parsed tree reference, or a structured error on parse failure.
	static result<tref> get_term(const std::string& term, bool simplified = true);
	/// @copydoc get_term
	/// @return GC-safe handle variant.
	static result<htref> geth_term(const std::string& term, bool simplified = true);

	/// Parse a well-formed formula (wff) from a string.
	/// @param formula  Source string in Tau syntax (e.g. "x = 0").
	/// @param simplified  When true, infer BA types and apply hooks.
	/// @return Parsed tree reference, or a structured error on parse failure.
	static result<tref> get_formula(const std::string& formula, bool simplified = true);
	/// @copydoc get_formula
	static result<htref> geth_formula(const std::string& formula, bool simplified = true);

	/// Parse a function definition (rec_relation with bf body).
	/// The input must parse as a rec_relation whose body is a bf or a ref.
	/// The definition is automatically registered in the global definition store.
	/// @return Parsed tree, or a structured error if the body is not a bf/ref.
	static result<tref> get_function_def(const std::string& function_def, bool simplified = true);
	/// @copydoc get_function_def
	static result<htref> geth_function_def(const std::string& function_def, bool simplified = true);

	/// Parse a predicate definition (rec_relation with wff body).
	/// The input must parse as a rec_relation whose body is a wff or a ref.
	/// The definition is automatically registered in the global definition store.
	/// @return Parsed tree, or a structured error if the body is not a wff/ref.
	static result<tref> get_predicate_def(const std::string& predicate_def, bool simplified = true);
	/// @copydoc get_predicate_def
	static result<htref> geth_predicate_def(const std::string& predicate_def, bool simplified = true);

	/// Parse an I/O stream definition (input_def or output_def).
	/// @return Parsed and trimmed tree, or a structured error on failure.
	static result<tref> get_stream_def(const std::string& stream_def);
	/// @copydoc get_stream_def
	static result<htref> geth_stream_def(const std::string& stream_def);

	/// Parse a full Tau specification (may contain definitions,
	/// stream declarations, and a main formula terminated by '.').
	/// @return Parsed spec tree, or a structured error on failure.
	static result<tref> get_spec(const std::string& spec);
	/// @copydoc get_spec
	static result<htref> geth_spec(const std::string& spec);

	/// Parse a recursive definition (rec_relation) and register it
	/// in the global definitions store.
	/// @return Parsed tree, or a structured error on failure.
	static result<tref> get_definition(const std::string& definition, bool simplified = true);
	/// @copydoc get_definition
	static result<htref> geth_definition(const std::string& definition, bool simplified = true);

	/// Parse input as a spec first; if that fails, try parsing as a bf term.
	/// Useful for REPL-style input where the user may type either.
	/// @return Parsed tree, or a structured error if neither parse succeeds.
	static result<tref> get_spec_or_term(const std::string& expression, bool simplified = true);
	/// @copydoc get_spec_or_term
	static result<htref> geth_spec_or_term(const std::string& expression, bool simplified = true);

	/// Parse input as either a wff or a bf term (single production rule).
	/// @return Parsed and trimmed tree, or a structured error on failure.
	static result<tref> get_formula_or_term(const std::string& expression, bool simplified = true);
	/// @copydoc get_formula_or_term
	static result<htref> geth_formula_or_term(const std::string& expression, bool simplified = true);

	/// Register a definition from pre-parsed head and body tree nodes.
	/// Both are converted to htref internally and added to the global
	/// definitions store.
	/// @return The 1-based definition id (>0) on success (AP1-6: ids are
	///         1-based precisely so the first definition is distinguishable
	///         from failure), or a structured error if either argument is
	///         nullptr.
	static result<size_t> add_definition(tref head, tref body);

	// -----------------------------------------------------------------------
	// Querying
	// ------------------------------------------------------------

	/// Check whether @p expression (or any of its descendants) contains
	/// a node of nonterminal type @p nt.  Uses a pre-order DFS.
	static bool contains(tref expression, typename node::type nt);
	/// @copydoc contains(tref,typename node::type)
	static bool contains(htref expression, typename node::type nt);

	/// Return true if @p expression parses as (or is) a bf term.
	/// The string overload attempts get_term() and checks for success.
	static bool is_term(const std::string& expression);
	/// Return true if the root node of @p expression is flagged as a term.
	static bool is_term(tref expression);
	/// @copydoc is_term(tref)
	static bool is_term(htref expression);

	/// Return true if @p expression parses as (or is) a wff.
	/// The string overload attempts get_formula() and checks for success.
	static bool is_formula(const std::string& expression);
	/// Return true if the root node of @p expression has type wff.
	static bool is_formula(tref expression);
	/// @copydoc is_formula(tref)
	static bool is_formula(htref expression);

	// -----------------------------------------------------------------------
	// Using definitions
	// ------------------------------------------------------------

	/// Apply a single recursive definition to an expression.
	/// Parses both strings, then delegates to the tref overload.
	/// @return The rewritten expression, or a structured error on parse failure.
	static result<std::string> apply_def(
		const std::string& def,
		const std::string& expression);
	/// Apply a single definition tree to an expression tree.
	/// Internally wraps it in a singleton set and calls apply_defs().
	static result<tref> apply_def(tref def, tref expression);
	/// @copydoc apply_def(tref,tref)
	static result<htref> apply_def(htref def, htref expression);

	/// Apply a set of recursive definitions to an expression.
	/// Each definition's head/body pair is added to the expression's
	/// rec_relation list, then apply_rr_to_formula() rewrites the tree.
	/// @return The rewritten expression, or a structured error on failure.
	static result<std::string> apply_defs(
		const std::set<std::string>& defs,
		const std::string& expression);
	/// @copydoc apply_defs(const std::set<std::string>&,const std::string&)
	static result<tref> apply_defs(subtree_set<node> defs, tref expression);
	/// @copydoc apply_defs(const std::set<std::string>&,const std::string&)
	static result<htref> apply_defs(const std::set<htref>& defs, htref expression);

	/// Apply all globally registered definitions to an expression.
	/// Equivalent to apply_defs() with an empty definition set (which
	/// causes only the global store's definitions to be used).
	static result<std::string> apply_all_defs(
		const std::string& expression);
	/// @copydoc apply_all_defs(const std::string&)
	static result<tref> apply_all_defs(tref expression);
	/// @copydoc apply_all_defs(const std::string&)
	static result<htref> apply_all_defs(htref expression);

	// -----------------------------------------------------------------------
	// Printing
	// ------------------------------------------------------------

	/// Pretty-print @p expression to the output stream @p os.
	/// Returns @p os for chaining.  If @p expression is nullptr, nothing
	/// is written.
	static std::ostream& print(std::ostream& os, tref expression);
	/// @copydoc print(std::ostream&,tref)
	static std::ostream& print(std::ostream& os, htref expression);

	/// Serialize @p expression to a string.  Returns "" if nullptr.
	static std::string to_str(tref expression);
	/// @copydoc to_str(tref)
	static std::string to_str(htref expression);

	// -----------------------------------------------------------------------
	// Substitution
	// ------------------------------------------------------------

	/// Replace every occurrence of @p that in @p expression with @p with.
	/// All three arguments must be either all terms or all formulas;
	/// mismatched types produce a structured error and log it.
	static result<std::string> substitute(
		const std::string& expression,
		const std::string& that,
		const std::string& with);
	/// @copydoc substitute(const std::string&,const std::string&,const std::string&)
	static result<tref> substitute(tref expression, tref that, tref with);
	/// @copydoc substitute(const std::string&,const std::string&,const std::string&)
	static result<htref> substitute(htref expression, htref that, htref with);

	/// Apply multiple substitutions sequentially (left to right).
	/// Each key in @p that_with is replaced by its corresponding value.
	static result<std::string> substitute(
		const std::string& expression,
		const std::map<std::string, std::string>& that_with);
	/// @copydoc substitute(const std::string&,const std::map<std::string,std::string>&)
	static result<tref> substitute(tref expression, std::map<tref, tref> that_with);
	/// @copydoc substitute(const std::string&,const std::map<std::string,std::string>&)
	static result<htref> substitute(
		htref expression,
		std::map<htref, htref> that_with);

	// -----------------------------------------------------------------------
	// Normal forms
	// ------------------------------------------------------------

	/// Convert an expression to Boole normal form.
	/// Applies all definitions first, then rewrites. Works on both
	/// bf terms and wff formulas.
	static result<std::string> boole_normal_form(
		const std::string& expression);
	/// @copydoc boole_normal_form(const std::string&)
	static result<tref> boole_normal_form(tref expression);
	/// @copydoc boole_normal_form(const std::string&)
	static result<htref> boole_normal_form(htref expression);

	/// Convert an expression to disjunctive normal form (DNF).
	/// Dispatches to bf or wff DNF depending on the root node type.
	static result<std::string> dnf(const std::string& expression);
	/// @copydoc dnf(const std::string&)
	static result<tref> dnf(tref expression);
	/// @copydoc dnf(const std::string&)
	static result<htref> dnf(htref expression);

	/// Convert an expression to conjunctive normal form (CNF).
	/// Dispatches to bf or wff CNF depending on the root node type.
	static result<std::string> cnf(const std::string& expression);
	/// @copydoc cnf(const std::string&)
	static result<tref> cnf(tref expression);
	/// @copydoc cnf(const std::string&)
	static result<htref> cnf(htref expression);

	/// Convert an expression to negation normal form (NNF).
	/// For wff: pushes negation inward via De Morgan's laws.
	/// For bf: pushes negation into sub-terms.
	static result<std::string> nnf(const std::string& expression);
	/// @copydoc nnf(const std::string&)
	static result<tref> nnf(tref expression);
	/// @copydoc nnf(const std::string&)
	static result<htref> nnf(htref expression);

	// -----------------------------------------------------------------------
	// Procedures
	// ------------------------------------------------------------

	/// Apply cheap non-local simplifications to a bf term, including
	/// symbolic clause contradiction detection.
	static result<std::string> syntactic_term_simplification(
		const std::string& term);
	/// @copydoc syntactic_term_simplification(const std::string&)
	static result<tref> syntactic_term_simplification(tref term);
	/// @copydoc syntactic_term_simplification(const std::string&)
	static result<htref> syntactic_term_simplification(htref term);

	/// Apply cheap non-local simplifications to a wff, including
	/// symbolic clause contradiction and simple equality reasoning.
	static result<std::string> syntactic_formula_simplification(
		const std::string& formula);
	/// @copydoc syntactic_formula_simplification(const std::string&)
	static result<tref> syntactic_formula_simplification(tref formula);
	/// @copydoc syntactic_formula_simplification(const std::string&)
	static result<htref> syntactic_formula_simplification(htref formula);

	/// Fully normalize a bf term: apply definitions, resolve
	/// recursive relations, and produce a canonical form.
	/// @return Normalized term, or a structured error if input is not a bf.
	static result<std::string> normalize_term(const std::string& term);
	/// @copydoc normalize_term(const std::string&)
	static result<tref> normalize_term(tref term);
	/// @copydoc normalize_term(const std::string&)
	static result<htref> normalize_term(htref term);

	/// Fully normalize a wff formula: apply definitions, resolve
	/// recursive relations, and produce a canonical form via the
	/// normalizer pipeline.
	/// @return Normalized formula, or a structured error if input is not a wff.
	static result<std::string> normalize_formula(const std::string& fm);
	/// @copydoc normalize_formula(const std::string&)
	static result<tref> normalize_formula(tref fm);
	/// @copydoc normalize_formula(const std::string&)
	static result<htref> normalize_formula(htref fm);

	/// Push quantifiers inward (anti-prenex transformation).
	/// Distributes ∀/∃ over ∧/∨ to reduce quantifier scope.
	static result<std::string> anti_prenex(const std::string& fm);
	/// @copydoc anti_prenex(const std::string&)
	static result<tref> anti_prenex(tref fm);
	/// @copydoc anti_prenex(const std::string&)
	static result<htref> anti_prenex(htref fm);

	/// Eliminate quantifiers by applying anti-prenex followed by
	/// quantifier resolution. All definitions are applied first.
	static result<std::string> eliminate_quantifiers(const std::string& fm);
	/// @copydoc eliminate_quantifiers(const std::string&)
	static result<tref> eliminate_quantifiers(tref fm);
	/// @copydoc eliminate_quantifiers(const std::string&)
	static result<htref> eliminate_quantifiers(htref fm);

	/// Check if a specification is realizable (∃ winning system strategy).
	/// Merges top-level G-conjuncts before normalization, then runs the
	/// LTL realizability pipeline.
	static result<bool> realizable(const std::string& spec);
	/// @copydoc realizable(const std::string&)
	static result<bool> realizable(tref spec);
	/// @copydoc realizable(const std::string&)
	static result<bool> realizable(htref spec);

	/// Check if a specification is unrealizable.  Equivalent to
	/// negating realizable(spec).
	static result<bool> unrealizable(const std::string& spec);
	/// @copydoc unrealizable(const std::string&)
	static result<bool> unrealizable(tref spec);
	/// @copydoc unrealizable(const std::string&)
	static result<bool> unrealizable(htref spec);

	/// Check satisfiability: true iff the formula is realizable.
	/// Merges top-level G-conjuncts before checking.
	static result<bool> sat(const std::string& formula);
	/// @copydoc sat(const std::string&)
	static result<bool> sat(tref formula);
	/// @copydoc sat(const std::string&)
	static result<bool> sat(htref formula);

	/// Check unsatisfiability.  Equivalent to negating sat(formula).
	static result<bool> unsat(const std::string& formula);
	/// @copydoc unsat(const std::string&)
	static result<bool> unsat(tref formula);
	/// @copydoc unsat(const std::string&)
	static result<bool> unsat(htref formula);

	/// Check validity: true iff the formula holds for all models.
	/// Merges top-level G-conjuncts, then checks via valid_spec().
	static result<bool> valid(const std::string& formula);
	/// @copydoc valid(const std::string&)
	static result<bool> valid(tref formula);
	/// @copydoc valid(const std::string&)
	static result<bool> valid(htref formula);

	/// Check if T (tautology) implies the normalized formula.
	/// This is the underlying validity check used by valid().
	static result<bool> valid_spec(const std::string& spec);
	/// @copydoc valid_spec(const std::string&)
	static result<bool> valid_spec(tref spec);
	/// @copydoc valid_spec(const std::string&)
	static result<bool> valid_spec(htref spec);

	// -----------------------------------------------------------------------
	// Solving
	// ------------------------------------------------------------

	/// Solve a formula for its free variables.
	/// Applies all definitions, then runs the solver.  Rejects formulas
	/// containing temporal quantifiers.
	/// @param formula  A quantifier-free wff.
	/// @param mode     Solver strategy: general, maximum, minimum, or bitvector.
	/// @return A map from variable names (or trefs/htrefs) to their
	///         solution values, or a structured error (code::unsat when no
	///         solution exists) on failure.
	static result<std::map<std::string, std::string>> solve(
		const std::string& formula,
		solver_mode mode = solver_mode::general);
	/// @copydoc solve(const std::string&,solver_mode)
	static result<subtree_map<node, tref>> solve(
		tref formula,
		solver_mode mode = solver_mode::general);
	/// @copydoc solve(const std::string&,solver_mode)
	static result<std::map<htref, htref>> solve(
		htref formula,
		solver_mode mode = solver_mode::general);

	/// Compute the Least General Recursive Solution (LGRS) for a
	/// Boolean equation.  The equation must be a single bf_eq equality
	/// with no non-Boolean operations.
	/// @return A map from variables to their LGRS values, or a structured
	///         error (code::unsat when no solution exists) on invalid input.
	static result<std::map<std::string, std::string>> lgrs(
		const std::string& equation);
	/// @copydoc lgrs(const std::string&)
	static result<std::map<htref, htref>> lgrs(htref equation);
	/// @copydoc lgrs(const std::string&)
	static result<subtree_map<node, tref>> lgrs(tref equation);

	// -----------------------------------------------------------------------
	// Execution
	// ------------------------------------------------------------

	/// Construct an interpreter from a Tau specification string.
	/// Parses, normalizes, and checks for free variables.  Returns a
	/// structured error on parse failure, normalization failure, or if
	/// the normalized formula has free variables.
	static result<interpreter<node>> get_interpreter(
		const std::string& spec);
	/// Construct an interpreter with explicit I/O stream remapping.
	static result<interpreter<node>> get_interpreter(
		const std::string& spec,
		interpreter_options& options);
	/// Construct an interpreter from a pre-parsed spec tree.
	static result<interpreter<node>> get_interpreter(
		tref spec);
	/// Construct an interpreter from a pre-parsed spec tree with options.
	static result<interpreter<node>> get_interpreter(
		tref spec,
		interpreter_options& options);
	/// Construct an interpreter from a parsed tau_spec object.
	static result<interpreter<node>> get_interpreter(
		tau_spec<node>& spec);
	/// Construct an interpreter from a parsed tau_spec with options.
	static result<interpreter<node>> get_interpreter(
		tau_spec<node>& spec,
		interpreter_options& options);

	/// Query which input streams the interpreter needs for its next step.
	/// Returns a vector of (stream_name, time_point) pairs that must be
	/// provided to the step() call.
	static std::vector<stream_at> get_inputs_for_step(interpreter<node>& i);

	/// Advance the interpreter by one time step with explicit inputs.
	/// Parses each input value string into the appropriate BA constant,
	/// calls the interpreter's step, writes outputs, and processes any
	/// specification update stream.
	/// @param interactive  When true (default), reports code::invalid_state
	///   if the interpreter signals auto_continue=false (used by CLI tools
	///   to pause and prompt).  When false, outputs are always returned.
	/// @return A map from (stream_name, time_point) to output value
	///         strings, or a structured error (code::invalid_state when
	///         awaiting input or an interactive pause) on failure.
	static result<std::map<stream_at, std::string>> step(
		interpreter<node>& i,
		std::map<stream_at, std::string> inputs,
		bool interactive = true);

	/// Advance the interpreter by one time step without explicit inputs.
	/// Used when the specification has no input streams. Reports
	/// code::invalid_state if auto_continue is false.
	static result<std::map<stream_at, std::string>> step(
		interpreter<node>& i);

	/// Drive the interpreter through its full step loop (see
	/// interpreter::run_loop): steps until the spec is exhausted, input
	/// ends, or the user quits.
	/// @param quit_on_idle  When true, stop instead of prompting once the
	///   loop goes idle (matches the CLI's `-q`); when false (default),
	///   prompt interactively.
	/// @return false (with a structured error) if a step's output failed
	///   to write; true otherwise.
	static result<bool> run(interpreter<node>& i, bool quit_on_idle = false);

	/// Run BA type inference on an expression.  Infers types, canonizes
	/// quantifier IDs, unnests G-in-G, and checks for semantic errors.
	/// @param use_defaults  When true, apply default type rules.
	/// @return The inferred expression, or a structured error on type errors.
	static result<tref> infer(tref expr, bool use_defaults = true);

	/// Infer types and apply local simplifications (e.g. 1 & 0 → 0,
	/// canonical quantifier IDs, reget in post-order).
	static result<std::string> simplify(const std::string& expr,
						bool use_defaults = true);
	/// @copydoc simplify(const std::string&,bool)
	static result<tref> simplify(tref expr, bool use_defaults = true);
	/// @copydoc simplify(const std::string&,bool)
	static result<htref> simplify(htref expr, bool use_defaults = true);

private:
	/// Extract a normalized rr<node> from an expression tree.
	/// Handles both spec nodes (via tau_lang::get_nso_rr) and bare
	/// wff/bf nodes (via resolve_io_vars).
	static result<rr<node>> get_nso_rr(tref expr);
};


} // namespace idni::tau_lang

#include "api.tmpl.h"
#include "api.tmpl.htref.h"
#include "api.tmpl.string.h"

#endif // __IDNI__TAU__API_H__
