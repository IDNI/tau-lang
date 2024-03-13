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

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#include "bdd_binding.h"
#include "repl.h"

namespace idni::tau {

template <typename... BAs>
struct repl_evaluator {
	struct options {
		options() {};
		bool status  = true;
		bool colors  = true;
		boost::log::trivial::severity_level
			severity = boost::log::trivial::error;
	} opt;
	using output = std::variant<
		nso<tau_ba<bdd_binding>, bdd_binding>,
		rr<nso<tau_ba<bdd_binding>, bdd_binding>>>;
	using outputs = std::vector<output>;

	repl_evaluator(options opt = options{});
	void set_repl(repl<repl_evaluator<BAs...>>& r_);
	int eval(const std::string& src);

	outputs m;
	repl<repl_evaluator<BAs...>>* r = 0;
};

} //idni::tau namespace
#include "repl_evaluator.tmpl.h"
#endif //__REPL_EVALUATOR_H__
