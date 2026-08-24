// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

#include "tau_pack.h"

#ifdef DEBUG
// including nso_ba, sbf_ba and interpreter directly instead of
// #include "tau.h" to avoid error lines pointing to a generated tau.h
#include "boolean_algebras/nso_ba.h"
#include "boolean_algebras/variant_ba.h"
#include "base_ba_dispatcher.h"
#include "api.h"
#else
#	include "tau.h"
#endif // DEBUG
#include "repl_evaluator.h"
#include "tau_compile.h"
#include "utility/cli.h"

using namespace std;
using namespace idni;
using namespace idni::tau_lang;

using node_t = tau_lang::tau_pack::node_t;
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
	opts["blasting"] = cli::option("blasting", 'B', true)
		.set_description("blasting (enabled by default)");
	opts["severity"] = cli::option("severity", 'S', "info")
		.set_description("severity level (trace/debug/info/error)");
	opts["indenting"] = cli::option("indenting", 'I', false)
		.set_description("indenting of formulas");
	opts["highlighting"] = cli::option("highlighting", 'H', false)
		.set_description("syntax highlighting");
	opts["benchmarks"] = cli::option("benchmarks", 'b', true)
		.set_description("print benchmarks (enabled by default)");
	opts["json"] = cli::option("json", 'J', false)
		.set_description("output in JSON format");
	opts["quit"] = cli::option("quit", 'q', false)
		.set_description("quit when no input");
	// REPL specific options
	opts["evaluate"] = cli::option("evaluate", 'e', "")
		.set_description("REPL command to evaluate");
	opts["legacy-repl"] = cli::option("legacy-repl", 'X', false)
		.set_description("use legacy terminal REPL instead of FTXUI");
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

cli::commands tau_commands() {
	cli::commands cs;
	cli::command compile("compile",
		"compiles a Tau spec file into a standalone executable");
	compile.add_option(cli::option("output", 'o', "")
		.set_description("output executable path (default: spec "
			"file path without extension)"));
	cs[compile.name()] = compile;
	return cs;
}

int error(const string& s) { TAU_LOG_ERROR << "" << s; return 1; }

int run_tau_spec(string spec_file, cli::options& opts) {
	measuring m("run");
	idni::measures::timer t;
	string src;
	t.start();
	auto result = [&](int r) {
		m.ms = t.stop();
		if (opts["benchmarks"].get<bool>()) m(std::cerr);
		return r;
	};
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
	m.part() = { "reading input", t.pause() };
	if (src.empty()) return result(0);
	t.unpause();
	auto maybe_i = tau_api::get_interpreter(m.part(), src);
	if (!maybe_i) return result(1);
	auto& i = maybe_i.value();
	bool quit_on_idle = opts["quit"].get<bool>();
	bool run_ok = tau_api::run(m.part(), i, quit_on_idle);
	if (quit_on_idle) TAU_LOG_INFO << "No more inputs provided."
		<< " Terminating.";
	return result(run_ok ? 0 : 1);
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

// TODO (MEDIUM) add command to read input file,...
int main(int argc, char** argv) {
	bdd_init<Bool>();

	vector<string> args;
	for (int i = 0; i < argc; i++) args.push_back(argv[i]);

	cli cl("tau", args, tau_commands(), "", tau_options());
	cl.set_help_header("Usage: tau [ <specification file> ]");

	if (cl.process_args() != 0) return cl.status();
	auto cmd = cl.get_processed_command();

	if (cmd.ok() && cmd.name() == "compile") {
		auto files = cl.get_files();
		if (files.empty())
			return error("Usage: tau compile <spec.tau> [-o out_exe]");
		std::string spec_file = files.front();
		std::string out_exe = cmd.get<std::string>("output");
		if (out_exe.empty()) {
			std::filesystem::path p(spec_file);
			out_exe = (p.parent_path() / p.stem()).string();
		}

		std::string src;
		std::ifstream ifs(spec_file, std::ios::binary | std::ios::ate);
		if (!ifs) return error("Cannot open file: " + spec_file);
		auto l = ifs.tellg();
		src.resize(l); ifs.seekg(0); ifs.read(&src[0], l);
		if (src.empty()) return error("Spec file is empty: " + spec_file);

		std::string build_dir = spec_file + ".build";
		TAU_LOG_INFO << "tau compile: " << spec_file;
		auto res = compile_spec<node_t>(src, out_exe, build_dir);
		if (!res.ok()) {
			TAU_LOG_ERROR << "compile failed: " << res.error;
			return 1;
		}
		TAU_LOG_INFO << "compiled: " << res.exe_path;
		return 0;
	}

	auto opts  = cl.get_processed_options();
	auto files = cl.get_files();

	if (opts["help"].get<bool>()) return cl.help(), 0;
	if (opts["version"].get<bool>())
		return std::cout << full_version << "\n"
			<< "algebras: " << node_t::ba::types_joined() << "\n", 0;
	if (opts["license"].get<bool>()) return std::cout << license, 0;

	std::string sevstr = opts["severity"].get<string>();
	boost::log::trivial::severity_level sev =
		sevstr == "error" ? boost::log::trivial::error :
		sevstr == "trace" ? boost::log::trivial::trace :
		sevstr == "debug" ? boost::log::trivial::debug :
				boost::log::trivial::info;


	tau_api::set_highlighting(opts["highlighting"].get<bool>());
	tau_api::set_indenting(opts["indenting"].get<bool>());
	tau_api::set_json(opts["json"].get<bool>());
	bool charvar = opts["charvar"].get<bool>();
	bool blasting = opts["blasting"].get<bool>();
	bool exp = opts["experimental"].get<bool>();

	if (files.size()) {
		DBG(TAU_LOG_TRACE << "running specification file: "
			<< files.front();)
		tau_api::set_severity(sev);
		tau_api::set_charvar(charvar);
		return run_tau_spec(files.front(), opts);
	}

	repl_evaluator<TAU_PACK_BASE_BAS> re({
		.status = opts["status"].get<bool>(),
		.colors = opts["color"].get<bool>(),
		.charvar = charvar,
		.blasting = blasting,
		.print_benchmarks = opts["benchmarks"].get<bool>(),
#ifdef DEBUG
		.debug_repl = opts["debug"].get<bool>(),
#endif // DEBUG
		.severity = sev,
		.experimental = exp
	});
	string e = opts["evaluate"].get<string>();
	if (e.size()) {
		DBG(TAU_LOG_TRACE << "evaluating REPL command: " << e;)
		return re.eval(e);
	}
	DBG(TAU_LOG_TRACE << "running REPL";)
	welcome();
#ifdef TAU_PARSER_HAS_FTXUI
	if (!opts["legacy-repl"].get<bool>()) {
		repl_ftxui<decltype(re)> rftx(re, "tau> ", ".tau_history");
		re.reprompt();
		return rftx.run();
	}
#endif
	repl<decltype(re)> r(re, "tau> ", ".tau_history");
	re.reprompt();
	return r.run();

}
