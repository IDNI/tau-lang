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

#include <iostream>
#include <fstream>

#include "cli.h"
#include "normalizer2.h"

using namespace std;
using namespace idni;
using namespace idni::tau;
using namespace idni::rewriter;

// Trought all the code we would use the following tags to denote different
// types of tasks or notes.
// We could use TODO, DOING, IDEA, FIXME, REVIEW, DOCUMENTATION and MARK. Also,
// we could give a priority to each task by adding HIGH, LOW or MEDIUM between
// parenthesis.
//
// We use the subtags as follows:
// - (IMPORTANT) for main tasks of the project,
// - (HIGH) for tasks that should be done asap as they are important missing
//   functionallity or important improvements,
// - (MEDIUM) for tasks that should be done soon as they are but they are not
//   missing functionallity or important improvements, mostly related to code
//   quality and testing,
// - (LOW) for tasks that should be done at some point but they are not
//   important, mostly related to code quality and testing but not as important as
//   MEDIUM tasks,
// - (VERY LOW) for tasks that should be done at some point but they are not
//   important, mostly related to code quality and testing but not as important as
//   LOW tasks,

//
// Global tasks
//

// TODO (IMPORTANT) normalizer

// TODO (IMPORTANT) satisfability

// TODO (IMPORTANT) execution

// TODO (IMPORTANT) basic UI and I/O api

// TODO (IMPORTANT) basic documentation

// TODO (IMPORTANT) error and runtime error handling

// TODO (IMPORTANT) basic logics (implementation, splitter,...)
// TODO (IMPORTANT) propositional logic
// TODO (IMPORTANT) tau BA

// TODO (IMPORTANT) semantic checks

// TODO (IMPORTANT) cryptographic support

// TODO (IMPORTANT) finite boolean algebras and arithmetic
// - Check SMT coompetition (best seems to be CVC5)
// - SMT-Lib format allow us to interact with several solvers, using the best
//   one for each task.
// - We would use them bit vectors and integer arithmetic (possibly Z3 for bit vectors for finite BA)
// - We could also use them for quantifier free formulas.
//
// Further info in https://en.wikipedia.org/wiki/Satisfiability_modulo_theories#Standardization_and_the_SMT-COMP_solver_competition

// TODO (IMPORTANT) blockchain support
// - acceptance conditions
// - selfamending network
// - user consensus

//
// General tasks that are not related to any specific file.
//


// TODO (MEDIUM) declare a static const lambda instead of a predicates if possible

// TODO (MEDIUM) declare a static const lambda instead of stateless struct possible

// TODO (LOW) split code into h/cpp files

// TODO (MEDIUM) final features should be cotrolled by a command line flag
//
// toggles for debugging, comment properly to enable/disable each feature/debug helper.

// TODO (LOW) review proper use of bf_subs_cb in code

// DOCUMENTATION (LOW) add documentation to the public parts of the code
//
// Just convert the current comments into proper doxygen description.

cli::commands tau_commands() {
	cli::commands cmds;
	cmds["help"] = cli::command("help",
		"detailed information about options");
	auto& run = cmds["run"] = cli::command("run",
		"run a tau program");
	run.add_option(cli::option("program", 'p', "@stdin")
		.set_description("program to run"));
	run.add_option(cli::option("help", 'h', false)
		.set_description("detailed information about run options"));
	run.add_option(cli::option("input", 'i', "@null")
		.set_description("program's input"));
	run.add_option(cli::option("output", 'o', "@stdout")
		.set_description("program's output"));
	return cmds;
}

cli::options tau_options() {
	cli::options opts;
	opts["help"] = cli::option("help", 'h', false)
		.set_description("detailed information about options");
	return opts;
}

bool is_stdin(const string& s)  { return s == "@stdin" || s == "-"; }
bool is_stdout(const string& s) { return s == "@stdout"; }
bool is_null(const string& s)   { return s == "@null"; }
int error(const string& s) {
	cerr << s << "\n";
	return 1;
}
int run_tau(const string& program, const string& input, const string& output) {
	if (program == input)
		return error("program and input cannot be the same");
	else if (program == output)
		return error("program and output cannot be the same");
	else if (input == output && !is_null(input))
		return error("input and output cannot be the same");

	sp_tau_source_node prg_node, in_node, out_node;

	if (is_null(program)) return error("program cannot be null");
	else if (is_stdin(program)) prg_node = make_tau_source(cin);
	else prg_node = make_tau_source_from_file(program);

	if (is_null(input)) ;
	else if (is_stdin(input)) in_node = make_tau_source(cin);
	else in_node = make_tau_source_from_file(input);

	// read input
	if (in_node) {
		DBG(cout << "input: `" << in_node << "`\n");
	}

	// read program
	if (!prg_node) return error("cannot read program");
	DBG(cout << "program: `" << prg_node << "`\n");

	// run program
	// TODO (MEDIUM) program execution
	out_node = prg_node; // simulate execution by setting out_node to prg

	ostream *out = 0;
	ofstream outf;
	if (is_null(output)) ;
	else if (is_stdout(output)) out = &cout;
	else if (!(outf = ofstream(output)).is_open())
		return error("cannot open output file");
	else out = &outf;

	// write output
	if (out && out_node) {
		DBG(cout << "output: `" << out_node << "`\n");
		*out << out_node;
	}

	return 0;
}

// TODO (MEDIUM) add command to read input file,...
int main(int argc, char** argv) {
	vector<string> args;
	for (int i = 0; i < argc; i++) args.push_back(argv[i]);

	cli cl("tau", args, tau_commands(), "run", tau_options());
	cl.set_description("Tau language");

	if (cl.process_args() != 0) return cl.status();

	auto opts = cl.get_processed_options();
	auto cmd  = cl.get_processed_command();

	// error if command is invalid
	if (!cmd.ok()) return cl.error("invalid command", true);

	// if --help/-h option is true, print help end exit
	if (cmd.name() == "help" || opts["help"].get<bool>())
		return cl.help(), 0;

	// if cmd's --help/-h option is true, print cmd's help and exit
	if (cmd.get<bool>("help")) return cl.help(cmd), 0;

	// run command
	if (cmd.name() == "run") return run_tau(
		cmd.get<string>("program"),
		cmd.get<string>("input"),
		cmd.get<string>("output"));

	return 0;
}
