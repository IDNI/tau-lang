// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

/**
 * @file api.h
 * @brief Public C++ API for Tau: parsing, normalisation, satisfiability, solving, and execution.
 *
 * All operations are exposed as static methods on `api<node>`. Callers typically
 * create a type alias `using tau_api = api<node<BAs...>>` and call through it.
 * Every operation has two flavours: a plain overload and an overload that also
 * records wall-clock timing into a `measuring` tree.
 *
 * Threading contract: the API mutates shared parser settings, pretty-printer
 * flags, definition registries, and other caches without synchronization.
 * Treat the public API, interpreter, and REPL integration as single-threaded
 * unless callers provide their own external serialization.
 *
 * **Status:** Highly unstable — contains bugs, subject to change.
 */

// TODO (LOW) add non string api for execution?
// TODO (HIGH) tests
// TODO (HIGH) error handling
// TODO (HIGH) decide which parsing get_* methods or other methods will go private if any
// TODO (MEDIUM) parsing with `bool simplify = true` argument

#ifndef __IDNI__TAU__API_H__
#define __IDNI__TAU__API_H__

#include "interpreter.h"
#include "utility/measure.h"

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
 * @brief Hierarchical wall-clock measurement tree.
 *
 * Each node records the name and elapsed milliseconds for one operation;
 * nested sub-operations appear in `parts`.
 */
struct measuring {
	std::string name;            ///< Label for this measurement node.
	double ms = 0;               ///< Elapsed time in milliseconds.
	std::vector<measuring> parts{}; ///< Child measurements.

	/** @brief Append a new empty child and return a reference to it. */
	measuring& part();
	/**
	 * @brief Print the tree to @p os at indentation @p level.
	 * @return @p os.
	 */
	std::ostream& operator()(std::ostream& os, size_t level = 0) const;
	/**
	 * @brief Print the tree as JSON to @p os at indentation @p level.
	 * @return @p os.
	 */
	std::ostream& to_json(std::ostream& os, size_t level = 0) const;
};

/** @brief Stream the `measuring` tree @p m to @p os in human-readable form. */
std::ostream& operator<<(std::ostream& os, const measuring& m);

/**
 * @brief RAII timer that records elapsed time into a `measuring` node.
 *
 * Starts timing on construction, stops and writes `ms` on destruction.
 */
struct api_measure {
	/** @brief Start timing for operation @p name and record into @p m. */
	api_measure(std::string name, measuring& m);
	/** @brief Stop timing and store elapsed milliseconds into `m.ms`. */
	~api_measure();
	measuring& m;           ///< Target measurement node.
	idni::measures::timer t; ///< Underlying timer.
};

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
/// Most methods also have a `measuring&` variant that records profiling data.
///
/// @tparam node  A type satisfying the NodeType concept (see tau_tree.h).
///               In practice, use `node<BAs...>` with your BA pack.
template <NodeType node>
struct api {
	using tau = tree<node>;
	using optional_string = std::optional<std::string>;
	using severity_level = boost::log::trivial::severity_level;

	friend tau;

	// -----------------------------------------------------------------------
	// Global options
	// -----------------------------------------------------------------------
	/// Switch between single-char variable names ("charvar" mode, e.g. x, y)
	/// and multi-char variable names ("var" mode, e.g. foo, bar).
	/// Affects both the tau parser and the SBF parser.
	static void set_charvar(bool state);
	/** @brief Enable/disable BV blasting. */
	static void set_blasting(bool state);
	/// Enable or disable indented pretty-printing of tree output.
	static void set_indenting(bool state);
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
	/// @return Parsed tree reference, or nullptr on parse failure.
	static tref get_term(const std::string& term, bool simplified = true);
	/// @copydoc get_term
	/// @return GC-safe handle variant.
	static htref geth_term(const std::string& term, bool simplified = true);

	/// Parse a well-formed formula (wff) from a string.
	/// @param formula  Source string in Tau syntax (e.g. "x = 0").
	/// @param simplified  When true, infer BA types and apply hooks.
	/// @return Parsed tree reference, or nullptr on parse failure.
	static tref get_formula(const std::string& formula, bool simplified = true);
	/// @copydoc get_formula
	static htref geth_formula(const std::string& formula, bool simplified = true);

	/// Parse a function definition (rec_relation with bf body).
	/// The input must parse as a rec_relation whose body is a bf or a ref.
	/// The definition is automatically registered in the global definition store.
	/// @return Parsed tree, or nullptr if the body is not a bf/ref.
	static tref get_function_def(const std::string& function_def, bool simplified = true);
	/// @copydoc get_function_def
	static htref geth_function_def(const std::string& function_def, bool simplified = true);

	/// Parse a predicate definition (rec_relation with wff body).
	/// The input must parse as a rec_relation whose body is a wff or a ref.
	/// The definition is automatically registered in the global definition store.
	/// @return Parsed tree, or nullptr if the body is not a wff/ref.
	static tref get_predicate_def(const std::string& predicate_def, bool simplified = true);
	/// @copydoc get_predicate_def
	static htref geth_predicate_def(const std::string& predicate_def, bool simplified = true);

	/// Parse an I/O stream definition (input_def or output_def).
	/// @return Parsed and trimmed tree, or nullptr on failure.
	static tref get_stream_def(const std::string& stream_def);
	/// @copydoc get_stream_def
	static htref geth_stream_def(const std::string& stream_def);

	/// Parse a full Tau specification (may contain definitions,
	/// stream declarations, and a main formula terminated by '.').
	/// @return Parsed spec tree, or nullptr on failure.
	static tref get_spec(const std::string& spec);
	/// @copydoc get_spec
	static htref geth_spec(const std::string& spec);

	/// Parse a recursive definition (rec_relation) and register it
	/// in the global definitions store.
	/// @return Parsed tree, or nullptr on failure.
	static tref get_definition(const std::string& definition, bool simplified = true);
	/// @copydoc get_definition
	static htref geth_definition(const std::string& definition, bool simplified = true);

	/// Parse input as a spec first; if that fails, try parsing as a bf term.
	/// Useful for REPL-style input where the user may type either.
	/// @return Parsed tree, or nullptr if neither parse succeeds.
	static tref get_spec_or_term(const std::string& expression, bool simplified = true);
	/// @copydoc get_spec_or_term
	static htref geth_spec_or_term(const std::string& expression, bool simplified = true);

	/// Parse input as either a wff or a bf term (single production rule).
	/// @return Parsed and trimmed tree, or nullptr on failure.
	static tref get_formula_or_term(const std::string& expression, bool simplified = true);
	/// @copydoc get_formula_or_term
	static htref geth_formula_or_term(const std::string& expression, bool simplified = true);

	/// Register a definition from pre-parsed head and body tree nodes.
	/// Both are converted to htref internally and added to the global
	/// definitions store.
	/// @return The definition index (>0) on success, or 0 if either
	///         argument is nullptr.
	static size_t add_definition(tref head, tref body);

	// -----------------------------------------------------------------------
	// Querying
	// ------------------------------------------------------------

	/// Check whether @p expression (or any of its descendants) contains
	/// a node of nonterminal type @p nt.  Uses a pre-order DFS.
	static bool contains(tref expression, typename node::type nt);
	/// @copydoc contains(tref,typename node::type)
	static bool contains(htref expression, typename node::type nt);

	/// Return true if @p expression parses as (or is) a bf term.
	/// The string overload attempts get_term() and checks for non-null.
	static bool is_term(const std::string& expression);
	/// Return true if the root node of @p expression is flagged as a term.
	static bool is_term(tref expression);
	/// @copydoc is_term(tref)
	static bool is_term(htref expression);

	/// Return true if @p expression parses as (or is) a wff.
	/// The string overload attempts get_formula() and checks for non-null.
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
	/// @return The rewritten expression, or nullopt on parse failure.
	static optional_string apply_def(
		const std::string& def,
		const std::string& expression);
	/// Apply a single definition tree to an expression tree.
	/// Internally wraps it in a singleton set and calls apply_defs().
	static tref apply_def(tref def, tref expression);
	/// @copydoc apply_def(tref,tref)
	static htref apply_def(htref def, htref expression);

	/// Apply a set of recursive definitions to an expression.
	/// Each definition's head/body pair is added to the expression's
	/// rec_relation list, then apply_rr_to_formula() rewrites the tree.
	/// @return The rewritten expression, or nullopt / nullptr on failure.
	static optional_string apply_defs(
		const std::set<std::string>& defs,
		const std::string& expression);
	/// @copydoc apply_defs(const std::set<std::string>&,const std::string&)
	static tref apply_defs(subtree_set<node> defs, tref expression);
	/// @copydoc apply_defs(const std::set<std::string>&,const std::string&)
	static htref apply_defs(const std::set<htref>& defs, htref expression);

	/// Apply all globally registered definitions to an expression.
	/// Equivalent to apply_defs() with an empty definition set (which
	/// causes only the global store's definitions to be used).
	static optional_string apply_all_defs(
		const std::string& expression);
	/// @copydoc apply_all_defs(const std::string&)
	static tref apply_all_defs(tref expression);
	/// @copydoc apply_all_defs(const std::string&)
	static htref apply_all_defs(htref expression);

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
	/// mismatched types return nullptr / nullopt and log an error.
	static optional_string substitute(
		const std::string& expression,
		const std::string& that,
		const std::string& with);
	/// @copydoc substitute(const std::string&,const std::string&,const std::string&)
	static tref substitute(tref expression, tref that, tref with);
	/// @copydoc substitute(const std::string&,const std::string&,const std::string&)
	static htref substitute(htref expression, htref that, htref with);

	/// Apply multiple substitutions sequentially (left to right).
	/// Each key in @p that_with is replaced by its corresponding value.
	static optional_string substitute(
		const std::string& expression,
		const std::map<std::string, std::string>& that_with);
	/// @copydoc substitute(const std::string&,const std::map<std::string,std::string>&)
	static tref substitute(tref expression, std::map<tref, tref> that_with);
	/// @copydoc substitute(const std::string&,const std::map<std::string,std::string>&)
	static htref substitute(
		htref expression,
		std::map<htref, htref> that_with);

	// -----------------------------------------------------------------------
	// Normal forms
	// ------------------------------------------------------------

	/// Convert an expression to Boole normal form.
	/// Applies all definitions first, then rewrites. Works on both
	/// bf terms and wff formulas.
	static optional_string boole_normal_form(
		const std::string& expression);
	/// @copydoc boole_normal_form(const std::string&)
	static tref boole_normal_form(tref expression);
	/// @copydoc boole_normal_form(const std::string&)
	static htref boole_normal_form(htref expression);

	/// Convert an expression to disjunctive normal form (DNF).
	/// Dispatches to bf or wff DNF depending on the root node type.
	static optional_string dnf(const std::string& expression);
	/// @copydoc dnf(const std::string&)
	static tref dnf(tref expression);
	/// @copydoc dnf(const std::string&)
	static htref dnf(htref expression);

	/// Convert an expression to conjunctive normal form (CNF).
	/// Dispatches to bf or wff CNF depending on the root node type.
	static optional_string cnf(const std::string& expression);
	/// @copydoc cnf(const std::string&)
	static tref cnf(tref expression);
	/// @copydoc cnf(const std::string&)
	static htref cnf(htref expression);

	/// Convert an expression to negation normal form (NNF).
	/// For wff: pushes negation inward via De Morgan's laws.
	/// For bf: pushes negation into sub-terms.
	static optional_string nnf(const std::string& expression);
	/// @copydoc nnf(const std::string&)
	static tref nnf(tref expression);
	/// @copydoc nnf(const std::string&)
	static htref nnf(htref expression);

	// -----------------------------------------------------------------------
	// Procedures
	// ------------------------------------------------------------

	/// Apply cheap non-local simplifications to a bf term, including
	/// symbolic clause contradiction detection.
	static optional_string syntactic_term_simplification(
		const std::string& term);
	/// @copydoc syntactic_term_simplification(const std::string&)
	static tref syntactic_term_simplification(tref term);
	/// @copydoc syntactic_term_simplification(const std::string&)
	static htref syntactic_term_simplification(htref term);

	/// Apply cheap non-local simplifications to a wff, including
	/// symbolic clause contradiction and simple equality reasoning.
	static optional_string syntactic_formula_simplification(
		const std::string& formula);
	/// @copydoc syntactic_formula_simplification(const std::string&)
	static tref syntactic_formula_simplification(tref formula);
	/// @copydoc syntactic_formula_simplification(const std::string&)
	static htref syntactic_formula_simplification(htref formula);

	/// Fully normalize a bf term: apply definitions, resolve
	/// recursive relations, and produce a canonical form.
	/// @return Normalized term, or nullptr if input is not a bf.
	static optional_string normalize_term(const std::string& term);
	/// @copydoc normalize_term(const std::string&)
	static tref normalize_term(tref term);
	/// @copydoc normalize_term(const std::string&)
	static htref normalize_term(htref term);

	/// Fully normalize a wff formula: apply definitions, resolve
	/// recursive relations, and produce a canonical form via the
	/// normalizer pipeline.
	/// @return Normalized formula, or nullptr if input is not a wff.
	static optional_string normalize_formula(const std::string& fm);
	/// @copydoc normalize_formula(const std::string&)
	static tref normalize_formula(tref fm);
	/// @copydoc normalize_formula(const std::string&)
	static htref normalize_formula(htref fm);

	/// Push quantifiers inward (anti-prenex transformation).
	/// Distributes ∀/∃ over ∧/∨ to reduce quantifier scope.
	static optional_string anti_prenex(const std::string& fm);
	/// @copydoc anti_prenex(const std::string&)
	static tref anti_prenex(tref fm);
	/// @copydoc anti_prenex(const std::string&)
	static htref anti_prenex(htref fm);

	/// Eliminate quantifiers by applying anti-prenex followed by
	/// quantifier resolution. All definitions are applied first.
	static optional_string eliminate_quantifiers(const std::string& fm);
	/// @copydoc eliminate_quantifiers(const std::string&)
	static tref eliminate_quantifiers(tref fm);
	/// @copydoc eliminate_quantifiers(const std::string&)
	static htref eliminate_quantifiers(htref fm);

	/// Check if a specification is realizable (∃ winning system strategy).
	/// Merges top-level G-conjuncts before normalization, then runs the
	/// LTL realizability pipeline.
	static bool realizable(const std::string& spec);
	/// @copydoc realizable(const std::string&)
	static bool realizable(tref spec);
	/// @copydoc realizable(const std::string&)
	static bool realizable(htref spec);

	/// Check if a specification is unrealizable.  Equivalent to
	/// `!realizable(spec)`.
	static bool unrealizable(const std::string& spec);
	/// @copydoc unrealizable(const std::string&)
	static bool unrealizable(tref spec);
	/// @copydoc unrealizable(const std::string&)
	static bool unrealizable(htref spec);

	/// Check satisfiability: true iff the formula is realizable.
	/// Merges top-level G-conjuncts before checking.
	static bool sat(const std::string& formula);
	/// @copydoc sat(const std::string&)
	static bool sat(tref formula);
	/// @copydoc sat(const std::string&)
	static bool sat(htref formula);

	/// Check unsatisfiability.  Equivalent to `!sat(formula)`.
	static bool unsat(const std::string& formula);
	/// @copydoc unsat(const std::string&)
	static bool unsat(tref formula);
	/// @copydoc unsat(const std::string&)
	static bool unsat(htref formula);

	/// Check validity: true iff the formula holds for all models.
	/// Merges top-level G-conjuncts, then checks via valid_spec().
	static bool valid(const std::string& formula);
	/// @copydoc valid(const std::string&)
	static bool valid(tref formula);
	/// @copydoc valid(const std::string&)
	static bool valid(htref formula);

	/// Check if T (tautology) implies the normalized formula.
	/// This is the underlying validity check used by valid().
	static bool valid_spec(const std::string& spec);
	/// @copydoc valid_spec(const std::string&)
	static bool valid_spec(tref spec);
	/// @copydoc valid_spec(const std::string&)
	static bool valid_spec(htref spec);

	// -----------------------------------------------------------------------
	// Solving
	// ------------------------------------------------------------

	/// Solve a formula for its free variables.
	/// Applies all definitions, then runs the solver.  Rejects formulas
	/// containing temporal quantifiers.
	/// @param formula  A quantifier-free wff.
	/// @param mode     Solver strategy: general, maximum, minimum, or bitvector.
	/// @return A map from variable names (or trefs/htrefs) to their
	///         solution values, or nullopt if no solution exists or
	///         an error occurred.
	static std::optional<std::map<std::string, std::string>> solve(
		const std::string& formula,
		solver_mode mode = solver_mode::general);
	/// @copydoc solve(const std::string&,solver_mode)
	static std::optional<subtree_map<node, tref>> solve(
		tref formula,
		solver_mode mode = solver_mode::general);
	/// @copydoc solve(const std::string&,solver_mode)
	static std::optional<std::map<htref, htref>> solve(
		htref formula,
		solver_mode mode = solver_mode::general);

	/// Compute the Least General Recursive Solution (LGRS) for a
	/// Boolean equation.  The equation must be a single bf_eq equality
	/// with no non-Boolean operations.
	/// @return A map from variables to their LGRS values, or nullopt
	///         on invalid input.
	static std::optional<std::map<std::string, std::string>> lgrs(
		const std::string& equation);
	/// @copydoc lgrs(const std::string&)
	static std::optional<std::map<htref, htref>> lgrs(htref equation);
	/// @copydoc lgrs(const std::string&)
	static std::optional<subtree_map<node, tref>> lgrs(tref equation);

	// -----------------------------------------------------------------------
	// Execution
	// ------------------------------------------------------------

	/// Construct an interpreter from a Tau specification string.
	/// Parses, normalizes, and checks for free variables.  Returns
	/// nullopt on parse failure, normalization failure, or if the
	/// normalized formula has free variables.
	static std::optional<interpreter<node>> get_interpreter(
		const std::string& spec);
	/// Construct an interpreter with explicit I/O stream remapping.
	static std::optional<interpreter<node>> get_interpreter(
		const std::string& spec,
		interpreter_options& options);
	/// Construct an interpreter from a pre-parsed spec tree.
	static std::optional<interpreter<node>> get_interpreter(
		tref spec);
	/// Construct an interpreter from a pre-parsed spec tree with options.
	static std::optional<interpreter<node>> get_interpreter(
		tref spec,
		interpreter_options& options);
	/// Construct an interpreter from a parsed tau_spec object.
	static std::optional<interpreter<node>> get_interpreter(
		tau_spec<node>& spec);
	/// Construct an interpreter from a parsed tau_spec with options.
	static std::optional<interpreter<node>> get_interpreter(
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
	/// @param interactive  When true (default), returns nullopt if the
	///   interpreter signals auto_continue=false (used by CLI tools
	///   to pause and prompt).  When false, always returns outputs
	///   (used by programmatic callers).
	/// @return A map from (stream_name, time_point) to output value
	///         strings, or nullopt on error or interactive pause.
	static std::optional<std::map<stream_at, std::string>> step(
		interpreter<node>& i,
		std::map<stream_at, std::string> inputs,
		bool interactive = true);

	/// Advance the interpreter by one time step without explicit inputs.
	/// Used when the specification has no input streams.  Always returns
	/// nullopt if auto_continue is false.
	static std::optional<std::map<stream_at, std::string>> step(
		interpreter<node>& i);

	/// Run BA type inference on an expression.  Infers types, canonizes
	/// quantifier IDs, unnests G-in-G, and checks for semantic errors.
	/// @param use_defaults  When true, apply default type rules.
	/// @return The inferred expression, or nullptr on type errors.
	static tref infer(tref expr, bool use_defaults = true);

	/// Infer types and apply local simplifications (e.g. 1 & 0 → 0,
	/// canonical quantifier IDs, reget in post-order).
	static optional_string simplify(const std::string& expr,
						bool use_defaults = true);
	/// @copydoc simplify(const std::string&,bool)
	static tref simplify(tref expr, bool use_defaults = true);
	/// @copydoc simplify(const std::string&,bool)
	static htref simplify(htref expr, bool use_defaults = true);

	// Measuring variants
	// ------------------------------------------------------------------
	// Each method below mirrors a non-measuring counterpart above but
	// accepts an additional `measuring& m` parameter to record timing.
	// See the corresponding non-measuring method for semantics.

	static tref get_term(measuring& m, const std::string& term, bool simplified = true);         // bf
	static htref geth_term(measuring& m, const std::string& term, bool simplified = true);

	/** @brief Parse @p formula and record timing into @p m. */
	static tref get_formula(measuring& m, const std::string& formula, bool simplified = true);
	/** @brief Parse @p formula (handle) and record timing. */
	static htref geth_formula(measuring& m, const std::string& formula, bool simplified = true);

	/** @brief Parse function definition and record timing. */
	static tref get_function_def(measuring& m, const std::string& function_def, bool simplified = true);
	/** @brief Parse function definition (handle) and record timing. */
	static htref geth_function_def(measuring& m, const std::string& function_def, bool simplified = true);

	/** @brief Parse predicate definition and record timing. */
	static tref get_predicate_def(measuring& m, const std::string& predicate_def, bool simplified = true);
	/** @brief Parse predicate definition (handle) and record timing. */
	static htref geth_predicate_def(measuring& m, const std::string& predicate_def, bool simplified = true);

	/** @brief Parse stream definition and record timing. */
	static tref get_stream_def(measuring& m, const std::string& stream_def);
	/** @brief Parse stream definition (handle) and record timing. */
	static htref geth_stream_def(measuring& m, const std::string& stream_def);

	/** @brief Parse specification and record timing. */
	static tref get_spec(measuring& m, const std::string& spec);
	/** @brief Parse specification (handle) and record timing. */
	static htref geth_spec(measuring& m, const std::string& spec);

	/** @brief Parse any definition and record timing. */
	static tref get_definition(measuring& m, const std::string& definition, bool simplified = true);
	/** @brief Parse any definition (handle) and record timing. */
	static htref geth_definition(measuring& m, const std::string& definition, bool simplified = true);

	/** @brief Parse spec/formula/term and record timing. */
	static tref get_spec_or_term(measuring& m, const std::string& expression, bool simplified = true);
	/** @brief Parse spec/formula/term (handle) and record timing. */
	static htref geth_spec_or_term(measuring& m, const std::string& expression, bool simplified = true);

	/** @brief Parse formula/term and record timing. */
	static tref get_formula_or_term(measuring& m, const std::string& expression, bool simplified = true);
	/** @brief Parse formula/term (handle) and record timing. */
	static htref geth_formula_or_term(measuring& m, const std::string& expression, bool simplified = true);

	/** @brief Register a definition and record timing. */
	static size_t add_definition(measuring& m, tref head, tref body);

	/** @brief Apply definition and record timing. */
	static optional_string apply_def(measuring& m, const std::string& def, const std::string& expression);
	/** @brief Apply definition and record timing. */
	static tref apply_def(measuring& m, tref def, tref expression);
	/** @brief Apply definition (handle) and record timing. */
	static htref apply_def(measuring& m, htref def, htref expression);

	/** @brief Apply a set of definitions and record timing. */
	static optional_string apply_defs(measuring& m, const std::set<std::string>& defs, const std::string& expression);
	/** @brief Apply a set of definitions and record timing. */
	static tref apply_defs(measuring& m, subtree_set<node> defs, tref expression);
	/** @brief Apply a set of definitions (handle) and record timing. */
	static htref apply_defs(measuring& m, const std::set<htref>& defs, htref expression);

	/** @brief Apply all definitions and record timing. */
	static optional_string apply_all_defs(measuring& m, const std::string& expression);
	/** @brief Apply all definitions and record timing. */
	static tref apply_all_defs(measuring& m, tref expression);
	/** @brief Apply all definitions (handle) and record timing. */
	static htref apply_all_defs(measuring& m, htref expression);

	/** @brief Substitute and record timing. */
	static optional_string substitute(measuring& m, const std::string& expression, const std::string& that, const std::string& with);
	/** @brief Substitute and record timing. */
	static tref substitute(measuring& m, tref expression, tref that, tref with);
	/** @brief Substitute (handle) and record timing. */
	static htref substitute(measuring& m, htref expression, htref that, htref with);

	/** @brief Apply substitution map and record timing. */
	static optional_string substitute(measuring& m, const std::string& expression, const std::map<std::string, std::string>& that_with);
	/** @brief Apply substitution map and record timing. */
	static tref substitute(measuring& m, tref expression, std::map<tref, tref> that_with);
	/** @brief Apply substitution map (handle) and record timing. */
	static htref substitute(measuring& m, htref expression, std::map<htref, htref> that_with);

	/** @brief Boole normal form and record timing. */
	static optional_string boole_normal_form(measuring& m, const std::string& expression);
	/** @brief Boole normal form and record timing. */
	static tref boole_normal_form(measuring& m, tref expression);
	/** @brief Boole normal form (handle) and record timing. */
	static htref boole_normal_form(measuring& m, htref expression);

	/** @brief DNF and record timing. */
	static optional_string dnf(measuring& m, const std::string& expression);
	/** @brief DNF and record timing. */
	static tref dnf(measuring& m, tref expression);
	/** @brief DNF (handle) and record timing. */
	static htref dnf(measuring& m, htref expression);

	/** @brief CNF and record timing. */
	static optional_string cnf(measuring& m, const std::string& expression);
	/** @brief CNF and record timing. */
	static tref cnf(measuring& m, tref expression);
	/** @brief CNF (handle) and record timing. */
	static htref cnf(measuring& m, htref expression);

	/** @brief NNF and record timing. */
	static optional_string nnf(measuring& m, const std::string& expression);
	/** @brief NNF and record timing. */
	static tref nnf(measuring& m, tref expression);
	/** @brief NNF (handle) and record timing. */
	static htref nnf(measuring& m, htref expression);

	/** @brief Syntactic term simplification and record timing. */
	static optional_string syntactic_term_simplification(measuring& m, const std::string& term);
	/** @brief Syntactic term simplification and record timing. */
	static tref syntactic_term_simplification(measuring& m, tref term);
	/** @brief Syntactic term simplification (handle) and record timing. */
	static htref syntactic_term_simplification(measuring& m, htref term);

	/** @brief Syntactic formula simplification and record timing. */
	static optional_string syntactic_formula_simplification(measuring& m, const std::string& formula);
	/** @brief Syntactic formula simplification and record timing. */
	static tref syntactic_formula_simplification(measuring& m, tref formula);
	/** @brief Syntactic formula simplification (handle) and record timing. */
	static htref syntactic_formula_simplification(measuring& m, htref formula);

	/** @brief Normalize term and record timing. */
	static optional_string normalize_term(measuring& m, const std::string& term);
	/** @brief Normalize term and record timing. */
	static tref normalize_term(measuring& m, tref term);
	/** @brief Normalize term (handle) and record timing. */
	static htref normalize_term(measuring& m, htref term);

	/** @brief Normalize formula and record timing. */
	static optional_string normalize_formula(measuring& m, const std::string& fm);
	/** @brief Normalize formula and record timing. */
	static tref normalize_formula(measuring& m, tref fm);
	/** @brief Normalize formula (handle) and record timing. */
	static htref normalize_formula(measuring& m, htref fm);

	/** @brief Anti-prenex and record timing. */
	static optional_string anti_prenex(measuring& m, const std::string& fm);
	/** @brief Anti-prenex and record timing. */
	static tref anti_prenex(measuring& m, tref fm);
	/** @brief Anti-prenex (handle) and record timing. */
	static htref anti_prenex(measuring& m, htref fm);

	/** @brief Eliminate quantifiers and record timing. */
	static optional_string eliminate_quantifiers(measuring& m, const std::string& fm);
	/** @brief Eliminate quantifiers and record timing. */
	static tref eliminate_quantifiers(measuring& m, tref fm);
	/** @brief Eliminate quantifiers (handle) and record timing. */
	static htref eliminate_quantifiers(measuring& m, htref fm);

	/** @brief Check realizability and record timing. */
	static bool realizable(measuring& m, const std::string& spec);
	/** @brief Check realizability and record timing. */
	static bool realizable(measuring& m, tref spec);
	/** @brief Check realizability (handle) and record timing. */
	static bool realizable(measuring& m, htref spec);

	/** @brief Check unrealizability and record timing. */
	static bool unrealizable(measuring& m, const std::string& spec);
	/** @brief Check unrealizability and record timing. */
	static bool unrealizable(measuring& m, tref spec);
	/** @brief Check unrealizability (handle) and record timing. */
	static bool unrealizable(measuring& m, htref spec);

	/** @brief Check satisfiability and record timing. */
	static bool sat(measuring& m, const std::string& formula);
	/** @brief Check satisfiability and record timing. */
	static bool sat(measuring& m, tref formula);
	/** @brief Check satisfiability (handle) and record timing. */
	static bool sat(measuring& m, htref formula);

	/** @brief Check unsatisfiability and record timing. */
	static bool unsat(measuring& m, const std::string& formula);
	/** @brief Check unsatisfiability and record timing. */
	static bool unsat(measuring& m, tref formula);
	/** @brief Check unsatisfiability (handle) and record timing. */
	static bool unsat(measuring& m, htref formula);

	/** @brief Check validity and record timing. */
	static bool valid(measuring& m, const std::string& formula);
	/** @brief Check validity and record timing. */
	static bool valid(measuring& m, tref formula);
	/** @brief Check validity (handle) and record timing. */
	static bool valid(measuring& m, htref formula);

	/** @brief Check specification validity and record timing. */
	static bool valid_spec(measuring& m, const std::string& spec);
	/** @brief Check specification validity and record timing. */
	static bool valid_spec(measuring& m, tref spec);
	/** @brief Check specification validity (handle) and record timing. */
	static bool valid_spec(measuring& m, htref spec);

	/** @brief Solve formula and record timing. */
	static std::optional<std::map<std::string, std::string>> solve(
		measuring& m, const std::string& formula, solver_mode mode = solver_mode::general);
	/** @brief Solve formula and record timing. */
	static std::optional<subtree_map<node, tref>> solve(
		measuring& m, tref formula, solver_mode mode = solver_mode::general);
	/** @brief Solve formula (handle) and record timing. */
	static std::optional<std::map<htref, htref>> solve(
		measuring& m, htref formula, solver_mode mode = solver_mode::general);

	/** @brief Compute LGRS and record timing. */
	static std::optional<std::map<std::string, std::string>> lgrs(
		measuring& m, const std::string& equation);
	/** @brief Compute LGRS (handle) and record timing. */
	static std::optional<std::map<htref, htref>> lgrs(measuring& m, htref equation);
	/** @brief Compute LGRS and record timing. */
	static std::optional<subtree_map<node, tref>> lgrs(measuring& m, tref equation);

	/** @brief Build interpreter and record timing. */
	static std::optional<interpreter<node>> get_interpreter(measuring& m, const std::string& spec);
	/** @brief Build interpreter with options and record timing. */
	static std::optional<interpreter<node>> get_interpreter(
		measuring& m, const std::string& spec, interpreter_options& options);
	/** @brief Build interpreter from tree and record timing. */
	static std::optional<interpreter<node>> get_interpreter(measuring& m, tref spec);
	/** @brief Build interpreter from tree with options and record timing. */
	static std::optional<interpreter<node>> get_interpreter(
		measuring& m, tref spec, interpreter_options& options);
	/** @brief Build interpreter from tau_spec and record timing. */
	static std::optional<interpreter<node>> get_interpreter(measuring& m, tau_spec<node>& spec);
	/** @brief Build interpreter from tau_spec with options and record timing. */
	static std::optional<interpreter<node>> get_interpreter(
		measuring& m, tau_spec<node>& spec, interpreter_options& options);

	/** @brief Get required inputs for next step and record timing. */
	static std::vector<stream_at> get_inputs_for_step(measuring& m, interpreter<node>& i);
	/** @brief Advance interpreter one step with inputs and record timing. */
	static std::optional<std::map<stream_at, std::string>> step(
		measuring& m, interpreter<node>& i, std::map<stream_at, std::string> inputs);
	/** @brief Advance interpreter one step and record timing. */
	static std::optional<std::map<stream_at, std::string>> step(measuring& m, interpreter<node>& i);

	/** @brief Infer types and record timing. */
	static tref infer(measuring& m, tref expr, bool use_defaults = true);
	/** @brief Simplify and record timing. */
	static optional_string simplify(measuring& m, const std::string& expr, bool use_defaults = true);
	/** @brief Simplify and record timing. */
	static tref simplify(measuring& m, tref expr, bool use_defaults = true);
	/** @brief Simplify (handle) and record timing. */
	static htref simplify(measuring& m, htref expr, bool use_defaults = true);

private:
	/// Extract a normalized rr<node> from an expression tree.
	/// Handles both spec nodes (via tau_lang::get_nso_rr) and bare
	/// wff/bf nodes (via resolve_io_vars).
	static std::optional<rr<node>> get_nso_rr(tref expr);
};


} // namespace idni::tau_lang

#include "api.tmpl.h"
#include "api.tmpl.htref.h"
#include "api.tmpl.string.h"
#include "api.tmpl.measuring.h"

#endif // __IDNI__TAU__API_H__
