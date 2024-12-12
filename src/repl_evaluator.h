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

#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <utility>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#include "sbf_ba.h"
#include "repl.h"
#include "interpreter.h"

namespace idni::tau_lang {

template <typename... BAs>
struct repl_evaluator {
	friend struct repl<repl_evaluator<BAs...>>;
	using tau_ba_t = tau_ba<BAs...>;
	using tau_nso_t = tau_nso<BAs...>;
	using memory = tau_nso_t;
	using memorys = std::vector<memory>;
	using memory_ref = std::optional<std::pair<memory, size_t>>;

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
	};

	repl_evaluator(options opt = options{});
	int eval(const std::string& src);
	std::string prompt();

private:

	int eval_cmd(const tau_nso_t& n);
	bool update_charvar(bool value);

	// helpers
	tau_nso_t make_cli(const std::string& src);
	boost::log::trivial::severity_level nt2severity(size_t nt) const;
	memory_ref memory_retrieve(const tau_nso_t& n, bool silent = false);
	void memory_store(memory o);
	std::optional<std::pair<size_t, tau_nso_t>>
		get_type_and_arg(const tau_nso_t& n);
	// returns nonterminal type of a node (as size_t and no value if not nt
	std::optional<size_t> get_type(const tau_nso_t& n);
	// returns if a subtree of a node contains a nonterminal
	bool contains(const tau_nso_t& n, tau_parser::nonterminal nt);
	// apply definitions and rr to a program
	tau_nso_t apply_rr_to_rr_tau_nso(const size_t type, const tau_nso_t& program);

	// commands
	void not_implemented_yet();

	void version_cmd();
	void help_cmd(const tau_nso_t& n);

	void get_cmd(tau_nso_t n);
	void set_cmd(tau_nso_t n);
	void update_bool_opt_cmd(const tau_nso_t& n,
		const std::function<bool(bool&)>& update_fn);

	void history_print_cmd(const tau_nso_t& command);
	void history_store_cmd(const tau_nso_t& command);
	void history_list_cmd();

	void def_rr_cmd(const tau_nso_t& n);
	void def_print_cmd(const tau_nso_t& n);
	void def_list_cmd();
	void def_input_cmd(const tau_nso_t& n);
	void def_output_cmd(const tau_nso_t& n);

	std::optional<tau_nso_t> qelim_cmd(const tau_nso_t& n);
	std::optional<tau_nso_t> normalize_cmd(const tau_nso_t& n);
	std::optional<tau_nso_t> sat_cmd(const tau_nso_t& n);
	void run_cmd(const tau_nso_t& n);
	void solve_cmd(const tau_nso_t& n);

	std::optional<tau_nso_t> is_valid_cmd(const tau_nso_t& n);
	std::optional<tau_nso_t> is_unsatisfiable_cmd(const tau_nso_t& n);

	std::optional<tau_nso_t> get_bf(const tau_nso_t& n, bool suppress_error=false);
	std::optional<tau_nso_t> get_wff(const tau_nso_t& n);

	std::optional<tau_nso_t> onf_cmd(const tau_nso_t& n);
	std::optional<tau_nso_t> dnf_cmd(const tau_nso_t& n);
	std::optional<tau_nso_t> cnf_cmd(const tau_nso_t& n);
	std::optional<tau_nso_t> nnf_cmd(const tau_nso_t& n);
	std::optional<tau_nso_t> mnf_cmd(const tau_nso_t& n);
	std::optional<tau_nso_t> snf_cmd(const tau_nso_t& n);

	std::optional<tau_nso_t> bf_substitute_cmd(const tau_nso_t& n);
	std::optional<tau_nso_t> substitute_cmd(const tau_nso_t& n);
	std::optional<tau_nso_t> instantiate_cmd(const tau_nso_t& n);

	memorys m;
	options opt{};
	// TODO (MEDIUM) this dependency should be removed
	repl<repl_evaluator<BAs...>>* r = 0;
	rec_relations<tau_nso_t> definitions;
	std::map<tau_nso_t, std::pair<type, filename>> inputs;
	std::map<tau_nso_t, std::pair<type, filename>> outputs;
	bool error = false;
	idni::term::colors TC{};
};

} //idni::tau_lang namespace

#include "repl_evaluator.tmpl.h"

#endif //__REPL_EVALUATOR_H__
