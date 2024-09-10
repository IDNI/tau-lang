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

// General improvements

// TODO (IMPORTANT) semantic checks
	// TODO (HIGH) no capture allowed in Tau source
	// TODO (MEDIUM) type checkings
// TODO (MEDIUM) declare a static const lambda instead of a predicates if possible
// TODO (LOW) split code into h/cpp files
// TODO (IMPORTANT) error and runtime error handling
	// TODO (IMPORTANT) parsing errors reporting
	// TODO (IMPORTANT) semantic errors reporting
	// TODO (IMPORTANT) runtime errors reporting
	// TODO (IMPORTANT) internal errors reporting
// TODO (MEDIUM) simplify bindings API
// TODO (HIGH) allow typing in variables

// Documentation related tasks

// TODO (LOW) tau Language API documentation
	// TODO (LOW) rewriter
	// TODO (LOW) nso
	// TODO (LOW) tau
	// TODO (LOW) normalizer
	// TODO (LOW) satisfability
	// TODO (LOW) executor
	// TODO (LOW) CLI
	// TODO (VERY_LOW) CONTRIBUTING.md
	// TODO (VERY_LOW) CODE_OF_CONDUCT.md
	// TODO (VERY_LOW) LICENSE

// Tasks related to the Tau language rewriting rules and their use

// TODO (HIGH) the callback to remove quantifiers call builder_xor that relais
// on xor, which is removed at the beginning only. Change the builder_xor to
// use the xor definition directly

// TODO (HIGH) we apply too much rules. Review the apply methods to apply the
// trivialities directly in the apply methods instead of calling them as
// separate steps

// TODO (MEDIUM) change the execution API to use tuples instead of vectors
// so we could give more fine grained executions.

#include <iostream>
#include <fstream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "cli.h"
#include "repl.h"
#include "repl_evaluator.h"
#include "normalizer.h"
#include "bdd_binding.h"

using namespace std;
using namespace idni;
using namespace idni::rewriter;
using namespace idni::tau;
using namespace boost::log;

cli::commands tau_commands() {
	cli::commands cmds;
	cmds["help"] = cli::command("help",
		"detailed information about options");
	auto& run = cmds["run"] = cli::command("run",
		"run a tau program");
	run.add_option(cli::option("program", 'p', "@stdin")
		.set_description("program to run"));
	run.add_option(cli::option("evaluate", 'e', "")
		.set_description("program to be evaluated (alternative to -p)"));
	run.add_option(cli::option("help", 'h', false)
		.set_description("detailed information about run options"));
	run.add_option(cli::option("input", 'i', "@null")
		.set_description("program's input"));
	run.add_option(cli::option("output", 'o', "@stdout")
		.set_description("program's output"));
	auto& repl = cmds["repl"] = cli::command("repl", "Tau REPL");
	repl.add_option(cli::option("help", 'h', false)
		.set_description("detailed information about repl options"));
	repl.add_option(cli::option("evaluate", 'e', "")
		.set_description("repl command to evaluate"));
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

// runs tau program or an evaluate string using input and output
int run_tau(const string& program, const string& input, const string& output,
	const string& evaluate = "")
{
	bool eval = evaluate.size();
	if (input == output && !is_null(input))
		return error("input and output cannot be the same");
	if (eval && program.size() && !is_null(program) && !is_stdin(program))
		return error("cannot use both --program and --evaluate");
	if (!eval) {
		if (program == input)
			return error("program and input cannot be the same");
		if (program == output)
			return error("program and output cannot be the same");
	}

	sp_tau_source_node prg_node, in_node, out_node;

	if (eval) prg_node = make_tau_source(evaluate);
	else if (is_null(program)) return error("program cannot be null");
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

void init_logging() {
	core::get()->set_filter(trivial::severity >= trivial::debug);
	add_console_log(cout, keywords::format =
		expressions::stream << expressions::smessage);
}

// TODO (MEDIUM) add command to read input file,...
int main(int argc, char** argv) {
	init_logging();
	bdd_init<Bool>();

	vector<string> args;
	for (int i = 0; i < argc; i++) args.push_back(argv[i]);

	cli cl("tau", args, tau_commands(), "repl", tau_options());
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

	// repl command
	if (cmd.name() == "repl") {
		string e = cmd.get<string>("evaluate");
		tau_bdd_binding_factory f;
		repl_evaluator<tau_bdd_binding_factory, bdd_binding> re(f);
		if (e.size()) return re.eval(e), 0;
		repl<decltype(re)> r(re, "tau> ", ".tau_history");
		re.prompt();
		return r.run();
	}

	// run command
	if (cmd.name() == "run") return run_tau(
		cmd.get<string>("program"),
		cmd.get<string>("input"),
		cmd.get<string>("output"),
		cmd.get<string>("evaluate"));
	return 0;
}
