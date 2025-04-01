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

#ifndef __REPL_EVALUATOR_H__
#define __REPL_EVALUATOR_H__

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "interpreter.h"
#include "utility/repl.h"

namespace idni::tau_lang {

enum repl_option { none_opt, invalid_opt, severity_opt, status_opt,
	colors_opt, charvar_opt, highlighting_opt, indenting_opt, debug_opt };

template <typename... BAs>
struct repl_evaluator {
	friend struct repl<repl_evaluator<BAs...>>;
	using tau_ba_t = tau_ba<BAs...>;
	using tau_nso_t = tau_nso<BAs...>;
	using memory = htree::sp;
	using memories = std::vector<memory>;
	using memory_ref = std::optional<std::pair<memory, size_t>>;
	using memory_depreciating = tau_nso_t;
	using memories_depreciating = std::vector<memory_depreciating>;
	using memory_ref_depreciating = std::optional<std::pair<memory_depreciating, size_t>>;

	// using type = tau_tree::node::type;

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
	int eval_depreciating(const std::string& src);
	std::string prompt();
	std::string prompt_depreciating();

private:

	int eval_cmd(const tt& n);
	int eval_cmd(const tau_nso_t& n);
	template <typename TT, typename RESULT>
	int eval_cmd(const TT& command, size_t command_type, RESULT& result);
	bool update_charvar(bool value);

	// helpers
	tref make_cli(const std::string& src);
	tau_nso_t make_cli_depreciating(const std::string& src);

	memory_ref memory_retrieve(const tt& n, bool silent = false);
	memory_ref_depreciating memory_retrieve(const tau_nso_t& n, bool silent = false);
	void memory_store(tref value);
	void memory_store(memory_depreciating o);
	std::optional<std::pair<size_t, tref>> get_type_and_arg(const tt& n);
	std::optional<std::pair<size_t, tau_nso_t>>
		get_type_and_arg(const tau_nso_t& n);
	// returns nonterminal type of a node (as size_t and no value if not nt
	std::optional<size_t> get_type(const tau_nso_t& n);
	// returns if a subtree of a node contains a nonterminal
	bool contains(const tt& n, tau_parser::nonterminal nt);
	bool contains(const tau_nso_t& n, tau_parser::nonterminal nt);
	// apply definitions and rr to a program
	tref apply_rr_to_rr_tau_nso(const size_t type, const tt& program);
	tau_nso_t apply_rr_to_rr_tau_nso(const size_t type,
		const tau_nso_t& program);

	// commands
	void not_implemented_yet();

	void version_cmd();

	void help_cmd(const tt& n) const;
	void help_cmd(const tau_nso_t& n) const;
	void help(size_t nt) const;

	void get_cmd(const tt& n);
	void get_cmd(const tau_nso_t& n);
	void get_cmd(repl_option opt);

	void set_cmd(const tt& n);
	void set_cmd(const tau_nso_t& n);
	void set_cmd(repl_option o, const std::string& v);

	void update_bool_opt_cmd(const tt& n,
		const std::function<bool(bool&)>& update_fn);
	void update_bool_opt_cmd(const tau_nso_t& n,
		const std::function<bool(bool&)>& update_fn);
	void update_bool_opt_cmd(repl_option o,
		const std::function<bool(bool&)>& update_fn);

	void history_print_cmd(const tt& command);
	void history_print_cmd(const tau_nso_t& command);
	void history_store_cmd(const tt& command);
	void history_store_cmd(const tau_nso_t& command);
	void history_list_cmd_depreciating();
	void history_list_cmd();

	void def_rr_cmd(const tt& n);
	void def_rr_cmd(const tau_nso_t& n);
	void def_print_cmd(const tt& n);
	void def_print_cmd(const tau_nso_t& n);
	void def_list_cmd();
	void def_input_cmd(const tt& n);
	void def_input_cmd(const tau_nso_t& n);
	void def_output_cmd(const tt& n);
	void def_output_cmd(const tau_nso_t& n);

	tref qelim_cmd(const tt& n);
	std::optional<tau_nso_t> qelim_cmd(const tau_nso_t& n);
	tref normalize_cmd(const tt& n);
	std::optional<tau_nso_t> normalize_cmd(const tau_nso_t& n);
	tref sat_cmd(const tt& n);
	std::optional<tau_nso_t> sat_cmd(const tau_nso_t& n);
	void run_cmd(const tt& n);
	void run_cmd(const tau_nso_t& n);
	void solve_cmd(const tt& n);
	void solve_cmd(const tau_nso_t& n);
	void lgrs_cmd(const tt& n);
	void lgrs_cmd(const tau_nso_t& n);

	tref is_valid_cmd(const tt& n);
	std::optional<tau_nso_t> is_valid_cmd(const tau_nso_t& n);
	tref is_unsatisfiable_cmd(const tt& n);
	std::optional<tau_nso_t> is_unsatisfiable_cmd(const tau_nso_t& n);

	tt get_(tau::node::type nt, const tt& n, bool suppress_error = false);
	tt get_bf(const tt& n, bool suppress_error = false);
	tt get_wff(const tt& n);
	std::optional<tau_nso_t> get_bf(const tau_nso_t& n,
		bool suppress_error = false);
	std::optional<tau_nso_t> get_wff(const tau_nso_t& n);

	tref onf_cmd(const tt& n);
	std::optional<tau_nso_t> onf_cmd(const tau_nso_t& n);
	tref dnf_cmd(const tt& n);
	std::optional<tau_nso_t> dnf_cmd(const tau_nso_t& n);
	tref cnf_cmd(const tt& n);
	std::optional<tau_nso_t> cnf_cmd(const tau_nso_t& n);
	tref nnf_cmd(const tt& n);
	std::optional<tau_nso_t> nnf_cmd(const tau_nso_t& n);
	tref mnf_cmd(const tt& n);
	std::optional<tau_nso_t> mnf_cmd(const tau_nso_t& n);
	tref snf_cmd(const tt& n);
	std::optional<tau_nso_t> snf_cmd(const tau_nso_t& n);

	tref bf_substitute_cmd(const tt& n);
	std::optional<tau_nso_t> bf_substitute_cmd(const tau_nso_t& n);
	tref substitute_cmd(const tt& n);
	std::optional<tau_nso_t> substitute_cmd(const tau_nso_t& n);
	tref instantiate_cmd(const tt& n);
	std::optional<tau_nso_t> instantiate_cmd(const tau_nso_t& n);

	memories_depreciating m_depreciating;
	memories m;
	options opt{};
	// TODO (MEDIUM) this dependency should be removed
	repl<repl_evaluator<BAs...>>* r = 0;
	rec_relations<tau_nso_t> definitions;
	std::map<tau_nso_t, std::pair<std::string, filename>> inputs;
	std::map<tau_nso_t, std::pair<std::string, filename>> outputs;
	bool error = false;
	idni::term::colors TC{};
};

} //idni::tau_lang namespace

#include "repl_evaluator.tmpl.h"

#endif //__REPL_EVALUATOR_H__
