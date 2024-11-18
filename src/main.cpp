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
#include <chrono>
#include <iomanip>
#include <sstream>

#include "init_log.h"
#include "cli.h"
#include "repl.h"
#include "repl_evaluator.h"
#include "normalizer.h"
#include "bdd_binding.h"

using namespace std;
using namespace idni;
using namespace idni::rewriter;
using namespace idni::tau;

cli::commands tau_commands() {
	cli::commands cmds;
	cmds["help"] = cli::command("help",
		"detailed information about options");

	auto& run = cmds["run"] = cli::command("run",
		"run a tau program");

	// common options used by multiple commands
	auto indenting_opt = cli::option("indenting", 'I', false)
		.set_description("indenting of formulas");
	auto highlighting_opt = cli::option("highlighting", 'H', false)
		.set_description("syntax highlighting");
	auto severity_opt = cli::option("severity", 'S', "info")
		.set_description("severity level (trace/debug/info/error)");
	auto charvar_opt = cli::option("charvar", 'V', true)
		.set_description("charvar (enabled by default)");
#ifdef DEBUG
	auto debug_opt = cli::option("debug", 'd', true)
				.set_description("debug mode");
	run.add_option(debug_opt);
#endif // DEBUG
	run.add_option(severity_opt);
	run.add_option(indenting_opt);
	run.add_option(highlighting_opt);
	run.add_option(charvar_opt);
	run.add_option(cli::option("program", 'p', "@stdin")
		.set_description("program to run"));
	run.add_option(cli::option("evaluate", 'e', "")
		.set_description("program to be evaluated (alternative to -p)"));
	run.add_option(cli::option("help", 'h', false)
		.set_description("detailed information about run options"));
	auto& repl = cmds["repl"] = cli::command("repl", "Tau REPL");
	DBG(repl.add_option(debug_opt);)
	repl.add_option(severity_opt);
	repl.add_option(indenting_opt);
	repl.add_option(highlighting_opt);
	repl.add_option(charvar_opt);
	repl.add_option(cli::option("help", 'h', false)
		.set_description("detailed information about repl options"));
	repl.add_option(cli::option("evaluate", 'e', "")
		.set_description("repl command to evaluate"));
	repl.add_option(cli::option("status", 's', true)
		.set_description("display status"));
	repl.add_option(cli::option("color", 'c', true)
		.set_description("use colors"));

	return cmds;
}

cli::options tau_options() {
	cli::options opts;
	opts["help"] = cli::option("help", 'h', false)
		.set_description("detailed information about options");
	opts["version"] = cli::option("version", 'v', false)
		.set_description("show the current Tau executable version");
	opts["license"] = cli::option("license", 'l', false)
		.set_description("show license for Tau");
	return opts;
}

bool is_stdin(const string& s)  { return s == "@stdin" || s == "-"; }
bool is_stdout(const string& s) { return s == "@stdout"; }
bool is_null(const string& s)   { return s == "@null"; }
int error(const string& s) {
	BOOST_LOG_TRIVIAL(error) << "(Error) " << s;
	return 1;
}

// runs tau program or an evaluate string using input and output
int run_tau(const cli::command& cmd, const vector<string>& files) {
	string program = cmd.get<string>("program"),
		e = cmd.get<string>("evaluate");
	if (e.size()&& program.size()&& !is_null(program) && !is_stdin(program))
		return error("Cannot use both --program and --evaluate");
	if (e.empty()) {
		if (files.size()) program = files[0];
		if (is_null(program)) return error("Program cannot be null");
		if (is_stdin(program)) {
			std::ostringstream oss;
			oss << std::cin.rdbuf(), e = oss.str();
		} else {
			std::ifstream ifs(program,
					std::ios::binary | std::ios::ate);
			if (!ifs) return error("Cannot open file " + program);
			auto l = ifs.tellg();
			e.resize(l), ifs.seekg(0), ifs.read(&e[0], l);
		}
	}

	repl_evaluator<bdd_binding> re({ .print_memory_store = false,
					.error_quits = true,
					.charvar = cmd.get<bool>("charvar"),
					.repl_running = false });
	if (e.empty()) return 0;
	if (auto status = re.eval(e); status) return status;
	return re.eval("run %");
}

// TODO (MEDIUM) add command to read input file,...
int main(int argc, char** argv) {
	bdd_init<Bool>();

	vector<string> args;
	for (int i = 0; i < argc; i++) args.push_back(argv[i]);

	// get compile date
    const char* date = __DATE__;
    std::string month_str = std::string(date, 3);
    std::string day_str = std::string(date + 4, 2);
    std::string year_str = std::string(date + 7, 4);
    std::map<std::string, std::string> month_map = {
        {"Jan", "01"}, {"Feb", "02"}, {"Mar", "03"},
        {"Apr", "04"}, {"May", "05"}, {"Jun", "06"},
        {"Jul", "07"}, {"Aug", "08"}, {"Sep", "09"},
        {"Oct", "10"}, {"Nov", "11"}, {"Dec", "12"}
    };

    std::ostringstream oss;
    oss << year_str << "-" << month_map[month_str] << "-" << day_str;

	BOOST_LOG_TRIVIAL(info)
		<< "Welcome to the Tau Language Framework Alpha version 0.7 (" << oss.str() << " build " << GIT_COMMIT_HASH << ") by IDNI AG. "
		<< "This product is protected by patents and copyright. By using this product, you agree to the license terms. "
		<< "To view the license run \"tau --license\".\n\n"
		<< "For documentation, open issues and reporting issues please visit https://github.com/IDNI/tau-lang/\n\n"
		<< "For built-in help, type \"help\" or \"help command\".\n\n";

	cli cl("tau", args, tau_commands(), "repl", tau_options());
	cl.set_description("Tau language");
	cl.set_default_command_when_files("run");

	if (cl.process_args() != 0) return cl.status();

	auto opts  = cl.get_processed_options();
	auto cmd   = cl.get_processed_command();
	auto files = cl.get_files();

	// error if command is invalid
	if (!cmd.ok()) {
		if (cmd.name() == "repl" && files.size())
			return error("repl command does not accept files");
		return cl.error("invalid command", true);
	}
	// if --help/-h option is true, print help end exit
	if (cmd.name() == "help" || opts["help"].get<bool>())
		return cl.help(), 0;

	// if --version/-v option is true, print version and exit
	if (opts["version"].get<bool>())
		return cl.version(), 0;

	if (opts["license"].get<bool>())
		return cl.license(), 0;

	// if cmd's --help/-h option is true, print cmd's help and exit
	if (cmd.get<bool>("help")) return cl.help(cmd), 0;

	// set charvar
	bool charvar = cmd.get<bool>("charvar");
	std::set<std::string> guards{ charvar ? "charvar" : "var" };
	tau_parser::instance().get_grammar().set_enabled_productions(guards);
	bdd_parser::instance().get_grammar().set_enabled_productions(guards);

	pretty_printer_highlighting = cmd.get<bool>("highlighting");
	pretty_printer_indenting = cmd.get<bool>("indenting");

	std::string sevstr = cmd.get<string>("severity");
	boost::log::trivial::severity_level sev =
		sevstr == "error" ? boost::log::trivial::error :
		sevstr == "trace" ? boost::log::trivial::trace :
		sevstr == "debug" ? boost::log::trivial::debug :
		boost::log::trivial::info;

	// repl command
	if (cmd.name() == "repl") {
		string e = cmd.get<string>("evaluate");
		repl_evaluator<bdd_binding> re({
			.status = cmd.get<bool>("status"),
			.colors = cmd.get<bool>("color"),
			.charvar = charvar,
#ifdef DEBUG
			.debug_repl = cmd.get<bool>("debug"),
#endif // DEBUG
			.severity = sev
		});
		if (e.size()) return re.eval(e), 0;
		repl<decltype(re)> r(re, "tau> ", ".tau_history");
		re.prompt();
		return r.run();
	}

	// run command
	if (cmd.name() == "run") return run_tau(cmd, files);
	return 0;
}
