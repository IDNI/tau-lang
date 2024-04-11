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
// TODO (HIGH) use only required parenthesis in the output or at least be able
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

namespace idni::tau {

template <typename factory_t, typename... BAs>
struct repl_evaluator {
	friend struct repl<repl_evaluator<factory_t, BAs...>>;
	// TODO (MEDIUM) remove variant as only nso are needed
	using output = std::variant<
		nso<tau_ba<BAs...>, BAs...>,
		rr<nso<tau_ba<BAs...>, BAs...>>>;
	using outputs = std::vector<output>;

	using output_ref = std::optional<std::pair<
				sp_tau_node<tau_ba<BAs...>, BAs...>, size_t>>;

	struct options {
		options() {};
		bool status     = true;
		bool colors     = true;
		bool debug_repl =
#ifdef DEBUG
					true;
#else
					false;
#endif
		boost::log::trivial::severity_level
			severity = boost::log::trivial::error;
	};

	repl_evaluator(factory_t& factory, options opt = options{});
	int eval(const std::string& src);
	std::string prompt();

private:

	repl_evaluator<factory_t, BAs...>::output_ref get_output_ref(
		const sp_tau_node<tau_ba<BAs...>, BAs...>& n,
		bool silent = false);
	void print_output(size_t id);

	void list_outputs_cmd();
	void clear_outputs_cmd();
	void store_output(repl_evaluator<factory_t, BAs...>::output o);

	std::optional<nso<tau_ba<BAs...>, BAs...>> get_bf(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> get_wff(
		const nso<tau_ba<BAs...>, BAs...>& n);

	std::optional<nso<tau_ba<BAs...>, BAs...>> wff_onf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> wff_dnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> wff_cnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> wff_nnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> wff_mnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);

	std::optional<nso<tau_ba<BAs...>, BAs...>> bf_dnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> bf_cnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> bf_nnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> bf_mnf_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);

	std::optional<nso<tau_ba<BAs...>, BAs...>> bf_substitute_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> wff_substitute_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> bf_instantiate_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);
	std::optional<nso<tau_ba<BAs...>, BAs...>> wff_instantiate_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);


	std::optional<nso<tau_ba<BAs...>, BAs...>> normalizer_cmd(
		const nso<tau_ba<BAs...>, BAs...>& n);

	int eval_cmd(const sp_tau_node<tau_ba<BAs...>, BAs...>& n);
	void print_output_cmd(
		const sp_tau_node<tau_ba<BAs...>, BAs...>& command);
	void get_cmd(sp_tau_node<tau_ba<BAs...>, BAs...> n);
	void set_cmd(sp_tau_node<tau_ba<BAs...>, BAs...> n);
	void toggle_cmd(const sp_tau_node<tau_ba<BAs...>, BAs...>& n);

	sp_tau_node<tau_ba<BAs...>, BAs...> make_cli(const std::string& src);
	size_t digits(sp_tau_node<tau_ba<BAs...>, BAs...> n);


	void version_cmd();
	void help_cmd(const nso<tau_ba<BAs...>, BAs...>& n);
	void not_implemented_yet();

	outputs m;
	factory_t factory;
	options opt{};
	repl<repl_evaluator<factory_t, BAs...>>* r = 0;
	idni::term::colors TC{};
	bool error = false;
};

} //idni::tau namespace

#include "repl_evaluator.tmpl.h"

#endif //__REPL_EVALUATOR_H__
