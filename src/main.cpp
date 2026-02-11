// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef DEBUG
// including nso_ba, sbf_ba and interpreter directly instead of
// #include "tau.h" to avoid error lines pointing to a generated tau.h
#include "boolean_algebras/bv_ba.h"
#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/nso_ba.h"
#include "boolean_algebras/tau_ba.h"
#include "boolean_algebras/variant_ba.h"
#include "runtime.h"
#include "api.h"
#else
#	include "tau.h"
#endif // DEBUG
#include "repl_evaluator.h"
#include "utility/cli.h"

using namespace std;
using namespace idni;
using namespace idni::tau_lang;

using node_t = tau_lang::node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
using tau = tree<node_t>;
using tau_api = api<node_t>;

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
	opts["experimental"] = cli::option("experimental", 'x', false)
		.set_description("enables transitioning features");
	return opts;
}

int error(const string& s) { TAU_LOG_ERROR << "" << s; return 1; }

int run_tau_spec(string spec_file) {
	string src = "";
	if (spec_file == "-") {
		std::ostringstream oss;
		oss << std::cin.rdbuf(), src = oss.str();
	} else {
		DBG(TAU_LOG_TRACE << "open file: " << spec_file;)
		std::ifstream ifs(spec_file, std::ios::binary | std::ios::ate);
		if (!ifs) return error("Cannot open file " + spec_file);
		auto l = ifs.tellg();
		src.resize(l), ifs.seekg(0), ifs.read(&src[0], l);
	}
	if (src.empty()) return 0;

	auto maybe_i = tau_api::get_interpreter(src);
	if (!maybe_i) return 1;
	auto& i = maybe_i.value();
	while (true) {
		auto maybe_outputs = tau_api::step(i);
		if (!maybe_outputs) {
			TAU_LOG_INFO << "No input provided."
				<< " q or quit to terminate."
				<< " Press ENTER to continue.";
			std::string line;
			term::enable_getline_mode();
			std::getline(std::cin, line);
			term::disable_getline_mode();
			if (line == "q" || line == "quit") break;
		}
	}
	return 0;
}

void welcome() {
	TAU_LOG_INFO << "Welcome to the " << full_version << " by IDNI AG.\n"
		<< "This product is protected by patents and copyright."
			" By using this product, you agree to the license terms."
			" To view the license run \"tau --license\".\n\n"
		<< "For documentation, open issues and reporting issues "
			"please visit https://github.com/IDNI/tau-lang/\n\n"
		<< "For built-in help, type \"help\" or \"help command\".\n\n";
}

void at_exit() {
	// On exit we must clean up the ba constants so the rest of the static
	// constants managers are able to properly clean everything (in particula,
	// cvc5::TermManager).
	ba_constants<node_t>::cleanup();
	// On the tests using ba_constants we also need to explicitly call
	// the clean up in the constants.
}

// TODO (MEDIUM) add command to read input file,...
int main(int argc, char** argv) {
	atexit(at_exit);

	bdd_init<Bool>();

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


	tau_api::set_highlighting(opts["highlighting"].get<bool>());
	tau_api::set_indenting(opts["indenting"].get<bool>());
	bool charvar = opts["charvar"].get<bool>();
	bool exp = opts["experimental"].get<bool>();

	if (files.size()) {
		DBG(TAU_LOG_TRACE << "running specification file: "
							<< files.front();)
		tau_api::set_severity(sev);
		tau_api::set_charvar(charvar);
		return run_tau_spec(files.front());
	}

	repl_evaluator<bv, sbf_ba> re({
		.status = opts["status"].get<bool>(),
		.colors = opts["color"].get<bool>(),
		.charvar = charvar,
#ifdef DEBUG
		.debug_repl = opts["debug"].get<bool>(),
#endif // DEBUG
		.severity = sev,
		.experimental = exp
	});
	string e = opts["evaluate"].get<string>();
	if (e.size()) {
		DBG(TAU_LOG_TRACE << "evaluating REPL command: " << e;)
		return re.eval(e), 0;
	}
	DBG(TAU_LOG_TRACE << "running REPL";)
	repl<decltype(re)> r(re, "tau> ", ".tau_history");
	welcome();
	re.prompt();
	return r.run();

}
