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
 * **Status:** Highly unstable — contains bugs, subject to change.
 */

// TODO (LOW) add non string api for execution?
// TODO (HIGH) tests
// TODO (HIGH) error handling
// TODO (HIGH) documentation
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

/**
 * @brief Static API for Tau: parsing, normalisation, satisfiability, solving, and execution.
 *
 * Alias this struct for a concrete BA pack:
 * `using tau_api = api<node<BAs...>>;`
 *
 * Every method has two overload families:
 * - **Plain** overloads accept `tref`, `htref`, or `std::string` arguments.
 * - **Measuring** overloads take an additional `measuring& m` first argument
 *   and record wall-clock time into @p m.
 *
 * @tparam node Tree node type satisfying `NodeType`.
 */
template <NodeType node>
struct api {
	using tau = tree<node>;
	using optional_string = std::optional<std::string>;
	using severity_level = boost::log::trivial::severity_level;

	friend tau;

	// -----------------------------------------------------------------------
	// Global options
	// -----------------------------------------------------------------------
	/** @brief Enable/disable character variable mode. */
	static void set_charvar(bool state);
	/** @brief Enable/disable BV blasting. */
	static void set_blasting(bool state);
	/** @brief Enable/disable indenting in pretty-printed output. */
	static void set_indenting(bool state);
	/** @brief Enable/disable ANSI colour highlighting in output. */
	static void set_highlighting(bool state);
	/** @brief Enable/disable JSON output mode. */
	static void set_json(bool state);
	/** @brief Set the active Boost.Log severity threshold. */
	static void set_severity(severity_level level);

	// -----------------------------------------------------------------------
	// Parsing
	// -----------------------------------------------------------------------
	/** @brief Parse @p term (a Boolean function / `bf`) and return its tree. */
	static tref get_term(const std::string& term, bool simplified = true);
	/** @brief Parse @p term and return a handle ref. */
	static htref geth_term(const std::string& term, bool simplified = true);

	/** @brief Parse @p formula (a well-formed formula / `wff`) and return its tree. */
	static tref get_formula(const std::string& formula, bool simplified = true);
	/** @brief Parse @p formula and return a handle ref. */
	static htref geth_formula(const std::string& formula, bool simplified = true);

	/** @brief Parse a `rec_relation { bf_ref ... bf ... }` function definition. */
	static tref get_function_def(const std::string& function_def, bool simplified = true);
	/** @brief Parse a function definition and return a handle ref. */
	static htref geth_function_def(const std::string& function_def, bool simplified = true);

	/** @brief Parse a `rec_relation { wff_ref ... wff ... }` predicate definition. */
	static tref get_predicate_def(const std::string& predicate_def, bool simplified = true);
	/** @brief Parse a predicate definition and return a handle ref. */
	static htref geth_predicate_def(const std::string& predicate_def, bool simplified = true);

	/** @brief Parse an `input_def` or `output_def` stream definition. */
	static tref get_stream_def(const std::string& stream_def);
	/** @brief Parse a stream definition and return a handle ref. */
	static htref geth_stream_def(const std::string& stream_def);

	/** @brief Parse a complete Tau specification. */
	static tref get_spec(const std::string& spec);
	/** @brief Parse a specification and return a handle ref. */
	static htref geth_spec(const std::string& spec);

	/** @brief Parse any `rec_relation` definition. */
	static tref get_definition(const std::string& definition, bool simplified = true);
	/** @brief Parse any definition and return a handle ref. */
	static htref geth_definition(const std::string& definition, bool simplified = true);

	/** @brief Parse a `spec`, `wff`, or `bf` expression. */
	static tref get_spec_or_term(const std::string& expression, bool simplified = true);
	/** @brief Parse spec/formula/term and return a handle ref. */
	static htref geth_spec_or_term(const std::string& expression, bool simplified = true);

	/** @brief Parse a `wff` or `bf` expression. */
	static tref get_formula_or_term(const std::string& expression, bool simplified = true);
	/** @brief Parse formula/term and return a handle ref. */
	static htref geth_formula_or_term(const std::string& expression, bool simplified = true);

	/** @brief Register the rule @p head → @p body and return its index. */
	static size_t add_definition(tref head, tref body);

	// -----------------------------------------------------------------------
	// Querying
	// -----------------------------------------------------------------------
	/** @brief Return `true` if @p expression contains a sub-node of type @p nt. */
	static bool contains(tref expression, typename node::type nt);
	/** @brief Return `true` if @p expression contains a sub-node of type @p nt. */
	static bool contains(htref expression, typename node::type nt);

	/** @brief Return `true` if @p expression is a Boolean-function term. */
	static bool is_term(const std::string& expression);
	/** @brief Return `true` if @p expression is a Boolean-function term. */
	static bool is_term(tref expression);
	/** @brief Return `true` if @p expression is a Boolean-function term. */
	static bool is_term(htref expression);

	/** @brief Return `true` if @p expression is a well-formed formula. */
	static bool is_formula(const std::string& expression);
	/** @brief Return `true` if @p expression is a well-formed formula. */
	static bool is_formula(tref expression);
	/** @brief Return `true` if @p expression is a well-formed formula. */
	static bool is_formula(htref expression);

	// -----------------------------------------------------------------------
	// Using definitions
	// -----------------------------------------------------------------------
	/** @brief Apply definition @p def to @p expression and return the result. */
	static optional_string apply_def(
		const std::string& def,
		const std::string& expression);
	/** @brief Apply definition @p def to @p expression. */
	static tref apply_def(tref def, tref expression);
	/** @brief Apply definition @p def to @p expression. */
	static htref apply_def(htref def, htref expression);

	/** @brief Apply all definitions in @p defs to @p expression. */
	static optional_string apply_defs(
		const std::set<std::string>& defs,
		const std::string& expression);
	/** @brief Apply all definitions in @p defs to @p expression. */
	static tref apply_defs(subtree_set<node> defs, tref expression);
	/** @brief Apply all definitions in @p defs to @p expression. */
	static htref apply_defs(std::set<htref> defs, htref expression);

	/** @brief Apply all registered definitions to @p expression. */
	static optional_string apply_all_defs(const std::string& expression);
	/** @brief Apply all registered definitions to @p expression. */
	static tref apply_all_defs(tref expression);
	/** @brief Apply all registered definitions to @p expression. */
	static htref apply_all_defs(htref expression);

	// -----------------------------------------------------------------------
	// Printing
	// -----------------------------------------------------------------------
	/** @brief Pretty-print @p expression to @p os. */
	static std::ostream& print(std::ostream& os, tref expression);
	/** @brief Pretty-print @p expression to @p os. */
	static std::ostream& print(std::ostream& os, htref expression);

	/** @brief Return the pretty-printed string for @p expression. */
	static std::string to_str(tref expression);
	/** @brief Return the pretty-printed string for @p expression. */
	static std::string to_str(htref expression);

	// -----------------------------------------------------------------------
	// Substitution
	// -----------------------------------------------------------------------
	/** @brief Replace @p that with @p with inside @p expression. */
	static optional_string substitute(
		const std::string& expression,
		const std::string& that,
		const std::string& with);
	/** @brief Replace @p that with @p with inside @p expression. */
	static tref substitute(tref expression, tref that, tref with);
	/** @brief Replace @p that with @p with inside @p expression. */
	static htref substitute(htref expression, htref that, htref with);

	/** @brief Apply all substitutions in @p that_with to @p expression. */
	static optional_string substitute(
		const std::string& expression,
		const std::map<std::string, std::string>& that_with);
	/** @brief Apply all substitutions in @p that_with to @p expression. */
	static tref substitute(tref expression, std::map<tref, tref> that_with);
	/** @brief Apply all substitutions in @p that_with to @p expression. */
	static htref substitute(
		htref expression,
		std::map<htref, htref> that_with);

	// -----------------------------------------------------------------------
	// Normal forms
	// -----------------------------------------------------------------------
	/** @brief Transform @p expression to Boole normal form. */
	static optional_string boole_normal_form(const std::string& expression);
	/** @brief Transform @p expression to Boole normal form. */
	static tref boole_normal_form(tref expression);
	/** @brief Transform @p expression to Boole normal form. */
	static htref boole_normal_form(htref expression);

	/** @brief Transform @p expression to disjunctive normal form. */
	static optional_string dnf(const std::string& expression);
	/** @brief Transform @p expression to disjunctive normal form. */
	static tref dnf(tref expression);
	/** @brief Transform @p expression to disjunctive normal form. */
	static htref dnf(htref expression);

	/** @brief Transform @p expression to conjunctive normal form. */
	static optional_string cnf(const std::string& expression);
	/** @brief Transform @p expression to conjunctive normal form. */
	static tref cnf(tref expression);
	/** @brief Transform @p expression to conjunctive normal form. */
	static htref cnf(htref expression);

	/** @brief Transform @p expression to negation normal form. */
	static optional_string nnf(const std::string& expression);
	/** @brief Transform @p expression to negation normal form. */
	static tref nnf(tref expression);
	/** @brief Transform @p expression to negation normal form. */
	static htref nnf(htref expression);

	// -----------------------------------------------------------------------
	// Procedures
	// -----------------------------------------------------------------------
	/** @brief Apply cheap non-local term simplifications (e.g. symbolic clause contradiction). */
	static optional_string syntactic_term_simplification(const std::string& term);
	/** @brief Apply cheap non-local term simplifications. */
	static tref syntactic_term_simplification(tref term);
	/** @brief Apply cheap non-local term simplifications. */
	static htref syntactic_term_simplification(htref term);

	/** @brief Apply cheap non-local formula simplifications (clause contradiction + equality). */
	static optional_string syntactic_formula_simplification(const std::string& formula);
	/** @brief Apply cheap non-local formula simplifications. */
	static tref syntactic_formula_simplification(tref formula);
	/** @brief Apply cheap non-local formula simplifications. */
	static htref syntactic_formula_simplification(htref formula);

	/** @brief Fully normalize the Boolean-function term @p term. */
	static optional_string normalize_term(const std::string& term);
	/** @brief Fully normalize the Boolean-function term @p term. */
	static tref normalize_term(tref term);
	/** @brief Fully normalize the Boolean-function term @p term. */
	static htref normalize_term(htref term);

	/** @brief Fully normalize the formula @p fm. */
	static optional_string normalize_formula(const std::string& fm);
	/** @brief Fully normalize the formula @p fm. */
	static tref normalize_formula(tref fm);
	/** @brief Fully normalize the formula @p fm. */
	static htref normalize_formula(htref fm);

	/** @brief Apply anti-prenexing to move quantifiers inward in @p fm. */
	static optional_string anti_prenex(const std::string& fm);
	/** @brief Apply anti-prenexing to @p fm. */
	static tref anti_prenex(tref fm);
	/** @brief Apply anti-prenexing to @p fm. */
	static htref anti_prenex(htref fm);

	/** @brief Eliminate all quantifiers from @p fm. */
	static optional_string eliminate_quantifiers(const std::string& fm);
	/** @brief Eliminate all quantifiers from @p fm. */
	static tref eliminate_quantifiers(tref fm);
	/** @brief Eliminate all quantifiers from @p fm. */
	static htref eliminate_quantifiers(htref fm);

	/** @brief Return `true` if specification @p spec is realizable. */
	static bool realizable(const std::string& spec);
	/** @brief Return `true` if specification @p spec is realizable. */
	static bool realizable(tref spec);
	/** @brief Return `true` if specification @p spec is realizable. */
	static bool realizable(htref spec);

	/** @brief Return `true` if specification @p spec is unrealizable. */
	static bool unrealizable(const std::string& spec);
	/** @brief Return `true` if specification @p spec is unrealizable. */
	static bool unrealizable(tref spec);
	/** @brief Return `true` if specification @p spec is unrealizable. */
	static bool unrealizable(htref spec);

	/** @brief Return `true` if @p formula is satisfiable. */
	static bool sat(const std::string& formula);
	/** @brief Return `true` if @p formula is satisfiable. */
	static bool sat(tref formula);
	/** @brief Return `true` if @p formula is satisfiable. */
	static bool sat(htref formula);

	/** @brief Return `true` if @p formula is unsatisfiable. */
	static bool unsat(const std::string& formula);
	/** @brief Return `true` if @p formula is unsatisfiable. */
	static bool unsat(tref formula);
	/** @brief Return `true` if @p formula is unsatisfiable. */
	static bool unsat(htref formula);

	/** @brief Return `true` if @p formula is valid (tautology). */
	static bool valid(const std::string& formula);
	/** @brief Return `true` if @p formula is valid. */
	static bool valid(tref formula);
	/** @brief Return `true` if @p formula is valid. */
	static bool valid(htref formula);

	/** @brief Return `true` if specification @p spec is valid. */
	static bool valid_spec(const std::string& spec);
	/** @brief Return `true` if specification @p spec is valid. */
	static bool valid_spec(tref spec);
	/** @brief Return `true` if specification @p spec is valid. */
	static bool valid_spec(htref spec);

	// -----------------------------------------------------------------------
	// Solving
	// -----------------------------------------------------------------------
	/**
	 * @brief Solve @p formula and return a variable→value map, or `nullopt` if unsat.
	 * @param formula Formula to solve.
	 * @param mode Solver mode (default: `general`).
	 */
	static std::optional<std::map<std::string, std::string>> solve(
		const std::string& formula,
		solver_mode mode = solver_mode::general);
	/** @brief Solve @p formula and return a variable→term map. */
	static std::optional<subtree_map<node, tref>> solve(
		tref formula,
		solver_mode mode = solver_mode::general);
	/** @brief Solve @p formula and return a variable→term map. */
	static std::optional<std::map<htref, htref>> solve(
		htref formula,
		solver_mode mode = solver_mode::general);

	/** @brief Compute the least general right solution (LGRS) of @p equation. */
	static std::optional<std::map<std::string, std::string>> lgrs(
		const std::string& equation);
	/** @brief Compute the LGRS of @p equation. */
	static std::optional<std::map<htref, htref>> lgrs(htref equation);
	/** @brief Compute the LGRS of @p equation. */
	static std::optional<subtree_map<node, tref>> lgrs(tref equation);

	// -----------------------------------------------------------------------
	// Execution
	// -----------------------------------------------------------------------
	/** @brief Build an interpreter for specification @p spec, or `nullopt` on failure. */
	static std::optional<interpreter<node>> get_interpreter(const std::string& spec);
	/** @brief Build an interpreter for @p spec with I/O remapping. */
	static std::optional<interpreter<node>> get_interpreter(
		const std::string& spec,
		interpreter_options& options);
	/** @brief Build an interpreter for @p spec (tree ref). */
	static std::optional<interpreter<node>> get_interpreter(tref spec);
	/** @brief Build an interpreter for @p spec (tree ref) with I/O remapping. */
	static std::optional<interpreter<node>> get_interpreter(
		tref spec,
		interpreter_options& options);
	/** @brief Build an interpreter from a parsed `tau_spec`. */
	static std::optional<interpreter<node>> get_interpreter(tau_spec<node>& spec);
	/** @brief Build an interpreter from a `tau_spec` with I/O remapping. */
	static std::optional<interpreter<node>> get_interpreter(
		tau_spec<node>& spec,
		interpreter_options& options);

	/** @brief Return the list of input streams required by the next step of @p i. */
	static std::vector<stream_at> get_inputs_for_step(interpreter<node>& i);
	/**
	 * @brief Advance interpreter @p i one step with @p inputs.
	 * @return Map of output streams to their assigned values, or `nullopt` on failure.
	 */
	static std::optional<std::map<stream_at, std::string>> step(
		interpreter<node>& i,
		std::map<stream_at, std::string> inputs);
	/** @brief Advance interpreter @p i one step with no explicit inputs. */
	static std::optional<std::map<stream_at, std::string>> step(interpreter<node>& i);

	// -----------------------------------------------------------------------
	// Simplification and inference
	// -----------------------------------------------------------------------
	/** @brief Infer types for @p expr and return the type-annotated tree. */
	static tref infer(tref expr, bool use_defaults = true);

	/** @brief Apply local simplifications (e.g. `1 & 0 → 0`) to @p expr. */
	static optional_string simplify(const std::string& expr, bool use_defaults = true);
	/** @brief Apply local simplifications to @p expr. */
	static tref simplify(tref expr, bool use_defaults = true);
	/** @brief Apply local simplifications to @p expr. */
	static htref simplify(htref expr, bool use_defaults = true);

	// -----------------------------------------------------------------------
	// Measuring overloads (same operations, record timing into m)
	// -----------------------------------------------------------------------
	/** @brief Parse @p term and record timing into @p m. */
	static tref get_term(measuring& m, const std::string& term, bool simplified = true);
	/** @brief Parse @p term (handle) and record timing. */
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
	static htref apply_defs(measuring& m, std::set<htref> defs, htref expression);

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
	/// @brief Extract an `rr<node>` recurrence relation from a specification tree.
	static std::optional<rr<node>> get_nso_rr(tref expr);
};


} // namespace idni::tau_lang

#include "api.tmpl.h"
#include "api.tmpl.htref.h"
#include "api.tmpl.string.h"
#include "api.tmpl.measuring.h"

#endif // __IDNI__TAU__API_H__
