// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __REPL_EVALUATOR_H__
#define __REPL_EVALUATOR_H__

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "nso_rr.h"
#include "repl.h"

using namespace std;
using namespace idni;
using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace idni::tau_lang {

enum repl_option { none_opt, invalid_opt, severity_opt, status_opt,
	colors_opt, charvar_opt, highlighting_opt, indenting_opt, debug_opt };

using type = std::string;
using filename = std::string;

template <typename... BAs>
struct repl_evaluator {
	friend struct repl<repl_evaluator<BAs...>>;
	using memory = tau<tau_ba<BAs...>, BAs...>;
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

	int eval_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	bool update_charvar(bool value);

	// helpers
	tau<tau_ba<BAs...>, BAs...> make_cli(const std::string& src);
	memory_ref memory_retrieve(const tau<tau_ba<BAs...>, BAs...>& n, bool silent = false);
	void memory_store(memory o);
	std::optional<std::pair<size_t, tau<tau_ba<BAs...>, BAs...>>>
		get_type_and_arg(const tau<tau_ba<BAs...>, BAs...>& n);
	// returns nonterminal type of a node (as size_t and no value if not nt
	std::optional<size_t> get_type(const tau<tau_ba<BAs...>, BAs...>& n);
	// returns if a subtree of a node contains a nonterminal
	bool contains(const tau<tau_ba<BAs...>, BAs...>& n, tau_parser::nonterminal nt);
	// apply definitions and rr to a program
	tau<tau_ba<BAs...>, BAs...> apply_rr_to_rr_tau_nso(const size_t type,
		const tau<tau_ba<BAs...>, BAs...>& program);

	// commands
	void not_implemented_yet();

	void version_cmd();
	void help_cmd(const tau<tau_ba<BAs...>, BAs...>& n);

	void get_cmd(tau<tau_ba<BAs...>, BAs...> n);
	void set_cmd(tau<tau_ba<BAs...>, BAs...> n);
	void update_bool_opt_cmd(const tau<tau_ba<BAs...>, BAs...>& n,
		const std::function<bool(bool&)>& update_fn);

	void history_print_cmd(const tau<tau_ba<BAs...>, BAs...>& command);
	void history_store_cmd(const tau<tau_ba<BAs...>, BAs...>& command);
	void history_list_cmd();

	void def_rr_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	void def_print_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	void def_list_cmd();
	void def_input_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	void def_output_cmd(const tau<tau_ba<BAs...>, BAs...>& n);

	std::optional<tau<tau_ba<BAs...>, BAs...>> qelim_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	std::optional<tau<tau_ba<BAs...>, BAs...>> normalize_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	std::optional<tau<tau_ba<BAs...>, BAs...>> sat_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	void run_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	void solve_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	void lgrs_cmd(const tau<tau_ba<BAs...>, BAs...>& n);

	std::optional<tau<tau_ba<BAs...>, BAs...>> is_valid_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	std::optional<tau<tau_ba<BAs...>, BAs...>> is_unsatisfiable_cmd(const tau<tau_ba<BAs...>, BAs...>& n);

	std::optional<tau<tau_ba<BAs...>, BAs...>> get_bf(const tau<tau_ba<BAs...>, BAs...>& n,
		bool suppress_error = false);
	std::optional<tau<tau_ba<BAs...>, BAs...>> get_wff(const tau<tau_ba<BAs...>, BAs...>& n);

	std::optional<tau<tau_ba<BAs...>, BAs...>> onf_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	std::optional<tau<tau_ba<BAs...>, BAs...>> dnf_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	std::optional<tau<tau_ba<BAs...>, BAs...>> cnf_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	std::optional<tau<tau_ba<BAs...>, BAs...>> nnf_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	std::optional<tau<tau_ba<BAs...>, BAs...>> mnf_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	std::optional<tau<tau_ba<BAs...>, BAs...>> snf_cmd(const tau<tau_ba<BAs...>, BAs...>& n);

	std::optional<tau<tau_ba<BAs...>, BAs...>> bf_substitute_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	std::optional<tau<tau_ba<BAs...>, BAs...>> substitute_cmd(const tau<tau_ba<BAs...>, BAs...>& n);
	std::optional<tau<tau_ba<BAs...>, BAs...>> instantiate_cmd(const tau<tau_ba<BAs...>, BAs...>& n);

	memorys m;
	options opt{};
	// TODO (MEDIUM) this dependency should be removed
	repl<repl_evaluator<BAs...>>* r = 0;
	rec_relations<tau<tau_ba<BAs...>, BAs...>> definitions;
	std::map<tau<tau_ba<BAs...>, BAs...>, std::pair<type, filename>> inputs;
	std::map<tau<tau_ba<BAs...>, BAs...>, std::pair<type, filename>> outputs;
	bool error = false;
	idni::term::colors TC{};
};

} // namespace idni::tau_lang

#include "repl_evaluator.tmpl.h"

#endif //__REPL_EVALUATOR_H__
