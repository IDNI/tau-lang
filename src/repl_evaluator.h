// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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
// TODO (HIGH) use only required parenthesis in the memory or at least be able
// to say something like `pretty(%)`  after executing a given command.

#ifndef __IDNI__TAU__REPL_EVALUATOR_H__
#define __IDNI__TAU__REPL_EVALUATOR_H__

#include "boolean_algebras/tau_ba.h"
#include "utility/repl.h"

namespace idni::tau_lang {

enum repl_option { none_opt, invalid_opt, severity_opt, status_opt,
	colors_opt, charvar_opt, highlighting_opt, indenting_opt, debug_opt };

template <typename... BAs>
requires BAsPack<BAs...>
struct repl_evaluator {
	friend struct repl<repl_evaluator<BAs...>>;
	using memory = htree::sp;
	using memories = std::vector<memory>;
	using memory_ref = std::optional<std::pair<memory, size_t>>;

	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	using tau = tree<node>;
	using tt = tau::traverser;

	struct options {
		bool status             = true;
		bool colors             = true;
		bool print_memory_store = true;
		bool error_quits        = false;
		bool charvar            = true;
		bool repl_running	= true;
#ifdef DEBUG
		bool debug_repl         = true;
		boost::log::trivial::severity_level
			severity = boost::log::trivial::debug;
#else
		bool debug_repl         = false;
		boost::log::trivial::severity_level
			severity = boost::log::trivial::info;
#endif
		bool experimental = false;
	};

	repl_evaluator(options opt = options{});
	int eval(const std::string& src);
	std::string prompt();

private:
	// eval
	int eval_cmd(const tt& n);

	// errors
	tref invalid_argument() const;

	// helpers
	tref make_cli(const std::string& src);

	// returns if a subtree of a node contains a nonterminal
	bool contains(const tt& n, typename node::type nt) const;
	bool update_charvar(bool value);

	// memory
	memory_ref memory_retrieve(const tt& n, bool silent = false) const;
	void memory_store(tref value);
	void print_memory(const htree::sp& mem, const size_t id,
		const size_t size, bool print_relative_index = true) const;
	std::optional<size_t> get_memory_index(const tt& n, const size_t size,
						bool silent = false) const;


	// get argument and type from input or from memory
	std::optional<std::pair<size_t, tref>> get_type_and_arg(
		const tt& n) const;
	// get bf or wff from argument or from memory
	tt get_(typename node::type nt, const tt& n,
		bool suppress_error = false) const;
	tt get_bf(const tt& n, bool suppress_error = false) const;
	tt get_wff(const tt& n) const;

	// get nso rr from argument and add definitions to it
	std::optional<rr> get_nso_rr_with_defs(const tt& n) const;

	// apply definitions and rr to a provided spec or formula
	tref apply_rr_to_nso_rr_with_defs(const tt& spec) const;

	// commands
	void not_implemented_yet();

	void version_cmd();

	void help_cmd(const tt& n) const;
	void help(size_t nt) const;

	void get_cmd(const tt& n);
	void get_cmd(repl_option opt);

	void set_cmd(const tt& n);
	void set_cmd(repl_option o, const std::string& v);

	void update_bool_opt_cmd(const tt& n,
		const std::function<bool(bool&)>& update_fn);
	void update_bool_opt_cmd(repl_option o,
		const std::function<bool(bool&)>& update_fn);

	void history_print_cmd(const tt& command);
	void history_store_cmd(const tt& command);
	void history_list_cmd();

	void def_rr_cmd(const tt& n);
	void def_print_cmd(const tt& n);
	void def_list_cmd();
	void def_input_cmd(const tt& n);
	void def_output_cmd(const tt& n);

	tref qelim_cmd(const tt& n);
	tref normalize_cmd(const tt& n);
	tref sat_cmd(const tt& n);
	tref unsat_cmd(const tt& n);
	tref valid_cmd(const tt& n);
	void run_cmd(const tt& n);
	void solve_cmd(const tt& n);
	void lgrs_cmd(const tt& n);

	tref onf_cmd(const tt& n);
	tref dnf_cmd(const tt& n);
	tref cnf_cmd(const tt& n);
	tref nnf_cmd(const tt& n);
	tref mnf_cmd(const tt& n);
	tref snf_cmd(const tt& n);

	tref bf_substitute_cmd(const tt& n);
	tref substitute_cmd(const tt& n);
	tref instantiate_cmd(const tt& n);

	memories m;
	options opt{};
	// TODO (MEDIUM) this dependency should be removed
	repl<repl_evaluator<BAs...>>* r = 0;
	rewriter::rules definitions;
	typed_io_vars inputs;
	typed_io_vars outputs;
	bool error = false;
	idni::term::colors TC{};
};

} //idni::tau_lang namespace

#include "repl_evaluator.tmpl.h"

#endif //__IDNI__TAU__REPL_EVALUATOR_H__
