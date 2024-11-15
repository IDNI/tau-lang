// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

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

#include "bdd_binding.h"
#include "repl.h"
#include "interpreter.h"

namespace idni::tau {

template <typename... BAs>
struct repl_evaluator {
	friend struct repl<repl_evaluator<BAs...>>;
	using memory = nso<tau_ba<BAs...>, BAs...>;
	using memorys = std::vector<memory>;
	using memory_ref = std::optional<std::pair<
				sp_tau_node<tau_ba<BAs...>, BAs...>, size_t>>;

	struct options {
		bool status             = true;
		bool colors             = true;
		bool print_memory_store = true;
		bool error_quits        = false;
		bool charvar            = true;
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

	int eval_cmd(const sp_tau_node<tau_ba<BAs...>, BAs...>& n);
	bool update_charvar(bool value);

	// helpers
	sp_tau_node<tau_ba<BAs...>, BAs...> make_cli(const std::string& src);
	boost::log::trivial::severity_level nt2severity(size_t nt) const;
	repl_evaluator<BAs...>::memory_ref memory_retrieve(
		const sp_tau_node<tau_ba<BAs...>, BAs...>& n,
		bool silent = false);
	void memory_store(repl_evaluator<BAs...>::memory o);
	std::optional<std::pair<size_t, nso<tau_ba<BAs...>, BAs...>>>
		get_type_and_arg(const nso<tau_ba<BAs...>, BAs...>& n);
	// returns nonterminal type of a node (as size_t and no value if not nt
	std::optional<size_t> get_type(const nso<tau_ba<BAs...>, BAs...>& n);
	// returns if a subtree of a node contains a nonterminal
	bool contains(const nso<tau_ba<BAs...>, BAs...>& n,
		tau_parser::nonterminal nt);
	// apply definitions and rr to a program
	gssotc<BAs...> apply_rr_to_rr_gssotc(const size_t type, const gssotc<BAs...>& program);

	// commands
	void not_implemented_yet();

	void version_cmd();
	void help_cmd(const nso<tau_ba<BAs...>, BAs...>& n);

	void get_cmd(sp_tau_node<tau_ba<BAs...>, BAs...> n);
	void set_cmd(sp_tau_node<tau_ba<BAs...>, BAs...> n);
	void update_bool_opt_cmd(const sp_tau_node<tau_ba<BAs...>, BAs...>& n,
		const std::function<bool(bool&)>& update_fn);

	void history_print_cmd(const sp_tau_node<tau_ba<BAs...>, BAs...>& command);
	void history_store_cmd(const sp_tau_node<tau_ba<BAs...>, BAs...>& command);
	void history_list_cmd();

	void def_rr_cmd(const nso<tau_ba<BAs...>, BAs...>& n);
	void def_print_cmd(const nso<tau_ba<BAs...>, BAs...>& n);
	void def_list_cmd();
	void def_input_cmd(const nso<tau_ba<BAs...>, BAs...>& n);
	void def_output_cmd(const nso<tau_ba<BAs...>, BAs...>& n);

	std::optional<nso<tau_ba<BAs...>, BAs...>> qelim_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);

	std::optional<nso<tau_ba<BAs...>, BAs...>> normalize_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);

	std::optional<nso<tau_ba<BAs...>, BAs...>> sat_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	void run_cmd(const nso<tau_ba<BAs...>, BAs...>& n);
	void solve_cmd(const nso<tau_ba<BAs...>, BAs...>& n);

	std::optional<nso<tau_ba<BAs...>, BAs...> > is_valid_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);

	std::optional<nso<tau_ba<BAs...>, BAs...> > is_unsatisfiable_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);

	std::optional<nso<tau_ba<BAs...>, BAs...>> get_bf(
		const nso<tau_ba<BAs...>, BAs...>& n, bool suppress_error=false);
	std::optional<nso<tau_ba<BAs...>, BAs...>> get_wff(
		const nso<tau_ba<BAs...>, BAs...>& n);

	std::optional<nso<tau_ba<BAs...>, BAs...>> onf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> dnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> cnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> nnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> mnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> snf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);

	std::optional<nso<tau_ba<BAs...>, BAs...>> bf_substitute_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> substitute_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> instantiate_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);

	memorys m;
	options opt{};
	// TODO (MEDIUM) this dependency should be removed
	repl<repl_evaluator<BAs...>>* r = 0;
	rec_relations<gssotc<BAs...>> definitions;
	std::map<nso<tau_ba<BAs...>, BAs...>, std::pair<type, filename>> inputs;
	std::map<nso<tau_ba<BAs...>, BAs...>, std::pair<type, filename>> outputs;
	bool error = false;
	idni::term::colors TC{};
};

} //idni::tau namespace

#include "repl_evaluator.tmpl.h"

#endif //__REPL_EVALUATOR_H__
