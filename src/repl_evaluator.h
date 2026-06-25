// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// TODO (HIGH) subformula selection for wff
// TODO (MEDIUM) convert selection to dnf
// TODO (MEDIUM) convert selection to cnf
// TODO (MEDIUM) convert selection to nnf
// TODO (MEDIUM) convert selection to bdd
// TODO (MEDIUM) convert selection to anf
// TODO (MEDIUM) convert selection to minterm

// TODO (HIGH) eliminate selected quantifier(s) in standard way
// TODO (MEDIUM) eliminate selected quantifier(s) in differential way
// TODO (MEDIUM) eliminate selected quantifier(s) in minterm way
// TODO (MEDIUM) eliminate selected quantifier(s) in order way

// TODO (HIGH) replace all occurrences of a variable in the selection with proper formula
// TODO (MEDIUM) convert to order normal form wrt selected (single) var

// TODO (HIGH) subformula selection for bf
// TODO (MEDIUM) convert selection to dnf
// TODO (MEDIUM) convert selection to cnf
// TODO (MEDIUM) convert selection to nnf
// TODO (MEDIUM) convert selection to bdd
// TODO (MEDIUM) convert selection to anf
// TODO (MEDIUM) convert selection to minterm

// TODO (HIGH) subst for a var in the selection
// TODO (MEDIUM) subst for a func in the selection

// TODO (MEDIUM) auto-find expressions that make something zero
	// TODO (MEDIUM) by x+f(x) (for univar)
	// TODO (MEDIUM) by the LGRS (for multivar)

// TODO (MEDIUM) find an expression expressing a single solution
// TODO (MEDIUM) expand quantification over bf/sbf into first order quantification wrt a selected set of vars
// TODO (MEDIUM) strong normalization of selected subformulas
// TODO (HIGH) use only required parenthesis in the history or at least be able
// to say something like `pretty(%)`  after executing a given command.


/**
 * @file repl_evaluator.h
 * @brief REPL evaluator for the Tau interactive shell.
 *
 * Declares `repl_evaluator<BAs...>`, which handles command parsing, dispatch
 * to the Tau API, history management, and configuration for the interactive
 * read-eval-print loop.
 */

#ifndef __IDNI__TAU__REPL_EVALUATOR_H__
#define __IDNI__TAU__REPL_EVALUATOR_H__

#include "boolean_algebras/tau_ba.h"
#include "api.h"
#include "utility/repl.h"

namespace idni::tau_lang {

/** @brief Identifiers for configurable REPL options. */
enum repl_option { none_opt, invalid_opt, severity_opt, status_opt,
	colors_opt, charvar_opt, blasting_opt, highlighting_opt, indenting_opt,
	print_benchmarks_opt, debug_opt };

/**
 * @brief REPL evaluator for the Tau interactive shell.
 *
 * Handles command parsing, dispatch to the Tau API, history management,
 * and option configuration for the interactive REPL session.
 * @tparam BAs Boolean-algebra type pack.
 */
template <typename... BAs>
requires BAsPack<BAs...>
struct repl_evaluator {
	friend struct repl<repl_evaluator<BAs...>>;
	using history = htref;
	using history_ref = std::optional<std::pair<history, size_t>>;

	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	using tau_api = api<node>;
	using tau = tree<node>;
	using tt = tau::traverser;

	/** @brief Runtime configuration options for the REPL session. */
	struct options {
		bool status              = true;  ///< Print status after each command.
		bool colors              = true;  ///< Enable ANSI color output.
		bool print_history_store = true;  ///< Print index when storing to history.
		bool error_quits         = false; ///< Exit on error.
		bool charvar             = true;  ///< Use character-variable notation.
		bool blasting            = true;  ///< Enable bitvector predicate blasting.
		bool repl_running 	 = true;  ///< Whether the REPL loop is active.
		bool print_benchmarks    = true;  ///< Print timing benchmarks.
#ifdef DEBUG
		bool debug_repl          = true;
		boost::log::trivial::severity_level
			severity = boost::log::trivial::debug;
#else // DEBUG
		bool debug_repl          = false;
		boost::log::trivial::severity_level
			severity = boost::log::trivial::info;
#endif // DEBUG
		bool experimental        = false; ///< Enable experimental features.
	};

	/**
	 * @brief Construct the evaluator with the given @p opt configuration.
	 * @param opt REPL options (default-constructed if not provided).
	 */
	repl_evaluator(options opt = options{});
	/**
	 * @brief Parse and evaluate the REPL source string @p src.
	 * @param src Command string entered by the user.
	 * @return Exit code (0 = success, non-zero = error/quit).
	 */
	int eval(const std::string& src);
	/** @brief Return the prompt string to display before each input line. */
	std::string prompt();

private:
	// commands
	/// @brief Execute the `version` command (print version info).
	void version_cmd();
	/// @brief Execute the `help` command for the node @p n.
	void help_cmd(const tt& n) const;
	/// @brief Print help for nonterminal @p nt.
	void help(size_t nt) const;

	// history of previous results
	/// @brief Print a history entry identified by @p command.
	void history_print_cmd(const tt& command);
	/// @brief Store the current result to history as identified by @p command.
	void history_store_cmd(const tt& command);
	/// @brief List all history entries.
	void history_list_cmd();

	// options
	/// @brief Get and print the option(s) specified by @p n.
	void get_cmd(const tt& n);
	/// @brief Get and print the value of option @p opt.
	void get_cmd(repl_option opt);
	/// @brief Set an option from the traverser node @p n.
	void set_cmd(const tt& n);
	/// @brief Set option @p o to string value @p v.
	void set_cmd(repl_option o, const std::string& v);
	/// @brief Toggle a bool option identified by @p n using @p update_fn.
	void update_bool_opt_cmd(const tt& n,
		const std::function<bool(bool&)>& update_fn);
	/// @brief Toggle bool option @p o using @p update_fn.
	void update_bool_opt_cmd(repl_option o,
		const std::function<bool(bool&)>& update_fn);

	// substitution and instantiation of formulas
	/// @brief Execute a substitution command from @p n.
	tref subst_cmd(const tt& n);
	/// @brief Execute an instantiation command from @p n.
	tref inst_cmd(const tt& n);

	// definitions
	/// @brief Define a recurrence relation from @p n.
	void def_rr_cmd(const tt& n);
	/// @brief Print the definition identified by @p n.
	void def_print_cmd(const tt& n);
	/// @brief List all current definitions.
	void def_list_cmd();
	/// @brief Define an input stream from @p n.
	void def_input_cmd(const tt& n);
	/// @brief Define an output stream from @p n.
	void def_output_cmd(const tt& n);

	/// @brief Print a "not implemented yet" message.
	void not_implemented_yet();

	// Tau API
	/// @brief Normalize the formula in @p n and return the result.
	tref normalize_cmd(const tt& n);
	/// @brief Check satisfiability of the formula in @p n.
	tref sat_cmd(const tt& n);
	/// @brief Check unsatisfiability of the formula in @p n.
	tref unsat_cmd(const tt& n);
	/// @brief Check validity of the formula in @p n.
	tref valid_cmd(const tt& n);
	/// @brief Eliminate quantifiers from the formula in @p n.
	tref qelim_cmd(const tt& n);
	/// @brief Run the specification in @p n.
	void run_cmd(const tt& n);
	/// @brief Solve the formula in @p n.
	void solve_cmd(const tt& n);
	/// @brief Compute the LGRS solution for the formula in @p n.
	void lgrs_cmd(const tt& n);
	// normal forms
	/// @brief Convert the formula in @p n to CNF.
	tref cnf_cmd(const tt& n);
	/// @brief Convert the formula in @p n to DNF.
	tref dnf_cmd(const tt& n);
	/// @brief Convert the formula in @p n to NNF.
	tref nnf_cmd(const tt& n);
	/// @brief Convert the formula in @p n to MNF.
	tref mnf_cmd(const tt& n);
	/// @brief Convert the formula in @p n to ONF.
	tref onf_cmd(const tt& n);

	/// @brief Evaluate a raw command node @p n.
	int eval_cmd(const tt& n);

	/// @brief Print an "invalid argument" error and return `nullptr`.
	tref invalid_argument() const;

	/// @brief Parse @p src as a CLI command and return the resulting tree.
	tref make_cli(const std::string& src);

	/// @brief Update the charvar option to @p value and return the old value.
	bool update_charvar(bool value);

	/// @brief Update the blasting option to @p value and return the old value.
	bool update_blasting(bool value);

	// history
	/// @brief Retrieve the history entry referenced by @p n.
	history_ref history_retrieve(const tt& n, bool silent = false) const;
	/// @brief Store @p value in the history list.
	void history_store(tref value);
	/// @brief Print one history entry at position @p id / @p size.
	void print_history(const htref& mem, const size_t id,
		const size_t size, bool print_relative_index = true) const;
	/// @brief Return the history index referenced by @p n, or `std::nullopt`.
	std::optional<size_t> get_history_index(const tt& n, const size_t size,
						bool silent = false) const;

	/// @brief Apply any pending transformations to @p arg and return the result.
	tref get_applied(tref arg) const;
	/// @brief Extract type id and formula from @p n or from history.
	std::optional<std::pair<size_t, tref>> get_type_and_arg(
		const tt& n) const;
	/// @brief Extract a formula of type @p nt from @p n or from history.
	tref get_(typename node::type nt, tref n, bool suppress_error = false)
									const;
	/// @brief Extract a BF formula from @p n or history.
	tref get_bf(tref n, bool suppress_error = false) const;
	/// @brief Extract a WFF formula from @p n or history.
	tref get_wff(tref n) const;
	/// @brief Extract any formula from @p arg or from history.
	tref get_any(tref arg) const;

	/// @brief Print benchmark measurements from @p m.
	std::ostream& benchmarks(measuring& m) const;
	/// @brief Print benchmark measurements from @p m and timer @p t.
	std::ostream& benchmarks(measuring& m, idni::measures::timer& t) const;

	std::vector<history> H;
	options opt{};
	trefs rr_defs;
	trefs io_defs;
	// TODO (MEDIUM) this dependency should be removed
	repl<repl_evaluator<BAs...>>* r = 0;
	bool error = false;
	term::colors TC{};
};

} //idni::tau_lang namespace

#include "repl_evaluator.tmpl.h"

#endif //__IDNI__TAU__REPL_EVALUATOR_H__
