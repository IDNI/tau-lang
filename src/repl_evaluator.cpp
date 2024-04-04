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

#include "defs.h" // for GIT_DESCRIBED
#include "repl_evaluator.h"

namespace idni::tau {

void version() { cout << "Tau version: " << GIT_DESCRIBED << "\n"; }

// TODO (LOW) write proper help messages
void help(size_t nt) {
	static const std::string bool_options =
		"  status                 show status        on/off\n"
		"  colors                 use term colors    on/off\n";
	static const std::string all_available_options = std::string{} +
		"Available options:\n" + bool_options +
		"  severity               severity           error/info/debug/trace\n";
	static const std::string bool_available_options = std::string{} +
		"Available options:\n" + bool_options;
	switch (nt) {
	case tau_parser::help_cmd_sym: cout
		<< "Commands:\n"
		<< "  help or h              print this help\n"
		<< "  quit, q, exit or e     exit the repl\n"
		<< "  version or v           print version\n"
		<< "\n"
		<< "Formula commands:\n"
		<< "  normalize or n         normalize formula\n"

		// not yet publicly available

		/*<< "  onf_cmd                    convert to order normal form\n"
		<< "  dnf                    convert to disjunctive normal form\n"
		<< "  cnf                    convert to conjunctive normal form\n"
		<< "  anf                    convert to algebraic normal form\n"
		<< "  nnf                    convert to negation normal form\n"
		<< "  pnf                    convert to prenex normal form\n"
		<< "  minterm                convert to minterm normal form\n"*/

		<< "\n"
		<< "Output commands:\n"
		<< "  output or o            output control\n"
		<< "  &                      output_ref. or short alias of output command\n"
		<< "  %                      output ref. or relative output control\n"
		<< "\n"
		<< "Settings commands:\n"
		<< "  get                    get options' values\n"
		<< "  set                    set option's value\n"
		<< "  toggle                 toggle option's value\n";
		break;
	case tau_parser::version_cmd_sym: cout
		<< "version or v prints out current Tau commit id\n";
		break;
	case tau_parser::quit_cmd_sym: cout
		<< "q, quit, e or exit exits the repl\n";
		break;
	case tau_parser::output_sym: cout
		<< "output or out command manages outputs of previous commands\n"
		<< "\n"
		<< "  output                 lists all outputs\n"
		<< "  output <output_id>     prints output\n"
		<< "  output clear           clears output memory\n";
		break;
	case tau_parser::absolute_output_sym: cout
		<< "& is a short alias of output command\n"
		<< "\n"
		<< "  &                      lists all outputs\n"
		<< "  &clear                 clears output memory\n"
		<< "  &<output_id>           is a reference to a previous output\n"
		<< "                         if used as a command it prints the output\n";
		break;
	case tau_parser::relative_output_sym: cout
		<< "% same as & but output_id is relative to the latest\n"
		<< "\n"
		<< "  %                      lists all outputs\n"
		<< "  %clear                 clears output memory\n"
		<< "  %<relative_output_id>  is a reference to a previous output\n"
		<< "                         if used as a command it prints the output\n";
		break;
	case tau_parser::selection_sym: cout
		<< "Command s, selection ...\n";
		break;
	case tau_parser::instantiate_cmd_sym: cout
		<< "Command i, instantiate ...\n";
		break;
	case tau_parser::substitute_cmd_sym: cout
		<< "Command s, substitute ...\n";
		break;
	case tau_parser::normalize_cmd_sym: cout
		<< "normalize or n command normalizes a formula, prints it and\n"
		<< "saves it into memory of previous outputs\n"
		<< "\n"
		<< "usage:\n"
		<< "  normalize '<NSORR>'    normalizes the given NSO RR\n"
		<< "  normalize '<WFF>'      normalizes the given WFF formula\n"
		<< "  normalize <output>     normalizes the output with the given id\n";
		break;
	case tau_parser::file_cmd_sym: cout
		<< "Command r, read ...\n";
		break;
	case tau_parser::get_cmd_sym: cout
		<< "get if used alone prints all options and their values\n"
		<< "get <option>        prints the value of the given option\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau_parser::set_cmd_sym: cout
		<< "set <option> [=] <value> sets option value\n"
		<< "\n"
		<< all_available_options;
		break;
	case tau_parser::toggle_cmd_sym: cout
		<< "toggle <option> toggles option value\n"
		<< "\n"
		<< bool_available_options;
		break;
	}
}

void not_implemented_yet() {
	std::cout << "Not implemented yet.\n";
}

} // idni::tau namespace
