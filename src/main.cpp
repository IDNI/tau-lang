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
#include "interpreter.h"
#else
#	include "tau.h"
#endif // DEBUG
#include "repl_evaluator.h"
#include "utility/cli.h"

using namespace std;
using namespace idni;
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
	opts["experimental"] = cli::option("experimental", 'x', false)
		.set_description("enables transitioning features");
	return opts;
}

int error(const string& s) { TAU_LOG_ERROR << "" << s; return 1; }

// parse tau spec from a string using line by line parsing attempts
// If a parse result is "Unexpected end of file", it appends next line and repeats.
// This is used for making the dot ('.') at the end of a statement optional at
// the end of a line.
std::optional<rr<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>> get_spec_multiline(
	const string& src)
{
	using node = tau_lang::node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
	using tau = tree<node>;
	std::istringstream iss(src);
	std::string attempt, line, msg;
	tref main = nullptr;
	definitions<node>& defs = definitions<node>::instance();

	while (std::getline(iss, line)) {
		if (!attempt.empty()) attempt += "\n";
		attempt += line;

		auto result = tau_parser::instance().parse(
			attempt.c_str(), attempt.size(),
			{ .start = tau_parser::spec_multiline });
		if (result.found) {
			auto& global_scope = defs.get_global_scope();
			tau::get_options opts = {
				.definition_heads = defs.get_definition_heads(),
				.global_scope = global_scope
			};
			tref n = tau::get(result, opts);
			if (!n) return {};
			global_scope = opts.global_scope;
			const auto& t = tau::get(n);
			if (t.child_is(tau::main)) {
				if (main) {
					msg = "Multiple main formulas";
					break;
				}
				main = tau::trim2(n);
			}
			else if (t.child_is(tau::rec_relation))
				defs.add(tau::geth(t[0][0]),
					tau::geth(resolve_io_vars<node>(
						defs.get_io_context(),
						t[0].second())));
			// else if (t.child_is(tau::input_def)) {
			// 	if (!get_io_def<node>(t.first(),
			// 			      defs.get_input_defs()))
			// 	{
			// 		TAU_LOG_ERROR << "Invalid type "
			// 			<< TAU_TO_STR(t.first());
			// 		return {};
			// 	}
			// }
			// else if (t.child_is(tau::output_def)) {
			// 	if (!get_io_def<node>(t.first(),
			// 			      defs.get_output_defs()))
			// 	{
			// 		TAU_LOG_ERROR << "Invalid type "
			// 			<< TAU_TO_STR(t.first());
			// 		return {};
			// 	}
			// }
			attempt = "";
		} else {
			// Check if the error contains "Unexpected end of file"
			msg = result.parse_error.to_str(
				tau_parser::error::info_lvl::INFO_BASIC);
			// if error is not "Unexpected end of file", break to log it and return fail
			if (msg.find("Unexpected end of file")
						== std::string::npos) break;
		}
	}
	if (msg.empty() && !main) msg = "No main formula";
	if (msg.size()) {
		TAU_LOG_ERROR << "[tau] " << msg << "\n";
		return {};
	}
	rr<node> nso_rr{ tau::geth(resolve_io_vars<node>(
		defs.get_io_context(), main)) };
	auto definitions = defs.get_sym_defs();
	for (auto& def : definitions) nso_rr.rec_relations.emplace_back(
		def.first,
		tau::geth(resolve_io_vars<node>(
			defs.get_io_context(), def.second->get())));
	return nso_rr;
}

int run_tau_spec(string spec_file, bool charvar, bool /*exp*/,
	boost::log::trivial::severity_level sev)
{
	logging::set_filter(sev);

	string src = "";
	if (spec_file == "-") {
		std::ostringstream oss;
		oss << std::cin.rdbuf(), src = oss.str();
	} else {
		TAU_LOG_TRACE << "open file: " << spec_file;
		std::ifstream ifs(spec_file, std::ios::binary | std::ios::ate);
		if (!ifs) return error("Cannot open file " + spec_file);
		auto l = ifs.tellg();
		src.resize(l), ifs.seekg(0), ifs.read(&src[0], l);
	}
	if (src.empty()) return 0;

	using node = tau_lang::node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;

	std::set<std::string> guards{ charvar ? "charvar" : "var" };
	tau_parser::instance().get_grammar().set_enabled_productions(guards);
	sbf_parser::instance().get_grammar().set_enabled_productions(guards);

	auto spec_opt = get_spec_multiline(src);
	if (!spec_opt) return 1;
	tref applied = apply_rr_to_formula(spec_opt.value());
	if (!applied) return 2;
	tref normalized = normalizer<node>(applied);
	if (!normalized) return 3;
	if (has_free_vars<node>(normalized)) return 4;
	definitions<node>& defs = definitions<node>::instance();
	auto result = run<node>(normalized, defs.get_io_context());
	if (!result) return 5;
	return 0;
}

// TODO (LOW) after new run_tau_spec is stable, remove this function
int run_tau_spec_old(string spec_file, bool charvar, bool exp,
	boost::log::trivial::severity_level sev)
{
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
	repl_evaluator<bv, sbf_ba> re({
		.print_history_store = false,
		.error_quits         = true,
		.charvar             = charvar,
		.repl_running        = false,
		.severity	     = sev,
		.experimental        = exp
	});
	if (auto status = re.eval(src); status) return status;
	return re.eval("run %");
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
	ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::cleanup();
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

	pretty_printer_highlighting = opts["highlighting"].get<bool>();
	pretty_printer_indenting    = opts["indenting"].get<bool>();

	bool charvar = opts["charvar"].get<bool>();
	std::set<std::string> guards{ charvar ? "charvar" : "var" };
	tau_parser::instance().get_grammar().set_enabled_productions(guards);
	sbf_parser::instance().get_grammar().set_enabled_productions(guards);

	bool exp = opts["experimental"].get<bool>();

	// spec provided, run it
	if (files.size()) {
		if (exp) return run_tau_spec(files.front(), charvar, exp, sev);
		else return run_tau_spec_old(files.front(), charvar, exp, sev);
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
	if (e.size()) return re.eval(e), 0;
	repl<decltype(re)> r(re, "tau> ", ".tau_history");
	welcome();
	re.prompt();
	return r.run();

}
