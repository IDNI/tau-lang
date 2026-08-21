// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef DEBUG
// including nso_ba, sbf_ba and interpreter directly instead of
// #include "tau.h" to avoid error lines pointing to a generated tau.h
#include "boolean_algebras/bv_ba.h"
#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/qint.h"
#include "boolean_algebras/qlt.h"
#include "boolean_algebras/nlang_ba.h"
#include "boolean_algebras/hsb.h"
#include "boolean_algebras/nso_ba.h"
#include "boolean_algebras/tau_ba.h"
#include "boolean_algebras/variant_ba.h"
#include "base_ba_dispatcher.h"
#include "api.h"
#else
#	include "tau.h"
#endif // DEBUG
#include "repl_evaluator.h"
#include "utility/cli.h"

using namespace std;
using namespace idni;
using namespace idni::tau_lang;

using node_t = tau_lang::node<tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>, qint, qlt, nlang_ba, bv, sbf_ba, hsb>;
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
	opts["spec-size-warn"] = cli::option("spec-size-warn", 'w', "0")
		.set_description("warn when an updated specification exceeds "
			"this many characters (0 = off)");
	opts["pwr-semantic"] = cli::option("pwr-semantic", 'W', false)
		.set_description("enable the semantic (winning-region) fallback "
			"of the temporal pointwise revision (off by default)");
	opts["max-revision-alts"] = cli::option("max-revision-alts", 'a', "0")
		.set_description("cap the revision alternatives kept per "
			"specification part, dropping middle preference tiers "
			"(0 = unlimited)");
	opts["block-max-splits"] = cli::option("block-max-splits", 'p', "0")
		.set_description("cap per-block Boole-decomposition splits in "
			"anti-prenexing (0 = unlimited)");
	opts["block-max-rounds"] = cli::option("block-max-rounds", 'r', "0")
		.set_description("cap anti-prenexing quantifier-block driver "
			"rounds (0 = unlimited)");
	// The two SO-1-exposed caps ship FINITE (matching the inline defaults
	// in satisfiability.tmpl.h): unlimited hangs on a non-converging spec.
	opts["max-fixpoint-steps"] = cli::option("max-fixpoint-steps", 'f', "500")
		.set_description("cap temporal-normalization fixpoint steps "
			"(default 500; 0 = unlimited)");
	opts["max-flag-search-steps"] =
		cli::option("max-flag-search-steps", 'F', "500")
		.set_description("cap the eventual-flag search past the flag "
			"boundary; give-up reports unsatisfiable "
			"(default 500; 0 = unlimited)");
	opts["max-blast-reentry-depth"] =
		cli::option("max-blast-reentry-depth", 'D', "0")
		.set_description("cap blast-block re-entry nesting in "
			"anti-prenexing (0 = unlimited)");
	opts["block-squeeze-cap"] = cli::option("block-squeeze-cap", 'z', "0")
		.set_description("skip block squeezing above this operand-set "
			"size (0 = unlimited)");
	opts["max-simplify-rounds"] =
		cli::option("max-simplify-rounds", 'm', "0")
		.set_description("cap bitvector simplification rewrite rounds "
			"(0 = unlimited)");
	opts["max-def-passes"] = cli::option("max-def-passes", 'P', "0")
		.set_description("cap definition-expansion passes "
			"(0 = unlimited)");
	opts["max-enum-steps"] = cli::option("max-enum-steps", 'E', "0")
		.set_description("cap recurrence-relation enumeration steps "
			"(0 = unlimited)");
	opts["max-rewrite-rounds"] = cli::option("max-rewrite-rounds", 'R', "0")
		.set_description("cap rewrite-to-fixpoint rounds "
			"(0 = unlimited)");
	opts["gc-min-size"] = cli::option("gc-min-size", 'G', "256")
		.set_description("tree-node count floor before gc may trigger");
	opts["gc-growth-factor"] = cli::option("gc-growth-factor", 'W', "1.5")
		.set_description("gc triggers when node count grows by this "
			"factor since last sweep (<= 0 disables gc)");
	return opts;
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
		if (!ifs) return result(error("Cannot open file " + spec_file));
		auto l = ifs.tellg();
		src.resize(l), ifs.seekg(0), ifs.read(&src[0], l);
	}
	m.part() = { "reading input", t.pause() };
	if (src.empty()) return result(0);
	t.unpause();
	auto maybe_i = tau_api::get_interpreter(m.part(), src);
	if (!maybe_i) return result(1);
	auto& i = maybe_i.value();
	while (true) {
		auto maybe_outputs = tau_api::step(m.part(), i);
		if (!maybe_outputs) {
			if (opts["quit"].get<bool>()) {
				TAU_LOG_INFO << "No more inputs provided."
					<< " Terminating.";
				break;
			}
			TAU_LOG_INFO << "No input provided."
				<< " q or quit to terminate."
				<< " Press ENTER to continue.";
			std::string line;
			term::enable_getline_mode();
			t.pause();
			std::getline(std::cin, line);
			t.unpause();
			term::disable_getline_mode();
			// On closed/exhausted stdin, getline never sets line to "q"/
			// "quit" and keeps returning immediately, spinning this loop
			// forever; stop the same way repl_evaluator::run_cmd does.
			if (std::cin.eof() || std::cin.fail()) { std::cin.clear(); break; }
			if (line == "q" || line == "quit") break;
		}
	}
	return result(0);
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
	tau_api::set_json(opts["json"].get<bool>());
	bool charvar = opts["charvar"].get<bool>();
	bool blasting = opts["blasting"].get<bool>();
	bool exp = opts["experimental"].get<bool>();
	// Every numeric limit goes through its api setter so the CLI and the
	// REPL `set` command share one wiring surface (0 = unlimited by
	// convention; the gc knobs take raw values).
	auto optnum = [&opts](const char* name) -> size_t {
		return (size_t)std::atoll(opts[name].get<string>().c_str()); };
	tau_api::set_spec_size_warn(optnum("spec-size-warn"));
	tau_api::set_max_revision_alts(optnum("max-revision-alts"));
	tau_api::set_pwr_semantic_fallback(opts["pwr-semantic"].get<bool>());
	tau_api::set_block_max_splits(optnum("block-max-splits"));
	tau_api::set_block_max_rounds(optnum("block-max-rounds"));
	tau_api::set_max_fixpoint_steps(optnum("max-fixpoint-steps"));
	tau_api::set_max_flag_search_steps(optnum("max-flag-search-steps"));
	tau_api::set_max_blast_reentry_depth(optnum("max-blast-reentry-depth"));
	tau_api::set_block_squeeze_cap(optnum("block-squeeze-cap"));
	tau_api::set_max_simplify_rounds(optnum("max-simplify-rounds"));
	tau_api::set_max_def_passes(optnum("max-def-passes"));
	tau_api::set_max_enum_steps(optnum("max-enum-steps"));
	tau_api::set_max_rewrite_rounds(optnum("max-rewrite-rounds"));
	tau_api::set_gc_min_size(optnum("gc-min-size"));
	tau_api::set_gc_growth_factor(
		std::atof(opts["gc-growth-factor"].get<string>().c_str()));

	if (files.size()) {
		DBG(TAU_LOG_TRACE << "running specification file: "
			<< files.front();)
		tau_api::set_severity(sev);
		tau_api::set_charvar(charvar);
		tau_api::set_blasting(blasting);
		return run_tau_spec(files.front(), opts);
	}

	repl_evaluator<qint, qlt, nlang_ba, bv, sbf_ba, hsb> re({
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
