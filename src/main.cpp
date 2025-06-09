// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "init_log.h"
#include "runtime.h"
#include "hooks.h"
#include "normalizer.h"
#include "cli.h"
#include "repl.h"
#include "repl_evaluator.h"

//#include "base_bas/z3.h"
#include "base_bas/cvc5.h"

using namespace std;
using namespace idni;
using namespace idni::rewriter;
using namespace idni::tau_lang;

cli::options tau_options() {
	cli::options opts;
	opts["help"] = cli::option("help", 'h', false)
		.set_description("detailed information about options");
	opts["version"] = cli::option("version", 'v', false)
		.set_description("show the current Tau executable version");
	opts["license"] = cli::option("license", 'l', false)
		.set_description("show license for Tau");

	opts["charvar"] = cli::option("charvar", 'V', true)
		.set_description("charvar (enabled by default)");
	opts["severity"] = cli::option("severity", 'S', "info")
		.set_description("severity level (trace/debug/info/error)");
	opts["indenting"] = cli::option("indenting", 'I', false)
		.set_description("indenting of formulas");
	opts["highlighting"] = cli::option("highlighting", 'H', false)
		.set_description("syntax highlighting");
	// REPL specific options
	opts["evaluate"] = cli::option("evaluate", 'e', "")
		.set_description("REPL command to evaluate");
	opts["status"] = cli::option("status", 's', true)
		.set_description("display status");
	opts["color"] = cli::option("color", 'c', true)
		.set_description("use colors");
	DBG(opts["debug"] = cli::option("debug", 'd', true)
		.set_description("debug mode");)
	return opts;
}

int error(const string& s) {BOOST_LOG_TRIVIAL(error)<< "(Error) "<< s;return 1;}

int run_tau_spec(string spec_file, bool charvar) {
	string src = "";
	if (spec_file == "-") {
		std::ostringstream oss;
		oss << std::cin.rdbuf(), src = oss.str();
	} else {
		std::ifstream ifs(spec_file, std::ios::binary | std::ios::ate);
		if (!ifs) return error("Cannot open file " + spec_file);
		auto l = ifs.tellg();
		src.resize(l), ifs.seekg(0), ifs.read(&src[0], l);
	}
	if (src.empty()) return 0;
	repl_evaluator<sbf_ba> re({
		.print_memory_store = false,
		.error_quits        = true,
		.charvar            = charvar,
		.repl_running       = false
	});
	if (auto status = re.eval(src); status) return status;
	return re.eval("run %");
}

void welcome() {
	BOOST_LOG_TRIVIAL(info)
		<< "Welcome to the " << full_version << " by IDNI AG.\n"
		<< "This product is protected by patents and copyright."
			" By using this product, you agree to the license terms."
			" To view the license run \"tau --license\".\n\n"
		<< "For documentation, open issues and reporting issues "
			"please visit https://github.com/IDNI/tau-lang/\n\n"
		<< "For built-in help, type \"help\" or \"help command\".\n\n";
}

// TODO (MEDIUM) add command to read input file,...
int main(int argc, char** argv) {
	bdd_init<Bool>();
	bv_config();

	vector<string> args;
	for (int i = 0; i < argc; i++) args.push_back(argv[i]);

	cli cl("tau", args, {}, "", tau_options());
	cl.set_help_header("Usage: tau [ <specification file> ]");

	if (cl.process_args() != 0) return cl.status();
	auto opts  = cl.get_processed_options();
	auto files = cl.get_files();

	if (opts["help"].get<bool>()) return cl.help(), 0;
	if (opts["version"].get<bool>())
				return std::cout << full_version << "\n", 0;
	if (opts["license"].get<bool>()) return std::cout << license, 0;

	std::string sevstr = opts["severity"].get<string>();
	boost::log::trivial::severity_level sev =
		sevstr == "error" ? boost::log::trivial::error :
		sevstr == "trace" ? boost::log::trivial::trace :
		sevstr == "debug" ? boost::log::trivial::debug :
				boost::log::trivial::info;

	pretty_printer_highlighting = opts["highlighting"].get<bool>();
	pretty_printer_indenting    = opts["indenting"].get<bool>();

	bool charvar = opts["charvar"].get<bool>();
	std::set<std::string> guards{ charvar ? "charvar" : "var" };
	tau_parser::instance().get_grammar().set_enabled_productions(guards);
	sbf_parser::instance().get_grammar().set_enabled_productions(guards);

	// spec provided, run it
	if (files.size()) return run_tau_spec(files.front(), charvar);

	// REPL
	repl_evaluator<sbf_ba> re({
		.status = opts["status"].get<bool>(),
		.colors = opts["color"].get<bool>(),
		.charvar = charvar,
#ifdef DEBUG
		.debug_repl = opts["debug"].get<bool>(),
#endif // DEBUG
		.severity = sev
	});
	string e = opts["evaluate"].get<string>();
	if (e.size()) return re.eval(e), 0;
	repl<decltype(re)> r(re, "tau> ", ".tau_history");
	welcome();
	re.prompt();
	return r.run();
}
