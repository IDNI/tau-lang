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

#include <iostream>
#include <memory>
#include <ostream>

#include "boolean_algebras/ba_pack_traits.h"
#include "boolean_algebras/tau/tau_ba.h"
#include "api.h"
#include "io_context.h"
#include "tau_spec.h"
#include "utility/diagnostics.h"
#include "utility/repl.h"
#include "parse_error_hint.h"
#ifdef TAU_PARSER_HAS_FTXUI
#include "utility/repl_ftxui.h"
#endif

namespace idni::tau_lang {

/**
 * @brief Identifiers for the REPL options of `get`, `set`, `enable`,
 * `disable` and `toggle`.
 *
 * The flags (`status_opt` to `debug_opt`, `pwr_semantic_opt` and
 * `step_prop_opt`) take on/off; `severity_opt` takes error/info/debug/trace.
 * Every other option is a runtime limit: it takes a count (`gcgrowth` a
 * decimal, `ltlalg` a word), so enable/disable/toggle do not apply to it.
 * A limit option writes the library value through its `api<node>::set_*`
 * setter, the same one the CLI flag uses, and `get` reads the effective
 * value back: the value set, else the TAU_* environment variable, else the
 * default.
 *
 * | option (REPL name)       | default | 0 means | environment variable |
 * |--------------------------|---------|---------|----------------------|
 * | block_max_splits (`maxsplits`) | unlimited | unlimited | `TAU_BLOCK_MAX_SPLITS` |
 * | block_max_rounds (`maxrounds`) | unlimited | unlimited | `TAU_BLOCK_MAX_ROUNDS` |
 * | cqe_max_clauses (`maxclauses`) | unlimited | unlimited | `TAU_CQE_MAX_CLAUSES` |
 * | decision_pins (`decisionpins`) | 4096 | none kept | `TAU_BA_DECISION_PINS` |
 * | fixpoint_steps (`fixpointsteps`) | 500 | unlimited | `TAU_MAX_FIXPOINT_STEPS` |
 * | flag_search_steps (`flagsteps`) | 500 | unlimited | `TAU_MAX_FLAG_SEARCH_STEPS` |
 * | squeeze_cap (`squeezecap`) | 0 | unlimited | `TAU_BLOCK_SQUEEZE_CAP` |
 * | simplify_rounds (`simplifyrounds`) | 0 | unlimited | `TAU_MAX_SIMPLIFY_ROUNDS` |
 * | def_passes (`defpasses`) | 0 | unlimited | `TAU_MAX_DEF_PASSES` |
 * | probe_steps (`probesteps`) | 10000 | unlimited | `TAU_MAX_PROBE_STEPS` |
 * | enum_steps (`enumsteps`) | 0 | unlimited | `TAU_MAX_ENUM_STEPS` |
 * | rewrite_rounds (`rewriterounds`) | 0 | unlimited | `TAU_MAX_REWRITE_ROUNDS` |
 * | gc_min_size (`gcminsize`) | 256 | no floor | `TAU_GC_MIN_SIZE` |
 * | gc_growth (`gcgrowth`) | 1.5 | <= 0 disables gc | `TAU_GC_GROWTH_FACTOR` |
 * | tref_budget (`trefbudget`) | 0 | unlimited | `TAU_TREF_BUDGET` |
 * | tref_budget_soft (`trefbudgetsoft`) | 75 (%) | warns, reads 75 | `TAU_TREF_BUDGET_SOFT` |
 * | spec_size_warn (`specsizewarn`) | 0 | off | `TAU_SPEC_SIZE_WARN` |
 * | revision_alts (`revisionalts`) | 0 | unlimited | `TAU_MAX_REVISION_ALTS` |
 * | consistency_subsets (`maxsubsets`) | 4096 | unlimited | `TAU_MAX_CONSISTENCY_SUBSETS` |
 * | cache_bound (`cachebound`) | 4096 | unbounded | `TAU_CACHE_BOUND` |
 * | cover_products (`maxcoverproducts`) | 256 | unlimited | `TAU_MAX_COVER_PRODUCTS` |
 * | constant_size (`maxconstantsize`) | 2000 | unlimited | `TAU_MAX_CONSTANT_SIZE` |
 * | ltl_timeout (`ltltimeout`, seconds) | 60 | no watchdog | `TAU_LTL_TIMEOUT` |
 * | ltl_alg (`ltlalg`, A/B/D/auto) | auto | -- | `TAU_LTL_ALG` |
 * | ltl_qe_max_vars (`ltlqemaxvars`) | 2 | no fast path | `TAU_LTL_QE_MAX_VARS` |
 * | ltl_hoa_max_states (`ltlhoamaxstates`) | 2^22 | unlimited | `TAU_LTL_HOA_MAX_STATES` |
 * | ltl_guard_max_cubes (`ltlguardmaxcubes`) | 512 | unlimited | `TAU_LTL_GUARD_MAX_CUBES` |
 * | ltl_refinement_rounds (`ltlrefinementrounds`) | 64 | unlimited | `TAU_LTL_REFINEMENT_ROUNDS` |
 * | ltl_window_max_paths (`ltlwindowmaxpaths`) | 4096 | unlimited | `TAU_LTL_WINDOW_MAX_PATHS` |
 * | ltl_closed_regions_timeout (`ltlclosedregionstimeout`, seconds) | 20 | no attempt | `TAU_LTL_CLOSED_REGIONS_TIMEOUT` |
 * | ltl_data_game_max_nodes (`ltldatagamemaxnodes`) | 2^23 | unlimited | `TAU_LTL_DATA_GAME_MAX_NODES` |
 * | ltl_data_game_max_memo (`ltldatagamemaxmemo`) | 2^25 | unlimited | `TAU_LTL_DATA_GAME_MAX_MEMO` |
 * | ltl_data_game_max_combinations (`ltldatagamemaxcombinations`) | 4096 | unlimited | `TAU_LTL_DATA_GAME_MAX_COMBINATIONS` |
 * | ltl_max_observations (`ltlmaxobservations`, at most 30) | 8 | 30 | `TAU_LTL_MAX_OBSERVATIONS` |
 * | ltl_mealy_max_states (`ltlmealymaxstates`) | 4096 | no Mealy view | `TAU_LTL_MEALY_MAX_STATES` |
 * | ltl_mealy_max_edges (`ltlmealymaxedges`) | 65536 | no Mealy view | `TAU_LTL_MEALY_MAX_EDGES` |
 * | compile_max_table_edges (`compilemaxtableedges`) | 400 | no table | `TAU_COMPILE_MAX_TABLE_EDGES` |
 * | compile_build_timeout (`compilebuildtimeout`, seconds) | 3600 | no timeout | `TAU_COMPILE_BUILD_TIMEOUT` |
 * | bf_dependence_max_nodes (`bfdependencemaxnodes`) | 65536 | unlimited | `TAU_BF_DEPENDENCE_MAX_NODES` |
 * | lgrs_max_vars (`lgrsmaxvars`) | 8 | unlimited | `TAU_LGRS_MAX_VARS` |
 *
 * `ltltimeout` clamps a value above `ltl_timeout_sec_max`. The option names
 * and their aliases are resolved by get_opt (repl_evaluator.tmpl.h).
 */
enum repl_option { none_opt, invalid_opt, severity_opt, status_opt,
	colors_opt, charvar_opt, preprocessing_opt, factoring_opt,
	highlighting_opt, indenting_opt,
	print_benchmarks_opt, debug_opt,
	// Numeric options, named by their full spelling only: the single
	// letters that fit are taken ("b" is benchmarks and "B" is
	// preprocessing; see get_opt).
	block_max_splits_opt, block_max_rounds_opt, cqe_max_clauses_opt,
	decision_pins_opt,
	fixpoint_steps_opt,
	flag_search_steps_opt, squeeze_cap_opt,
	simplify_rounds_opt, def_passes_opt, probe_steps_opt, enum_steps_opt,
	rewrite_rounds_opt, gc_min_size_opt, gc_growth_opt,
	tref_budget_opt, tref_budget_soft_opt,
	spec_size_warn_opt, revision_alts_opt, consistency_subsets_opt,
	cache_bound_opt, cover_products_opt, constant_size_opt,
	// LTL(ABA) synthesis knobs (ltl_aba.h); ltl_alg_opt takes a word
	// (A/B/D/auto), the others a count.
	ltl_timeout_opt, ltl_alg_opt, ltl_qe_max_vars_opt,
	ltl_hoa_max_states_opt, ltl_guard_max_cubes_opt,
	ltl_refinement_rounds_opt, ltl_window_max_paths_opt,
	ltl_closed_regions_timeout_opt,
	ltl_data_game_max_nodes_opt, ltl_data_game_max_memo_opt,
	ltl_data_game_max_combinations_opt, ltl_max_observations_opt,
	ltl_mealy_max_states_opt, ltl_mealy_max_edges_opt,
	compile_max_table_edges_opt, compile_build_timeout_opt,
	bf_dependence_max_nodes_opt,
	// The solver's lgrs-route variable cap (`-g --lgrs-max-vars`).
	lgrs_max_vars_opt,
	// Boolean, like the first group: the semantic (winning-region) fallback
	// of the temporal pointwise revision (`-Z --pwr-semantic`) and the
	// step's definitional propagation (`-t --step-definitional-propagation`).
	pwr_semantic_opt, step_prop_opt };

/// Logic fragment of a REPL session: which temporal operators a command
/// accepts. `fragment_ltl` (the default) rejects the CTL* operators A, E
/// and -; `fragment_ctl_star` accepts them.
enum logic_fragment { fragment_ltl, fragment_ctl_star };

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
#ifdef TAU_PARSER_HAS_FTXUI
	friend struct repl_ftxui<repl_evaluator<BAs...>>;
#endif
	using history = htref;
	using history_ref = std::optional<std::pair<history, size_t>>;

	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	using tau_api = api<node>;
	using tau = tree<node>;
	using tt = tau::traverser;

	/// @brief Sinks for the evaluator's normal and error output; declared
	/// before every other member so they are ready for any member whose
	/// construction could print.
	std::ostream& out;
	std::ostream& err;

	/** @brief Runtime configuration options for the REPL session. */
	struct options {
		bool status              = true;  ///< Print status after each command.
		bool colors              = true;  ///< Enable ANSI color output.
		bool print_history_store = true;  ///< Print index when storing to history.
		bool error_quits         = false; ///< Exit on error.
		bool charvar             = true;  ///< Use character-variable notation.
		bool preprocessing       = idni::tau_lang::preprocessing; ///< BA preprocessing passes, e.g. bv predicate blasting; follows the library default.
		bool factoring           = ba_component_factoring; ///< Tau-BA component factoring; follows the library default.
		bool repl_running 	 = true;  ///< Whether the REPL loop is active.
		bool print_benchmarks    = true;  ///< Print timing benchmarks.
		// The numeric limit options have no fields here: `set` writes
		// the library values through the api setters and `get` reads them
		// back, so a copy here could only fall out of sync.
#ifdef DEBUG
		/// Print each command's tree and result tree; on in a DEBUG build.
		bool debug_repl          = true;
		/// Log severity threshold; debug in a DEBUG build, else info.
		boost::log::trivial::severity_level
			severity = boost::log::trivial::debug;
#else // DEBUG
		/// Print each command's tree and result tree; on in a DEBUG build.
		bool debug_repl          = false;
		/// Log severity threshold; debug in a DEBUG build, else info.
		boost::log::trivial::severity_level
			severity = boost::log::trivial::info;
#endif // DEBUG
		bool experimental        = false; ///< Enable experimental features.
		logic_fragment fragment   = fragment_ltl; ///< Active logic fragment (LTL or CTL*).
	};

	/**
	 * @brief Construct the evaluator with the given @p opt configuration.
	 *
	 * Applies @p opt's colors, severity, charvar and preprocessing to the
	 * library, and makes every console input stream of a later `run` a
	 * non-blocking stream answered through eval().
	 * @param opt REPL options (default-constructed if not provided).
	 * @param out Sink of the normal output.
	 * @param err Sink of the errors and warnings.
	 */
	repl_evaluator(options opt = options{},
		std::ostream& out = std::cout, std::ostream& err = std::cerr);
	/**
	 * @brief Parse and evaluate the REPL source string @p src.
	 *
	 * While a `run` session waits for an answer, @p src is that answer (a
	 * stream value, or Enter/q at the continue prompt) instead of a
	 * command. Errors are printed to the error stream, not returned.
	 * @param src One or more `.`-separated commands entered by the user.
	 * @return The REPL code: 0 to go on, 1 to quit (the `quit` command, or
	 * any error when `error_quits` is set), 2 when @p src is incomplete and
	 * more lines are needed.
	 */
	idni::diagnostics::result<int> eval(const std::string& src);
	/** @brief Rebuild the prompt string and push it to the active REPL frontend. */
	void reprompt();
#ifdef TAU_PARSER_HAS_FTXUI
	/**
	 * @brief Single-key hook for the FTXUI REPL: drives the interactive
	 * `run` continue/quit gate and Ctrl-C abort without a submitted line.
	 * @param key The key name, e.g. "enter", "q", "ctrl-c".
	 * @return What the frontend does with the key: nothing claimed while no
	 * `run` waits, submit (Enter continues, q or Ctrl-C quits; Ctrl-C at a
	 * stream prompt aborts the run) or consume.
	 */
	repl_key_action on_repl_key(const std::string& key);
#endif

private:
	/// @brief State for one resumable `run` session: the interpreter and
	/// its benchmarking survive across multiple eval() calls. `rep`
	/// accumulates one timed scope per continue_running() invocation, so
	/// the session total never includes the interactive wait between
	/// invocations.
	struct run_session {
		interpreter<node> interp;
		report rep;
		/// @brief Step count at which `run N steps` pauses; 0 means
		/// unbounded.
		size_t steps_to_run = 0;
		/// Steps that produced output so far in this session.
		size_t steps_done   = 0;
		/// Take ownership of the interpreter @p i.
		run_session(interpreter<node> i) : interp(std::move(i)) {}
	};
	/// @brief What the *next* eval() call's input line answers, while set;
	/// eval() checks this before parsing src as a normal CLI command.
	struct pending_request {
		/// The continue/quit gate, or a value for a console input stream.
		enum kind_t { continue_or_quit, stream_value } kind;
		std::string label; ///< prompt text shown while awaiting the answer
		/// Stream the answer is written to.
		std::shared_ptr<repl_pending_input_stream> stream; // stream_value only
		/// Time point the stream waits at.
		size_t time_point = 0; // stream_value only
		/// BA type of the value; null for a tuple-typed (ADT) stream.
		tref type_tree = nullptr; // stream_value only: for incomplete check
	};

	// commands
	/// @brief Execute the `version` command: print the version and the
	/// algebras of the pack.
	void version_cmd();
	/// @brief Execute the `help` command @p n: help on its argument, or the
	/// command overview when it has none.
	void help_cmd(const tt& n) const;
	/// @brief Print the help of the command whose symbol is the
	/// nonterminal @p nt (`help_sym` prints the overview).
	void help(size_t nt) const;

	// history of previous results
	/// @brief Print the history entry @p command names (`%n` or `%-n`).
	void history_print_cmd(const tt& command);
	/// @brief Store the expression of @p command in the history.
	void history_store_cmd(const tt& command);
	/// @brief List all history entries, with absolute and relative index.
	void history_list_cmd();

	// options
	/// @brief Execute the `get` command @p n: print the option it names, a
	/// `family-option` BA option, or every option when it names none.
	void get_cmd(const tt& n);
	/// @brief Print the value of option @p opt; every core and BA-declared
	/// option for `none_opt`, nothing for `invalid_opt`. A limit prints its
	/// effective value (set, else environment, else default).
	void get_cmd(repl_option opt);
	/// @brief Execute the `set` command @p n, then print the option's new
	/// value.
	void set_cmd(const tt& n);
	/// @brief Set option @p o to the text @p v: on/off spellings for a flag,
	/// a decimal count for a limit (a number for gcgrowth, A/B/D/auto for
	/// ltlalg). An invalid value is reported and changes nothing.
	void set_cmd(repl_option o, const std::string& v);
	/// @brief Execute the `enable`/`disable`/`toggle` command @p n with
	/// @p update_fn, then print the option's new value.
	void update_bool_opt_cmd(const tt& n,
		const std::function<bool(bool&)>& update_fn);
	/// @brief Apply @p update_fn to the flag option @p o; a numeric option
	/// is an error, since it takes a count.
	void update_bool_opt_cmd(repl_option o,
		const std::function<bool(bool&)>& update_fn);

	// BA-declared options, addressed by their name in the options repository
	// (e.g. "bv-blasting"), rather than through get_opt()/repl_option.
	/// @brief Print the BA-declared option named @p dotted; a count prints
	/// "unlimited" for 0.
	void get_cmd_ba_option(const std::string& dotted);
	/// @brief Set the BA-declared option named @p dotted to the text @p v;
	/// an invalid value is reported and changes nothing.
	void set_cmd_ba_option(const std::string& dotted, const std::string& v);
	/// @brief Toggle the BA-declared flag option named @p dotted using
	/// @p update_fn (enable/disable/toggle).
	void update_bool_opt_cmd_ba_option(const std::string& dotted,
		const std::function<bool(bool&)>& update_fn);
	/// @brief Find @p dotted among the pack's BA-declared options,
	/// reporting "no such algebra" and "no such option" distinctly.
	/// @return The spec, or nullptr (with the error printed) on failure.
	const option_spec* resolve_ba_option(const std::string& dotted);

	// substitution and instantiation of formulas
	/// @brief Execute the `subst` command @p n: each bracket group of
	/// match/replace pairs applies simultaneously to the previous group's
	/// result. A pattern that matches nothing is a warning; a result that
	/// no longer type-checks is rejected.
	/// @return The substituted expression, or nullptr after an error.
	tref subst_cmd(const tt& n);
	/// @brief Execute the `inst` command @p n: `subst` whose match sides
	/// must all be variables.
	/// @return The instantiated expression, or nullptr after an error.
	tref inst_cmd(const tt& n);

	// definitions
	/// @brief Store the recurrence relation of @p n and register its head;
	/// a relation whose relative offset its head cannot bind is rejected.
	void def_rr_cmd(const tt& n);
	/// @brief Print the stored recurrence relation numbered by @p n
	/// (1-based).
	void def_print_cmd(const tt& n);
	/// @brief List the stored recurrence relations, streams and the io
	/// context.
	void def_list_cmd();
	/// @brief Store the input stream definition of @p n; a name with the
	/// reserved witness prefix `w_` is rejected.
	void def_input_cmd(const tt& n);
	/// @brief Store the output stream definition of @p n; a name with the
	/// reserved witness prefix `w_` is rejected.
	void def_output_cmd(const tt& n);
	/// @brief Store the ADT type definition of @p n, replacing an earlier
	/// one of the same name.
	void def_type_cmd(const tt& n);

	// session management
	/// @brief Execute `reset`: stop the run, clear the history and the
	/// definitions, and free the unreachable tree nodes (api::reset).
	void reset_cmd();
	// type inspection
	/// @brief Execute `whatis` on @p n: print its node type and, for a term
	/// or formula, its BA type.
	/// @return The argument as applied, or nullptr when it has none.
	tref whatis_cmd(const tt& n);

	// Tau API
	// The commands below that return a tref return the result to store in
	// the history, or nullptr when there is none (an error was printed, or
	// the argument was rejected).
	/// @brief Normalize the formula or term in @p n.
	tref normalize_cmd(const tt& n);
	/// @brief Check satisfiability of the formula in @p n.
	/// @return T or F.
	tref sat_cmd(const tt& n);
	/// @brief Check unsatisfiability of the formula in @p n.
	/// @return T or F.
	tref unsat_cmd(const tt& n);
	/// @brief Check validity of the formula in @p n.
	/// @return T or F.
	tref valid_cmd(const tt& n);
	/// @brief Check realizability of the formula in @p n.
	/// @return T or F.
	tref realizable_cmd(const tt& n);
	/// @brief Check unrealizability of the formula in @p n.
	/// @return T or F.
	tref unrealizable_cmd(const tt& n);
	/// @brief Eliminate the non-temporal quantifiers of the formula in @p n.
	tref qelim_cmd(const tt& n);
	/// @brief Execute `run [N steps] [<spec>]` from @p n: start a session
	/// on the spec and the stored definitions (replacing any stored one),
	/// or continue the stored session; N bounds the steps of this call.
	void run_cmd(const tt& n);
	/// @brief `stop` command: clear the stored `run` session (if any).
	void stop_cmd();
	/// @brief `memory` command: print the interpreter's variable map.
	void memory_cmd();
	/// @brief Execute `ltl` on @p n: print the LTL(ABA) explanation of the
	/// formula `realizable` decides, with its verdict.
	void ltl_cmd(const tt& n);
	/// @brief Resume a `run` session until it finishes or needs input
	/// (suspends via `pending`); @p retry re-asks it on a rejected value.
	void continue_running(std::optional<pending_request> retry = std::nullopt);
	/// @brief Print benchmarks and clear the current `run` session.
	void finish_running();
	/// @brief True if @p src is an incomplete (multiline) value of BA type
	/// @p type_tree, enabling the same continuation as top-level input.
	bool stream_value_incomplete(const std::string& src,
		tref type_tree) const;
	/// @brief Solve the formula in @p n (general, minimum or maximum mode)
	/// and print the solution, or "no solution".
	void solve_cmd(const tt& n);
	/// @brief Compute and print the LGRS solution of the equation in @p n,
	/// or "no solution".
	void lgrs_cmd(const tt& n);
	// normal forms
	/// @brief Convert the term in @p n to ANF; no REPL command reaches it
	/// while the grammar's `anf` command is commented out.
	tref anf_cmd(const tt& n);
	/// @brief Convert the formula in @p n to CNF.
	tref cnf_cmd(const tt& n);
	/// @brief Convert the formula in @p n to DNF.
	tref dnf_cmd(const tt& n);
	/// @brief Convert the formula in @p n to NNF.
	tref nnf_cmd(const tt& n);
	/// @brief Convert the formula in @p n to PNF; no REPL command reaches
	/// it while the grammar's `pnf` command is commented out.
	tref pnf_cmd(const tt& n);
	/// @brief Convert the formula in @p n to MNF.
	tref mnf_cmd(const tt& n);
	/// @brief Convert the formula in @p n to ONF with respect to the
	/// variable @p n names.
	tref onf_cmd(const tt& n);

	/// @brief Evaluate the single command @p n and store its result in the
	/// history. A command other than a control command (quit, clear, help,
	/// version, get, set, enable, disable, toggle, reset) is refused while
	/// the tref budget is exceeded.
	/// @return 1 for `quit`, 0 otherwise.
	int eval_cmd(const tt& n);

	/// @brief Print an "invalid argument" error and return `nullptr`.
	tref invalid_argument() const;

	/// @brief Print one command-level error to the REPL's error stream.
	void print_error(code c, std::string_view msg,
		std::initializer_list<idni::diagnostics::attr_in> extra = {}) const;
	/// @brief Print one command-level warning to the REPL's error stream.
	void print_warning(std::string_view msg,
		std::initializer_list<idni::diagnostics::attr_in> extra = {}) const;

	/// @brief Parse @p src as a REPL command line, with the session's ADT
	/// types, and infer its BA types. A parse error is printed here and sets
	/// `error`.
	/// @return The command line tree; a null value when @p src ends before
	/// the command does (more input is needed); an error on a parse
	/// failure.
	result<tref> make_cli(const std::string& src);

	/// @brief Set the charvar option and the library's to @p value.
	/// @return @p value.
	bool update_charvar(bool value);

	/// @brief Set the preprocessing option and the library's to @p value.
	/// @return @p value.
	bool update_preprocessing(bool value);

	/// @brief CTL* fragment gate: print an error when @p fm holds a CTL*
	/// operator and the session is not in the ctl_star fragment.
	/// @return True when @p fm is rejected; false for a null @p fm.
	bool reject_ctl_star_if_disabled(tref fm);

	/// @brief Execute `fragment ltl|ctl_star` from @p n.
	void fragment_cmd(const tt& n);

	/// @brief Set the factoring option and the library's to @p value.
	/// @return @p value.
	bool update_factoring(bool value);

	// history
	/// @brief Retrieve the history entry @p n references.
	/// @param silent Do not print why the location does not exist.
	/// @return The entry and its 0-based position, or nullopt (with an
	/// error printed) when it does not exist.
	history_ref history_retrieve(const tt& n, bool silent = false) const;
	/// @brief Append @p value to the history unless it equals the last
	/// entry, and print the last entry when `print_history_store` is on.
	void history_store(tref value);
	/// @brief Print the history entry @p mem at 0-based position @p id of
	/// @p size entries, as `%id+1`, and also its relative index when
	/// @p print_relative_index.
	void print_history(const htref& mem, const size_t id,
		const size_t size, bool print_relative_index = true) const;
	/// @brief The 0-based index of the history location @p n (`%n`,
	/// 1-based, or `%-n`, counted back from the last) in a history of
	/// @p size entries.
	/// @param silent Do not print why the location does not exist.
	/// @return The index, or `std::nullopt` when it is out of range.
	std::optional<size_t> get_history_index(const tt& n, const size_t size,
						bool silent = false) const;

	/// @brief @p arg with the session's stored ADT types, recurrence
	/// relations and streams added and the relations applied; unless
	/// @p as_written, the relations are also added to the global
	/// definitions.
	/// @param as_written As in tau_spec::keep_as_written.
	/// @return The applied formula or term, or nullptr (with the report
	/// printed) when the spec does not build.
	tref get_applied(tref arg, bool as_written = false) const;
	/// @brief The argument @p n, or the history entry it names, applied by
	/// get_applied().
	/// @return Its node type (the grammar nonterminal, e.g. wff or bf) and
	/// the applied tree, or nullopt on failure.
	std::optional<std::pair<size_t, tref>> get_type_and_arg(
		const tt& n, bool as_written = false) const;
	/// @brief @p n when it is a @p nt node, or the history entry it names
	/// when that is one.
	/// @param suppress_error Do not print "wrong type".
	/// @return The tree, or nullptr when the type does not match.
	tref get_(typename node::type nt, tref n, bool suppress_error = false)
									const;
	/// @brief get_() for a term (bf).
	tref get_bf(tref n, bool suppress_error = false) const;
	/// @brief get_() for a formula (wff), printing a wrong type.
	tref get_wff(tref n) const;
	/// @brief The applied tree of get_type_and_arg(), or nullptr.
	tref get_any(tref arg) const;
	/// @brief get_any() for the commands that decide or run a
	/// specification: every literal as written, so that each clause keeps
	/// its warm-up (pin_written_warm_ups).
	tref get_spec_as_written(tref arg) const;
	/// @brief Infer @p n's BA types so it can be matched against an
	/// already inferred expression. Returns @p n if inference fails.
	tref infer_for_match(tref n) const;

	/// @brief Print @p res's diagnostics report to the error stream, if
	/// benchmarking is on.
	template <typename T>
	void print_benchmarks(const result<T>& res) const;
	/// @brief Print @p rep to the error stream, if benchmarking is on.
	void print_benchmarks(const report& rep) const;
	/// @brief Print the warnings of @p rep, a command that succeeded, while
	/// benchmarks are off; with them on, the benchmark tree carries them.
	void print_warnings(const report& rep) const;

	/// @brief Structural equality of @p a and @p b ignoring type
	/// annotations and resolved BA type ids.
	bool equal_modulo_types(tref a, tref b) const;

	/// The history, oldest first.
	std::vector<history> H;
	/// The session's options.
	options opt{};
	// Held as htrefs, not raw trefs: interpreter::step() calls maybe_gc(),
	// and bintree<node>::gc() destroys every node that is neither reachable
	// from a live htref nor in the keep set collect_live_refs() builds --
	// which never mentions the REPL. A `run` would otherwise free the
	// definitions this session keeps reading afterwards.
	/// Stored recurrence relations, in definition order.
	htrefs rr_defs;
	/// Stored input and output stream definitions, as declared.
	htrefs io_defs;
	// ADT type_def statements accepted via def_type_cmd, kept so they can be
	// prepended (before rr_defs/io_defs) wherever a spec is assembled from
	// stored definitions -- see get_applied() -- and, via names, so a `type`
	// statement typed at the REPL still parses as a type_name on every
	// later line.
	htrefs type_defs;
	/// Names the session's parses grow into type_name, spanning every line.
	tau_dynamic_context names;
	// TODO (MEDIUM) this dependency should be removed
	/// The terminal frontend, set by the repl itself; null otherwise.
	repl<repl_evaluator<BAs...>>* r = 0;
#ifdef TAU_PARSER_HAS_FTXUI
	/// The FTXUI frontend, set by it; null otherwise.
	repl_ftxui<repl_evaluator<BAs...>>* r_ftx = nullptr;
#endif
	/// The current line failed; colors the prompt and, with error_quits,
	/// ends the REPL.
	bool error = false;
	/// Terminal colors, on when `opt.colors` is.
	term::colors TC{};

	/// The stored `run` session; null when none.
	std::unique_ptr<run_session> running;
	/// What the next eval() call answers, while a `run` waits.
	std::optional<pending_request> pending;
	// Set by on_repl_key (Ctrl-C) to abort the current run on the next
	// resume, instead of treating the (empty) submit as a stream value.
	bool run_abort_ = false;
};

} //idni::tau_lang namespace

#include "repl_evaluator.tmpl.h"

#endif //__IDNI__TAU__REPL_EVALUATOR_H__
